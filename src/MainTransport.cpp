//	MainTransport.cpp - Singleton representing the app's main playback
//						transport.
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

#include "MainTransport.h"

//------------------------------------------------------------------------------
juce_ImplementSingleton(MainTransport)

//------------------------------------------------------------------------------
MainTransport::~MainTransport()
{
	removeAllChangeListeners();
	clearSingletonInstance();
}

//------------------------------------------------------------------------------
void MainTransport::registerTransport(ChangeListener *transport)
{
	addChangeListener(transport);
	transports.add(transport);
}

//------------------------------------------------------------------------------
void MainTransport::unregisterTransport(ChangeListener *transport)
{
	removeChangeListener(transport);
	transports.removeFirstMatchingValue(transport);
}

//------------------------------------------------------------------------------
void MainTransport::transportFinished()
{
	--transportsPlaying;

	//1 because the MainPanel playButton is also registered as a transport, and
	//will never call this method.
	if(transportsPlaying == 1)
	{
		state = false;
		returnToZero = transports.size();
		sendChangeMessage();
	}
}

//------------------------------------------------------------------------------
void MainTransport::toggleState()
{
	state = !state;
	if(state)
		transportsPlaying = transports.size();
	else
		transportsPlaying = 1;
	sendChangeMessage();
}

//------------------------------------------------------------------------------
void MainTransport::setReturnToZero()
{
	returnToZero = transports.size();
	sendChangeMessage();
}

//------------------------------------------------------------------------------
bool MainTransport::getReturnToZero()
{
	bool retval = returnToZero > 0;

	if(returnToZero > 0)
		--returnToZero;

	return retval;
}

//------------------------------------------------------------------------------
MainTransport::MainTransport():
state(false),
returnToZero(0),
transportsPlaying(1)
{
	
}
