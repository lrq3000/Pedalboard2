//	MidiCcAlertWindow.cpp - Overridden AlertWindow allowing us to use MIDI
//							learn.
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

#include "MidiCcAlertWindow.h"

//------------------------------------------------------------------------------
MidiCcAlertWindow::MidiCcAlertWindow(MidiMappingManager *midi):
AlertWindow("MIDI CC mapping", 
			"Select a MIDI CC to map this command to:",
			AlertWindow::NoIcon),
midiManager(midi),
listening(false),
asyncCc(-1)
{
	StringArray tempArr;

	tempArr.add("<< MIDI Learn >>");
	tempArr.addArray(MidiMappingManager::getCCNames());
	addComboBox("midiCc", tempArr);

	addButton("OK", 1, KeyPress(KeyPress::returnKey));
	addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));

	getComboBoxComponent("midiCc")->addListener(this);
	getComboBoxComponent("midiCc")->setSelectedId(2);

}

//------------------------------------------------------------------------------
MidiCcAlertWindow::~MidiCcAlertWindow()
{
	if(listening)
		midiManager->unregisterMidiLearnCallback(this);
}

//------------------------------------------------------------------------------
void MidiCcAlertWindow::handleAsyncUpdate()
{
	if(asyncCc > -1)
		getComboBoxComponent("midiCc")->setSelectedId(asyncCc+2);
}

//------------------------------------------------------------------------------
void MidiCcAlertWindow::comboBoxChanged(ComboBox *comboBoxThatHasChanged)
{
	if(comboBoxThatHasChanged->getSelectedId() == 1)
	{
		listening = true;
		midiManager->registerMidiLearnCallback(this);
	}
}

//------------------------------------------------------------------------------
void MidiCcAlertWindow::midiCcReceived(int val)
{
	asyncCc = val;
	triggerAsyncUpdate();
}
