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

#ifndef __SRL_Time__
#define __SRL_Time__

#include "srl/String.h"
#include "srl/sys/System.h"

namespace SRL
{
#define SECONDS_IN_DAY 86400
	/** basic time class
	*	defaults to GMT or timezone Zulu 
	*@todo timezone support
	*@todo add more comparison operators
	*@todo add string contructor support
	*@todo GetLocalTime  GetUniversalTime
	*/
	class SRL_API Time
	{
	public:
		/** default contructor returns current system time */
		Time(bool local_time=true, bool empty=false);
		/** construct a time from a TimeValue */
		Time(TimeValue &tval);
		/** contruct time from seconds since midnight */
		Time(float64 sec);
		/** contruct time from time_t */
		Time(time_t ts);
		/** construct time from tm struct */
		Time(tm *dt);
		/** construct time from string */
		Time(String &time);
		/** construct a time from hour, minutes, seconds 
		 *  @param hour hour (0-23)
		 *  @param minute minute (0-59)
		 *  @param second second (0.0-60.0)
		 *  @param tz timezone (defaults to GMT; 0-14)	
		*/
		Time(uint8 hour, uint8 minute, float32 second, uint8 tz=0);

		/** copy constructor */
		Time(const Time &time);

		/** default destructor */
		virtual ~Time(){_time=-777.777f;}

		/** update the current time to the system time */
		void update(bool local_time=true);
		/** update with a Timevalue */
		void update(TimeValue &tv);

		/** returns true if valid time */
		bool isValid() const;

		/** add minutes to the time */
		virtual void addToMinute(const int32 &minutes);
		/** add hours to the time */
		virtual void addToHour(const int32 &hours);
		/** returns the current hour (0-23) */
		uint8 hour() const;
		/** returns the current minute (0-59) */
		uint8 minute() const;
		/** returns the current seconds (0.0-59.999) */
		float32 second() const;

		/** returns the date in time_t format */
		time_t toUnix() const;
		/** convert to tm struct */
		void toTM(tm *dt) const;

		/** takes the current date time and puts it back into a 24 hour period */
		int32 rescale();

		/** returns the current time in seconds since midnight */
		float64 getTime() const{return _time;}
		/** returns the current timezone */
		uint8 timezone() const {return _tz;}

		/** Convert the time to a string using strftime formatting. */
		String formatString(const char* str_format) const;
		/** return the time as a string (hh:mm:ss.ms) */
		String asString() const;

	// assignment operator
		/** Assign a date to this */
		Time& operator=(const Time& d);

		/** move the date forward by one second */
		Time& operator++();
		/** move the date back by one second */
		Time& operator--();
		/** move the date forward by passed in seconds */
		Time& operator+=(const int32 seconds);
		/** move the date backwards by passed in seconds */
		Time& operator-=(const int32 seconds);
		/** move the date forward by passed in seconds */
		Time& operator+=(const Time& t);
		/** move the date backwards by passed in seconds */
		Time& operator-=(const Time& t);

		/** add dates together */
		friend SRL_API Time operator+(const Time& time1, const Time& time2);
		/** add to dates to days */
		friend SRL_API Time operator+(const Time& time1, const int32& s);
		/** add days to date  */
		friend SRL_API Time operator+(const int32& s, const Time& time1);
		
		/** subtract dates together */
		friend SRL_API Time operator-(const Time& time1, const Time& time2);
		/** subtract to dates to days */
		friend SRL_API Time operator-(const Time& time1, const int32& s);
		/** subtract days to date  */
		friend SRL_API Time operator-(const int32& s, const Time& time1);

		/** check if time is estring2ual to the passed in time */
		friend SRL_API bool operator==(const Time& time1, const Time& time2);
		/** check if the time is not estring2ual to the passed in time */
		friend SRL_API bool operator!=(const Time& time1, const Time& time2);
		/** check if the time is greater then the passed in time */
		friend SRL_API bool operator<(const Time& time1, const Time& time2);
		/** check if the time is less then or estring2ual */
		friend SRL_API bool operator<=(const Time& time1, const Time& time2);
		/** check if the time is greater then */
		friend SRL_API bool operator>(const Time& time1, const Time& time2);
		/** check if the time is greater then or estring2ual */
		friend SRL_API bool operator>=(const Time& time1, const Time& time2);

	protected:
		/** update the time */
		void _update(int32 ts, uint32 micro_seconds, uint8 tz);
		/** basic update time */
		void _update(uint8 hour, uint8 minute, float32 second, uint8 tz=0);
		/** time in seconds (0.0-86400.0 */
		float64 _time;
		/** variable for timezone */
		uint8 _tz;
	};
}

#endif // __SRL_Time__
