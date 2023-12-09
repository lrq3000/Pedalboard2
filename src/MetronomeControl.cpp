/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  16 Feb 2012 11:59:34am

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
#include "Vectors.h"
#include "ColourScheme.h"

//[/Headers]

#include "MetronomeControl.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
MetronomeControl::MetronomeControl (MetronomeProcessor *proc, bool editors)
    : processor(proc),
	  playing(false),
	  showFileEditors(editors),
      syncButton (0),
      playPauseButton (0),
      accentFile (0),
      accentLabel (0),
      clickFile (0),
      clickLabel (0),
      numeratorLabel (0),
      denominatorLabel (0),
      separatorLabel (0)
{
    addAndMakeVisible (syncButton = new ToggleButton (L"syncButton"));
    syncButton->setTooltip (L"Sync metronome playback to the main transport");
    syncButton->setButtonText (L"Sync to main transport");
    syncButton->addListener (this);

    addAndMakeVisible (playPauseButton = new DrawableButton ("playPauseButton", DrawableButton::ImageOnButtonBackground));
    playPauseButton->setName (L"playPauseButton");

    addAndMakeVisible (accentFile = new FilenameComponent ("accentFile", File::nonexistent, true, false, false, "*.wav;*.aif", "", "<no file loaded>"));
    accentFile->setName (L"accentFile");

    addAndMakeVisible (accentLabel = new Label (L"accentLabel",
                                                L"Accent:"));
    accentLabel->setFont (Font (15.0000f, Font::plain));
    accentLabel->setJustificationType (Justification::centredLeft);
    accentLabel->setEditable (false, false, false);
    accentLabel->setColour (TextEditor::textColourId, Colours::black);
    accentLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (clickFile = new FilenameComponent ("clickFile", File::nonexistent, true, false, false, "*.wav;*.aif", "", "<no file loaded>"));
    clickFile->setName (L"clickFile");

    addAndMakeVisible (clickLabel = new Label (L"clickLabel",
                                               L"Click:"));
    clickLabel->setFont (Font (15.0000f, Font::plain));
    clickLabel->setJustificationType (Justification::centredLeft);
    clickLabel->setEditable (false, false, false);
    clickLabel->setColour (TextEditor::textColourId, Colours::black);
    clickLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (numeratorLabel = new Label (L"numeratorLabel",
                                                   L"4"));
    numeratorLabel->setFont (Font (Font::getDefaultSerifFontName(), 250.0000f, Font::bold));
    numeratorLabel->setJustificationType (Justification::centred);
    numeratorLabel->setEditable (true, true, false);
    numeratorLabel->setColour (TextEditor::textColourId, Colours::black);
    numeratorLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));
    numeratorLabel->addListener (this);

    addAndMakeVisible (denominatorLabel = new Label (L"denominatorLabel",
                                                     L"4"));
    denominatorLabel->setFont (Font (Font::getDefaultSerifFontName(), 250.0000f, Font::bold));
    denominatorLabel->setJustificationType (Justification::centred);
    denominatorLabel->setEditable (true, true, false);
    denominatorLabel->setColour (TextEditor::textColourId, Colours::black);
    denominatorLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));
    denominatorLabel->addListener (this);

    addAndMakeVisible (separatorLabel = new Label (L"separatorLabel",
                                                   L"/"));
    separatorLabel->setFont (Font (Font::getDefaultSerifFontName(), 250.0000f, Font::bold));
    separatorLabel->setJustificationType (Justification::centred);
    separatorLabel->setEditable (false, false, false);
    separatorLabel->setColour (TextEditor::textColourId, Colours::black);
    separatorLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));


    //[UserPreSize]

	String tempstr;

	playImage = loadSVGFromMemory(Vectors::playbutton_svg,
								  Vectors::playbutton_svgSize);
	pauseImage = loadSVGFromMemory(Vectors::pausebutton_svg,
								   Vectors::pausebutton_svgSize);
	playPauseButton->setImages(playImage);
	playPauseButton->setColour(DrawableButton::backgroundColourId,
							   ColourScheme::getInstance().colours[L"Button Colour"]);
	playPauseButton->setColour(DrawableButton::backgroundOnColourId,
							   ColourScheme::getInstance().colours[L"Button Colour"]);
	playPauseButton->addListener(this);
	playPauseButton->setTooltip(L"Play/pause metronome");

	clickFile->addListener(this);
	accentFile->addListener(this);
	processor->addChangeListener(this);

	tempstr << (int)processor->getParameter(MetronomeProcessor::Numerator);
	numeratorLabel->setText(tempstr, dontSendNotification);
	tempstr = L"";
	tempstr << (int)processor->getParameter(MetronomeProcessor::Denominator);
	denominatorLabel->setText(tempstr, dontSendNotification);

	clickFile->setCurrentFile(processor->getClickFile(), false);
	accentFile->setCurrentFile(processor->getAccentFile(), false);

	if(!showFileEditors)
	{
		clickLabel->setVisible(false);
		clickFile->setVisible(false);
		accentLabel->setVisible(false);
		accentFile->setVisible(false);
	}

    //[/UserPreSize]

    setSize (170, 100);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

MetronomeControl::~MetronomeControl()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (syncButton);
    deleteAndZero (playPauseButton);
    deleteAndZero (accentFile);
    deleteAndZero (accentLabel);
    deleteAndZero (clickFile);
    deleteAndZero (clickLabel);
    deleteAndZero (numeratorLabel);
    deleteAndZero (denominatorLabel);
    deleteAndZero (separatorLabel);


    //[Destructor]. You can add your own custom destruction code here..
	processor->removeChangeListener(this);
    //[/Destructor]
}

//==============================================================================
void MetronomeControl::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MetronomeControl::resized()
{
    syncButton->setBounds (0, getHeight() - 23, 168, 24);
    playPauseButton->setBounds (getWidth() - 26, 0, 24, 24);
    accentFile->setBounds (56, 24, getWidth() - 58, 24);
    accentLabel->setBounds (0, 24, 64, 24);
    clickFile->setBounds (56, 0, getWidth() - 84, 24);
    clickLabel->setBounds (0, 0, 48, 24);
    numeratorLabel->setBounds (0, 48, proportionOfWidth (0.4896f), getHeight() - 71);
    denominatorLabel->setBounds (proportionOfWidth (0.5104f), 48, proportionOfWidth (0.4896f), getHeight() - 71);
    separatorLabel->setBounds (proportionOfWidth (0.4301f), 48, proportionOfWidth (0.1399f), getHeight() - 71);
    //[UserResized] Add your own custom resize handling here..

	float fontSize = getHeight() * (250.0f/400.0f);
	Font resizedFont(Font::getDefaultSerifFontName(),
					 (fontSize > 14.0f) ? fontSize : 14.0f,
					 Font::bold);

	numeratorLabel->setFont(resizedFont);
	denominatorLabel->setFont(resizedFont);
	separatorLabel->setFont(resizedFont);

	if(!showFileEditors)
	{
		playPauseButton->setTopLeftPosition(0, 0);
		numeratorLabel->setBounds (0, 24, proportionOfWidth(0.4896f), getHeight() - 48);
		denominatorLabel->setBounds (proportionOfWidth(0.5104f), 24, proportionOfWidth(0.4896f), getHeight() - 48);
		separatorLabel->setBounds (proportionOfWidth(0.4301f), 24, proportionOfWidth(0.1399f), getHeight() - 48);
	}

	//Set controls according to the processor's state.
	{
		String tempstr;

		tempstr << (int)(processor->getParameter(MetronomeProcessor::Numerator));
		numeratorLabel->setText(tempstr, dontSendNotification);
		tempstr = L"";
		tempstr << (int)(processor->getParameter(MetronomeProcessor::Denominator));
		denominatorLabel->setText(tempstr, dontSendNotification);
		syncButton->setToggleState(processor->getParameter(MetronomeProcessor::SyncToMainTransport) > 0.5f, false);
	}

    //[/UserResized]
}

void MetronomeControl::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == syncButton)
    {
        //[UserButtonCode_syncButton] -- add your button handler code here..

		bool val = syncButton->getToggleState();

		processor->setParameter(MetronomeProcessor::SyncToMainTransport,
								val ? 1.0f : 0.0f);

        //[/UserButtonCode_syncButton]
    }

    //[UserbuttonClicked_Post]
	else if(buttonThatWasClicked == playPauseButton)
	{
		if(!playing)
			playPauseButton->setImages(pauseImage);
		else
			playPauseButton->setImages(playImage);
		playing = !playing;
		processor->setParameter(MetronomeProcessor::Play, 1.0f);
	}
    //[/UserbuttonClicked_Post]
}

void MetronomeControl::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == numeratorLabel)
    {
        //[UserLabelCode_numeratorLabel] -- add your label text handling code here..

		float tempf;

		tempf = (float)(numeratorLabel->getText().getIntValue());
		processor->setParameter(MetronomeProcessor::Numerator, tempf);

        //[/UserLabelCode_numeratorLabel]
    }
    else if (labelThatHasChanged == denominatorLabel)
    {
        //[UserLabelCode_denominatorLabel] -- add your label text handling code here..

		float tempf;

		tempf = (float)(denominatorLabel->getText().getIntValue());
		processor->setParameter(MetronomeProcessor::Denominator, tempf);

        //[/UserLabelCode_denominatorLabel]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
void MetronomeControl::filenameComponentChanged(FilenameComponent *fileComponentThatHasChanged)
{
	if(fileComponentThatHasChanged == accentFile)
		processor->setAccentFile(fileComponentThatHasChanged->getCurrentFile());
	else if(fileComponentThatHasChanged == clickFile)
		processor->setClickFile(fileComponentThatHasChanged->getCurrentFile());
}

//------------------------------------------------------------------------------
void MetronomeControl::changeListenerCallback(ChangeBroadcaster *source)
{
	if(source == processor)
	{
		String tempstr;

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

		tempstr << (int)(processor->getParameter(MetronomeProcessor::Numerator));
		numeratorLabel->setText(tempstr, dontSendNotification);
		tempstr = L"";
		tempstr << (int)(processor->getParameter(MetronomeProcessor::Denominator));
		denominatorLabel->setText(tempstr, dontSendNotification);
		syncButton->setToggleState(processor->getParameter(MetronomeProcessor::SyncToMainTransport) > 0.5f, false);
	}
}

//------------------------------------------------------------------------------
Drawable *MetronomeControl::loadSVGFromMemory(const void *dataToInitialiseFrom,
											  size_t sizeInBytes)
{
	Drawable *retval = 0;

	MemoryBlock memBlock(dataToInitialiseFrom, sizeInBytes);
	XmlDocument doc(memBlock.toString());
	ScopedPointer<XmlElement> svgData(doc.getDocumentElement());

	retval = Drawable::createFromSVG(*svgData);

	return retval;
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MetronomeControl" componentName=""
                 parentClasses="public Component, public FilenameComponentListener, public ChangeListener"
                 constructorParams="MetronomeProcessor *proc, bool editors" variableInitialisers="processor(proc),&#10;showFileEditors(editors),&#10;playing(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="0" initialWidth="170" initialHeight="100">
  <BACKGROUND backgroundColour="eeece1"/>
  <TOGGLEBUTTON name="syncButton" id="b0788f5602fb3d7f" memberName="syncButton"
                virtualName="" explicitFocusOrder="0" pos="0 23R 168 24" tooltip="Sync metronome playback to the main transport"
                buttonText="Sync to main transport" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="0"/>
  <GENERICCOMPONENT name="playPauseButton" id="da66be207abe8144" memberName="playPauseButton"
                    virtualName="" explicitFocusOrder="0" pos="26R 0 24 24" class="DrawableButton"
                    params="&quot;playPauseButton&quot;, DrawableButton::ImageOnButtonBackground"/>
  <GENERICCOMPONENT name="accentFile" id="f1e80797e31ff742" memberName="accentFile"
                    virtualName="" explicitFocusOrder="0" pos="56 24 58M 24" class="FilenameComponent"
                    params="&quot;accentFile&quot;, File::nonexistent, true, false, false, &quot;*.wav;*.aif&quot;, &quot;&quot;, &quot;&lt;no file loaded&gt;&quot;"/>
  <LABEL name="accentLabel" id="9472b17c180344c3" memberName="accentLabel"
         virtualName="" explicitFocusOrder="0" pos="0 24 64 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Accent:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="clickFile" id="ac1e462a1246a2f2" memberName="clickFile"
                    virtualName="" explicitFocusOrder="0" pos="56 0 84M 24" class="FilenameComponent"
                    params="&quot;clickFile&quot;, File::nonexistent, true, false, false, &quot;*.wav;*.aif&quot;, &quot;&quot;, &quot;&lt;no file loaded&gt;&quot;"/>
  <LABEL name="clickLabel" id="e6e80ff93426bdf4" memberName="clickLabel"
         virtualName="" explicitFocusOrder="0" pos="0 0 48 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Click:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="numeratorLabel" id="9e2ccd063dfc55d" memberName="numeratorLabel"
         virtualName="" explicitFocusOrder="0" pos="0 48 48.96% 71M" edTextCol="ff000000"
         edBkgCol="0" labelText="4" editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="250"
         bold="1" italic="0" justification="36"/>
  <LABEL name="denominatorLabel" id="b7a03711249e2ce1" memberName="denominatorLabel"
         virtualName="" explicitFocusOrder="0" pos="51.04% 48 48.96% 71M"
         edTextCol="ff000000" edBkgCol="0" labelText="4" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="250" bold="1" italic="0" justification="36"/>
  <LABEL name="separatorLabel" id="76de549e03ecf3ff" memberName="separatorLabel"
         virtualName="" explicitFocusOrder="0" pos="43.006% 48 13.989% 71M"
         edTextCol="ff000000" edBkgCol="0" labelText="/" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="250" bold="1" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
