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

//[Headers] You can add your own extra header files here...

#include "PedalboardProcessors.h"
#include "JuceHelperStuff.h"
#include "ColourScheme.h"
#include "Vectors.h"

//[/Headers]

#include "LooperControl.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

File LooperControl::lastDir(File::getSpecialLocation(File::userHomeDirectory));

//[/MiscUserDefs]

//==============================================================================
LooperControl::LooperControl (LooperProcessor *proc, AudioThumbnail *thumbnail)
    : processor(proc),
      fileDisplay (0),
      filename (0),
      syncButton (0),
      stopAfterBarButton (0),
      playPauseButton (0),
      rtzButton (0),
      recordButton (0)
{
    addAndMakeVisible (fileDisplay = new WaveformDisplay (thumbnail, false));
    fileDisplay->setName (L"fileDisplay");

    addAndMakeVisible (filename = new FilenameComponent ("filename", File::nonexistent, true, false, true, "*.wav;*.aif", "", "<no file loaded>"));
    filename->setName (L"filename");

    addAndMakeVisible (syncButton = new ToggleButton (L"syncButton"));
    syncButton->setTooltip (L"Sync file playback to the main transport");
    syncButton->setButtonText (L"Sync to main transport");
    syncButton->addListener (this);

    addAndMakeVisible (stopAfterBarButton = new ToggleButton (L"stopAfterBarButton"));
    stopAfterBarButton->setTooltip (L"Stop recording after a bar has elapsed.");
    stopAfterBarButton->setButtonText (L"Stop after bar");
    stopAfterBarButton->addListener (this);

    addAndMakeVisible (playPauseButton = new DrawableButton ("playPauseButton", DrawableButton::ImageOnButtonBackground));
    playPauseButton->setName (L"playPauseButton");

    addAndMakeVisible (rtzButton = new DrawableButton ("rtzButton", DrawableButton::ImageOnButtonBackground));
    rtzButton->setName (L"rtzButton");

    addAndMakeVisible (recordButton = new DrawableButton ("recordButton", DrawableButton::ImageOnButtonBackground));
    recordButton->setName (L"recordButton");


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

	recording = false;

	recordImage = JuceHelperStuff::loadSVGFromMemory(Vectors::recordbutton_svg,
													 Vectors::recordbutton_svgSize);
	stopImage = JuceHelperStuff::loadSVGFromMemory(Vectors::stopbutton_svg,
												   Vectors::stopbutton_svgSize);
	recordButton->setImages(recordImage);
	recordButton->setColour(DrawableButton::backgroundColourId,
						    ColourScheme::getInstance().colours[L"Button Colour"]);
	recordButton->setColour(DrawableButton::backgroundOnColourId,
						    ColourScheme::getInstance().colours[L"Button Colour"]);
	recordButton->addListener(this);
	recordButton->setTooltip(L"Record/stop recording a loop");

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

	syncButton->setToggleState(processor->getParameter(LooperProcessor::SyncToMainTransport) > 0.5f,
							   false);
	stopAfterBarButton->setToggleState(processor->getParameter(LooperProcessor::StopAfterBar) > 0.5f,
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

LooperControl::~LooperControl()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	processor->removeChangeListener(this);

    //[/Destructor_pre]

    deleteAndZero (fileDisplay);
    deleteAndZero (filename);
    deleteAndZero (syncButton);
    deleteAndZero (stopAfterBarButton);
    deleteAndZero (playPauseButton);
    deleteAndZero (rtzButton);
    deleteAndZero (recordButton);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void LooperControl::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void LooperControl::resized()
{
    fileDisplay->setBounds (0, 28, getWidth() - 2, getHeight() - 48);
    filename->setBounds (0, 0, getWidth() - 84, 24);
    syncButton->setBounds (0, getHeight() - 23, 168, 24);
    stopAfterBarButton->setBounds (176, getHeight() - 23, 112, 24);
    playPauseButton->setBounds (getWidth() - 80, 0, 24, 24);
    rtzButton->setBounds (getWidth() - 52, 0, 24, 24);
    recordButton->setBounds (getWidth() - 24, 0, 24, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void LooperControl::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == syncButton)
    {
        //[UserButtonCode_syncButton] -- add your button handler code here..

		bool val = syncButton->getToggleState();

		processor->setParameter(LooperProcessor::SyncToMainTransport,
								val ? 1.0f : 0.0f);

        //[/UserButtonCode_syncButton]
    }
    else if (buttonThatWasClicked == stopAfterBarButton)
    {
        //[UserButtonCode_stopAfterBarButton] -- add your button handler code here..

		bool val = stopAfterBarButton->getToggleState();

		processor->setParameter(LooperProcessor::StopAfterBar,
								val ? 1.0f : 0.0f);

        //[/UserButtonCode_stopAfterBarButton]
    }

    //[UserbuttonClicked_Post]
	else if(buttonThatWasClicked == playPauseButton)
	{
		if(!playing)
			playPauseButton->setImages(pauseImage);
		else
			playPauseButton->setImages(playImage);
		playing = !playing;
		processor->setParameter(LooperProcessor::Play, 1.0f);
	}
	else if(buttonThatWasClicked == rtzButton)
	{
		processor->setParameter(LooperProcessor::ReturnToZero, 1.0f);
		fileDisplay->setReadPointer(0.0f);
	}
	else if(buttonThatWasClicked == recordButton)
	{
		recording = !recording;

		if(recording)
		{
			//fileDisplay->setFile(File::nonexistent);
			if(playing)
			{
				playPauseButton->setImages(playImage);
				playing = false;
				processor->setParameter(LooperProcessor::Play, 1.0f);
			}

			//processor->setFile(filename->getCurrentFile());
		}

		processor->setParameter(LooperProcessor::Record, 1.0f);
	}

	//So LooperEditor gets updated too.
	processor->sendChangeMessage();

    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
void LooperControl::filenameComponentChanged(FilenameComponent *filenameComp)
{
	File phil = filenameComp->getCurrentFile();
	fileDisplay->setFile(phil);
	processor->setFile(phil);
	lastDir = phil.getParentDirectory();

	//So LooperEditor gets updated too.
	processor->sendChangeMessage();
}

//------------------------------------------------------------------------------
void LooperControl::timerCallback()
{
	if(playing)
		fileDisplay->setReadPointer((float)processor->getReadPosition());
}

//------------------------------------------------------------------------------
void LooperControl::changeListenerCallback(ChangeBroadcaster *source)
{
	if(source == fileDisplay)
	{
		processor->setParameter(LooperProcessor::ReadPosition,
								fileDisplay->getReadPointer());
		fileDisplay->setReadPointer((float)processor->getReadPosition());
	}
	else if(source == processor)
	{
		if(processor->getNewFileLoaded())
			fileDisplay->setFile(processor->getFile());

		if(filename->getCurrentFile() != processor->getFile())
			filename->setCurrentFile(processor->getFile(), true, dontSendNotification);

		if(processor->isRecording())
		{
			playPauseButton->setImages(playImage);
			playPauseButton->setEnabled(false);

			if(!recording)
				fileDisplay->setFile(File::nonexistent);
			recordButton->setImages(stopImage);
			recording = true;
		}
		else
		{
			playPauseButton->setEnabled(true);
			recordButton->setImages(recordImage);
			recording = false;

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
		}
		fileDisplay->setReadPointer((float)processor->getReadPosition());
		syncButton->setToggleState(processor->getParameter(LooperProcessor::SyncToMainTransport) > 0.5f, false);
		stopAfterBarButton->setToggleState(processor->getParameter(LooperProcessor::StopAfterBar) > 0.5f, false);
	}
}

//------------------------------------------------------------------------------
void LooperControl::setWaveformBackground(const Colour& col)
{
	fileDisplay->setBackgroundColour(col);
}

//------------------------------------------------------------------------------
void LooperControl::clearDisplay()
{
	fileDisplay->setFile(File::nonexistent);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LooperControl" componentName=""
                 parentClasses="public Component, public FilenameComponentListener, public Timer, public ChangeListener"
                 constructorParams="LooperProcessor *proc, AudioThumbnail *thumbnail"
                 variableInitialisers="processor(proc)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330000013" fixedSize="0" initialWidth="300"
                 initialHeight="100">
  <BACKGROUND backgroundColour="eeece1"/>
  <GENERICCOMPONENT name="fileDisplay" id="ca679484fd8a4509" memberName="fileDisplay"
                    virtualName="" explicitFocusOrder="0" pos="0 28 2M 48M" class="WaveformDisplay"
                    params="thumbnail, false"/>
  <GENERICCOMPONENT name="filename" id="f1e80797e31ff742" memberName="filename" virtualName=""
                    explicitFocusOrder="0" pos="0 0 84M 24" class="FilenameComponent"
                    params="&quot;filename&quot;, File::nonexistent, true, false, true, &quot;*.wav;*.aif&quot;, &quot;&quot;, &quot;&lt;no file loaded&gt;&quot;"/>
  <TOGGLEBUTTON name="syncButton" id="b0788f5602fb3d7f" memberName="syncButton"
                virtualName="" explicitFocusOrder="0" pos="0 23R 168 24" tooltip="Sync file playback to the main transport"
                buttonText="Sync to main transport" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="stopAfterBarButton" id="eb49769414af66b2" memberName="stopAfterBarButton"
                virtualName="" explicitFocusOrder="0" pos="176 23R 112 24" tooltip="Stop recording after a bar has elapsed."
                buttonText="Stop after bar" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="0"/>
  <GENERICCOMPONENT name="playPauseButton" id="da66be207abe8144" memberName="playPauseButton"
                    virtualName="" explicitFocusOrder="0" pos="80R 0 24 24" class="DrawableButton"
                    params="&quot;playPauseButton&quot;, DrawableButton::ImageOnButtonBackground"/>
  <GENERICCOMPONENT name="rtzButton" id="8087375604e93800" memberName="rtzButton"
                    virtualName="" explicitFocusOrder="0" pos="52R 0 24 24" class="DrawableButton"
                    params="&quot;rtzButton&quot;, DrawableButton::ImageOnButtonBackground"/>
  <GENERICCOMPONENT name="recordButton" id="9d6c14be2cdfd38e" memberName="recordButton"
                    virtualName="" explicitFocusOrder="0" pos="24R 0 24 24" class="DrawableButton"
                    params="&quot;recordButton&quot;, DrawableButton::ImageOnButtonBackground"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
