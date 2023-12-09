//	MainTransport.h - Singleton representing the app's main playback transport.
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

#ifndef MAINTRANSPORT_H_
#define MAINTRANSPORT_H_

#include "../JuceLibraryCode/JuceHeader.h"

///	Singleton representing the app's main playback transport.
/*!
	We use the ChangeBroadcaster mechanism to inform any interested transports
	about changes in the main transport's state. So when the user clicks the
	main transport play button, change messages will be sent to any registered
	ChangeListeners (i.e. any interested transports), which will then query
	MainTransport to determine whether they should start or stop playing, and
	whether they should return to zero. This happens asynchronously, because
	it's possible the signal to start/stop the main transport could come from
	a MIDI source, which would be running in the audio thread.
 */
class MainTransport : public ChangeBroadcaster
{
  public:
	///	Destructor.
	~MainTransport();

	///	Registers a transport with this singleton.
	void registerTransport(ChangeListener *transport);
	///	Unregisters a transport with this singleton.
	void unregisterTransport(ChangeListener *transport);

	///	Called from each transport when they have reached the end of their timeline.
	void transportFinished();

	///	Called by MainPanel's play button to update the current state.
	void toggleState();
	///	Called by MainPanel's rtz button to return all transports to their start position.
	void setReturnToZero();

	///	Returns the transport's current state.
	/*!
		\return True if we're playing, false otherwise.
	 */
	bool getState() const {return state;};
	///	Returns true if the calling transport needs to return to its start position.
	/*!
		This is a slightly complicated method because we rely on the
		ChangeBroadcaster mechanism to inform transports they need to return to
		zero. When setReturnToZero() is called, we set the rtzCount variable to
		transports.size(), and decrement it every time getReturnToZero() is
		called to ensure that returnToZero is set to false after all the
		transports have been informed about the change.
	 */
	bool getReturnToZero();

	juce_DeclareSingleton(MainTransport, true)
	juce_UseDebuggingNewOperator
  private:
	///	Constructor.
	MainTransport();

	///	Keeps track of the registered transports.
	/*!
		This would be unnecessary if juce let you query the number of listeners
		registered to a ChangeBroadcaster.
	 */
	Array<ChangeListener *> transports;

	///	The current state.
	bool state;
	///	Used to determine whether a getReturnToZero() caller should return to zero.
	int returnToZero;
	///	Used to determine when to switch the main play button back to the play icon.
	int transportsPlaying;
};

#endif
