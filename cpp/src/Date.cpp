
#include "srl/Date.h"
#include "srl/sys/System.h"

#include <string.h>

#ifndef HAVE_SYS_TIME_H
	#include <time.h>
#endif 

using namespace SRL;

const uint8 Date::__days_in_months[2][13] =
{  
  {  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
  {  0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } /* leap year */
};
const uint16 Date::__days_in_year[2][14] =
{ 
  {  0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
  {  0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

// Basic Date Logic
Date::Date(bool local_time, bool empty)
{
	// get the current system date
	if (!empty)
		update(local_time);
}

Date::Date(time_t ts)
{
	_update(ts);
}

Date::Date(tm *dt)
{
	_update(dt);
}

Date::Date(uint8 day, uint8 month, int16 year)
{
	_update(day,month,year);
}

Date::Date(int32 julian_date)
{
	_julian = julian_date;
	_update_ymd();
}

// Date::Date(String &str_date)
// {
//     // TODO implement
// }

Date::Date(const Date &date)
{
	*this = date;
}

void Date::update(bool local_time)
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
	_update(tv.tv_sec);
}

void Date::update(TimeValue &tv)
{
	_update(tv.tv_sec);
}

bool Date::isValid() const
{
	if (_day > 31)
		return false;

	if (_month > 12)
		return false;

	if (_year > 4000)
		return false;

	return true;
}

time_t Date::toUnix() const
{
	// !! ians: still need to implement
	return 0;
}

void Date::toTM(tm *dt) const
{
	memset (dt, 0x0, sizeof (struct tm));

	dt->tm_mday = _day;
	dt->tm_mon  = _month - 1;
	dt->tm_year = ((int)_year) - 1900; 

	uint8 day = dayOfWeek();
	if (day == 7) 
		day = 0;

	dt->tm_wday = (int)day;

	dt->tm_yday = dayOfYear() - 1;
	dt->tm_isdst = -1;
}

uint8 Date::dayOfWeek() const
{
	return static_cast<uint8>((_julian % 7) + 1);
}

uint16 Date::daysInMonth() const
{
	return (__days_in_months[isLeapYear()][_month]);
}

uint16 Date::dayOfYear() const
{
	return (__days_in_year[isLeapYear()][_month] + _day);
}

bool Date::isLeapYear() const
{
	return ( (((_year % 4) == 0) && ((_year % 100) != 0)) || (_year % 400) == 0 );
}

bool Date::isToday() const
{
	if (*this == Date())
		return true;
	return false;
}

void Date::_update(time_t ts)
{
	_update(gmtime(&ts));
}

void Date::_update(tm *dt)
{
	_day = dt->tm_mday;
	_month = dt->tm_mon + 1;
	_year = dt->tm_year + 1900;	
	_update_julian();
}

void Date::_update(uint8 day, uint8 month, int16 year)
{
	_day = day; _month = month; _year = year;
	_update_julian();
}

void Date::_update_julian()
{
	/**
	* Convert YMD to integer Julian date.
	*
	* Computes the julian date (JD) given a gregorian calendar date
	* (year,month,day).  Adapted from Fliegel and van Flandern (1968)
	*/
	int y = _year;
	int m = _month;
	int d = _day;
	_julian = d-32075+1461*(y+4800+(m-14)/12)/4+367*(m-2-(m-14)/12*12)/12-3*((y+4900+(m-14)/12)/100)/4;
}

void Date::_update_ymd()
{
  unsigned int A, B, C, D, E, M;

  /* Formula taken from the Calendar FAQ */

  A = _julian + 32045;
  B = ( 4 *(A + 36524) )/ 146097 - 1;
  C = A - (146097 * B)/4;
  D = ( 4 * (C + 365) ) / 1461 - 1;
  E = C - ((1461*D) / 4);
  M = (5 * (E - 1) + 2)/153;

  _month = M + 3 - (12*(M/10));
  _day = E - (153*M + 2)/5;
  _year = 100 * B + D - 4800 + (M/10);
}

String Date::formatString(const char *str_format) const
{
	char tmp[256];
	tm dt;
	toTM(&dt);
	strftime(tmp, 255, str_format, &dt);
	return tmp;
}

String Date::asString() const
{
	return formatString("%Y/%m/%d");
}

void Date::addToDay(const int32 days)
{
	_julian += days;
	_update_ymd();
}

void Date::addToMonth(const int32 months)
{
	// get the total number of months
	int32 tmonths = _month;
	tmonths += months;
	// find if we have to add years
	int years = tmonths/12;
	// find the remaining new months 
	tmonths = tmonths%12;

	if (tmonths < 0)
	{
		--years;
		tmonths = 12 + tmonths;
	}
	_month = tmonths;
	_year += years;

	if (_day > __days_in_months[isLeapYear()][_month]) 
	{
		_day = __days_in_months[isLeapYear()][_month];
	}
	_update_julian();	
}

void Date::addToYear(const int32 years)
{
	_year += years;
	if (_month == 2 && _day == 29) {
		if (!isLeapYear()) _day = 28;
	}
	_update_julian();
}

Date& Date::operator=(const Date &d)
{
	_julian = d._julian;
	_update_ymd();
	return *this;
}

Date& Date::operator++()
{
	++_julian;
	_update_ymd();
	return *this;
}

Date& Date::operator--()
{
	--_julian;
	_update_ymd();
	return *this;
}

Date& Date::operator +=(const Date& date)
{
	_julian += date.julian();
	_update_ymd();
	return *this;
}

Date& Date::operator -=(const Date& date)
{
	_julian -= date.julian();
	_update_ymd();
	return *this;
}

Date& Date::operator +=(const int32 days)
{
	_julian += days;
	_update_ymd();
	return *this;
}

Date& Date::operator -=(const int32 days)
{
	_julian -= days;
	_update_ymd();
	return *this;
}

bool SRL::operator<(const Date &date1, const Date &date2)
{
	if (date1.julian() < date2.julian())
		return true;

	return false;
}

bool SRL::operator<=(const Date &date1, const Date &date2)
{
	if (date1.julian() <= date2.julian())
		return true;

	return false;
}

bool SRL::operator>(const Date &date1, const Date &date2)
{
	if (date1.julian() > date2.julian())
		return true;

	return false;
}

bool SRL::operator>=(const Date &date1, const Date &date2)
{
	if (date1.julian() >= date2.julian())
		return true;

	return false;
}

bool SRL::operator==(const Date &date1, const Date &date2)
{
	if (date1.julian() == date2.julian())
		return true;

	return false;
}

bool SRL::operator!=(const Date &date1, const Date &date2)
{
	
	if (date1.julian() != date2.julian())
		return true;

	return false;
}

Date SRL::operator-(const Date& date1, const Date& date2)
{
	return Date(date1.julian() - date2.julian());
}

Date SRL::operator-(const Date& date, const int32& days)
{
	return Date(date.julian() - days);
}

Date SRL::operator-(const int32& days, const Date& date)
{
	return Date(days - date.julian());
}

Date SRL::operator+(const Date& date1, const Date& date2)
{
	return Date(date1.julian() + date2.julian());
}

Date SRL::operator+(const Date& date, const int32& days)
{
	return Date(date.julian() + days);
}

Date SRL::operator+(const int32& days, const Date& date)
{
	return Date(days + date.julian());
}
