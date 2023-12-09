/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  7 Nov 2011 11:39:47am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_AUDIORECORDERCONTROL_AUDIORECORDERCONTROL_504296E2__
#define __JUCER_HEADER_AUDIORECORDERCONTROL_AUDIORECORDERCONTROL_504296E2__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"

class RecorderProcessor;
class WaveformDisplayLite;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class AudioRecorderControl  : public Component,
                              public FilenameComponentListener,
                              public ChangeListener,
                              public ButtonListener
{
public:
    //==============================================================================
    AudioRecorderControl (RecorderProcessor *proc, AudioThumbnail& thumbnail);
    ~AudioRecorderControl();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Used to load a sound file.
	void filenameComponentChanged(FilenameComponent *filenameComp);
	///	Used to change the player's read position when the user clicks on the WaveformDisplay.
	void changeListenerCallback(ChangeBroadcaster *source);

	///	Changes the colour of the WaveformDisplay background.
	void setWaveformBackground(const Colour& col);

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	Our copy of the AudioProcessor pointer.
	RecorderProcessor *processor;

	///	The two drawables we use for the playButton.
	ScopedPointer<Drawable> recordImage;
	ScopedPointer<Drawable> stopImage;
	///	Whether the recordButton is currently displaying the record icon.
	bool recording;

    //[/UserVariables]

    //==============================================================================
    WaveformDisplayLite* fileDisplay;
    FilenameComponent* filename;
    ToggleButton* syncButton;
    DrawableButton* recordButton;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    AudioRecorderControl (const AudioRecorderControl&);
    const AudioRecorderControl& operator= (const AudioRecorderControl&);
};


#endif   // __JUCER_HEADER_AUDIORECORDERCONTROL_AUDIORECORDERCONTROL_504296E2__
