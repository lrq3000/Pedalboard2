/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  18 Oct 2012 11:38:06am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_MAPPINGENTRYOSC_MAPPINGENTRYOSC_7DF52EAC__
#define __JUCER_HEADER_MAPPINGENTRYOSC_MAPPINGENTRYOSC_7DF52EAC__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"

class MappingsDialog;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MappingEntryOsc  : public Component,
                         public ComboBoxListener,
                         public SliderListener
{
public:
    //==============================================================================
    MappingEntryOsc (MappingsDialog *dlg, int arrayIndex, const String& oscAddress, int oscParam, const StringArray& possibleAddresses);
    ~MappingEntryOsc();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Used to update the mapping's OSC address.
	void textEditorTextChanged(TextEditor &editor);
	///	Used to update the mapping's OSC address.
	void textEditorReturnKeyPressed(TextEditor &editor);
	///	Used to revert any changes the user made.
	void textEditorEscapeKeyPressed(TextEditor &editor);
	///	Used to update the mapping's OSC address.
	void textEditorFocusLost(TextEditor &editor);

	///	Fills out the parameter combo box.
	void addParameter(const String& param);
	///	Selects the currently-selected parameter in the combo box.
	void selectParameter(int index);

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void sliderValueChanged (Slider* sliderThatWasMoved);



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	The MappingsDialog which holds the mappings array for this plugin.
	MappingsDialog *mappingsDialog;
	///	The index of the mapping within its parent's Array.
	int index;

    //[/UserVariables]

    //==============================================================================
    ComboBox* paramComboBox;
    Label* addressLabel;
    ComboBox* addressEditor;
    Label* oscParamLabel;
    Slider* oscParamSlider;
    Path internalPath1;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    MappingEntryOsc (const MappingEntryOsc&);
    const MappingEntryOsc& operator= (const MappingEntryOsc&);
};


#endif   // __JUCER_HEADER_MAPPINGENTRYOSC_MAPPINGENTRYOSC_7DF52EAC__
