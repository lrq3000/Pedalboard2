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

//[Headers] You can add your own extra header files here...

#include "PedalboardProcessors.h"
#include "FilePlayerControl.h"
#include "JuceHelperStuff.h"
#include "WaveformDisplay.h"
#include "ColourScheme.h"
#include "Vectors.h"

//[/Headers]

#include "AudioRecorderControl.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AudioRecorderControl::AudioRecorderControl (RecorderProcessor *proc, AudioThumbnail& thumbnail)
    : processor(proc),
      fileDisplay (0),
      filename (0),
      syncButton (0),
      recordButton (0)
{
    addAndMakeVisible (fileDisplay = new WaveformDisplayLite (thumbnail));
    fileDisplay->setName (L"fileDisplay");

    addAndMakeVisible (filename = new FilenameComponent ("filename", File::nonexistent, true, false, true, "*.wav", ".wav", "<no file loaded>"));
    filename->setName (L"filename");

    addAndMakeVisible (syncButton = new ToggleButton (L"syncButton"));
    syncButton->setButtonText (L"Sync to main transport");
    syncButton->addListener (this);

    addAndMakeVisible (recordButton = new DrawableButton ("recordButton", DrawableButton::ImageOnButtonBackground));
    recordButton->setName (L"recordButton");


    //[UserPreSize]

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
	recordButton->setTooltip(L"Record audio input");

	const File& soundFile = processor->getFile();
	if(soundFile != File::nonexistent)
		filename->setCurrentFile(soundFile, true, dontSendNotification);
	else
		filename->setDefaultBrowseTarget(FilePlayerControl::lastDir);

	syncButton->setToggleState(processor->getParameter(RecorderProcessor::SyncToMainTransport) > 0.5f,
							   false);

	filename->addListener(this);

	processor->addChangeListener(this);

    //[/UserPreSize]

    setSize (300, 100);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

AudioRecorderControl::~AudioRecorderControl()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	processor->removeChangeListener(this);

    //[/Destructor_pre]

    deleteAndZero (fileDisplay);
    deleteAndZero (filename);
    deleteAndZero (syncButton);
    deleteAndZero (recordButton);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AudioRecorderControl::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AudioRecorderControl::resized()
{
    fileDisplay->setBounds (0, 28, getWidth() - 2, getHeight() - 48);
    filename->setBounds (0, 0, getWidth() - 28, 24);
    syncButton->setBounds (0, getHeight() - 23, 168, 24);
    recordButton->setBounds (getWidth() - 26, 0, 24, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AudioRecorderControl::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == syncButton)
    {
        //[UserButtonCode_syncButton] -- add your button handler code here..

		bool val = syncButton->getToggleState();

		processor->setParameter(RecorderProcessor::SyncToMainTransport,
								val ? 1.0f : 0.0f);

        //[/UserButtonCode_syncButton]
    }

    //[UserbuttonClicked_Post]
	else if(buttonThatWasClicked == recordButton)
	{
		if(!recording)
			recordButton->setImages(stopImage);
		else
			recordButton->setImages(recordImage);
		recording = !recording;

		/*if(recording)
			processor->setFile(filename->getCurrentFile());*/

		processor->setParameter(RecorderProcessor::Record, 1.0f);
	}
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
void AudioRecorderControl::filenameComponentChanged(FilenameComponent *filenameComp)
{
	File phil = filenameComp->getCurrentFile();
	//processor->setFile(phil);
	processor->cacheFile(phil);
	FilePlayerControl::lastDir = phil.getParentDirectory();
}

//------------------------------------------------------------------------------
void AudioRecorderControl::changeListenerCallback(ChangeBroadcaster *source)
{
	if(source == processor)
	{
		if(processor->isRecording())
		{
			recordButton->setImages(stopImage);
			recording = true;
		}
		else
		{
			recordButton->setImages(recordImage);
			recording = false;
		}
		syncButton->setToggleState(processor->getParameter(RecorderProcessor::SyncToMainTransport) > 0.5f, false);

		filename->setCurrentFile(processor->getFile(), true, dontSendNotification);
	}
}

//------------------------------------------------------------------------------
void AudioRecorderControl::setWaveformBackground(const Colour& col)
{
	fileDisplay->setBackgroundColour(col);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AudioRecorderControl" componentName=""
                 parentClasses="public Component, public FilenameComponentListener, public ChangeListener"
                 constructorParams="RecorderProcessor *proc, AudioThumbnail&amp; thumbnail"
                 variableInitialisers="processor(proc)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330000013" fixedSize="0" initialWidth="300"
                 initialHeight="100">
  <BACKGROUND backgroundColour="eeece1"/>
  <GENERICCOMPONENT name="fileDisplay" id="ca679484fd8a4509" memberName="fileDisplay"
                    virtualName="" explicitFocusOrder="0" pos="0 28 2M 48M" class="WaveformDisplayLite"
                    params="thumbnail"/>
  <GENERICCOMPONENT name="filename" id="f1e80797e31ff742" memberName="filename" virtualName=""
                    explicitFocusOrder="0" pos="0 0 28M 24" class="FilenameComponent"
                    params="&quot;filename&quot;, File::nonexistent, true, false, true, &quot;*.wav&quot;, &quot;.wav&quot;, &quot;&lt;no file loaded&gt;&quot;"/>
  <TOGGLEBUTTON name="syncButton" id="b0788f5602fb3d7f" memberName="syncButton"
                virtualName="" explicitFocusOrder="0" pos="0 23R 168 24" buttonText="Sync to main transport"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <GENERICCOMPONENT name="recordButton" id="8087375604e93800" memberName="recordButton"
                    virtualName="" explicitFocusOrder="0" pos="26R 0 24 24" class="DrawableButton"
                    params="&quot;recordButton&quot;, DrawableButton::ImageOnButtonBackground"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
