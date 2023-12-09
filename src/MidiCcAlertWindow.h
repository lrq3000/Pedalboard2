//	MidiCcAlertWindow.h - Overridden AlertWindow allowing us to use MIDI learn.
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

#ifndef MIDICCALERTWINDOW_H_
#define MIDICCALERTWINDOW_H_

#include "MidiMappingManager.h"

#include "../JuceLibraryCode/JuceHeader.h"

///	Overridden AlertWindow allowing us to use MIDI learn.
class MidiCcAlertWindow : public AlertWindow,
						  public AsyncUpdater,
						  public ComboBox::Listener,
						  public MidiMappingManager::MidiLearnCallback
{
  public:
	///	Constructor.
	MidiCcAlertWindow(MidiMappingManager *midi);
	///	Destructor.
	~MidiCcAlertWindow();

	///	Updates the combo box when we receive a MIDI CC message.
	void handleAsyncUpdate();
	///	So we know when to listen for MIDI learn messages.
	void comboBoxChanged(ComboBox *comboBoxThatHasChanged);

	///	Called when we receive a MIDI CC message.
	void midiCcReceived(int val);
  private:
	///	Our copy of the MidiMappingManager.
	MidiMappingManager *midiManager;

	///	True if we're currently listening for MIDI learn messages.
	bool listening;
	///	The CC returned from midiCcReceived's AsyncUpdater.
	int asyncCc;
};

#endif
