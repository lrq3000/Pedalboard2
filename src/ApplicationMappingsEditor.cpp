//	ApplicationMappingsEditor.cpp - Component used to edit the application
//								  mappings.
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

#include "ApplicationMappingsEditor.h"
#include "MidiMappingManager.h"
#include "OscMappingManager.h"

//------------------------------------------------------------------------------
ApplicationMappingsEditor::ApplicationMappingsEditor(ApplicationCommandManager *app,
													 MidiMappingManager *midi,
													 OscMappingManager *osc):
appManager(app),
midiManager(midi),
oscManager(osc)
{
	mappingsTree = new TreeView();
	mappingsTree->setRootItem(new RootItem(app, midi, osc));
	mappingsTree->setRootItemVisible(false);
	mappingsTree->setSize(400, 430);
	addAndMakeVisible(mappingsTree);

	resetButton = new TextButton(L"Reset to defaults");
	resetButton->setBounds(298, 444, 110, 24);
	resetButton->addListener(this);
	addAndMakeVisible(resetButton);

	setSize(400, 524);
}

//------------------------------------------------------------------------------
ApplicationMappingsEditor::~ApplicationMappingsEditor()
{
	mappingsTree->deleteRootItem();
	deleteAllChildren();
}

//------------------------------------------------------------------------------
void ApplicationMappingsEditor::paint(Graphics& g)
{
	g.fillAll(ColourScheme::getInstance().colours[L"Window Background"]);
}

//------------------------------------------------------------------------------
void ApplicationMappingsEditor::resized()
{
	mappingsTree->setSize(getWidth(), getHeight()-28);

	resetButton->setTopLeftPosition((getWidth()-112), (getHeight()-26));
}

//------------------------------------------------------------------------------
void ApplicationMappingsEditor::buttonClicked(Button *button)
{
	int i;

	if(button == resetButton)
	{
		appManager->getKeyMappings()->resetToDefaultMappings();

		for(i=(midiManager->getNumAppMappings()-1);i>=0;--i)
			delete midiManager->getAppMapping(i);

		for(i=(oscManager->getNumAppMappings()-1);i>=0;--i)
			delete oscManager->getAppMapping(i);

		for(i=0;i<mappingsTree->getRootItem()->getNumSubItems();++i)
		{
			mappingsTree->getRootItem()->getSubItem(i)->setOpen(false);
			mappingsTree->getRootItem()->getSubItem(i)->setOpen(true);
		}
	}
}
