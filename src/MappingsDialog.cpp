/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  17 Jan 2013 11:32:41am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...

#include "MappingEntryMidi.h"
#include "MappingEntryOsc.h"
#include "MidiMappingManager.h"
#include "OscMappingManager.h"
#include "Mapping.h"
#include "PluginField.h"
#include "ColourScheme.h"
#include "BypassableInstance.h"

//[/Headers]

#include "MappingsDialog.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

//MappingsDialog::MappingComparator comparator;

//[/MiscUserDefs]

//==============================================================================
MappingsDialog::MappingsDialog (MidiMappingManager *manager, OscMappingManager *manager2, AudioProcessorGraph::Node *plugin, Array<Mapping *> maps, PluginField *field)
    : mappingsList (0),
      addMidiButton (0),
      addOscButton (0),
      deleteButton (0),
      overrideMidiButton (0),
      oscMidiAddressLabel (0),
      oscMidiAddress (0),
      oscHintLabel (0),
      midiChannelLabel (0),
      midiChannelComboBox (0)
{
    addAndMakeVisible (mappingsList = new ListBox (String::empty, this));
    mappingsList->setName (L"mappingsList");

    addAndMakeVisible (addMidiButton = new TextButton (L"addMidiButton"));
    addMidiButton->setButtonText (L"add MIDI");
    addMidiButton->addListener (this);

    addAndMakeVisible (addOscButton = new TextButton (L"addOscButton"));
    addOscButton->setButtonText (L"add OSC");
    addOscButton->addListener (this);

    addAndMakeVisible (deleteButton = new TextButton (L"deleteButton"));
    deleteButton->setButtonText (L"remove");
    deleteButton->addListener (this);

    addAndMakeVisible (overrideMidiButton = new ToggleButton (L"overrideMidiButton"));
    overrideMidiButton->setButtonText (L"Override plugin\'s default MIDI behaviour");
    overrideMidiButton->addListener (this);

    addAndMakeVisible (oscMidiAddressLabel = new Label (L"oscMidiAddressLabel",
                                                        L"OSC MIDI Address:"));
    oscMidiAddressLabel->setFont (Font (15.0000f, Font::plain));
    oscMidiAddressLabel->setJustificationType (Justification::centredLeft);
    oscMidiAddressLabel->setEditable (false, false, false);
    oscMidiAddressLabel->setColour (TextEditor::textColourId, Colours::black);
    oscMidiAddressLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (oscMidiAddress = new TextEditor (L"oscMidiAddress"));
    oscMidiAddress->setMultiLine (false);
    oscMidiAddress->setReturnKeyStartsNewLine (false);
    oscMidiAddress->setReadOnly (false);
    oscMidiAddress->setScrollbarsShown (true);
    oscMidiAddress->setCaretVisible (true);
    oscMidiAddress->setPopupMenuEnabled (true);
    oscMidiAddress->setText (String::empty);

    addAndMakeVisible (oscHintLabel = new Label (L"oscHintLabel",
                                                 L"(leave blank if you don\'t want to receive MIDI over OSC)"));
    oscHintLabel->setFont (Font (15.0000f, Font::plain));
    oscHintLabel->setJustificationType (Justification::centredRight);
    oscHintLabel->setEditable (false, false, false);
    oscHintLabel->setColour (Label::textColourId, Colours::black);
    oscHintLabel->setColour (TextEditor::textColourId, Colours::black);
    oscHintLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (midiChannelLabel = new Label (L"midiChannelLabel",
                                                     L"MIDI Channel:"));
    midiChannelLabel->setFont (Font (15.0000f, Font::plain));
    midiChannelLabel->setJustificationType (Justification::centredLeft);
    midiChannelLabel->setEditable (false, false, false);
    midiChannelLabel->setColour (TextEditor::textColourId, Colours::black);
    midiChannelLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (midiChannelComboBox = new ComboBox (L"midiChannelComboBox"));
    midiChannelComboBox->setEditableText (false);
    midiChannelComboBox->setJustificationType (Justification::centredLeft);
    midiChannelComboBox->setTextWhenNothingSelected (L"Omni");
    midiChannelComboBox->setTextWhenNoChoicesAvailable (L"(no choices)");
    midiChannelComboBox->addItem (L"Omni", 1);
    midiChannelComboBox->addItem (L"1", 2);
    midiChannelComboBox->addItem (L"2", 3);
    midiChannelComboBox->addItem (L"3", 4);
    midiChannelComboBox->addItem (L"4", 5);
    midiChannelComboBox->addItem (L"5", 6);
    midiChannelComboBox->addItem (L"6", 7);
    midiChannelComboBox->addItem (L"7", 8);
    midiChannelComboBox->addItem (L"8", 9);
    midiChannelComboBox->addItem (L"9", 10);
    midiChannelComboBox->addItem (L"10", 11);
    midiChannelComboBox->addItem (L"11", 12);
    midiChannelComboBox->addItem (L"12", 13);
    midiChannelComboBox->addItem (L"13", 14);
    midiChannelComboBox->addItem (L"14", 15);
    midiChannelComboBox->addItem (L"15", 16);
    midiChannelComboBox->addItem (L"16", 17);
    midiChannelComboBox->addListener (this);


    //[UserPreSize]
	midiManager = manager;
	oscManager = manager2;
	pluginNode = plugin;
	mappings = maps;
	pluginField = field;

	midiLearnIndex = -1;

	mappingsList->updateContent();
	mappingsList->setOutlineThickness(1);
	mappingsList->setColour(ListBox::outlineColourId, Colour(0x60000000));
	mappingsList->setRowHeight(40);
	mappingsList->setMouseClickGrabsKeyboardFocus(true);
	mappingsList->setWantsKeyboardFocus(true);
	mappingsList->setColour(ListBox::backgroundColourId,
							ColourScheme::getInstance().colours[L"Dialog Inner Background"]);

	overrideMidiButton->setToggleState(!pluginField->getMidiEnabledForNode(pluginNode), false);

	{
		BypassableInstance *proc = dynamic_cast<BypassableInstance *>(pluginNode->getProcessor());

		if(proc)
		{
			oscMidiAddress->setText(oscManager->getMIDIProcessorAddress(proc));

			midiChannelComboBox->setSelectedId(proc->getMIDIChannel()+1);
		}
	}
	oscMidiAddress->addListener(this);
    //[/UserPreSize]

    setSize (738, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

MappingsDialog::~MappingsDialog()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	if(midiLearnIndex > -1)
		midiManager->unregisterMidiLearnCallback(this);

    //[/Destructor_pre]

    deleteAndZero (mappingsList);
    deleteAndZero (addMidiButton);
    deleteAndZero (addOscButton);
    deleteAndZero (deleteButton);
    deleteAndZero (overrideMidiButton);
    deleteAndZero (oscMidiAddressLabel);
    deleteAndZero (oscMidiAddress);
    deleteAndZero (oscHintLabel);
    deleteAndZero (midiChannelLabel);
    deleteAndZero (midiChannelComboBox);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MappingsDialog::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffeeece1));

    //[UserPaint] Add your own custom painting code here..

	g.fillAll(ColourScheme::getInstance().colours[L"Window Background"]);

    //[/UserPaint]
}

void MappingsDialog::resized()
{
    mappingsList->setBounds (8, 40, getWidth() - 16, getHeight() - 79);
    addMidiButton->setBounds (8, getHeight() - 31, 80, 24);
    addOscButton->setBounds (96, getHeight() - 31, 72, 24);
    deleteButton->setBounds (getWidth() - 70, getHeight() - 31, 62, 24);
    overrideMidiButton->setBounds (317 - ((282) / 2), getHeight() - 31, 282, 24);
    oscMidiAddressLabel->setBounds (8, 8, 136, 24);
    oscMidiAddress->setBounds (136, 8, getWidth() - 498, 24);
    oscHintLabel->setBounds (getWidth() - 362, 8, 360, 24);
    midiChannelLabel->setBounds (472, getHeight() - 31, 104, 24);
    midiChannelComboBox->setBounds (576, getHeight() - 31, 64, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MappingsDialog::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == addMidiButton)
    {
        //[UserButtonCode_addMidiButton] -- add your button handler code here..

		MidiMapping *mapping = new MidiMapping(midiManager,
											   pluginField->getFilterGraph(),
											   pluginNode->nodeId,
											   0,
											   0,
											   false,
											   midiChannelComboBox->getSelectedId()-1,
											   0.0f,
											   1.0f);
		midiManager->registerMapping(0, mapping);
		mappings.add(mapping);
		pluginField->addMapping(mapping);
		mappingsList->updateContent();
		repaint();

        //[/UserButtonCode_addMidiButton]
    }
    else if (buttonThatWasClicked == addOscButton)
    {
        //[UserButtonCode_addOscButton] -- add your button handler code here..

		OscMapping *mapping = new OscMapping(oscManager,
											 pluginField->getFilterGraph(),
											 pluginNode->nodeId,
											 0,
											 "",
											 0);
		oscManager->registerMapping("", mapping);
		mappings.add(mapping);
		pluginField->addMapping(mapping);
		mappingsList->updateContent();
		repaint();

        //[/UserButtonCode_addOscButton]
    }
    else if (buttonThatWasClicked == deleteButton)
    {
        //[UserButtonCode_deleteButton] -- add your button handler code here..

		int i;
		Array<int> mappingsToDelete;
		const SparseSet<int> selectedRows = mappingsList->getSelectedRows();

		for(i=0;i<selectedRows.size();++i)
		{
			jassert(selectedRows[i] < mappings.size());

			mappingsToDelete.add(selectedRows[i]);
		}

		mappingsToDelete.sort(comparator);
		for(i=(mappingsToDelete.size()-1);i>=0;--i)
		{
			pluginField->removeMapping(mappings[mappingsToDelete[i]]);
			mappings.remove(mappingsToDelete[i]);
		}

		mappingsList->updateContent();

        //[/UserButtonCode_deleteButton]
    }
    else if (buttonThatWasClicked == overrideMidiButton)
    {
        //[UserButtonCode_overrideMidiButton] -- add your button handler code here..

		pluginField->enableMidiForNode(pluginNode,
									   overrideMidiButton->getToggleState());

        //[/UserButtonCode_overrideMidiButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void MappingsDialog::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == midiChannelComboBox)
    {
        //[UserComboBoxCode_midiChannelComboBox] -- add your combo box handling code here..

		int i;
		int channel = midiChannelComboBox->getSelectedId()-1;
		BypassableInstance *proc = dynamic_cast<BypassableInstance *>(pluginNode->getProcessor());

		if(proc)
			proc->setMIDIChannel(channel);

		for(i=0;i<mappings.size();++i)
		{
			MidiMapping *tempMapping = dynamic_cast<MidiMapping *>(mappings[i]);

			if(tempMapping)
				tempMapping->setChannel(channel);
		}

        //[/UserComboBoxCode_midiChannelComboBox]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
int MappingsDialog::getNumRows()
{
	return mappings.size();
}

//------------------------------------------------------------------------------
void MappingsDialog::paintListBoxItem(int rowNumber,
									  Graphics &g,
									  int width,
									  int height,
									  bool rowIsSelected)
{
	Colour highlight = ColourScheme::getInstance().colours[L"List Selected Colour"];

	if(rowIsSelected)
	{
		ColourGradient basil(highlight.brighter(0.4f),
							 0.0f,
							 0.0f,
							 highlight.darker(0.125f),
							 0.0f,
							 (float)height,
							 false);

		g.setGradientFill(basil);

		g.fillAll();
	}
	else if(rowNumber%2)
		g.fillAll(Colour(0x10000000));
	/*else
		g.fillAll(Colour(0xFFFFFFFF));*/
}

//------------------------------------------------------------------------------
Component *MappingsDialog::refreshComponentForRow(int rowNumber,
												  bool isRowSelected,
												  Component *existingComponentToUpdate)
{
	int i;
	int numParams;
	int selectedParam;
	Component *retval = 0;
	MidiMapping *midiMapping = dynamic_cast<MidiMapping *>(mappings[rowNumber]);
	OscMapping *oscMapping = dynamic_cast<OscMapping *>(mappings[rowNumber]);

	if(existingComponentToUpdate)
		delete existingComponentToUpdate;

	if(midiMapping)
	{
		retval = new MappingEntryMidi(this,
									  rowNumber,
									  midiMapping->getCc(),
									  midiMapping->getLatched(),
									  midiMapping->getLowerBound(),
									  midiMapping->getUpperBound());

		//Fill out the parameters combo box.
		numParams = pluginNode->getProcessor()->getNumParameters();
		for(i=0;i<numParams;++i)
			((MappingEntryMidi *)retval)->addParameter(pluginNode->getProcessor()->getParameterName(i));
		((MappingEntryMidi *)retval)->addParameter("Bypass");

		selectedParam = midiMapping->getParameter();
		if(selectedParam == -1)
			selectedParam = numParams;
		((MappingEntryMidi *)retval)->selectParameter(selectedParam);
	}
	else if(oscMapping)
	{
		retval = new MappingEntryOsc(this,
									 rowNumber,
									 oscMapping->getAddress(),
									 oscMapping->getParameterIndex(),
									 oscManager->getReceivedAddresses());

		//Fill out the parameters combo box.
		numParams = pluginNode->getProcessor()->getNumParameters();
		for(i=0;i<numParams;++i)
			((MappingEntryOsc *)retval)->addParameter(pluginNode->getProcessor()->getParameterName(i));
		((MappingEntryOsc *)retval)->addParameter("Bypass");

		selectedParam = oscMapping->getParameter();
		if(selectedParam == -1)
			selectedParam = numParams;
		((MappingEntryOsc *)retval)->selectParameter(selectedParam);
	}

	return retval;
}



//------------------------------------------------------------------------------
void MappingsDialog::listBoxItemClicked(int row, const MouseEvent &e)
{
	mappingsList->selectRow(row, false, !e.mods.isCtrlDown());
}

//------------------------------------------------------------------------------
void MappingsDialog::backgroundClicked()
{
	mappingsList->deselectAllRows();
}

//------------------------------------------------------------------------------
void MappingsDialog::textEditorTextChanged(TextEditor &editor)
{
	BypassableInstance *proc = dynamic_cast<BypassableInstance *>(pluginNode->getProcessor());

	if(proc)
		oscManager->registerMIDIProcessor(editor.getText(), proc);
}

//------------------------------------------------------------------------------
void MappingsDialog::setParameter(int index, int val)
{
	if(val == pluginNode->getProcessor()->getNumParameters())
		val = -1;
	mappings[index]->setParameter(val);
}

//------------------------------------------------------------------------------
void MappingsDialog::setCc(int index, int val)
{
	MidiMapping *midiMapping = dynamic_cast<MidiMapping *>(mappings[index]);

	if(midiMapping)
		midiMapping->setCc(val);
	else
		jassertfalse;
}

//------------------------------------------------------------------------------
void MappingsDialog::setLatch(int index, bool val)
{
	MidiMapping *midiMapping = dynamic_cast<MidiMapping *>(mappings[index]);

	if(midiMapping)
		midiMapping->setLatched(val);
	else
		jassertfalse;
}

//------------------------------------------------------------------------------
void MappingsDialog::setLowerBound(int index, float val)
{
	MidiMapping *midiMapping = dynamic_cast<MidiMapping *>(mappings[index]);

	if(midiMapping)
		midiMapping->setLowerBound(val);
	else
		jassertfalse;
}

//------------------------------------------------------------------------------
void MappingsDialog::setUpperBound(int index, float val)
{
	MidiMapping *midiMapping = dynamic_cast<MidiMapping *>(mappings[index]);

	if(midiMapping)
		midiMapping->setUpperBound(val);
	else
		jassertfalse;
}

//------------------------------------------------------------------------------
void MappingsDialog::setAddress(int index, const String& address)
{
	OscMapping *oscMapping = dynamic_cast<OscMapping *>(mappings[index]);

	if(oscMapping)
		oscMapping->setAddress(address);
	else
		jassertfalse;
}

//------------------------------------------------------------------------------
void MappingsDialog::setOscParameter(int index, int val)
{
	OscMapping *oscMapping = dynamic_cast<OscMapping *>(mappings[index]);

	if(oscMapping)
		oscMapping->setParameterIndex(val);
	else
		jassertfalse;
}

//------------------------------------------------------------------------------
void MappingsDialog::activateMidiLearn(int index)
{
	if(index > -1)
	{
		midiLearnIndex = index;

		midiManager->registerMidiLearnCallback(this);
	}
}

//------------------------------------------------------------------------------
void MappingsDialog::deactivateMidiLearn(int index)
{
	midiLearnIndex = -1;

	midiManager->unregisterMidiLearnCallback(this);
}

//------------------------------------------------------------------------------
void MappingsDialog::midiCcReceived(int val)
{
	if(midiLearnIndex > -1)
	{
		MidiMapping *mapping = dynamic_cast<MidiMapping *>(mappings[midiLearnIndex]);

		if(mapping)
		{
			mapping->setCc(val);
			triggerAsyncUpdate();
		}

		midiLearnIndex = -1;
	}
}

//------------------------------------------------------------------------------
void MappingsDialog::handleAsyncUpdate()
{
	updateListBox();
}

//------------------------------------------------------------------------------
void MappingsDialog::updateListBox()
{
	mappingsList->updateContent();
}

//------------------------------------------------------------------------------
int MappingsDialog::MappingComparator::compareElements(int first, int second)
{
	int retval = 0;

	if(first < second)
		retval = -1;
	else if(first > second)
		retval = 1;

	return retval;
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MappingsDialog" componentName=""
                 parentClasses="public Component, public ListBoxModel, public TextEditor::Listener, public MidiMappingManager::MidiLearnCallback, public AsyncUpdater"
                 constructorParams="MidiMappingManager *manager, OscMappingManager *manager2, AudioProcessorGraph::Node *plugin, Array&lt;Mapping *&gt; maps, PluginField *field"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330000013" fixedSize="0" initialWidth="738"
                 initialHeight="400">
  <BACKGROUND backgroundColour="ffeeece1"/>
  <GENERICCOMPONENT name="mappingsList" id="b8ea3400fe029128" memberName="mappingsList"
                    virtualName="" explicitFocusOrder="0" pos="8 40 16M 79M" class="ListBox"
                    params="String::empty, this"/>
  <TEXTBUTTON name="addMidiButton" id="606b2b58df7c2434" memberName="addMidiButton"
              virtualName="" explicitFocusOrder="0" pos="8 31R 80 24" buttonText="add MIDI"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="addOscButton" id="30bc4009812abafc" memberName="addOscButton"
              virtualName="" explicitFocusOrder="0" pos="96 31R 72 24" buttonText="add OSC"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="deleteButton" id="142384607c3ebd41" memberName="deleteButton"
              virtualName="" explicitFocusOrder="0" pos="70R 31R 62 24" buttonText="remove"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TOGGLEBUTTON name="overrideMidiButton" id="3c38be6cc9087230" memberName="overrideMidiButton"
                virtualName="" explicitFocusOrder="0" pos="317c 31R 282 24" buttonText="Override plugin's default MIDI behaviour"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <LABEL name="oscMidiAddressLabel" id="f81d94aa2ba2808f" memberName="oscMidiAddressLabel"
         virtualName="" explicitFocusOrder="0" pos="8 8 136 24" edTextCol="ff000000"
         edBkgCol="0" labelText="OSC MIDI Address:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="oscMidiAddress" id="7887c734e442bb8a" memberName="oscMidiAddress"
              virtualName="" explicitFocusOrder="0" pos="136 8 498M 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <LABEL name="oscHintLabel" id="2cc88438ca611703" memberName="oscHintLabel"
         virtualName="" explicitFocusOrder="0" pos="362R 8 360 24" textCol="ff000000"
         edTextCol="ff000000" edBkgCol="0" labelText="(leave blank if you don't want to receive MIDI over OSC)"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15" bold="0" italic="0" justification="34"/>
  <LABEL name="midiChannelLabel" id="bca9a4e091ce9c35" memberName="midiChannelLabel"
         virtualName="" explicitFocusOrder="0" pos="472 31R 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="MIDI Channel:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="midiChannelComboBox" id="27f0e8e4a5a27c06" memberName="midiChannelComboBox"
            virtualName="" explicitFocusOrder="0" pos="576 31R 64 24" editable="0"
            layout="33" items="Omni&#10;1&#10;2&#10;3&#10;4&#10;5&#10;6&#10;7&#10;8&#10;9&#10;10&#10;11&#10;12&#10;13&#10;14&#10;15&#10;16"
            textWhenNonSelected="Omni" textWhenNoItems="(no choices)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
