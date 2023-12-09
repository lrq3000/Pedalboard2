/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  19 Oct 2012 11:10:47am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_PREFERENCESDIALOG_PREFERENCESDIALOG_3C07F714__
#define __JUCER_HEADER_PREFERENCESDIALOG_PREFERENCESDIALOG_3C07F714__

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"

class MainPanel;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PreferencesDialog  : public Component,
                           public TextEditor::Listener,
                           public ButtonListener
{
public:
    //==============================================================================
    PreferencesDialog (MainPanel *panel, const String& port, const String& multicastAddress);
    ~PreferencesDialog();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	///	Not used.
	void textEditorTextChanged(TextEditor &editor) {};
	///	Used to update the mapping's OSC address.
	void textEditorReturnKeyPressed(TextEditor &editor);
	///	Used to revert any changes the user made.
	void textEditorEscapeKeyPressed(TextEditor &editor);
	///	Used to update the mapping's OSC address.
	void textEditorFocusLost(TextEditor &editor);

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.

	///	The app's MainPanel.
	MainPanel *mainPanel;
	///	The current port to listen on.
	String currentPort;
	///	The current multicast address to listen on.
	String currentMulticast;

    //[/UserVariables]

    //==============================================================================
    Label* oscPortLabel;
    TextEditor* oscPortEditor;
    Label* oscLabel;
    Label* oscMulticastLabel;
    TextEditor* oscMulticastEditor;
    Label* multicastHintLabel;
    Label* ioOptionsLabel;
    ToggleButton* audioInputButton;
    ToggleButton* midiInputButton;
    ToggleButton* oscInputButton;
    Label* otherLabel;
    ToggleButton* mappingsWindowButton;
    ToggleButton* loopPatchesButton;
    ToggleButton* windowsOnTopButton;
    ToggleButton* ignorePinNamesButton;
    Label* midiLabel;
    ToggleButton* midiProgramChangeButton;
    ToggleButton* mmcTransportButton;
    ToggleButton* useTrayIconButton;
    ToggleButton* startInTrayButton;
    ToggleButton* fixedSizeButton;
    ToggleButton* pdlAudioSettingsButton;


    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    PreferencesDialog (const PreferencesDialog&);
    const PreferencesDialog& operator= (const PreferencesDialog&);
};


#endif   // __JUCER_HEADER_PREFERENCESDIALOG_PREFERENCESDIALOG_3C07F714__
