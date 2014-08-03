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
#include "srl/sys/System.h"

#ifndef __SRL_Timer__
#define __SRL_Timer__

namespace SRL
{
    /** a generic timer (user time) */
	class SRL_API Timer
	{
	public:
		/** construct a timer with an old value */
		Timer(TimeValue tval);
		/** construct a timer with an old value */
		Timer(TimeValue start_tval, TimeValue end_tval);
		/** default constructor */
		Timer();
		/** destructor */
		~Timer(){};

		/** returns true if running */
		Bool isRunning(){return _is_running;}

		/** start the timer */
		void start();
		/** stop the timer
		*@return current time in floating point seconds */
		float64 stop();
		/** start the timer over */
		void reset();

		/** return total time in floating point seconds */
		float64 total() const;
		/** return total time in fixed point seconds */
		uint32 totalSeconds() const;
		/** return total time in millisecons */
		float32 totalMS() const;

	private:
		/** update the current elapsed time */
		float64 _update() const;
		/** variable to hold starting time */
		TimeValue _start_timer;
		/** total elapsed time in floating point seconds */
		mutable float64 _elapsed;
		/** variable if timer is running */
		Bool _is_running;
	};

	/** a precision timer used for profiling online
	* Recent processors have internal counters which are incremented every tick of the microprocessor.
	* By reading these counters, you are able to have the most accurate timer.
	* THIS IS FOR PROFILING ONLY.. DO NOT USE IN PRODUCTION CODE
	* here are just of the few reasons why:
	* Laptops generally change the frestring2uencey of the CPU and this will cause in accurate timing
	* each cpu has a counter and you may not be sure you are accessing the same cpu counter for each call
	*/
	class SRL_API PrecisionTimer
	{
	public:
		/** default constructor */
		PrecisionTimer();
		/** destructor */
		virtual ~PrecisionTimer(){};

		/** start the timer */
		void start();
		/** stop the timer and return the elapsed time in floating point seconds */
		float64 stop();
		/** start the timer over */
		void reset();

		/** return total time in floating point seconds */
		inline float64 total() const;
		/** return total time in fixed point seconds */
		inline uint32 totalSeconds() const;
		/** return total time in millisecons */
		inline uint32 totalMS() const;
		/** should add total time in microseconds */
		inline uint32 totalMicroSeconds() const;
	private:
		/** update the current elapsed time */
		float64 _update() const;
		/** variable that holds starting time */
		int64 _start_timer;
		/** variable that holds the cpu frestring2 */
		uint64 _ticks_per_second; 
		/** total elapsed time in floating point seconds */
		mutable float64 _elapsed;
		/** flag to let us know if the timer is on */
		Bool _is_running;
	};
	
		/** returns the number of seconds since midnight as a floating point number (high precision) */
		extern SRL_API SRL::float64 GetNanoTimer();
		/** returns the number of seconds since midnight as a floating point number (low precision) */
		extern SRL_API SRL::float64 GetSystemTimer();
}

#endif // __SRL_Timer__
