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
#ifndef __SRL_DATETIME__
#define __SRL_DATETIME__

#include "srl/Date.h"
#include "srl/Time.h"

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _WIN32_WINNT 0x0501 
	#include "windows.h"
#endif

namespace SRL
{
	/** standard Date Time class
	*/
	class SRL_API DateTime : public Date, public Time
	{
	public:
		/** default constructor (returns the current system date time) */
		DateTime(bool local_time=true);
		/** contruct a date and time */
		DateTime(uint8 day, uint8 month, int16 year,uint8 hour, uint8 minute, float32 second);
		/** construct date time from 24 hour seconds and julian days */
		DateTime(float64 sec, int32 julian_days);
		/** contruct time from time_t */
		DateTime(time_t ts);
		/** construct time from tm struct */
		DateTime(tm *dt);
		/** construct time from string */
		DateTime(String &time);
#ifdef WIN32
		/** special construtor for WIN32 FILETIME structure */
		DateTime(FILETIME &ft);
#endif 
		/** copy constructor */
		DateTime(const DateTime &date);

		/** update the current date & time to the system date & time */
		void update(bool local_time=true);
		/** update using a timevalue */
		void update(TimeValue &tv);

		/** returns true if valid time */
		bool isValid() const;
		
		/** add minutes to the time */
		virtual void addToMinute(const int32 &minutes);
		/** add hours to the time */
		virtual void addToHour(const int32 &hours);

		/** Convert the time to a string using strftime formatting. */
		String formatString(const char* str_format) const;
		/** return the time as a string (YYYY/MM/DD hh:mm:ss.ms) */
		String asString() const;

	// assignment operator
		/** Assign a date to this */
		DateTime& operator=(const DateTime& d);

		/** move the date forward by one second */
		DateTime& operator++();
		/** move the date back by one second */
		DateTime& operator--();
		/** move the date forward by passed in seconds */
		DateTime& operator+=(const int32 seconds);
		/** move the date backwards by passed in seconds */
		DateTime& operator-=(const int32 seconds);
		/** move the date forward by passed in seconds */
		DateTime& operator+=(const DateTime& t);
		/** move the date backwards by passed in seconds */
		DateTime& operator-=(const DateTime& t);

		/** add dates together */
		friend SRL_API DateTime operator+(const DateTime& time1, const DateTime& time2);
		/** add to dates to days */
		friend SRL_API DateTime operator+(const DateTime& time1, const int32& s);
		/** add days to date  */
		friend SRL_API DateTime operator+(const int32& s, const DateTime& time1);
		
		/** subtract dates together */
		friend SRL_API DateTime operator-(const DateTime& time1, const DateTime& time2);
		/** subtract to dates to days */
		friend SRL_API DateTime operator-(const DateTime& time1, const int32& s);
		/** subtract days to date  */
		friend SRL_API DateTime operator-(const int32& s, const DateTime& time1);

		/** check if time is estring2ual to the passed in time */
		friend SRL_API bool operator==(const DateTime& time1, const DateTime& time2);
		/** check if the time is not estring2ual to the passed in time */
		friend SRL_API bool operator!=(const DateTime& time1, const DateTime& time2);
		/** check if the time is greater then the passed in time */
		friend SRL_API bool operator<(const DateTime& time1, const DateTime& time2);
		/** check if the time is less then or estring2ual */
		friend SRL_API bool operator<=(const DateTime& time1, const DateTime& time2);
		/** check if the time is greater then */
		friend SRL_API bool operator>(const DateTime& time1, const DateTime& time2);
		/** check if the time is greater then or estring2ual */
		friend SRL_API bool operator>=(const DateTime& time1, const DateTime& time2);


	protected:

	};
}

#endif // __SRL_DateTime__
