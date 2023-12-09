/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  20 Oct 2012 7:09:35pm

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
#include "LooperControl.h"
#include "ColourScheme.h"
#include "Vectors.h"

//[/Headers]

#include "LooperEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
LooperEditor::LooperEditor (LooperProcessor *proc, AudioThumbnail *thumbnail)
    : AudioProcessorEditor(proc),
      processor(proc),
      fileDisplay (0),
      filename (0),
      syncButton (0),
      stopAfterBarButton (0),
      playPauseButton (0),
      rtzButton (0),
      recordButton (0),
      autoPlayButton (0),
      barLengthLabel (0),
      separatorLabel (0),
      numeratorLabel (0),
      denominatorLabel (0),
      loopLevelLabel (0),
      loopLevelSlider (0),
      inputLevelLabel (0),
      inputLevelSlider (0)
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

    addAndMakeVisible (autoPlayButton = new ToggleButton (L"autoPlayButton"));
    autoPlayButton->setButtonText (L"Autoplay");
    autoPlayButton->addListener (this);

    addAndMakeVisible (barLengthLabel = new Label (L"barLengthLabel",
                                                   L"Bar length:"));
    barLengthLabel->setFont (Font (15.0000f, Font::plain));
    barLengthLabel->setJustificationType (Justification::centredLeft);
    barLengthLabel->setEditable (false, false, false);
    barLengthLabel->setColour (TextEditor::textColourId, Colours::black);
    barLengthLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (separatorLabel = new Label (L"separatorLabel",
                                                   L"/"));
    separatorLabel->setFont (Font (105.0000f, Font::bold));
    separatorLabel->setJustificationType (Justification::centred);
    separatorLabel->setEditable (false, false, false);
    separatorLabel->setColour (TextEditor::textColourId, Colours::black);
    separatorLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (numeratorLabel = new Label (L"numeratorLabel",
                                                   L"4"));
    numeratorLabel->setFont (Font (Font::getDefaultSerifFontName(), 105.0000f, Font::bold));
    numeratorLabel->setJustificationType (Justification::centred);
    numeratorLabel->setEditable (true, true, false);
    numeratorLabel->setColour (TextEditor::textColourId, Colours::black);
    numeratorLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));
    numeratorLabel->addListener (this);

    addAndMakeVisible (denominatorLabel = new Label (L"denominatorLabel",
                                                     L"4"));
    denominatorLabel->setFont (Font (Font::getDefaultSerifFontName(), 105.0000f, Font::bold));
    denominatorLabel->setJustificationType (Justification::centred);
    denominatorLabel->setEditable (true, true, false);
    denominatorLabel->setColour (TextEditor::textColourId, Colours::black);
    denominatorLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));
    denominatorLabel->addListener (this);

    addAndMakeVisible (loopLevelLabel = new Label (L"loopLevelLabel",
                                                   L"Loop level"));
    loopLevelLabel->setFont (Font (15.0000f, Font::plain));
    loopLevelLabel->setJustificationType (Justification::centred);
    loopLevelLabel->setEditable (false, false, false);
    loopLevelLabel->setColour (TextEditor::textColourId, Colours::black);
    loopLevelLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (loopLevelSlider = new Slider (L"loopLevelSlider"));
    loopLevelSlider->setRange (0, 1, 0);
    loopLevelSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    loopLevelSlider->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    loopLevelSlider->addListener (this);

    addAndMakeVisible (inputLevelLabel = new Label (L"inputLevelLabel",
                                                    L"Input level"));
    inputLevelLabel->setFont (Font (15.0000f, Font::plain));
    inputLevelLabel->setJustificationType (Justification::centred);
    inputLevelLabel->setEditable (false, false, false);
    inputLevelLabel->setColour (TextEditor::textColourId, Colours::black);
    inputLevelLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (inputLevelSlider = new Slider (L"inputLevelSlider"));
    inputLevelSlider->setRange (0, 1, 0);
    inputLevelSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    inputLevelSlider->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    inputLevelSlider->addListener (this);


    //[UserPreSize]

	String tempstr;
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
		fileDisplay->setReadPointer((float)processor->getReadPosition());
	}
	else
		filename->setDefaultBrowseTarget(LooperControl::lastDir);

	syncButton->setToggleState(processor->getParameter(LooperProcessor::SyncToMainTransport) > 0.5f,
							   false);
	stopAfterBarButton->setToggleState(processor->getParameter(LooperProcessor::StopAfterBar) > 0.5f,
									   false);
	autoPlayButton->setToggleState(processor->getParameter(LooperProcessor::AutoPlay) > 0.5f,
								   false);
	inputLevelSlider->setDoubleClickReturnValue(true, 0.5);
	inputLevelSlider->setValue(processor->getParameter(LooperProcessor::InputLevel));
	loopLevelSlider->setDoubleClickReturnValue(true, 0.5);
	loopLevelSlider->setValue(processor->getParameter(LooperProcessor::LoopLevel));

	tempstr << (int)processor->getParameter(LooperProcessor::BarNumerator);
	numeratorLabel->setText(tempstr, dontSendNotification);
	tempstr = L"";
	tempstr << (int)processor->getParameter(LooperProcessor::BarDenominator);
	denominatorLabel->setText(tempstr, dontSendNotification);

	filename->addListener(this);
	fileDisplay->addChangeListener(this);
	processor->addChangeListener(this);

	inputLevelSlider->setColour(Slider::rotarySliderFillColourId,
							    ColourScheme::getInstance().colours[L"Level Dial Colour"]);
	loopLevelSlider->setColour(Slider::rotarySliderFillColourId,
							   ColourScheme::getInstance().colours[L"Level Dial Colour"]);

	startTimer(60);

    //[/UserPreSize]

    setSize (500, 300);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

LooperEditor::~LooperEditor()
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
    deleteAndZero (autoPlayButton);
    deleteAndZero (barLengthLabel);
    deleteAndZero (separatorLabel);
    deleteAndZero (numeratorLabel);
    deleteAndZero (denominatorLabel);
    deleteAndZero (loopLevelLabel);
    deleteAndZero (loopLevelSlider);
    deleteAndZero (inputLevelLabel);
    deleteAndZero (inputLevelSlider);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void LooperEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void LooperEditor::resized()
{
    fileDisplay->setBounds (0, 28, getWidth() - 2, getHeight() - 155);
    filename->setBounds (0, 0, getWidth() - 84, 24);
    syncButton->setBounds (0, getHeight() - 23, 168, 24);
    stopAfterBarButton->setBounds (176, getHeight() - 23, 112, 24);
    playPauseButton->setBounds (getWidth() - 80, 0, 24, 24);
    rtzButton->setBounds (getWidth() - 52, 0, 24, 24);
    recordButton->setBounds (getWidth() - 24, 0, 24, 24);
    autoPlayButton->setBounds (296, getHeight() - 23, 80, 24);
    barLengthLabel->setBounds (0, getHeight() - 127, 80, 24);
    separatorLabel->setBounds (64, getHeight() - 111, 80, 80);
    numeratorLabel->setBounds (8, getHeight() - 103, 80, 80);
    denominatorLabel->setBounds (112, getHeight() - 103, 80, 80);
    loopLevelLabel->setBounds (getWidth() - 82, getHeight() - 127, 78, 24);
    loopLevelSlider->setBounds (getWidth() - 82, getHeight() - 103, 80, 80);
    inputLevelLabel->setBounds (getWidth() - 170, getHeight() - 127, 78, 24);
    inputLevelSlider->setBounds (getWidth() - 170, getHeight() - 103, 80, 80);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void LooperEditor::buttonClicked (Button* buttonThatWasClicked)
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
    else if (buttonThatWasClicked == autoPlayButton)
    {
        //[UserButtonCode_autoPlayButton] -- add your button handler code here..

		bool val = autoPlayButton->getToggleState();

		processor->setParameter(LooperProcessor::AutoPlay,
								val ? 1.0f : 0.0f);

        //[/UserButtonCode_autoPlayButton]
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

	//So LooperControl gets updated too.
	processor->sendChangeMessage();

    //[/UserbuttonClicked_Post]
}

void LooperEditor::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == numeratorLabel)
    {
        //[UserLabelCode_numeratorLabel] -- add your label text handling code here..

		float tempf;

		tempf = (float)(numeratorLabel->getText().getIntValue());
		processor->setParameter(LooperProcessor::BarNumerator, tempf);

        //[/UserLabelCode_numeratorLabel]
    }
    else if (labelThatHasChanged == denominatorLabel)
    {
        //[UserLabelCode_denominatorLabel] -- add your label text handling code here..

		float tempf;

		tempf = (float)(denominatorLabel->getText().getIntValue());
		processor->setParameter(LooperProcessor::BarDenominator, tempf);

        //[/UserLabelCode_denominatorLabel]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}

void LooperEditor::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == loopLevelSlider)
    {
        //[UserSliderCode_loopLevelSlider] -- add your slider handling code here..

		processor->setParameter(LooperProcessor::LoopLevel,
								(float)loopLevelSlider->getValue());

        //[/UserSliderCode_loopLevelSlider]
    }
    else if (sliderThatWasMoved == inputLevelSlider)
    {
        //[UserSliderCode_inputLevelSlider] -- add your slider handling code here..

		processor->setParameter(LooperProcessor::InputLevel,
								(float)inputLevelSlider->getValue());

        //[/UserSliderCode_inputLevelSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
void LooperEditor::filenameComponentChanged(FilenameComponent *filenameComp)
{
	File phil = filenameComp->getCurrentFile();
	fileDisplay->setFile(phil);
	processor->setFile(phil);
	LooperControl::lastDir = phil.getParentDirectory();

	//So LooperControl gets updated too.
	processor->sendChangeMessage();
}

//------------------------------------------------------------------------------
void LooperEditor::timerCallback()
{
	if(playing)
		fileDisplay->setReadPointer((float)processor->getReadPosition());
}

//------------------------------------------------------------------------------
void LooperEditor::changeListenerCallback(ChangeBroadcaster *source)
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
void LooperEditor::setWaveformBackground(const Colour& col)
{
	fileDisplay->setBackgroundColour(col);
}

//------------------------------------------------------------------------------
void LooperEditor::clearDisplay()
{
	fileDisplay->setFile(File::nonexistent);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LooperEditor" componentName=""
                 parentClasses="public AudioProcessorEditor, public FilenameComponentListener, public Timer, public ChangeListener"
                 constructorParams="LooperProcessor *proc, AudioThumbnail *thumbnail"
                 variableInitialisers="AudioProcessorEditor(proc),&#10;processor(proc)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="0" initialWidth="500" initialHeight="300">
  <BACKGROUND backgroundColour="eeece1"/>
  <GENERICCOMPONENT name="fileDisplay" id="ca679484fd8a4509" memberName="fileDisplay"
                    virtualName="" explicitFocusOrder="0" pos="0 28 2M 155M" class="WaveformDisplay"
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
  <TOGGLEBUTTON name="autoPlayButton" id="6a1054f4621d1438" memberName="autoPlayButton"
                virtualName="" explicitFocusOrder="0" pos="296 23R 80 24" buttonText="Autoplay"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <LABEL name="barLengthLabel" id="67d32710de38db61" memberName="barLengthLabel"
         virtualName="" explicitFocusOrder="0" pos="0 127R 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Bar length:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <LABEL name="separatorLabel" id="54c9fc2c8a44b75c" memberName="separatorLabel"
         virtualName="" explicitFocusOrder="0" pos="64 111R 80 80" edTextCol="ff000000"
         edBkgCol="0" labelText="/" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="105"
         bold="1" italic="0" justification="36"/>
  <LABEL name="numeratorLabel" id="4a1e8968f4b93c68" memberName="numeratorLabel"
         virtualName="" explicitFocusOrder="0" pos="8 103R 80 80" edTextCol="ff000000"
         edBkgCol="0" labelText="4" editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="105"
         bold="1" italic="0" justification="36"/>
  <LABEL name="denominatorLabel" id="a40183ba13b2abd1" memberName="denominatorLabel"
         virtualName="" explicitFocusOrder="0" pos="112 103R 80 80" edTextCol="ff000000"
         edBkgCol="0" labelText="4" editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="105"
         bold="1" italic="0" justification="36"/>
  <LABEL name="loopLevelLabel" id="dbe6aeffe7ed960a" memberName="loopLevelLabel"
         virtualName="" explicitFocusOrder="0" pos="82R 127R 78 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Loop level" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="36"/>
  <SLIDER name="loopLevelSlider" id="44604dcd61115dbc" memberName="loopLevelSlider"
          virtualName="" explicitFocusOrder="0" pos="82R 103R 80 80" min="0"
          max="1" int="0" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <LABEL name="inputLevelLabel" id="1abc33e42faa5045" memberName="inputLevelLabel"
         virtualName="" explicitFocusOrder="0" pos="170R 127R 78 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Input level" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="36"/>
  <SLIDER name="inputLevelSlider" id="415ad67729236604" memberName="inputLevelSlider"
          virtualName="" explicitFocusOrder="0" pos="170R 103R 80 80" min="0"
          max="1" int="0" style="RotaryVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
