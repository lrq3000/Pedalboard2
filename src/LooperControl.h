/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  20 Oct 2012 7:09:23pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_LOOPERCONTROL_LOOPERCONTROL_995E972B__
#define __JUCER_HEADER_LOOPERCONTROL_LOOPERCONTROL_995E972B__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplay.h"

class LooperProcessor;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class LooperControl  : public Component,
                       public FilenameComponentListener,
                       public Timer,
                       public ChangeListener,
                       public ButtonListener
{
public:
    //==============================================================================
    LooperControl (LooperProcessor *proc, AudioThumbnail *thumbnail);
    ~LooperControl();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Used to load a sound file.
	void filenameComponentChanged(FilenameComponent *filenameComp);
	///	Used to update the read position graphic.
	void timerCallback();
	///	Used to change the player's read position when the user clicks on the WaveformDisplay.
	void changeListenerCallback(ChangeBroadcaster *source);

	///	Changes the colour of the WaveformDisplay background.
	void setWaveformBackground(const Colour& col);
	///	Clears the waveform display.
	void clearDisplay();

	///	Used to keep track of the last directory the user loaded a sound from.
	static File lastDir;

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	Our copy of the AudioProcessor pointer.
	LooperProcessor *processor;

	///	The two drawables we use for the playButton.
	ScopedPointer<Drawable> playImage;
	ScopedPointer<Drawable> pauseImage;
	///	Whether the playPauseButton is currently displaying the play icon.
	bool playing;

	///	The two drawables we use for the recordButton.
	ScopedPointer<Drawable> recordImage;
	ScopedPointer<Drawable> stopImage;
	///	Whether the recordButton is currently displaying the record icon.
	bool recording;

    //[/UserVariables]

    //==============================================================================
    WaveformDisplay* fileDisplay;
    FilenameComponent* filename;
    ToggleButton* syncButton;
    ToggleButton* stopAfterBarButton;
    DrawableButton* playPauseButton;
    DrawableButton* rtzButton;
    DrawableButton* recordButton;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    LooperControl (const LooperControl&);
    const LooperControl& operator= (const LooperControl&);
};


#endif   // __JUCER_HEADER_LOOPERCONTROL_LOOPERCONTROL_995E972B__
