/********************************************************************************
* Copyright (C) 1999,2004 by srl, Inc.   All Rights Reserved.                *
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
* Last $Author: ians $     $Revision: 252 $                                
* Last Modified $Date: 2006-03-13 17:31:38 -0500 (Mon, 13 Mar 2006) $                                    
*********************************************************************************/

#include "srl/DateTime.h"

#ifndef HAVE_SYS_TIME_H
	#include <time.h>
#endif 

using namespace SRL;

DateTime::DateTime(bool local_time):
Date(local_time, true), Time(local_time, true)
{
	update(local_time);
}

DateTime::DateTime(time_t ts):
Date(ts), Time(ts)
{
	Date::addToDay(Time::rescale());
}

DateTime::DateTime(uint8 day, uint8 month, int16 year,uint8 hour, uint8 minute, float32 second) :
Date(day, month, year),
Time(hour, minute, second)
{
	
}

DateTime::DateTime(float64 sec, int32 julian_days) :
Date(julian_days),
Time(sec)
{
	Date::addToDay(Time::rescale());
}

DateTime::DateTime(tm *dt) :
Date(dt),
Time(dt)
{
	Date::addToDay(Time::rescale());
}

// DateTime::DateTime(String &dt_str) :
// Date(dt_str),
// Time(dt_str)
// {
//  Date::addToDay(Time::rescale());
// }

#ifdef WIN32
DateTime::DateTime(FILETIME &ft)
{
	SYSTEMTIME st;
	if (!::FileTimeToSystemTime(&ft, &st))
		throw std::logic_error("FileTimeToSystemTime call failed");
	Date::_update(static_cast<uint8>(st.wDay), static_cast<uint8>(st.wMonth),
				  static_cast<uint8>(st.wYear) );
	float32 seconds = (float32)st.wSecond + (((float32)st.wMilliseconds / 1000.0f));
	Time::_update(static_cast<uint8>(st.wHour), static_cast<uint8>(st.wMinute), seconds);
}
#endif

DateTime::DateTime(const DateTime &date) :
Date(),
Time()
{
	*this = date;
}

void DateTime::addToHour(const int32 &hours)
{
	Time::addToHour(hours);
	addToDay(rescale());
}

void DateTime::addToMinute(const int32 &minutes)
{
	Time::addToMinute(minutes);
	addToDay(rescale());
}


void DateTime::update(bool local_time)
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
    Date::update(tv);
    Time::update(tv);
}

void DateTime::update(TimeValue &tv)
{
	Date::update(tv);
	Time::update(tv);
}

bool DateTime::isValid() const
{
	return Time::isValid() && Date::isValid();
}

String DateTime::formatString(const char* str_format) const
{
	tm dt;
	char tmp[256];
	Date::toTM(&dt);
	Time::toTM(&dt);
	strftime(tmp, 255, str_format, &dt);
	return String(tmp);
}

String DateTime::asString() const
{
	return (Date::asString() + " " + Time::asString());
}


DateTime& DateTime::operator=(const DateTime &d)
{
	_time = d._time;
	_julian = d._julian;
	_update_ymd();
	return *this;
}


DateTime& DateTime::operator++()
{
	Time::operator++();
	Date::addToDay(Time::rescale());
	return *this;
}

DateTime& DateTime::operator--()
{
	Time::operator--();
	Date::addToDay(Time::rescale());
	return *this;
}

DateTime& DateTime::operator+=(const DateTime& t)
{
	_julian += t.julian();
	_time += t.getTime();
	Date::addToDay(Time::rescale());
	return *this;
}

DateTime& DateTime::operator-=(const DateTime& t)
{
	_julian -= t.julian();
	_time -= t.getTime();
	Date::addToDay(Time::rescale());
	return *this;
}

DateTime& DateTime::operator+=(const int32 seconds)
{
	_time += seconds;
	Date::addToDay(Time::rescale());
	return *this;
}

DateTime& DateTime::operator-=(const int32 seconds)
{
	_time -= seconds;
	Date::addToDay(Time::rescale());
	return *this;
}



DateTime SRL::operator+(const DateTime& t1, const DateTime& t2)
{
	return DateTime(t1.getTime()+t2.getTime(), t1.julian()+t2.julian());
}

DateTime SRL::operator+(const DateTime& t1, const int32& seconds)
{
	return DateTime(t1.getTime()+seconds, t1.julian());
}

DateTime SRL::operator+(const int32& seconds, const DateTime& t1)
{
	return DateTime(seconds+t1.getTime(), t1.julian());
}

DateTime SRL::operator-(const DateTime& t1, const DateTime& t2)
{
	return DateTime(t1.getTime()-t2.getTime(), t1.julian()-t2.julian());
}

DateTime SRL::operator-(const DateTime& t1, const int32& seconds)
{
	return DateTime(t1.getTime()-seconds, t1.julian());
}

DateTime SRL::operator-(const int32& seconds, const DateTime& t1)
{
	return DateTime(seconds-t1.getTime(), t1.julian());
}

// COMPARISON OPERATIONS
bool SRL::operator==(const DateTime& dt1, const DateTime& dt2)
{
	return ((dt1.getTime() == dt2.getTime())&&(dt1.julian() == dt2.julian()));
}

bool SRL::operator!=(const DateTime& dt1, const DateTime& dt2)
{
	return ((dt1.getTime() != dt2.getTime())&&(dt1.julian() == dt2.julian()));
}

bool SRL::operator<(const DateTime& dt1, const DateTime& dt2)
{
	return ((dt1.getTime() < dt2.getTime())&&(dt1.julian() == dt2.julian()));
}

bool SRL::operator<=(const DateTime& dt1, const DateTime& dt2)
{
	return ((dt1.getTime() <= dt2.getTime())&&(dt1.julian() == dt2.julian()));
}

bool SRL::operator>(const DateTime& dt1, const DateTime& dt2)
{
	return ((dt1.getTime() > dt2.getTime())&&(dt1.julian() == dt2.julian()));
}

bool SRL::operator>=(const DateTime& dt1, const DateTime& dt2)
{
	return ((dt1.getTime() >= dt2.getTime())&&(dt1.julian() == dt2.julian()));
}

