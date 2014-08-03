/*********************************************************************************
* Copyright (C) 1999,2006 by Ian C. Starnes   All Rights Reserved.        
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
*******************************************************************************/
#ifndef __SRL_Date__
#define __SRL_Date__

#include "srl/String.h"

namespace SRL
{

	/** basic date class 
	*/
	class SRL_API Date
	{
	public:
		/** days of week */
		enum WEEKDAY{
			SUNDAY = 1,
			MONDAY,
			TUESDAY,
			WEDNESDAY,
			THURSDAY,
			FRIDAY,
			SATURDAY
		};

		/** months */
		enum MONTH{
			JANUARY	  = 1,
			FEBRUARY  = 2,
			MARCH     = 3,
			APRIL     = 4,
			MAY       = 5,
			JUNE      = 6,
			JULY      = 7,
			AUGUST    = 8,
			SEPTEMBER = 9,
			OCTOBER   = 10,
			NOVEMBER  = 11,
			DECEMBER  = 12
		};

		/** default contructor current date */
		Date(bool local_time=true, bool empty=false);
		/** contruct date with specified day, month, year */
		Date(uint8 day, uint8 month, int16 year);
		/** construct date from time_t */
		Date(time_t tm); // !ians: possible conflict with julian date
		/** construct date from tm struct */
		Date(tm *dt);
		/** contruct date from julian date */
		Date(int32 julian_date);
		/** construct date from string */
		Date(String &str_date);
		/** copy constructor */
		Date(const Date &date);
		
		/** decontructor */
		virtual ~Date(){};

		/** update the current date to the current system date */
		void update(bool local_time=true);
		/** update with a Timevalue */
		void update(TimeValue &tv);

		/** returns true if the date is valid */
		bool isValid() const;

		/** returns the day of the month */
		uint8 day() const {return _day;}
		/** returns the month */
		uint8 month() const {return _month;}
		/** returns the year */
		int16 year() const {return _year;}

		/** returns the day of week (1=sunday...7=Saturday) */
		uint8 dayOfWeek() const;
		/** returns the day of the year */
		uint16 dayOfYear() const;

		/** returns the number of days in the current month */
		uint16 daysInMonth() const;

		/** returns the date in time_t format */
		time_t toUnix() const;
		/** convert to tm struct */
		void toTM(tm *dt) const;
		/** returns the date as a julian date */
		int32 julian() const{return _julian;}

		/** return true if the date is today */
		bool isToday() const; 
		/** returns true if date is leap year */
		bool isLeapYear() const;

		/** Convert the date to a string using strftime formatting. */
		String formatString(const char* str_format) const;
		/** return date as string in format YYYY/MM/DD */
		String asString() const;

		/** add to year  */
		void addToYear(const int32 years);
		/** add to month */
		void addToMonth(const int32 months);
		/** add to day */
		void addToDay(const int32 days);

	// assignment operator
		/** Assign a date to this */
		Date& operator=(const Date& d);

		/** move the date forward by one day */
		Date& operator++();
		/** move the date back by one day */
		Date& operator--();
		/** move the date forward by passed in days */
		Date& operator+=(const int32 days);
		/** move the date backwards by passed in days */
		Date& operator-=(const int32 days);
		/** move the date forward by passed in days */
		Date& operator+=(const Date& date);
		/** move the date backwards by passed in days */
		Date& operator-=(const Date& date);

		/** add dates together */
		friend SRL_API Date operator+(const Date& date1, const Date& date2);
		/** add to dates to days */
		friend SRL_API Date operator+(const Date& date, const int32& days);
		/** add days to date  */
		friend SRL_API Date operator+(const int32& days, const Date& date);
	
		/** subtract dates together */
		friend SRL_API Date operator-(const Date& date1, const Date& date2);
		/** subtract to dates to days */
		friend SRL_API Date operator-(const Date& date, const int32& days);
		/** subtract days to date  */
		friend SRL_API Date operator-(const int32& days, const Date& date);


		/** check if date is estring2ual to the passed in date */
		friend SRL_API bool operator==(const Date &date1, const Date &date2);
		/** check if the date is not estring2ual to the passed in date */
		friend SRL_API bool operator!=(const Date &date1, const Date &date2);
		/** check if the date is greater then the passed in date */
		friend SRL_API bool operator<(const Date &date1, const Date &date2);
		/** check if the date is less then or estring2ual */
		friend SRL_API bool operator<=(const Date &date1, const Date &date2);
		/** check if the date is greater then */
		friend SRL_API bool operator>(const Date &date1, const Date &date2);
		/** check if the date is greater then or estring2ual */
		friend SRL_API bool operator>=(const Date &date1, const Date &date2);

	protected:
		/** update the julian value */
		void _update_julian();
		/** update the day,month,year values */
		void _update_ymd();
		/** update the date from a time_t */
		void _update(time_t ts);
		/** update the date from a tm struct */
		void _update(tm *dt);
		/** update the date from the basic day, month, year */
		void _update(uint8 day, uint8 month, int16 year);
		/** store variable as a julian date */
		int32 _julian;
		/** variable for day */
		uint8 _day;
		/** variable for month */
		uint8 _month;
		/** variable for year */
		int32 _year;

		// 2d array of days in month
		static const uint8 __days_in_months[2][13];
		// 2d array of days in year
		static const uint16 __days_in_year[2][14];
	};

}

#endif // __SRL_Date__
