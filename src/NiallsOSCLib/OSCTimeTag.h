//	OSCTimeTag.h - Class representing an OSC Time Tag, used in Bundles.
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

#ifndef OSCTIMETAG_H_
#define OSCTIMETAG_H_

#include <string>
#include <stdint.h>

//Stupid MSVC v6 warning about the STL...
#ifdef WIN32
#pragma warning(disable:4786)
#endif

namespace OSC
{

///	Class representing an OSC TimeTag, used by Bundle.
class TimeTag
{
  public:
	///	Copy constructor.
	/*!
		Necessary?
	 */
	TimeTag(const TimeTag& copy);
	///	Default Constructor.
	/*!
		Initialises the time to the 'immediately' - i.e. 0x0000000000000001.
	 */
	TimeTag();
	///	Constructor to initialise the time to a particular time.
	TimeTag(const uint32_t seconds, const uint32_t fraction);
	///	Constructor to initialise the time from a char block of 8 bytes.
	/*!
		Note: the data passed in must be at least 8 bytes long!

		This is most useful when you've just received a block of char data and
		need to determine it's time tag.
	 */
	TimeTag(char *data);

	///	Returns the seconds value for this time tag.
	uint32_t getSeconds() const {return seconds;};
	///	Returnes the fractional part for this time tag.
	uint32_t getFraction() const {return fraction;};
	///	Returns the time represented as a string.
	std::string getTimeAsText();

	///	Sets the seconds value for this time tag.
	void setSeconds(const uint32_t val);
	///	Sets the fractional value for this time tag.
	void setFraction(const uint32_t val);
	///	Sets the seconds and fractional values for this time tag.
	void setTime(const uint32_t seconds, const uint32_t fraction);

	///	Static method to create a new TimeTag with the current time.
	static TimeTag getCurrentTime();

	///	So we can test TimeTags against each other.
	bool operator==(const TimeTag& other);
	///	So we can test TimeTags against each other.
	bool operator!=(const TimeTag& other);
	///	So we can test TimeTags against each other.
	bool operator<(const TimeTag& other);
	///	So we can test TimeTags against each other.
	bool operator>(const TimeTag& other);
	///	So we can test TimeTags against each other.
	bool operator<=(const TimeTag& other);
	///	So we can test TimeTags against each other.
	bool operator>=(const TimeTag& other);

	///	So we can assign one TimeTag's values to another.
	TimeTag operator=(const TimeTag& other);
  private:
	///	The seconds value for this time tag.
	uint32_t seconds;
	///	The fractional part of this time tag.
	uint32_t fraction;
};

}

#endif
