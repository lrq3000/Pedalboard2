//	TapTempoHelper.cpp - Class used to calculate the tempo from taps.
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

#include "TapTempoHelper.h"

#include <cmath>

//------------------------------------------------------------------------------
TapTempoHelper::TapTempoHelper()
{
	int i;

	for(i=0;i<NumValues;++i)
		times[i] = 0.0;
}

//------------------------------------------------------------------------------
TapTempoHelper::~TapTempoHelper()
{

}

//------------------------------------------------------------------------------
double TapTempoHelper::updateTempo(double seconds)
{
	int i;
	double tempd;
	double delta = 0.0;
	int numCounted = 0;
	double retval = 0.0;
	bool blankTimes = false;
	const double lowerLimit = (60.0/30.0); //Lower limit is 30bpm => 2 seconds between taps.

	//Check if we need to blank the times.
	for(i=0;i<NumValues;++i)
	{
		if(times[i] <= 0.0)
			blankTimes = true;
		else if(fabs(times[i]-seconds) > lowerLimit)
			blankTimes = true;
	}

	//Blank the times if necessary.
	if(blankTimes)
	{
		for(i=0;i<NumValues;++i)
			times[i] = seconds;
	}
	else
	{
		//Update the stored values.
		for(i=(NumValues-1);i>0;--i)
			times[i] = times[i-1];
		times[0] = seconds;
	}

	//Now add up the differences between the stored values.
	for(i=1;i<NumValues;++i)
	{
		tempd = fabs(times[i]-times[i-1]);

		//We're only interested in the values that are not 0.
		if(tempd > 0.0)
		{

			delta += tempd;
			++numCounted;
		}
	}

	//Take the average as the tempo.
	if(numCounted > 0)
	{
		retval = delta/(double)numCounted;
		retval = (1.0/retval) * 60.0; //BPM calculation.
	}

	return retval;
}
