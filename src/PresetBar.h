/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  26 Jan 2012 10:24:15am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_PRESETBAR_PRESETBAR_30B11D9C__
#define __JUCER_HEADER_PRESETBAR_PRESETBAR_30B11D9C__

//[Headers]     -- You can add your own extra header files here --

#include "../JuceLibraryCode/JuceHeader.h"

class PluginComponent;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PresetBar  : public Component,
                   public Button::Listener,
                   public ComboBoxListener
{
public:
    //==========================================================================
    PresetBar (PluginComponent *comp);
    ~PresetBar();

    //==========================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	So we can do things when the user clicks one of the buttons.
	void buttonClicked(Button *button);

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);



private:
	JUCE_LEAK_DETECTOR(PresetBar)

    //[UserVariables]   -- You can add your own custom variables in this section.

	///	Helper method. Fills out presetsComboBox correctly.
	void fillOutComboBox();

	///	Helper method to load an SVG file from a binary chunk of data.
	Drawable *loadSVGFromMemory(const void *dataToInitialiseFrom, size_t sizeInBytes);

	///	The 'parent' PluginComponent.
	PluginComponent *component;

	///	Used to figure out which preset the user's just changed the name of.
	int lastComboBox;

    //[/UserVariables]

    //==========================================================================
    ComboBox* presetsComboBox;
    Label* presetsLabel;
    DrawableButton* importButton;
    DrawableButton* saveButton;
    Path internalPath1;


    //==========================================================================
    // (prevent copy constructor and operator= being generated..)
    PresetBar (const PresetBar&);
    const PresetBar& operator= (const PresetBar&);
};


#endif   // __JUCER_HEADER_PRESETBAR_PRESETBAR_30B11D9C__
