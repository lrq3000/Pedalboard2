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

//[Headers] You can add your own extra header files here...

#include "LogFile.h"

//[/Headers]

#include "LogDisplay.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
LogDisplay::LogDisplay ()
    : logEditor (0),
      startStopButton (0),
      midiButton (0),
      oscButton (0),
      pedalboardButton (0),
      filterLabel (0)
{
    addAndMakeVisible (logEditor = new TextEditor (L"logEditor"));
    logEditor->setMultiLine (true);
    logEditor->setReturnKeyStartsNewLine (true);
    logEditor->setReadOnly (true);
    logEditor->setScrollbarsShown (true);
    logEditor->setCaretVisible (false);
    logEditor->setPopupMenuEnabled (true);
    logEditor->setText (String::empty);

    addAndMakeVisible (startStopButton = new TextButton (L"startStopButton"));
    startStopButton->setButtonText (L"Start Logging");
    startStopButton->addListener (this);

    addAndMakeVisible (midiButton = new ToggleButton (L"midiButton"));
    midiButton->setButtonText (L"MIDI");
    midiButton->addListener (this);
    midiButton->setToggleState (true, false);

    addAndMakeVisible (oscButton = new ToggleButton (L"oscButton"));
    oscButton->setButtonText (L"OSC");
    oscButton->addListener (this);
    oscButton->setToggleState (true, false);

    addAndMakeVisible (pedalboardButton = new ToggleButton (L"pedalboardButton"));
    pedalboardButton->setButtonText (L"Pedalboard");
    pedalboardButton->addListener (this);
    pedalboardButton->setToggleState (true, false);

    addAndMakeVisible (filterLabel = new Label (L"filterLabel",
                                                L"Filter:"));
    filterLabel->setFont (Font (15.0000f, Font::plain));
    filterLabel->setJustificationType (Justification::centredLeft);
    filterLabel->setEditable (false, false, false);
    filterLabel->setColour (TextEditor::textColourId, Colours::black);
    filterLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));


    //[UserPreSize]

	if(LogFile::getInstance().getIsLogging())
	{
		updateLog(true);

		startStopButton->setButtonText("Stop Logging");
	}

	LogFile::getInstance().addChangeListener(this);

    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

LogDisplay::~LogDisplay()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	LogFile::getInstance().removeChangeListener(this);

    //[/Destructor_pre]

    deleteAndZero (logEditor);
    deleteAndZero (startStopButton);
    deleteAndZero (midiButton);
    deleteAndZero (oscButton);
    deleteAndZero (pedalboardButton);
    deleteAndZero (filterLabel);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void LogDisplay::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffeeece1));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void LogDisplay::resized()
{
    logEditor->setBounds (8, 8, getWidth() - 16, getHeight() - 40);
    startStopButton->setBounds (8, getHeight() - 28, 150, 24);
    midiButton->setBounds (208, getHeight() - 28, 56, 24);
    oscButton->setBounds (264, getHeight() - 28, 56, 24);
    pedalboardButton->setBounds (320, getHeight() - 28, 96, 24);
    filterLabel->setBounds (164, getHeight() - 28, 48, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void LogDisplay::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == startStopButton)
    {
        //[UserButtonCode_startStopButton] -- add your button handler code here..

		if(LogFile::getInstance().getIsLogging())
		{
			//Stop logging.
			LogFile::getInstance().stop();
			logEditor->setText("");
			lastEvent = Time();

			startStopButton->setButtonText("Start Logging");
		}
		else
		{
			//Start logging.
			LogFile::getInstance().start();

			startStopButton->setButtonText("Stop Logging");
		}
        //[/UserButtonCode_startStopButton]
    }
    else if (buttonThatWasClicked == midiButton)
    {
        //[UserButtonCode_midiButton] -- add your button handler code here..

		updateLog(true);

        //[/UserButtonCode_midiButton]
    }
    else if (buttonThatWasClicked == oscButton)
    {
        //[UserButtonCode_oscButton] -- add your button handler code here..

		updateLog(true);

        //[/UserButtonCode_oscButton]
    }
    else if (buttonThatWasClicked == pedalboardButton)
    {
        //[UserButtonCode_pedalboardButton] -- add your button handler code here..

		updateLog(true);

        //[/UserButtonCode_pedalboardButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
void LogDisplay::changeListenerCallback(ChangeBroadcaster *source)
{
	if(source == &(LogFile::getInstance()))
		updateLog(false);
}

//------------------------------------------------------------------------------
void LogDisplay::updateLog(bool fromTheBeginning)
{
	StringArray tempArr;

	if(fromTheBeginning)
	{
		lastEvent = Time();
		logEditor->setText("");
	}
	//Make sure the caret's at the end of the string.
	else
		logEditor->moveCaretToEnd();

	//Fill out tempArr with the event types we're interested in.
	if(midiButton->getToggleState())
		tempArr.add("MIDI");
	if(oscButton->getToggleState())
		tempArr.add("OSC");
	if(pedalboardButton->getToggleState())
		tempArr.add("Pedalboard");

	//Append the new events to our editor.
	if(fromTheBeginning)
		logEditor->setText(LogFile::getInstance().getLogContents(tempArr, lastEvent));
	else
		logEditor->insertTextAtCaret(LogFile::getInstance().getLogContents(tempArr, lastEvent));

	//So that the next time this gets called getLogContents() doesn't return
	//the last line a second time.
	lastEvent += RelativeTime(0.001);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LogDisplay" componentName=""
                 parentClasses="public Component, public ChangeListener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330000013" fixedSize="0" initialWidth="600"
                 initialHeight="400">
  <BACKGROUND backgroundColour="ffeeece1"/>
  <TEXTEDITOR name="logEditor" id="6dbba2e3c0226e0" memberName="logEditor"
              virtualName="" explicitFocusOrder="0" pos="8 8 16M 40M" initialText=""
              multiline="1" retKeyStartsLine="1" readonly="1" scrollbars="1"
              caret="0" popupmenu="1"/>
  <TEXTBUTTON name="startStopButton" id="50e4fe174773b321" memberName="startStopButton"
              virtualName="" explicitFocusOrder="0" pos="8 28R 150 24" buttonText="Start Logging"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TOGGLEBUTTON name="midiButton" id="e128f4353765b317" memberName="midiButton"
                virtualName="" explicitFocusOrder="0" pos="208 28R 56 24" buttonText="MIDI"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="1"/>
  <TOGGLEBUTTON name="oscButton" id="6069ea976c3bf57c" memberName="oscButton"
                virtualName="" explicitFocusOrder="0" pos="264 28R 56 24" buttonText="OSC"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="1"/>
  <TOGGLEBUTTON name="pedalboardButton" id="8a2bcea281961e69" memberName="pedalboardButton"
                virtualName="" explicitFocusOrder="0" pos="320 28R 96 24" buttonText="Pedalboard"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="1"/>
  <LABEL name="filterLabel" id="c15fff90957c7ab0" memberName="filterLabel"
         virtualName="" explicitFocusOrder="0" pos="164 28R 48 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Filter:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
