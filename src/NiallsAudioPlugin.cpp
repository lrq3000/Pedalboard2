//	NiallsAudioPlugin.cpp - Base class for NAPs.
//	----------------------------------------------------------------------------
//	This file is part of Pedalboard2, an audio plugin host.
//	Copyright (c) 2009 Niall Moody.
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

#include "NiallsAudioPlugin.h"

using namespace std;

//------------------------------------------------------------------------------
NiallsAudioPlugin::NiallsAudioPlugin(int numAudioInputs,
								     int numAudioOutputs,
								     int numParams,
								     const string& pluginName):
name(pluginName),
numInputs(numAudioInputs),
numOutputs(numAudioOutputs),
numParameters(numParams)
{
	inputNames = new string[numInputs];
	outputNames = new string[numOutputs];
	parameterNames = new string[numParameters];
}

//------------------------------------------------------------------------------
NiallsAudioPlugin::~NiallsAudioPlugin()
{
	delete [] inputNames;
	delete [] outputNames;
	delete [] parameterNames;
}

//------------------------------------------------------------------------------
void NiallsAudioPlugin::setInputName(int index, const string& inputName)
{
	if(index < numInputs)
		inputNames[index] = inputName;
}

//------------------------------------------------------------------------------
void NiallsAudioPlugin::setOutputName(int index, const string& outputName)
{
	if(index < numOutputs)
		outputNames[index] = outputName;
}

//------------------------------------------------------------------------------
void NiallsAudioPlugin::setParameterName(int index, const string& parameterName)
{
	if(index < numParameters)
		parameterNames[index] = parameterName;
}

