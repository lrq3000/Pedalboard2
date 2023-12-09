/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  6 Feb 2013 8:46:23pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_LOGDISPLAY_LOGDISPLAY_FCAF45DE__
#define __JUCER_HEADER_LOGDISPLAY_LOGDISPLAY_FCAF45DE__

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
class LogDisplay  : public Component,
                    public ChangeListener,
                    public ButtonListener
{
public:
    //==============================================================================
    LogDisplay ();
    ~LogDisplay();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Called when the LogFile is updated.
	void changeListenerCallback(ChangeBroadcaster *source);

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	Helper method. Updates logEditor.
	void updateLog(bool fromTheBeginning);

	///	The timestamp of the most recent log event.
	Time lastEvent;

    //[/UserVariables]

    //==============================================================================
    TextEditor* logEditor;
    TextButton* startStopButton;
    ToggleButton* midiButton;
    ToggleButton* oscButton;
    ToggleButton* pedalboardButton;
    Label* filterLabel;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    LogDisplay (const LogDisplay&);
    const LogDisplay& operator= (const LogDisplay&);
};


#endif   // __JUCER_HEADER_LOGDISPLAY_LOGDISPLAY_FCAF45DE__
