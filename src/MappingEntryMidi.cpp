/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  17 Jan 2013 11:13:47am

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

#include "MappingsDialog.h"
#include "ColourScheme.h"

//[/Headers]

#include "MappingEntryMidi.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
MappingEntryMidi::MappingEntryMidi (MappingsDialog *dlg, int arrayIndex, int cc, bool latch, float lowerBound, float upperBound)
    : ccComboBox (0),
      latchButton (0),
      slider (0),
      rangeLabel (0),
      paramComboBox (0)
{
    addAndMakeVisible (ccComboBox = new ComboBox (L"ccComboBox"));
    ccComboBox->setEditableText (false);
    ccComboBox->setJustificationType (Justification::centredLeft);
    ccComboBox->setTextWhenNothingSelected (String::empty);
    ccComboBox->setTextWhenNoChoicesAvailable (L"(no choices)");
    ccComboBox->addItem (L"<< MIDI Learn >>", 1);
    ccComboBox->addItem (L"0: Bank Select", 2);
    ccComboBox->addItem (L"1: Mod Wheel", 3);
    ccComboBox->addItem (L"2: Breath", 4);
    ccComboBox->addItem (L"3:", 5);
    ccComboBox->addItem (L"4: Foot Pedal", 6);
    ccComboBox->addItem (L"5: Portamento", 7);
    ccComboBox->addItem (L"6: Data Entry", 8);
    ccComboBox->addItem (L"7: Volume", 9);
    ccComboBox->addItem (L"8: Balance", 10);
    ccComboBox->addItem (L"9:", 11);
    ccComboBox->addItem (L"10: Pan", 12);
    ccComboBox->addItem (L"11: Expression", 13);
    ccComboBox->addItem (L"12: Effect Control 1", 14);
    ccComboBox->addItem (L"13: EffectControl 2", 15);
    ccComboBox->addItem (L"14:", 16);
    ccComboBox->addItem (L"15:", 17);
    ccComboBox->addItem (L"16: General Purpose 1", 18);
    ccComboBox->addItem (L"17: General Purpose 2", 19);
    ccComboBox->addItem (L"18: General Purpose 3", 20);
    ccComboBox->addItem (L"19: General Purpose 4", 21);
    ccComboBox->addItem (L"20:", 22);
    ccComboBox->addItem (L"21:", 23);
    ccComboBox->addItem (L"22:", 24);
    ccComboBox->addItem (L"23:", 25);
    ccComboBox->addItem (L"24:", 26);
    ccComboBox->addItem (L"25:", 27);
    ccComboBox->addItem (L"26:", 28);
    ccComboBox->addItem (L"27:", 29);
    ccComboBox->addItem (L"28:", 30);
    ccComboBox->addItem (L"29:", 31);
    ccComboBox->addItem (L"30:", 32);
    ccComboBox->addItem (L"31:", 33);
    ccComboBox->addItem (L"32: Bank Select (fine)", 34);
    ccComboBox->addItem (L"33: Mod Wheel (fine)", 35);
    ccComboBox->addItem (L"34: Breath (fine)", 36);
    ccComboBox->addItem (L"35:", 37);
    ccComboBox->addItem (L"36: Foot Pedal (fine)", 38);
    ccComboBox->addItem (L"37: Portamento (fine)", 39);
    ccComboBox->addItem (L"38: Data Entry (fine)", 40);
    ccComboBox->addItem (L"39: Volume (fine)", 41);
    ccComboBox->addItem (L"40: Balance (fine)", 42);
    ccComboBox->addItem (L"41:", 43);
    ccComboBox->addItem (L"42: Pan (fine)", 44);
    ccComboBox->addItem (L"43: Expression (fine)", 45);
    ccComboBox->addItem (L"44: Effect Control 1 (fine)", 46);
    ccComboBox->addItem (L"45: Effect Control 2 (fine)", 47);
    ccComboBox->addItem (L"46:", 48);
    ccComboBox->addItem (L"47:", 49);
    ccComboBox->addItem (L"48:", 50);
    ccComboBox->addItem (L"49:", 51);
    ccComboBox->addItem (L"50:", 52);
    ccComboBox->addItem (L"51:", 53);
    ccComboBox->addItem (L"52:", 54);
    ccComboBox->addItem (L"53:", 55);
    ccComboBox->addItem (L"54:", 56);
    ccComboBox->addItem (L"55:", 57);
    ccComboBox->addItem (L"56:", 58);
    ccComboBox->addItem (L"57:", 59);
    ccComboBox->addItem (L"58:", 60);
    ccComboBox->addItem (L"59:", 61);
    ccComboBox->addItem (L"60:", 62);
    ccComboBox->addItem (L"61:", 63);
    ccComboBox->addItem (L"62:", 64);
    ccComboBox->addItem (L"63:", 65);
    ccComboBox->addItem (L"64: Hold Pedal", 66);
    ccComboBox->addItem (L"65: Portamento (on/off)", 67);
    ccComboBox->addItem (L"66: Sustenuto Pedal", 68);
    ccComboBox->addItem (L"67: Soft Pedal", 69);
    ccComboBox->addItem (L"68: Legato Pedal", 70);
    ccComboBox->addItem (L"69: Hold 2 Pedal", 71);
    ccComboBox->addItem (L"70: Sound Variation", 72);
    ccComboBox->addItem (L"71: Sound Timbre", 73);
    ccComboBox->addItem (L"72: Sound Release Time", 74);
    ccComboBox->addItem (L"73: Sound Attack Time", 75);
    ccComboBox->addItem (L"74: Sound Brightness", 76);
    ccComboBox->addItem (L"75: Sound Control 6", 77);
    ccComboBox->addItem (L"76: Sound Control 7", 78);
    ccComboBox->addItem (L"77: Sound Control 8", 79);
    ccComboBox->addItem (L"78: Sound Control 9", 80);
    ccComboBox->addItem (L"79: Sound Control 10", 81);
    ccComboBox->addItem (L"80: General Purpose Button 1", 82);
    ccComboBox->addItem (L"81: General Purpose Button 2", 83);
    ccComboBox->addItem (L"82: General Purpose Button 3", 84);
    ccComboBox->addItem (L"83: General Purpose Button 4", 85);
    ccComboBox->addItem (L"84:", 86);
    ccComboBox->addItem (L"85:", 87);
    ccComboBox->addItem (L"86:", 88);
    ccComboBox->addItem (L"87:", 89);
    ccComboBox->addItem (L"88:", 90);
    ccComboBox->addItem (L"89:", 91);
    ccComboBox->addItem (L"90:", 92);
    ccComboBox->addItem (L"91: Effects Level", 93);
    ccComboBox->addItem (L"92: Tremolo Level", 94);
    ccComboBox->addItem (L"93: Chorus Level", 95);
    ccComboBox->addItem (L"94: Celeste Level", 96);
    ccComboBox->addItem (L"95: Phaser Level", 97);
    ccComboBox->addItem (L"96: Data Button Inc", 98);
    ccComboBox->addItem (L"97: Data Button Dec", 99);
    ccComboBox->addItem (L"98: NRPN (fine)", 100);
    ccComboBox->addItem (L"99: NRPN (coarse)", 101);
    ccComboBox->addItem (L"100: RPN (fine)", 102);
    ccComboBox->addItem (L"101: RPN (coarse)", 103);
    ccComboBox->addItem (L"102:", 104);
    ccComboBox->addItem (L"103:", 105);
    ccComboBox->addItem (L"104:", 106);
    ccComboBox->addItem (L"105:", 107);
    ccComboBox->addItem (L"106:", 108);
    ccComboBox->addItem (L"107:", 109);
    ccComboBox->addItem (L"108:", 110);
    ccComboBox->addItem (L"109:", 111);
    ccComboBox->addItem (L"110:", 112);
    ccComboBox->addItem (L"111:", 113);
    ccComboBox->addItem (L"112:", 114);
    ccComboBox->addItem (L"113:", 115);
    ccComboBox->addItem (L"114:", 116);
    ccComboBox->addItem (L"115:", 117);
    ccComboBox->addItem (L"116:", 118);
    ccComboBox->addItem (L"117:", 119);
    ccComboBox->addItem (L"118:", 120);
    ccComboBox->addItem (L"119:", 121);
    ccComboBox->addItem (L"120: All Sound Off", 122);
    ccComboBox->addItem (L"121: All Controllers Off", 123);
    ccComboBox->addItem (L"122: Local Keyboard", 124);
    ccComboBox->addItem (L"123: All Notes Off", 125);
    ccComboBox->addItem (L"124: Omni Mode Off", 126);
    ccComboBox->addItem (L"125: Omni Mode On", 127);
    ccComboBox->addItem (L"126: Mono Operation", 128);
    ccComboBox->addItem (L"127: Poly Operation", 129);
    ccComboBox->addListener (this);

    addAndMakeVisible (latchButton = new ToggleButton (L"latchButton"));
    latchButton->setButtonText (L"Latch CC Value");
    latchButton->addListener (this);

    addAndMakeVisible (slider = new MappingSlider (L"new slider"));
    slider->setRange (0, 1, 0);
    //slider->setSliderStyle (MappingSlider::TwoValueHorizontal);
    slider->setTextBoxStyle (MappingSlider::NoTextBox, false, 80, 20);
    slider->setColour (Slider::thumbColourId, Colour (0xff9a9181));
    slider->addListener (this);

    addAndMakeVisible (rangeLabel = new Label (L"rangeLabel",
                                               L"Parameter Range:"));
    rangeLabel->setFont (Font (15.0000f, Font::plain));
    rangeLabel->setJustificationType (Justification::centredLeft);
    rangeLabel->setEditable (false, false, false);
    rangeLabel->setColour (TextEditor::textColourId, Colours::black);
    rangeLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (paramComboBox = new ComboBox (L"paramComboBox"));
    paramComboBox->setEditableText (false);
    paramComboBox->setJustificationType (Justification::centredLeft);
    paramComboBox->setTextWhenNothingSelected (String::empty);
    paramComboBox->setTextWhenNoChoicesAvailable (L"(no choices)");
    paramComboBox->addListener (this);


    //[UserPreSize]

	setInterceptsMouseClicks(false, true);

	mappingsDialog = dlg;
	index = arrayIndex;

	midiLearn = false;

	ccComboBox->setSelectedId(cc+2, true);
	latchButton->setToggleState(latch, false);
	slider->setMaxValue(upperBound, dontSendNotification);
	slider->setMinValue(lowerBound, dontSendNotification);

	rangeLabel->setInterceptsMouseClicks(false, true);

	slider->setColour(MappingSlider::thumbColourId,
					  ColourScheme::getInstance().colours[L"Slider Colour"]);

    //[/UserPreSize]

    setSize (728, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

MappingEntryMidi::~MappingEntryMidi()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (ccComboBox);
    deleteAndZero (latchButton);
    deleteAndZero (slider);
    deleteAndZero (rangeLabel);
    deleteAndZero (paramComboBox);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MappingEntryMidi::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..

	g.setColour(ColourScheme::getInstance().colours[L"Vector Colour"]);

    //[/UserPrePaint]

    g.setColour (Colour (0x80000000));
    g.strokePath (internalPath1, PathStrokeType (5.0000f, PathStrokeType::curved, PathStrokeType::rounded));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MappingEntryMidi::resized()
{
    ccComboBox->setBounds (8, 8, 144, 24);
    latchButton->setBounds (160, 8, 120, 24);
    slider->setBounds (440, 8, 128, 24);
    rangeLabel->setBounds (320, 8, 128, 24);
    paramComboBox->setBounds (576, 8, 144, 24);
    internalPath1.clear();
    internalPath1.startNewSubPath (298.0f, 12.0f);
    internalPath1.lineTo (304.0f, 20.0f);
    internalPath1.lineTo (298.0f, 28.0f);

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MappingEntryMidi::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == ccComboBox)
    {
        //[UserComboBoxCode_ccComboBox] -- add your combo box handling code here..

		int selected = ccComboBox->getSelectedId();

		if(selected == 1)
		{
			midiLearn = true;
			mappingsDialog->activateMidiLearn(index);
		}
		else
		{
			if(midiLearn)
			{
				mappingsDialog->deactivateMidiLearn(index);
				midiLearn = false;
			}
			mappingsDialog->setCc(index, selected-2);
		}

        //[/UserComboBoxCode_ccComboBox]
    }
    else if (comboBoxThatHasChanged == paramComboBox)
    {
        //[UserComboBoxCode_paramComboBox] -- add your combo box handling code here..

		mappingsDialog->setParameter(index, paramComboBox->getSelectedId()-1);

        //[/UserComboBoxCode_paramComboBox]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void MappingEntryMidi::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == latchButton)
    {
        //[UserButtonCode_latchButton] -- add your button handler code here..

		mappingsDialog->setLatch(index, latchButton->getToggleState());

        //[/UserButtonCode_latchButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void MappingEntryMidi::sliderValueChanged (MappingSlider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == slider)
    {
        //[UserSliderCode_slider] -- add your slider handling code here..

		mappingsDialog->setLowerBound(index, (float)slider->getMinValue());
		mappingsDialog->setUpperBound(index, (float)slider->getMaxValue());

        //[/UserSliderCode_slider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
void MappingEntryMidi::addParameter(const String& param)
{
	String tempstr = param;

	if(tempstr.isEmpty())
		tempstr = L"<no name>";
	paramComboBox->addItem(tempstr, paramComboBox->getNumItems()+1);
}

//------------------------------------------------------------------------------
void MappingEntryMidi::selectParameter(int index)
{
	paramComboBox->setSelectedId(index+1, true);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MappingEntryMidi" componentName=""
                 parentClasses="public Component" constructorParams="MappingsDialog *dlg, int arrayIndex, int cc, bool latch, float lowerBound, float upperBound"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330000013" fixedSize="0" initialWidth="728"
                 initialHeight="400">
  <BACKGROUND backgroundColour="ffffff">
    <PATH pos="0 0 100 100" fill="solid: 2aa558" hasStroke="1" stroke="5, curved, rounded"
          strokeColour="solid: 80000000" nonZeroWinding="1">s 298 12 l 304 20 l 298 28</PATH>
  </BACKGROUND>
  <COMBOBOX name="ccComboBox" id="3a1fda5e4bd4bd7d" memberName="ccComboBox"
            virtualName="" explicitFocusOrder="0" pos="8 8 144 24" editable="0"
            layout="33" items="&lt;&lt; MIDI Learn &gt;&gt;&#10;0: Bank Select&#10;1: Mod Wheel&#10;2: Breath&#10;3:&#10;4: Foot Pedal&#10;5: Portamento&#10;6: Data Entry&#10;7: Volume&#10;8: Balance&#10;9:&#10;10: Pan&#10;11: Expression&#10;12: Effect Control 1&#10;13: EffectControl 2&#10;14:&#10;15:&#10;16: General Purpose 1&#10;17: General Purpose 2&#10;18: General Purpose 3&#10;19: General Purpose 4&#10;20:&#10;21:&#10;22:&#10;23:&#10;24:&#10;25:&#10;26:&#10;27:&#10;28:&#10;29:&#10;30:&#10;31:&#10;32: Bank Select (fine)&#10;33: Mod Wheel (fine)&#10;34: Breath (fine)&#10;35:&#10;36: Foot Pedal (fine)&#10;37: Portamento (fine)&#10;38: Data Entry (fine)&#10;39: Volume (fine)&#10;40: Balance (fine)&#10;41:&#10;42: Pan (fine)&#10;43: Expression (fine)&#10;44: Effect Control 1 (fine)&#10;45: Effect Control 2 (fine)&#10;46:&#10;47:&#10;48:&#10;49:&#10;50:&#10;51:&#10;52:&#10;53:&#10;54:&#10;55:&#10;56:&#10;57:&#10;58:&#10;59:&#10;60:&#10;61:&#10;62:&#10;63:&#10;64: Hold Pedal&#10;65: Portamento (on/off)&#10;66: Sustenuto Pedal&#10;67: Soft Pedal&#10;68: Legato Pedal&#10;69: Hold 2 Pedal&#10;70: Sound Variation&#10;71: Sound Timbre&#10;72: Sound Release Time&#10;73: Sound Attack Time&#10;74: Sound Brightness&#10;75: Sound Control 6&#10;76: Sound Control 7&#10;77: Sound Control 8&#10;78: Sound Control 9&#10;79: Sound Control 10&#10;80: General Purpose Button 1&#10;81: General Purpose Button 2&#10;82: General Purpose Button 3&#10;83: General Purpose Button 4&#10;84:&#10;85:&#10;86:&#10;87:&#10;88:&#10;89:&#10;90:&#10;91: Effects Level&#10;92: Tremolo Level&#10;93: Chorus Level&#10;94: Celeste Level&#10;95: Phaser Level&#10;96: Data Button Inc&#10;97: Data Button Dec&#10;98: NRPN (fine)&#10;99: NRPN (coarse)&#10;100: RPN (fine)&#10;101: RPN (coarse)&#10;102:&#10;103:&#10;104:&#10;105:&#10;106:&#10;107:&#10;108:&#10;109:&#10;110:&#10;111:&#10;112:&#10;113:&#10;114:&#10;115:&#10;116:&#10;117:&#10;118:&#10;119:&#10;120: All Sound Off&#10;121: All Controllers Off&#10;122: Local Keyboard&#10;123: All Notes Off&#10;124: Omni Mode Off&#10;125: Omni Mode On&#10;126: Mono Operation&#10;127: Poly Operation"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TOGGLEBUTTON name="latchButton" id="e183a47ace339269" memberName="latchButton"
                virtualName="" explicitFocusOrder="0" pos="160 8 120 24" buttonText="Latch CC Value"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <SLIDER name="new slider" id="b0d5b4052d95765c" memberName="slider" virtualName=""
          explicitFocusOrder="0" pos="440 8 128 24" thumbcol="ff9a9181"
          min="0" max="1" int="0" style="TwoValueHorizontal" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
  <LABEL name="rangeLabel" id="8be14a24ac2042d4" memberName="rangeLabel"
         virtualName="" explicitFocusOrder="0" pos="320 8 128 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Parameter Range:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="paramComboBox" id="8ff48af1860d9818" memberName="paramComboBox"
            virtualName="" explicitFocusOrder="0" pos="576 8 144 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
