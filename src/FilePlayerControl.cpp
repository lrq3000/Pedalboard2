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

//[Headers] You can add your own extra header files here...

#include "PedalboardProcessors.h"
#include "JuceHelperStuff.h"
#include "ColourScheme.h"
#include "Vectors.h"

//[/Headers]

#include "FilePlayerControl.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

File FilePlayerControl::lastDir(File::getSpecialLocation(File::userHomeDirectory));

//[/MiscUserDefs]

//==============================================================================
FilePlayerControl::FilePlayerControl (FilePlayerProcessor *proc)
    : processor(proc),
      fileDisplay (0),
      filename (0),
      syncButton (0),
      loopButton (0),
      playPauseButton (0),
      rtzButton (0)
{
    addAndMakeVisible (fileDisplay = new WaveformDisplay());
    fileDisplay->setName (L"fileDisplay");

    addAndMakeVisible (filename = new FilenameComponent ("filename", File::nonexistent, true, false, false, "*.wav;*.aif", "", "<no file loaded>"));
    filename->setName (L"filename");

    addAndMakeVisible (syncButton = new ToggleButton (L"syncButton"));
    syncButton->setTooltip (L"Sync file playback to the main transport");
    syncButton->setButtonText (L"Sync to main transport");
    syncButton->addListener (this);

    addAndMakeVisible (loopButton = new ToggleButton (L"loopButton"));
    loopButton->setTooltip (L"Loop this file");
    loopButton->setButtonText (L"Loop");
    loopButton->addListener (this);

    addAndMakeVisible (playPauseButton = new DrawableButton ("playPauseButton", DrawableButton::ImageOnButtonBackground));
    playPauseButton->setName (L"playPauseButton");

    addAndMakeVisible (rtzButton = new DrawableButton ("rtzButton", DrawableButton::ImageOnButtonBackground));
    rtzButton->setName (L"rtzButton");


    //[UserPreSize]
	ScopedPointer<Drawable> rtzImage(JuceHelperStuff::loadSVGFromMemory(Vectors::rtzbutton_svg,
																	    Vectors::rtzbutton_svgSize));

	playing = false;

	playImage = JuceHelperStuff::loadSVGFromMemory(Vectors::playbutton_svg,
												   Vectors::playbutton_svgSize);
	pauseImage = JuceHelperStuff::loadSVGFromMemory(Vectors::pausebutton_svg,
												    Vectors::pausebutton_svgSize);
	playPauseButton->setImages(playImage);
	playPauseButton->setColour(DrawableButton::backgroundColourId,
							   ColourScheme::getInstance().colours[L"Button Colour"]);
	playPauseButton->setColour(DrawableButton::backgroundOnColourId,
							   ColourScheme::getInstance().colours[L"Button Colour"]);
	playPauseButton->addListener(this);
	playPauseButton->setTooltip(L"Play/pause audio file");

	//Used to make sure we're playing if the processor is already playing.
	changeListenerCallback(processor);

	rtzButton->setImages(rtzImage);
	rtzButton->setColour(DrawableButton::backgroundColourId,
					     ColourScheme::getInstance().colours[L"Button Colour"]);
	rtzButton->setColour(DrawableButton::backgroundOnColourId,
					     ColourScheme::getInstance().colours[L"Button Colour"]);
	rtzButton->addListener(this);
	rtzButton->setTooltip(L"Return to the start of the audio file");

	const File& soundFile = processor->getFile();
	if(soundFile != File::nonexistent)
	{
		filename->setCurrentFile(soundFile, true, dontSendNotification);
		fileDisplay->setFile(soundFile);
		fileDisplay->setReadPointer((float)processor->getReadPosition());
	}
	else
		filename->setDefaultBrowseTarget(lastDir);

	loopButton->setToggleState(processor->getParameter(FilePlayerProcessor::Looping) > 0.5f,
							   false);
	syncButton->setToggleState(processor->getParameter(FilePlayerProcessor::SyncToMainTransport) > 0.5f,
							   false);

	filename->addListener(this);
	fileDisplay->addChangeListener(this);
	processor->addChangeListener(this);

	startTimer(60);

    //[/UserPreSize]

    setSize (300, 100);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

FilePlayerControl::~FilePlayerControl()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	processor->removeChangeListener(this);

    //[/Destructor_pre]

    deleteAndZero (fileDisplay);
    deleteAndZero (filename);
    deleteAndZero (syncButton);
    deleteAndZero (loopButton);
    deleteAndZero (playPauseButton);
    deleteAndZero (rtzButton);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void FilePlayerControl::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void FilePlayerControl::resized()
{
    fileDisplay->setBounds (0, 28, getWidth() - 2, getHeight() - 48);
    filename->setBounds (0, 0, getWidth() - 58, 24);
    syncButton->setBounds (0, getHeight() - 23, 168, 24);
    loopButton->setBounds (176, getHeight() - 23, 56, 24);
    playPauseButton->setBounds (getWidth() - 54, 0, 24, 24);
    rtzButton->setBounds (getWidth() - 26, 0, 24, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void FilePlayerControl::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == syncButton)
    {
        //[UserButtonCode_syncButton] -- add your button handler code here..

		bool val = syncButton->getToggleState();

		processor->setParameter(FilePlayerProcessor::SyncToMainTransport,
								val ? 1.0f : 0.0f);

        //[/UserButtonCode_syncButton]
    }
    else if (buttonThatWasClicked == loopButton)
    {
        //[UserButtonCode_loopButton] -- add your button handler code here..

		bool val = loopButton->getToggleState();

		processor->setParameter(FilePlayerProcessor::Looping,
								val ? 1.0f : 0.0f);

        //[/UserButtonCode_loopButton]
    }

    //[UserbuttonClicked_Post]
	else if(buttonThatWasClicked == playPauseButton)
	{
		if(!playing)
			playPauseButton->setImages(pauseImage);
		else
			playPauseButton->setImages(playImage);
		playing = !playing;
		processor->setParameter(FilePlayerProcessor::Play, 1.0f);
	}
	else if(buttonThatWasClicked == rtzButton)
	{
		processor->setParameter(FilePlayerProcessor::ReturnToZero, 1.0f);
		fileDisplay->setReadPointer(0.0f);
	}
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
void FilePlayerControl::filenameComponentChanged(FilenameComponent *filenameComp)
{
	File phil = filenameComp->getCurrentFile();
	processor->setFile(phil);
	fileDisplay->setFile(phil);
	lastDir = phil.getParentDirectory();
}

//------------------------------------------------------------------------------
void FilePlayerControl::timerCallback()
{
	if(playing)
		fileDisplay->setReadPointer((float)processor->getReadPosition());
}

//------------------------------------------------------------------------------
void FilePlayerControl::changeListenerCallback(ChangeBroadcaster *source)
{
	if(source == fileDisplay)
	{
		processor->setParameter(FilePlayerProcessor::ReadPosition,
								fileDisplay->getReadPointer());
		fileDisplay->setReadPointer((float)processor->getReadPosition());
	}
	else if(source == processor)
	{
		if(processor->isPlaying())
		{
			playPauseButton->setImages(pauseImage);
			playing = true;
		}
		else
		{
			playPauseButton->setImages(playImage);
			playing = false;
		}
		fileDisplay->setReadPointer((float)processor->getReadPosition());
		loopButton->setToggleState(processor->getParameter(FilePlayerProcessor::Looping) > 0.5f, false);
		syncButton->setToggleState(processor->getParameter(FilePlayerProcessor::SyncToMainTransport) > 0.5f, false);
	}
}

//------------------------------------------------------------------------------
void FilePlayerControl::setWaveformBackground(const Colour& col)
{
	fileDisplay->setBackgroundColour(col);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="FilePlayerControl" componentName=""
                 parentClasses="public Component, public FilenameComponentListener, public Timer, public ChangeListener"
                 constructorParams="FilePlayerProcessor *proc" variableInitialisers="processor(proc)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="0" initialWidth="300" initialHeight="100">
  <BACKGROUND backgroundColour="eeece1"/>
  <GENERICCOMPONENT name="fileDisplay" id="ca679484fd8a4509" memberName="fileDisplay"
                    virtualName="" explicitFocusOrder="0" pos="0 28 2M 48M" class="WaveformDisplay"
                    params=""/>
  <GENERICCOMPONENT name="filename" id="f1e80797e31ff742" memberName="filename" virtualName=""
                    explicitFocusOrder="0" pos="0 0 58M 24" class="FilenameComponent"
                    params="&quot;filename&quot;, File::nonexistent, true, false, false, &quot;*.wav;*.aif&quot;, &quot;&quot;, &quot;&lt;no file loaded&gt;&quot;"/>
  <TOGGLEBUTTON name="syncButton" id="b0788f5602fb3d7f" memberName="syncButton"
                virtualName="" explicitFocusOrder="0" pos="0 23R 168 24" tooltip="Sync file playback to the main transport"
                buttonText="Sync to main transport" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="loopButton" id="eb49769414af66b2" memberName="loopButton"
                virtualName="" explicitFocusOrder="0" pos="176 23R 56 24" tooltip="Loop this file"
                buttonText="Loop" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <GENERICCOMPONENT name="playPauseButton" id="da66be207abe8144" memberName="playPauseButton"
                    virtualName="" explicitFocusOrder="0" pos="54R 0 24 24" class="DrawableButton"
                    params="&quot;playPauseButton&quot;, DrawableButton::ImageOnButtonBackground"/>
  <GENERICCOMPONENT name="rtzButton" id="8087375604e93800" memberName="rtzButton"
                    virtualName="" explicitFocusOrder="0" pos="26R 0 24 24" class="DrawableButton"
                    params="&quot;rtzButton&quot;, DrawableButton::ImageOnButtonBackground"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
