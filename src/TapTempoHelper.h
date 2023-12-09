//	TapTempoHelper.h - Class used to calculate the tempo from taps.
//	----------------------------------------------------------------------------
//	This file is part of Pedalboard2, an audio plugin host.
//	Copyright (c) 2011 Niall Moody.
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

#ifndef TAPTEMPOHELPER_H_
#define TAPTEMPOHELPER_H_

///	Class used to calculate the tempo from taps.
/*!
	It stores 4 values and uses the average of the difference between these
	values to calculate the tempo. The values each represent the number of
	seconds since the app start/some arbitrary start point.

	If the tempo calculated is less than 30bpm, the values will be reset.
 */
class TapTempoHelper
{
  public:
	///	Constructor.
	TapTempoHelper();
	///	Destructor.
	~TapTempoHelper();

	///	Updates the tempo, and returns the new value.
	/*!
		\param seconds This is the number of seconds since the app start/some
		arbitrary start point, representing the time at which the user 'tapped'
		a key, a MIDI button etc.

		\return The new tempo. This may be 0 if we do not yet have enough
		information to calculate the tempo accurately.
	 */
	double updateTempo(double seconds);
  private:
	///	The number of values used to calculate the tempo.
	enum
	{
		NumValues = 4
	};

	///	The 4 values used to calculate the tempo.
	double times[NumValues];
};

#endif
