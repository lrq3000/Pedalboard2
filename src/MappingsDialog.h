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

#ifndef __JUCER_HEADER_MAPPINGSDIALOG_MAPPINGSDIALOG_CC1A73F8__
#define __JUCER_HEADER_MAPPINGSDIALOG_MAPPINGSDIALOG_CC1A73F8__

//[Headers]     -- You can add your own extra header files here --

#include "MidiMappingManager.h"

#include "../JuceLibraryCode/JuceHeader.h"

class OscMappingManager;
class PluginField;
class Mapping;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MappingsDialog  : public Component,
                        public ListBoxModel,
                        public TextEditor::Listener,
                        public MidiMappingManager::MidiLearnCallback,
                        public AsyncUpdater,
                        public ButtonListener,
                        public ComboBoxListener
{
public:
    //==============================================================================
    MappingsDialog (MidiMappingManager *manager, OscMappingManager *manager2, AudioProcessorGraph::Node *plugin, Array<Mapping *> maps, PluginField *field);
    ~MappingsDialog();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Returns the number of active mappings.
	int getNumRows();
	///	Draws a row.
	void paintListBoxItem(int rowNumber, Graphics &g, int width, int height, bool rowIsSelected);
	///	Returns the component for a single row.
	Component *refreshComponentForRow(int rowNumber, bool isRowSelected, Component *existingComponentToUpdate);
	///	So the user can select rows.
	void listBoxItemClicked(int row, const MouseEvent &e);
	///	So the user can deselect rows.
	void backgroundClicked();

	///	So the OscMappingManager can get updated with the correct MIDI over OSC address.
	void textEditorTextChanged(TextEditor &editor);

	///	Sets the indexed mapping's parameter.
	void setParameter(int index, int val);
	///	Sets the indexed mapping's CC.
	void setCc(int index, int val);
	///	Sets the indexed mapping's latch value.
	void setLatch(int index, bool val);
	///	Sets the indexed mapping's lower bound.
	void setLowerBound(int index, float val);
	///	Sets the indexed mapping's upper bound.
	void setUpperBound(int index, float val);
	///	Sets the indexed mapping's OSC address.
	void setAddress(int index, const String& address);
	///	Sets the indexed mapping's OSC parameter index.
	void setOscParameter(int index, int val);

	///	Activates MIDI learn for the indexed mapping.
	void activateMidiLearn(int index);
	///	Deactivates MIDI learn for the indexed mapping.
	void deactivateMidiLearn(int index);
	///	The method which gets called when a MIDI learn CC is received.
	void midiCcReceived(int val);
	///	Used to refresh the mappings list when a MIDI learn CC is received.
	void handleAsyncUpdate();

	///	Updates the listbox contents.
	void updateListBox();

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	For sorting the results of mappingsList->getSelectedRows().
	struct MappingComparator
	{
		///	For sorting the results of mappingsList->getSelectedRows().
		int compareElements(int first, int second);
	};

	///	For sorting the results of mappingsList->getSelectedRows().
	MappingComparator comparator;

	///	The MidiMappingsManager for any MidiMappings.
	MidiMappingManager *midiManager;
	///	The OscMappingsManager for any OscMappings.
	OscMappingManager *oscManager;
	///	The plugin this dialog refers to.
	AudioProcessorGraph::Node *pluginNode;
	///	The mappings this dialog displays.
	Array<Mapping *> mappings;
	///	The PluginField to apply any mapping changes to.
	PluginField *pluginField;

	///	If we're doing a midi learn, this will hold the index to associated mapping.
	/*!
		Otherwise it will be -1.
	 */
	int midiLearnIndex;

    //[/UserVariables]

    //==============================================================================
    ListBox* mappingsList;
    TextButton* addMidiButton;
    TextButton* addOscButton;
    TextButton* deleteButton;
    ToggleButton* overrideMidiButton;
    Label* oscMidiAddressLabel;
    TextEditor* oscMidiAddress;
    Label* oscHintLabel;
    Label* midiChannelLabel;
    ComboBox* midiChannelComboBox;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    MappingsDialog (const MappingsDialog&);
    const MappingsDialog& operator= (const MappingsDialog&);
};


#endif   // __JUCER_HEADER_MAPPINGSDIALOG_MAPPINGSDIALOG_CC1A73F8__
