/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  12 Nov 2011 4:51:55pm

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

#include "Vectors.h"
#include "ColourScheme.h"
#include "PropertiesSingleton.h"
#include "JuceHelperStuff.h"

//[/Headers]

#include "ColourSchemeEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

using namespace std;

//[/MiscUserDefs]

//==============================================================================
ColourSchemeEditor::ColourSchemeEditor ()
    : colourEditor (0),
      colourSelector (0),
      presetSelector (0),
      deleteButton (0),
      saveButton (0),
      newButton (0)
{
    addAndMakeVisible (colourEditor = new ColourSelector (ColourSelector::showAlphaChannel|ColourSelector::showColourAtTop|ColourSelector::showSliders|ColourSelector::showColourspace, 0));
    colourEditor->setName (L"colourEditor");

    addAndMakeVisible (colourSelector = new ListBox (L"colourSelector", this));
    colourSelector->setName (L"colourSelector");

    addAndMakeVisible (presetSelector = new ComboBox (L"presetSelector"));
    presetSelector->setEditableText (true);
    presetSelector->setJustificationType (Justification::centredLeft);
    presetSelector->setTextWhenNothingSelected (String::empty);
    presetSelector->setTextWhenNoChoicesAvailable (L"(no choices)");
    presetSelector->addListener (this);

    addAndMakeVisible (deleteButton = new DrawableButton (L"deleteButton", DrawableButton::ImageOnButtonBackground));
    deleteButton->setName (L"deleteButton");

    addAndMakeVisible (saveButton = new DrawableButton (L"saveButton", DrawableButton::ImageOnButtonBackground));
    saveButton->setName (L"saveButton");

    addAndMakeVisible (newButton = new DrawableButton (L"newButton", DrawableButton::ImageOnButtonBackground));
    newButton->setName (L"newButton");


    //[UserPreSize]

	Colour tempCol = ColourScheme::getInstance().colours[L"Button Colour"];
	ScopedPointer<Drawable> newImage(loadSVGFromMemory(Vectors::newbutton_svg,
													   Vectors::newbutton_svgSize));
	ScopedPointer<Drawable> saveImage(loadSVGFromMemory(Vectors::savebutton_svg,
													    Vectors::savebutton_svgSize));
	ScopedPointer<Drawable> deleteImage(loadSVGFromMemory(Vectors::deletebutton_svg,
													      Vectors::deletebutton_svgSize));
	
	newButton->setImages(newImage);
	newButton->setColour(DrawableButton::backgroundColourId, tempCol);
	newButton->setColour(DrawableButton::backgroundOnColourId, tempCol);
	newButton->setTooltip(L"New colour scheme");
	saveButton->setImages(saveImage);
	saveButton->setColour(DrawableButton::backgroundColourId, tempCol);
	saveButton->setColour(DrawableButton::backgroundOnColourId, tempCol);
	saveButton->setTooltip(L"Save current colour scheme");
	deleteButton->setImages(deleteImage);
	deleteButton->setColour(DrawableButton::backgroundColourId, tempCol);
	deleteButton->setColour(DrawableButton::backgroundOnColourId, tempCol);
	deleteButton->setTooltip(L"Delete selected colour scheme");

	colourEditor->setColour(ColourSelector::backgroundColourId, tempCol);

	colourSelector->setOutlineThickness(1);
	colourSelector->setColour(ListBox::outlineColourId, Colour(0x60000000));

	colourSelector->updateContent();
	colourSelector->selectRow(0);
	colourEditor->setCurrentColour(ColourScheme::getInstance().colours.begin()->second);
	colourEditor->addChangeListener(this);

	//Fill out the preset combo box.
	int i;
	Array<File> presets;
	File settingsDir = JuceHelperStuff::getAppDataFolder();

	settingsDir.findChildFiles(presets, File::findFiles, false, L"*.colourscheme");
	for(i=0;i<presets.size();++i)
	{
		String tempstr = presets[i].getFileNameWithoutExtension();

		presetSelector->addItem(tempstr, i+1);
		if(tempstr == ColourScheme::getInstance().presetName)
			presetSelector->setSelectedId(i+1, true);
	}

	newButton->addListener(this);
	saveButton->addListener(this);
	deleteButton->addListener(this);

    //[/UserPreSize]

    setSize (550, 375);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

ColourSchemeEditor::~ColourSchemeEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	//Check if the selected preset has been saved.
	if(!ColourScheme::getInstance().doesColoursMatchPreset(presetSelector->getText()))
	{
		if(AlertWindow::showOkCancelBox(AlertWindow::WarningIcon,
										L"Colour scheme not saved",
										L"Save current scheme?",
										L"Yes",
										L"No"))
		{
			ColourScheme::getInstance().savePreset(presetSelector->getText());
		}
	}

	//Save the selected preset to the properties file.
	PropertiesSingleton::getInstance().getUserSettings()->setValue(L"colourScheme",
																   presetSelector->getText());

    //[/Destructor_pre]

    deleteAndZero (colourEditor);
    deleteAndZero (colourSelector);
    deleteAndZero (presetSelector);
    deleteAndZero (deleteButton);
    deleteAndZero (saveButton);
    deleteAndZero (newButton);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ColourSchemeEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffeeece1));

    //[UserPaint] Add your own custom painting code here..

	g.fillAll(ColourScheme::getInstance().colours[L"Window Background"]);

    //[/UserPaint]
}

void ColourSchemeEditor::resized()
{
    colourEditor->setBounds (192, 40, getWidth() - 200, getHeight() - 48);
    colourSelector->setBounds (8, 40, 176, getHeight() - 48);
    presetSelector->setBounds (8, 8, getWidth() - 106, 24);
    deleteButton->setBounds (getWidth() - 32, 8, 24, 24);
    saveButton->setBounds (getWidth() - 62, 8, 24, 24);
    newButton->setBounds (getWidth() - 92, 8, 24, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ColourSchemeEditor::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == presetSelector)
    {
        //[UserComboBoxCode_presetSelector] -- add your combo box handling code here..

		ColourScheme::getInstance().loadPreset(presetSelector->getText());

		//Update colourEditor.
		{
			int i = 0;
			map<String, Colour>::iterator it;
			int row = colourSelector->getSelectedRow();
			map<String, Colour>& colours = ColourScheme::getInstance().colours;

			for(it=colours.begin();it!=colours.end();++it,++i)
			{
				if(i == row)
				{
					colourEditor->setCurrentColour(it->second);
					break;
				}
			}
		}

		repaint();

        //[/UserComboBoxCode_presetSelector]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
int ColourSchemeEditor::getNumRows()
{
	return ColourScheme::getInstance().colours.size();
}

//------------------------------------------------------------------------------
void ColourSchemeEditor::paintListBoxItem(int rowNumber,
										  Graphics &g,
										  int width,
										  int height,
										  bool rowIsSelected)
{
	int i = 0;
	map<String, Colour>::iterator it;
	map<String, Colour>& colours = ColourScheme::getInstance().colours;

	g.fillAll(colours[L"Dialog Inner Background"]);

	for(it=colours.begin();it!=colours.end();++it,++i)
	{
		if(i == rowNumber)
		{
			//Colour in the background.
			if(rowIsSelected)
			{
				ColourGradient basil(colours[L"List Selected Colour"].brighter(0.4f),
									 0.0f,
									 0.0f,
									 colours[L"List Selected Colour"].darker(0.125f),
									 0.0f,
									 (float)height,
									 false);

				g.setGradientFill(basil);

				g.fillAll();

				g.setColour(colours[L"List Selected Colour"].contrasting());
			}
			else
			{
				g.fillAll(it->second);

				g.setColour(it->second.contrasting());
			}

			//And draw the colour's name.
			g.drawSingleLineText(it->first, 4, 12);

			break;
		}
	}
}

//------------------------------------------------------------------------------
void ColourSchemeEditor::listBoxItemClicked(int row, const MouseEvent &e)
{
	int i = 0;
	map<String, Colour>::iterator it;
	map<String, Colour>& colours = ColourScheme::getInstance().colours;

	for(it=colours.begin();it!=colours.end();++it,++i)
	{
		if(i == row)
		{
			colourEditor->setCurrentColour(it->second);
			break;
		}
	}
}

//------------------------------------------------------------------------------
void ColourSchemeEditor::buttonClicked(Button *button)
{
	if(button == newButton)
	{
		presetSelector->addItem(L"New Colour Scheme", presetSelector->getNumItems()+1);
		presetSelector->setSelectedId(presetSelector->getNumItems());
	}
	else if(button == saveButton)
	{
		ColourScheme::getInstance().savePreset(presetSelector->getText());
	}
	else if(button == deleteButton)
	{
		if(presetSelector->getNumItems() > 1)
		{
			int i;
			File tempFile;
			String tempstr;
			StringArray presetsArray;
			String presetName = presetSelector->getText();
			File settingsDir = JuceHelperStuff::getAppDataFolder();

			tempstr << presetName << L".colourscheme";
			tempFile = settingsDir.getChildFile(tempstr);
			tempFile.deleteFile();

			if(presetSelector->getSelectedId() > 1)
			{
				for(i=0;i<presetSelector->getNumItems();++i)
				{
					tempstr = presetSelector->getItemText(i);
					if(presetSelector->getText() != tempstr)
						presetsArray.add(tempstr);
				}

				presetSelector->clear();
				for(i=0;i<presetsArray.size();++i)
					presetSelector->addItem(presetsArray[i], i+1);
				presetSelector->setSelectedId(1);
			}
		}
	}
}

//------------------------------------------------------------------------------
void ColourSchemeEditor::changeListenerCallback(ChangeBroadcaster *source)
{
	if(source == colourEditor)
	{
		int i = 0;
		map<String, Colour>::iterator it;
		int row = colourSelector->getSelectedRow();
		map<String, Colour>& colours = ColourScheme::getInstance().colours;

		for(it=colours.begin();it!=colours.end();++it,++i)
		{
			if(i == row)
			{
				Colour tempCol = ColourScheme::getInstance().colours[L"Button Colour"];

				it->second = colourEditor->getCurrentColour();
				colourSelector->updateContent();
				newButton->setColour(DrawableButton::backgroundColourId, tempCol);
				newButton->setColour(DrawableButton::backgroundOnColourId, tempCol);
				saveButton->setColour(DrawableButton::backgroundColourId, tempCol);
				saveButton->setColour(DrawableButton::backgroundOnColourId, tempCol);
				deleteButton->setColour(DrawableButton::backgroundColourId, tempCol);
				deleteButton->setColour(DrawableButton::backgroundOnColourId, tempCol);
				colourEditor->setColour(ColourSelector::backgroundColourId,
										ColourScheme::getInstance().colours[L"Window Background"]);
				repaint();
				sendChangeMessage();

				break;
			}

		}
	}
}

//------------------------------------------------------------------------------
Drawable *ColourSchemeEditor::loadSVGFromMemory(const void *dataToInitialiseFrom,
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

<JUCER_COMPONENT documentType="Component" className="ColourSchemeEditor" componentName=""
                 parentClasses="public Component, public ListBoxModel, public Button::Listener, public ChangeListener, public ChangeBroadcaster"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330000013" fixedSize="0" initialWidth="550"
                 initialHeight="375">
  <BACKGROUND backgroundColour="ffeeece1"/>
  <GENERICCOMPONENT name="colourEditor" id="76f6a0998cb5fd5a" memberName="colourEditor"
                    virtualName="" explicitFocusOrder="0" pos="192 40 200M 48M" class="ColourSelector"
                    params="ColourSelector::showAlphaChannel|ColourSelector::showColourAtTop|ColourSelector::showSliders|ColourSelector::showColourspace, 0"/>
  <GENERICCOMPONENT name="colourSelector" id="b04a23ed5df37b97" memberName="colourSelector"
                    virtualName="" explicitFocusOrder="0" pos="8 40 176 48M" class="ListBox"
                    params="L&quot;colourSelector&quot;, this"/>
  <COMBOBOX name="presetSelector" id="e3904e7675c2dda5" memberName="presetSelector"
            virtualName="" explicitFocusOrder="0" pos="8 8 106M 24" editable="1"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <GENERICCOMPONENT name="deleteButton" id="c95b5e735fa3fb80" memberName="deleteButton"
                    virtualName="" explicitFocusOrder="0" pos="32R 8 24 24" class="DrawableButton"
                    params="L&quot;deleteButton&quot;, DrawableButton::ImageOnButtonBackground"/>
  <GENERICCOMPONENT name="saveButton" id="dff06a6e15c27b5f" memberName="saveButton"
                    virtualName="" explicitFocusOrder="0" pos="62R 8 24 24" class="DrawableButton"
                    params="L&quot;saveButton&quot;, DrawableButton::ImageOnButtonBackground"/>
  <GENERICCOMPONENT name="newButton" id="71d36375f7c39432" memberName="newButton"
                    virtualName="" explicitFocusOrder="0" pos="92R 8 24 24" class="DrawableButton"
                    params="L&quot;newButton&quot;, DrawableButton::ImageOnButtonBackground"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
