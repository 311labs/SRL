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
* Last $Author: ians $     $Revision: 184 $                                
* Last Modified $Date: 2005-10-17 18:26:59 -0400 (Mon, 17 Oct 2005) $                                    
*********************************************************************************/

#include "srl/Timer.h"

#ifdef WIN32
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
#endif

using namespace SRL;

// Basic Timer Logic
Timer::Timer(TimeValue tval) : 
_start_timer(tval),
_elapsed(0.0),
_is_running(false)
{
	_update();
}

Timer::Timer(TimeValue start_tval, TimeValue end_tval) :
_start_timer(start_tval),
_elapsed(0.0),
_is_running(false)
{
	_elapsed = end_tval.tv_sec - _start_timer.tv_sec;
	_elapsed += (end_tval.tv_usec - _start_timer.tv_usec) / 1000000.0;
}


Timer::Timer() : 
_start_timer(),
_elapsed(0.0),
_is_running(false)
{
}

void Timer::reset()
{
	_elapsed = 0.0;
	System::GetCurrentTime(_start_timer);
}

void Timer::start()
{
	reset();
	_is_running = true;
}

float64 Timer::stop()
{
	_is_running = false;
	return _update();
}

float64 Timer::total() const
{
	if (_is_running)
		return _update();

	return _elapsed;
}

uint32 Timer::totalSeconds() const
{
	if (_is_running)
		return (uint32)_update();

	return (uint32)_elapsed;
}

float32 Timer::totalMS() const
{
	if (_is_running)
		_update();

	// convert to milliseconds
	return (float32)(_elapsed*1000);
}

float64 Timer::_update() const
{
	TimeValue now;
	System::GetCurrentTime(now);
	_elapsed = now.tv_sec - _start_timer.tv_sec;
	_elapsed += (now.tv_usec - _start_timer.tv_usec) / 1000000.0;
	return _elapsed;
}

// Presicion Timer Logic
PrecisionTimer::PrecisionTimer() :
_start_timer(0),
_ticks_per_second(System::CurrentCpuFreq()),
_elapsed(0.0), 
_is_running(false)
{

}

void PrecisionTimer::reset()
{
	_elapsed = 0.0;
	_start_timer = System::CurrentCpuClocks();
}

void PrecisionTimer::start()
{
	reset();
	_is_running = true;
}

float64 PrecisionTimer::stop()
{
	_is_running = false;
	return _update();
}

float64 PrecisionTimer::total() const
{
	if (_is_running)
		return _update();

	return _elapsed;
}

uint32 PrecisionTimer::totalSeconds() const
{
	if (_is_running)
		return (uint32)_update();

	return (uint32)_elapsed;
}

uint32 PrecisionTimer::totalMS() const
{
	if (_is_running)
		_update();

	// convert to milliseconds
	return (uint32)(_elapsed*1000);
}

float64 PrecisionTimer::_update() const
{
	_elapsed = (float64)(System::CurrentCpuClocks() - _start_timer)/_ticks_per_second;
	return _elapsed;
}



