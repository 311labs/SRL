/*********************************************************************************
* Copyright (C) 1999,2004 by srl, Inc.   All Rights Reserved.        *
*********************************************************************************
* Authors: Ian C. Starnes, Dr. J. Shane Frasier, Shaun Grant
*********************************************************************************
* This library is free software; you can redistribute it and/or                 *
* modify it under the terms of the GNU Lesser General Public                    *
* License as published by the Free Software Foundation; either                  *
* version 2.1 of the License, or (at your option) any later version.            *
*                                                                               *
* This library is distributed in the hope that it will be useful,               *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
* Lesser General Public License for more details.                               *
*                                                                               *
* You should have received a copy of the GNU Lesser General Public              *
* License along with this library; if not, write to the Free Software           *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.    *
*********************************************************************************                                                  
* Last $Author: ians $     $Revision: 248 $                                
* Last Modified $Date: 2006-03-08 22:50:35 -0500 (Wed, 08 Mar 2006) $                                    
*********************************************************************************/

#include "srl/Time.h"

using namespace SRL;

// Basic Time Logic
Time::Time(bool local_time, bool empty)
{
    if (!empty)
        update(local_time);
}

Time::Time(TimeValue &tval)
{
    _update(tval.tv_sec, tval.tv_usec, tval.tv_zone);
}

Time::Time(float64 sec) : _time(sec)
{

}

Time::Time(uint8 hour, uint8 minute, float32 second, uint8 tz)
{
    _update(hour, minute, second, tz);
}

Time::Time(time_t ts)
{
    _update(static_cast<int32>(ts), 0, 0);
}

Time::Time(tm *dt)
{
    _time = dt->tm_hour*3600.0f + dt->tm_min*60.0f + dt->tm_sec;
    //_tz = dt->tm_tz;
    
}

// Time::Time(String &str_time)
// {
//  // TODO: Add String Construtor support
// }

Time::Time(const Time &time)
{
    *this = time;
}

uint8 Time::hour() const
{
    return (uint8)((uint32)(_time / 3600))%24;
}

uint8 Time::minute() const
{
    return (uint8)((int32)(_time / 60)%60);
}

float32 Time::second() const
{
    register uint32 itime = (uint32)_time;
    float32 ms = _time - itime;
    return ((float32)(itime % 60))+ms;
}

void Time::addToHour(const int32 &hours)
{
    _time += hours * 3600;
}

void Time::addToMinute(const int32 &minutes)
{
    _time += minutes * 60;
}

void Time::update(bool local_time)
{
    TimeValue tv;
    System::GetCurrentTime(tv);
    if (local_time)
    {
    #ifdef linux
            const time_t t = tv.tv_sec;
            tm *lt = localtime(&t);
            tv.tv_dst = lt->tm_isdst;
    #endif
            tv.tv_sec -= (tv.tv_zone*3600) - (tv.tv_dst ? 3600 : 0);
    }
    _update(tv.tv_sec, tv.tv_usec, tv.tv_zone);
}

void Time::update(TimeValue &tv)
{
    _update(tv.tv_sec, tv.tv_usec, tv.tv_zone);
}

void Time::_update(int32 ts, uint32 micro_seconds, uint8 tz)
{
    if (ts > 86400)
    {
        _time = ts % 86400;
        //_time = ts-((int32)(ts / 86400.0f)*(86400.0f));
    }
    else
    {
        _time = (float64)ts;
    }
    _time += ((float64)micro_seconds/1000000.0);
    _tz = tz;
}

void Time::_update(uint8 hour, uint8 minute, float32 second, uint8 tz)
{
    _time = hour*3600.0f + minute*60.0f + second;
    _tz = tz;
}
bool Time::isValid() const
{
    if ((_time > 0) && (_time < 86400))
        return true;

    return false;
}


// String Time::formatString(const char *str_format) const
// {
//  char tmp[256];
//  //tm dt;
//  //toTM(&dt);
//  //strftime(tmp, 255, str_format, dt);
//  return tmp;
// }

String Time::asString() const
{
    return String::Format("%02d:%02d:%2.4f", hour(), minute(), second());
}

time_t Time::toUnix() const
{
    return (time_t)_time;
}

void Time::toTM(tm *dt) const
{
    //memset (dt, 0x0, sizeof (struct tm));
    dt->tm_hour = hour();
    dt->tm_min = minute();
    dt->tm_sec = (int32)second();
}

int32 Time::rescale()
{
    if ((_time > SECONDS_IN_DAY)||(_time < 0))
    {
        // calculate how many days to change
        int32 new_days = (int32)(_time / SECONDS_IN_DAY);
        _time = _time - ((float32)(new_days * SECONDS_IN_DAY));

        return new_days;
    }
    return 0;
}

Time& Time::operator=(const Time &d)
{
    _time = d._time;
    return *this;
}

Time& Time::operator++()
{
    ++_time;
    return *this;
}

Time& Time::operator--()
{
    --_time;
    return *this;
}

Time& Time::operator+=(const Time& t)
{
    _time += t.getTime();
    return *this;
}

Time& Time::operator-=(const Time& t)
{
    _time -= t.getTime();
    return *this;
}

Time& Time::operator+=(const int32 seconds)
{
    _time += seconds;
    return *this;
}

Time& Time::operator-=(const int32 seconds)
{
    _time -= seconds;
    return *this;
}



Time SRL::operator+(const Time& t1, const Time& t2)
{
    return Time(t1.getTime()+t2.getTime());
}

Time SRL::operator+(const Time& t1, const int32& seconds)
{
    return Time(t1.getTime()+seconds);
}

Time SRL::operator+(const int32& seconds, const Time& t1)
{
    return Time(seconds+t1.getTime());
}

Time SRL::operator-(const Time& t1, const Time& t2)
{
    return Time(t1.getTime()-t2.getTime());
}

Time SRL::operator-(const Time& t1, const int32& seconds)
{
    return Time(t1.getTime()-seconds);
}

Time SRL::operator-(const int32& seconds, const Time& t1)
{
    return Time(seconds-t1.getTime());
}

// COMPARISON OPERATIONS
bool SRL::operator==(const Time& time1, const Time& time2)
{
    // lets estimate if it is within milliseconds
    return time1.getTime() == time2.getTime();
}

bool SRL::operator!=(const Time& time1, const Time& time2)
{
    return time1.getTime() != time2.getTime();
}

bool SRL::operator<(const Time& time1, const Time& time2)
{
    return time1.getTime() < time2.getTime();
}

bool SRL::operator<=(const Time& time1, const Time& time2)
{
    return time1.getTime() <= time2.getTime();
}

bool SRL::operator>(const Time& time1, const Time& time2)
{
    return time1.getTime() > time2.getTime();
}

bool SRL::operator>=(const Time& time1, const Time& time2)
{
    return time1.getTime() >= time2.getTime();
}
