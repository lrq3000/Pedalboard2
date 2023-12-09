/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  16 Feb 2012 11:00:24am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_FILEPLAYERCONTROL_FILEPLAYERCONTROL_54B42392__
#define __JUCER_HEADER_FILEPLAYERCONTROL_FILEPLAYERCONTROL_54B42392__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplay.h"

class FilePlayerProcessor;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class FilePlayerControl  : public Component,
                           public FilenameComponentListener,
                           public Timer,
                           public ChangeListener,
                           public ButtonListener
{
public:
    //==========================================================================
    FilePlayerControl (FilePlayerProcessor *proc);
    ~FilePlayerControl();

    //==========================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Used to load a sound file.
	void filenameComponentChanged(FilenameComponent *filenameComp);
	///	Used to update the read position graphic.
	void timerCallback();
	///	Used to change the player's read position when the user clicks on the WaveformDisplay.
	void changeListenerCallback(ChangeBroadcaster *source);

	///	Changes the colour of the WaveformDisplay background.
	void setWaveformBackground(const Colour& col);

	///	Used to keep track of the last directory the user loaded a sound from.
	static File lastDir;

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



private:
	JUCE_LEAK_DETECTOR(FilePlayerControl)

    //[UserVariables]   -- You can add your own custom variables in this section.

	///	Our copy of the AudioProcessor pointer.
	FilePlayerProcessor *processor;

	///	The two drawables we use for the playButton.
	ScopedPointer<Drawable> playImage;
	ScopedPointer<Drawable> pauseImage;
	///	Whether the playPauseButton is currently displaying the play icon.
	bool playing;

    //[/UserVariables]

    //==========================================================================
    WaveformDisplay* fileDisplay;
    FilenameComponent* filename;
    ToggleButton* syncButton;
    ToggleButton* loopButton;
    DrawableButton* playPauseButton;
    DrawableButton* rtzButton;


    //==========================================================================
    // (prevent copy constructor and operator= being generated..)
    FilePlayerControl (const FilePlayerControl&);
    const FilePlayerControl& operator= (const FilePlayerControl&);
};


#endif   // __JUCER_HEADER_FILEPLAYERCONTROL_FILEPLAYERCONTROL_54B42392__
