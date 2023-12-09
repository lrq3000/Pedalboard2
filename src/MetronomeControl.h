/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  16 Feb 2012 11:59:34am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_METRONOMECONTROL_METRONOMECONTROL_CEEEE7BB__
#define __JUCER_HEADER_METRONOMECONTROL_METRONOMECONTROL_CEEEE7BB__

//[Headers]     -- You can add your own extra header files here --

#include "../JuceLibraryCode/JuceHeader.h"

class MetronomeProcessor;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MetronomeControl  : public Component,
                          public FilenameComponentListener,
                          public ChangeListener,
                          public ButtonListener,
                          public LabelListener
{
public:
    //==========================================================================
    MetronomeControl (MetronomeProcessor *proc, bool editors);
    ~MetronomeControl();

    //==========================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Used to set the accent and click sounds.
	void filenameComponentChanged(FilenameComponent *fileComponentThatHasChanged);
	///	Used to update the play/pause button.
	void changeListenerCallback(ChangeBroadcaster *source);

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    void labelTextChanged (Label* labelThatHasChanged);



private:
	JUCE_LEAK_DETECTOR(MetronomeControl)

    //[UserVariables]   -- You can add your own custom variables in this section.

	///	Helper method to load an SVG file from a binary chunk of data.
	Drawable *loadSVGFromMemory(const void *dataToInitialiseFrom, size_t sizeInBytes);

	///	Our copy of the MetronomeProcessor.
	MetronomeProcessor *processor;

	///	The two drawables we use for the playButton.
	ScopedPointer<Drawable> playImage;
	ScopedPointer<Drawable> pauseImage;

	///	Whether or not we're currently playing.
	bool playing;

	///	Whether we should display the file editors or not.
	bool showFileEditors;

    //[/UserVariables]

    //==========================================================================
    ToggleButton* syncButton;
    DrawableButton* playPauseButton;
    FilenameComponent* accentFile;
    Label* accentLabel;
    FilenameComponent* clickFile;
    Label* clickLabel;
    Label* numeratorLabel;
    Label* denominatorLabel;
    Label* separatorLabel;


    //==========================================================================
    // (prevent copy constructor and operator= being generated..)
    MetronomeControl (const MetronomeControl&);
    const MetronomeControl& operator= (const MetronomeControl&);
};


#endif   // __JUCER_HEADER_METRONOMECONTROL_METRONOMECONTROL_CEEEE7BB__
