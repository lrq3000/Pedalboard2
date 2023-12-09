/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  18 Oct 2012 11:38:06am

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

#include "MappingEntryOsc.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
MappingEntryOsc::MappingEntryOsc (MappingsDialog *dlg, int arrayIndex, const String& oscAddress, int oscParam, const StringArray& possibleAddresses)
    : paramComboBox (0),
      addressLabel (0),
      addressEditor (0),
      oscParamLabel (0),
      oscParamSlider (0)
{
    addAndMakeVisible (paramComboBox = new ComboBox (L"paramComboBox"));
    paramComboBox->setEditableText (false);
    paramComboBox->setJustificationType (Justification::centredLeft);
    paramComboBox->setTextWhenNothingSelected (String::empty);
    paramComboBox->setTextWhenNoChoicesAvailable (L"(no choices)");
    paramComboBox->addListener (this);

    addAndMakeVisible (addressLabel = new Label (L"addressLabel",
                                                 L"OSC Address:"));
    addressLabel->setFont (Font (15.0000f, Font::plain));
    addressLabel->setJustificationType (Justification::centredLeft);
    addressLabel->setEditable (false, false, false);
    addressLabel->setColour (TextEditor::textColourId, Colours::black);
    addressLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (addressEditor = new ComboBox (L"addressEditor"));
    addressEditor->setEditableText (true);
    addressEditor->setJustificationType (Justification::centredLeft);
    addressEditor->setTextWhenNothingSelected (String::empty);
    addressEditor->setTextWhenNoChoicesAvailable (L"(no choices)");
    addressEditor->addListener (this);

    addAndMakeVisible (oscParamLabel = new Label (L"oscParamLabel",
                                                  L"Parameter:"));
    oscParamLabel->setFont (Font (15.0000f, Font::plain));
    oscParamLabel->setJustificationType (Justification::centredLeft);
    oscParamLabel->setEditable (false, false, false);
    oscParamLabel->setColour (TextEditor::textColourId, Colours::black);
    oscParamLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (oscParamSlider = new Slider (L"oscParamSlider"));
    oscParamSlider->setRange (0, 100, 1);
    oscParamSlider->setSliderStyle (Slider::IncDecButtons);
    oscParamSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 30, 20);
    oscParamSlider->addListener (this);


    //[UserPreSize]

	int i;
	bool textExists = false;

	setInterceptsMouseClicks(false, true);

	for(i=0;i<possibleAddresses.size();++i)
		addressEditor->addItem(possibleAddresses[i], i+1);

	if(oscAddress != "")
	{
		for(i=0;i<addressEditor->getNumItems();++i)
		{
			if(addressEditor->getItemText(i) == oscAddress)
			{
				addressEditor->setSelectedId(i+1);
				textExists = true;
				break;
			}
		}
		if(!textExists)
		{
			addressEditor->addItem(oscAddress, addressEditor->getNumItems()+1);
			addressEditor->setSelectedId(addressEditor->getNumItems());
		}
	}

	addressEditor->addListener(this);

	mappingsDialog = dlg;
	index = arrayIndex;

	addressLabel->setColour(TextEditor::textColourId,
							ColourScheme::getInstance().colours["Text Colour"]);
	oscParamLabel->setColour(TextEditor::textColourId,
							 ColourScheme::getInstance().colours["Text Colour"]);
	oscParamSlider->setColour(Slider::textBoxTextColourId,
							  ColourScheme::getInstance().colours["Text Colour"]);
	oscParamSlider->setColour(Slider:: textBoxBackgroundColourId,
							  ColourScheme::getInstance().colours["Text Editor Colour"]);

	oscParamSlider->setValue((double)oscParam, dontSendNotification);

    //[/UserPreSize]

    setSize (728, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

MappingEntryOsc::~MappingEntryOsc()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (paramComboBox);
    deleteAndZero (addressLabel);
    deleteAndZero (addressEditor);
    deleteAndZero (oscParamLabel);
    deleteAndZero (oscParamSlider);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MappingEntryOsc::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..

	g.setColour(ColourScheme::getInstance().colours[L"Vector Colour"]);

    //[/UserPrePaint]

    g.setColour (Colour (0x80000000));
    g.strokePath (internalPath1, PathStrokeType (5.0000f, PathStrokeType::curved, PathStrokeType::rounded));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MappingEntryOsc::resized()
{
    paramComboBox->setBounds (576, 8, 144, 24);
    addressLabel->setBounds (8, 8, 96, 24);
    addressEditor->setBounds (104, 8, 272, 24);
    oscParamLabel->setBounds (376, 8, 80, 24);
    oscParamSlider->setBounds (456, 8, 80, 24);
    internalPath1.clear();
    internalPath1.startNewSubPath (556.0f, 12.0f);
    internalPath1.lineTo (562.0f, 20.0f);
    internalPath1.lineTo (556.0f, 28.0f);

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MappingEntryOsc::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == paramComboBox)
    {
        //[UserComboBoxCode_paramComboBox] -- add your combo box handling code here..

		mappingsDialog->setParameter(index, paramComboBox->getSelectedId()-1);

        //[/UserComboBoxCode_paramComboBox]
    }
    else if (comboBoxThatHasChanged == addressEditor)
    {
        //[UserComboBoxCode_addressEditor] -- add your combo box handling code here..

		int i;
		bool textExists = false;
		String tempstr = addressEditor->getText();

		if(tempstr != "")
		{
			//Add the new text to the combobox if the user typed it in.
			for(i=0;i<addressEditor->getNumItems();++i)
			{
				if(addressEditor->getItemText(i) == tempstr)
				{
					textExists = true;
					break;
				}
			}
			if(!textExists)
				addressEditor->addItem(tempstr, addressEditor->getNumItems()+1);

			//Update the OscManager.
			mappingsDialog->setAddress(index, addressEditor->getText());
		}

        //[/UserComboBoxCode_addressEditor]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void MappingEntryOsc::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == oscParamSlider)
    {
        //[UserSliderCode_oscParamSlider] -- add your slider handling code here..

		mappingsDialog->setOscParameter(index, (int)sliderThatWasMoved->getValue());

        //[/UserSliderCode_oscParamSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
void MappingEntryOsc::textEditorTextChanged(TextEditor &editor)
{
	mappingsDialog->setAddress(index, editor.getText());
}

//------------------------------------------------------------------------------
void MappingEntryOsc::textEditorReturnKeyPressed(TextEditor &editor)
{
	mappingsDialog->setAddress(index, editor.getText());
}

//------------------------------------------------------------------------------
void MappingEntryOsc::textEditorEscapeKeyPressed(TextEditor &editor)
{
	mappingsDialog->updateListBox();
}

//------------------------------------------------------------------------------
void MappingEntryOsc::textEditorFocusLost(TextEditor &editor)
{
	mappingsDialog->setAddress(index, editor.getText());
}

//------------------------------------------------------------------------------
void MappingEntryOsc::addParameter(const String& param)
{
	String paramName = param;

	if(paramName.isEmpty())
		paramName = "<no name>";

	paramComboBox->addItem(paramName, paramComboBox->getNumItems()+1);
}

//------------------------------------------------------------------------------
void MappingEntryOsc::selectParameter(int index)
{
	paramComboBox->setSelectedId(index+1, true);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MappingEntryOsc" componentName=""
                 parentClasses="public Component" constructorParams="MappingsDialog *dlg, int arrayIndex, const String&amp; oscAddress, int oscParam, const StringArray&amp; possibleAddresses"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330000013" fixedSize="0" initialWidth="728"
                 initialHeight="400">
  <BACKGROUND backgroundColour="ffffff">
    <PATH pos="0 0 100 100" fill="solid: 2aa558" hasStroke="1" stroke="5, curved, rounded"
          strokeColour="solid: 80000000" nonZeroWinding="1">s 556 12 l 562 20 l 556 28</PATH>
  </BACKGROUND>
  <COMBOBOX name="paramComboBox" id="8ff48af1860d9818" memberName="paramComboBox"
            virtualName="" explicitFocusOrder="0" pos="576 8 144 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="addressLabel" id="22a7baa38ed4e012" memberName="addressLabel"
         virtualName="" explicitFocusOrder="0" pos="8 8 96 24" edTextCol="ff000000"
         edBkgCol="0" labelText="OSC Address:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="addressEditor" id="78d260c96bcdfcd3" memberName="addressEditor"
            virtualName="" explicitFocusOrder="0" pos="104 8 272 24" editable="1"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="oscParamLabel" id="d441633836d6eab8" memberName="oscParamLabel"
         virtualName="" explicitFocusOrder="0" pos="376 8 80 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Parameter:" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <SLIDER name="oscParamSlider" id="19626f92c550f381" memberName="oscParamSlider"
          virtualName="" explicitFocusOrder="0" pos="456 8 80 24" min="0"
          max="100" int="1" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="30" textBoxHeight="20" skewFactor="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
