/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  17 Jan 2013 11:13:47am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_MAPPINGENTRYMIDI_MAPPINGENTRYMIDI_3DC59256__
#define __JUCER_HEADER_MAPPINGENTRYMIDI_MAPPINGENTRYMIDI_3DC59256__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "MappingSlider.h"

class MappingsDialog;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MappingEntryMidi  : public Component,
                          public ComboBoxListener,
                          public ButtonListener,
                          public MappingSliderListener
{
public:
    //==============================================================================
    MappingEntryMidi (MappingsDialog *dlg, int arrayIndex, int cc, bool latch, float lowerBound, float upperBound);
    ~MappingEntryMidi();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Fills out the parameter combo box.
	void addParameter(const String& param);
	///	Selects the currently-selected parameter in the combo box.
	void selectParameter(int index);

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void buttonClicked (Button* buttonThatWasClicked);
    void sliderValueChanged (MappingSlider* sliderThatWasMoved);



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	The MappingsDialog which holds the mappings array for this plugin.
	MappingsDialog *mappingsDialog;
	///	The index of the mapping within its parent's Array.
	int index;

	///	True if we're currently in MIDI learn mode.
	bool midiLearn;

    //[/UserVariables]

    //==============================================================================
    ComboBox* ccComboBox;
    ToggleButton* latchButton;
    MappingSlider* slider;
    Label* rangeLabel;
    ComboBox* paramComboBox;
    Path internalPath1;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    MappingEntryMidi (const MappingEntryMidi&);
    const MappingEntryMidi& operator= (const MappingEntryMidi&);
};


#endif   // __JUCER_HEADER_MAPPINGENTRYMIDI_MAPPINGENTRYMIDI_3DC59256__
