//	OSCTimeTag.cpp - Class representing an OSC Time Tag, used in Bundles.
//	----------------------------------------------------------------------------
//	This file is part of Niall's OSC Lib - a simple library providing the basic
//	operations necessary for communicating via Open Sound Control.
//	Copyright (C) 2005-2009  Niall Moody
//	
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//	----------------------------------------------------------------------------

#ifdef WIN32
//Needed so we can use ntohl().
#include <winsock.h>
//Needed so we can use _ftime.
#include <sys/timeb.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#endif

#include "OSCTimeTag.h"
#include <sstream>
#include <cmath>
#include <ctime>

using namespace std;

namespace OSC
{

//------------------------------------------------------------------------------
TimeTag::TimeTag(const TimeTag& copy)
{
	seconds = copy.seconds;
	fraction = copy.fraction;
}

//------------------------------------------------------------------------------
TimeTag::TimeTag()
{
	seconds = 0;
	fraction = 1;
}

//------------------------------------------------------------------------------
TimeTag::TimeTag(const uint32_t seconds, const uint32_t fraction)
{
	this->seconds = seconds;
	this->fraction = fraction;
}

//------------------------------------------------------------------------------
TimeTag::TimeTag(char *data)
{
	uint32_t *tempBytes;

	//Get the seconds.
	tempBytes = reinterpret_cast<uint32_t *>(data);
	seconds = ntohl(*tempBytes);

	//Get the fractional part.
	tempBytes = reinterpret_cast<uint32_t *>(data+4);
	fraction = ntohl(*tempBytes);
}

//------------------------------------------------------------------------------
string TimeTag::getTimeAsText()
{
	ostringstream out(ostringstream::out);

	//*********************************************************************
	//****TODO: Make sure fraction displays the correct number of zeros****
	//*********************************************************************
	out << (unsigned int)seconds << "." << (unsigned int)fraction;

	return out.str();
}

//------------------------------------------------------------------------------
void TimeTag::setSeconds(const uint32_t val)
{
	seconds = val;
}

//------------------------------------------------------------------------------
void TimeTag::setFraction(const uint32_t val)
{
	fraction = val;
}

//------------------------------------------------------------------------------
void TimeTag::setTime(const uint32_t seconds, const uint32_t fraction)
{
	this->seconds = seconds;
	this->fraction = fraction;
}

//------------------------------------------------------------------------------
TimeTag TimeTag::getCurrentTime()
{
	uint32_t retSeconds;
	uint32_t retFraction;

#ifdef WIN32
	_timeb currentTime;

	_ftime_s(&currentTime);

	//2208988800 = Num. seconds from 1900 to 1970, where _ftime starts from.
	retSeconds = 2208988800 + static_cast<uint32_t>(currentTime.time);
	//Correct for timezone.
	retSeconds -= static_cast<uint32_t>(60 * currentTime.timezone);
	//Correct for daylight savings time.
	if(currentTime.dstflag)
		retSeconds += static_cast<uint32_t>(3600);

	retFraction = static_cast<uint32_t>(currentTime.millitm);
	//Fill up all 32 bits...
	retFraction *= static_cast<uint32_t>(static_cast<float>(1<<31)/1000000.0f);
#else
	retSeconds = 0;
	retFraction = 0;
#endif

	return TimeTag(retSeconds, retFraction);
}

//------------------------------------------------------------------------------
bool TimeTag::operator==(const TimeTag& other)
{
	bool retval = false;

	if(seconds == other.seconds)
	{
		if(fraction == other.fraction)
			retval = true;
	}

	return retval;
}

//------------------------------------------------------------------------------
bool TimeTag::operator!=(const TimeTag& other)
{
	bool retval = false;

	if(seconds != other.seconds)
		retval = true;
	else if(fraction != other.fraction)
		retval = true;

	return retval;
}

//------------------------------------------------------------------------------
bool TimeTag::operator<(const TimeTag& other)
{
	bool retval = false;

	if(fraction < other.fraction)
	{
		if(seconds <= other.seconds)
			retval = true;
	}
	else if(seconds < other.seconds)
		retval = true;

	return retval;
}

//------------------------------------------------------------------------------
bool TimeTag::operator>(const TimeTag& other)
{
	bool retval = false;

	if(fraction > other.fraction)
	{
		if(seconds >= other.seconds)
			retval = true;
	}
	else if(seconds > other.seconds)
		retval = true;

	return retval;
}

//------------------------------------------------------------------------------
bool TimeTag::operator<=(const TimeTag& other)
{
	bool retval = false;

	if(fraction <= other.fraction)
	{
		if(seconds <= other.seconds)
			retval = true;
	}
	else if(seconds <= other.seconds)
		retval = true;

	return retval;
}

//------------------------------------------------------------------------------
bool TimeTag::operator>=(const TimeTag& other)
{
	bool retval = false;

	if(fraction > other.fraction)
	{
		if(seconds >= other.seconds)
			retval = true;
	}
	else if(seconds > other.seconds)
		retval = true;

	return retval;
}

//------------------------------------------------------------------------------
TimeTag TimeTag::operator=(const TimeTag& other)
{
	seconds = other.seconds;
	fraction = other.fraction;

	return *this;
}

}
