/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  12 Nov 2011 4:51:55pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_COLOURSCHEMEEDITOR_COLOURSCHEMEEDITOR_4DDFE783__
#define __JUCER_HEADER_COLOURSCHEMEEDITOR_COLOURSCHEMEEDITOR_4DDFE783__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class ColourSchemeEditor  : public Component,
                            public ListBoxModel,
                            public Button::Listener,
                            public ChangeListener,
                            public ChangeBroadcaster,
                            public ComboBoxListener
{
public:
    //==============================================================================
    ColourSchemeEditor ();
    ~ColourSchemeEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Returns the number of colours which can be edited.
	int getNumRows();
	///	Draws the colour selector list.
	void paintListBoxItem(int rowNumber,
						  Graphics &g,
						  int width,
						  int height,
						  bool rowIsSelected);
	///	So we know when the user selects a new colour.
	void listBoxItemClicked(int row, const MouseEvent &e);

	///	Handles the user clicking the save or delete preset buttons.
	void buttonClicked(Button *button);

	///	Called when the colour changes.
	void changeListenerCallback(ChangeBroadcaster *source);

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	Helper method to load an SVG file from a binary chunk of data.
	Drawable *loadSVGFromMemory(const void *dataToInitialiseFrom, size_t sizeInBytes);

    //[/UserVariables]

    //==============================================================================
    ColourSelector* colourEditor;
    ListBox* colourSelector;
    ComboBox* presetSelector;
    DrawableButton* deleteButton;
    DrawableButton* saveButton;
    DrawableButton* newButton;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    ColourSchemeEditor (const ColourSchemeEditor&);
    const ColourSchemeEditor& operator= (const ColourSchemeEditor&);
};


#endif   // __JUCER_HEADER_COLOURSCHEMEEDITOR_COLOURSCHEMEEDITOR_4DDFE783__
