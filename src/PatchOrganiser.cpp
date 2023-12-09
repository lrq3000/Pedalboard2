/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  5 Jun 2011 2:06:45pm

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

#include "MainPanel.h"
#include "ColourScheme.h"

//[/Headers]

#include "PatchOrganiser.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

using namespace std;

//[/MiscUserDefs]

//==============================================================================
PatchOrganiser::PatchOrganiser (MainPanel *panel, Array<XmlElement *>& patchArray)
    : mainPanel(panel), patches(patchArray),
      patchList (0),
      addButton (0),
      copyButton (0),
      removeButton (0),
      moveUpButton (0),
      moveDownButton (0),
      importButton (0)
{
    addAndMakeVisible (patchList = new ListBox (L"patchList", this));
    patchList->setName (L"patchList");

    addAndMakeVisible (addButton = new TextButton (L"addButton"));
    addButton->setButtonText (L"Add");
    addButton->addListener (this);

    addAndMakeVisible (copyButton = new TextButton (L"copyButton"));
    copyButton->setButtonText (L"Copy");
    copyButton->addListener (this);

    addAndMakeVisible (removeButton = new TextButton (L"removeButton"));
    removeButton->setButtonText (L"Remove");
    removeButton->addListener (this);

    addAndMakeVisible (moveUpButton = new TextButton (L"moveUpButton"));
    moveUpButton->setButtonText (L"Move Up");
    moveUpButton->addListener (this);

    addAndMakeVisible (moveDownButton = new TextButton (L"moveDownButton"));
    moveDownButton->setButtonText (L"Move Down");
    moveDownButton->addListener (this);

    addAndMakeVisible (importButton = new TextButton (L"importButton"));
    importButton->setButtonText (L"Import...");
    importButton->addListener (this);


    //[UserPreSize]

	addButton->setTooltip(L"Add a new patch");
	copyButton->setTooltip(L"Duplicate selected patch");
	removeButton->setTooltip(L"Delete selected patch");
	moveUpButton->setTooltip(L"Move patch up");
	moveDownButton->setTooltip(L"Move patch down");
	importButton->setTooltip(L"Import patch from another .pdl file");

	patchList->setOutlineThickness(1);
	patchList->setMultipleSelectionEnabled(true);
	patchList->setColour(ListBox::backgroundColourId,
						 ColourScheme::getInstance().colours[L"Dialog Inner Background"]);

    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

PatchOrganiser::~PatchOrganiser()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (patchList);
    deleteAndZero (addButton);
    deleteAndZero (copyButton);
    deleteAndZero (removeButton);
    deleteAndZero (moveUpButton);
    deleteAndZero (moveDownButton);
    deleteAndZero (importButton);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void PatchOrganiser::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..

	g.fillAll(ColourScheme::getInstance().colours[L"Window Background"]);

    //[/UserPrePaint]

    //g.fillAll (Colour (0xffeeece1));

    g.setColour (Colour (0x40000000));
    g.fillRect (getWidth() - 89, 140, 80, 1);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void PatchOrganiser::resized()
{
    patchList->setBounds (8, 8, getWidth() - 106, getHeight() - 15);
    addButton->setBounds (getWidth() - 90, 8, 82, 24);
    copyButton->setBounds (getWidth() - 90, 40, 82, 24);
    removeButton->setBounds (getWidth() - 90, 72, 82, 24);
    moveUpButton->setBounds (getWidth() - 90, 152, 82, 24);
    moveDownButton->setBounds (getWidth() - 90, 184, 82, 24);
    importButton->setBounds (getWidth() - 90, 104, 82, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void PatchOrganiser::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]

	int i, j;

    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == addButton)
    {
        //[UserButtonCode_addButton] -- add your button handler code here..

		int currentId;
		ComboBox *patchComboBox = mainPanel->getPatchComboBox();

		currentId = patchComboBox->getSelectedId();

		//Creates the new patch, updates the combo box, loads the new patch.
		//(we do it this way because ComboBox uses an AsyncUpdater to alert any
		// listeners to its state change, which means most times
		// comboBoxChanged() will not get called before
		// patchList->updateContent(). Which is a pain in the bum.)
		patchComboBox->setSelectedId(patchComboBox->getNumItems(), false);
		mainPanel->comboBoxChanged(patchComboBox);

		patchList->updateContent();
		repaint();

        //[/UserButtonCode_addButton]
    }
    else if (buttonThatWasClicked == copyButton)
    {
        //[UserButtonCode_copyButton] -- add your button handler code here..
		for(i=0;i<patchList->getNumSelectedRows();++i)
		{
			mainPanel->duplicatePatch(patchList->getSelectedRow(i));

			patchList->updateContent();
			repaint();
		}
        //[/UserButtonCode_copyButton]
    }
    else if (buttonThatWasClicked == removeButton)
    {
        //[UserButtonCode_removeButton] -- add your button handler code here..

		ComboBox *patchComboBox = mainPanel->getPatchComboBox();

		if(patches.size() > 1)
		{
			//Delete the selected patches.
			for(i=(patchList->getNumSelectedRows()-1);i>=0;--i)
			{
				//Make sure the user can't delete the last patch.
				if(patches.size() == 1)
					break;

				j = patchList->getSelectedRow(i);

				//Switch the active patch if we're deleting it.
				if(patchComboBox->getSelectedItemIndex() == j)
				{
					if(j > 0)
					{
						patchComboBox->setSelectedItemIndex(j-1, true);
						mainPanel->comboBoxChanged(patchComboBox);
					}
					else
					{
						patchComboBox->setSelectedItemIndex(j+1, true);
						mainPanel->comboBoxChanged(patchComboBox);
					}
				}

				//Delete the patch.
				delete patches[j];
				patches.remove(j);
			}

			//Update the combobox.
			patchComboBox->clear(true);
			for(i=0;i<patches.size();++i)
				patchComboBox->addItem(patches[i]->getStringAttribute("name"), i+1);
			patchComboBox->addItem("<new patch>", patches.size()+1);
			patchComboBox->setSelectedId(1);

			//Update the list box accordingly.
			patchList->updateContent();
			repaint();
		}

        //[/UserButtonCode_removeButton]
    }
    else if (buttonThatWasClicked == moveUpButton)
    {
        //[UserButtonCode_moveUpButton] -- add your button handler code here..

		ComboBox *patchComboBox = mainPanel->getPatchComboBox();

		XmlElement *e1;
		XmlElement *e2;
		int tempint;

		//for(i=0;i<patchList->getNumSelectedRows();++i)
		{
			tempint = patchList->getSelectedRow(0);

			//Swap the two patches.
			if(tempint > 0)
			{
				e1 = patches[tempint];
				e2 = patches[tempint-1];

				patches.set(tempint-1, e1);
				patches.set(tempint, e2);

				patchList->selectRow(patchList->getSelectedRow(0)-1);
			}
		}

		//Update the combo box.
		patchComboBox->clear(true);
		for(i=0;i<patches.size();++i)
			patchComboBox->addItem(patches[i]->getStringAttribute("name"), i+1);
		patchComboBox->addItem("<new patch>", patches.size()+1);
		mainPanel->nextSwitchDoNotSavePrev();
		patchComboBox->setSelectedId(1);

		//Update the list box.
		patchList->updateContent();
		repaint();

        //[/UserButtonCode_moveUpButton]
    }
    else if (buttonThatWasClicked == moveDownButton)
    {
        //[UserButtonCode_moveDownButton] -- add your button handler code here..

		ComboBox *patchComboBox = mainPanel->getPatchComboBox();

		XmlElement *e1;
		XmlElement *e2;
		XmlElement *e3;
		XmlElement *e4;
		int tempint;

		//for(i=(patchList->getNumSelectedRows()-1);i>=0;--i)
		{
			tempint = patchList->getSelectedRow(0);

			//Swap the two patches.
			if(tempint < (patches.size()-1))
			{
				e1 = patches[tempint];
				e2 = patches[tempint+1];

				patches.set(tempint+1, e1);
				patches.set(tempint, e2);

				e3 = patches[tempint];
				e4 = patches[tempint+1];

				patchList->selectRow(patchList->getSelectedRow(0)+1);
			}
		}

		//Update the combo box.
		patchComboBox->clear(true);
		for(i=0;i<patches.size();++i)
			patchComboBox->addItem(patches[i]->getStringAttribute("name"), i+1);
		patchComboBox->addItem("<new patch>", patches.size()+1);
		mainPanel->nextSwitchDoNotSavePrev();
		patchComboBox->setSelectedId(1);

		//Update the list box.
		patchList->updateContent();
		repaint();

        //[/UserButtonCode_moveDownButton]
    }
    else if (buttonThatWasClicked == importButton)
    {
        //[UserButtonCode_importButton] -- add your button handler code here..

		FileChooser phil(L"Select file to import patch from...",
						 File::nonexistent,
						 L"*.pdl");

		if(phil.browseForFileToOpen())
		{
			File philResult = phil.getResult();
			XmlDocument doc(philResult);
			ScopedPointer<XmlElement> root(doc.getDocumentElement());

			if(root)
			{
				String tempstr;
				StringArray patchNames;
				XmlElement *tempEl = 0;

				forEachXmlChildElementWithTagName(*root, tempEl2, "Patch")
				{
					patchNames.add(tempEl2->getStringAttribute("name"));
				}

				tempstr << "Patches in file: " << philResult.getFileName();
				AlertWindow win(L"Select patch...",
								tempstr,
								AlertWindow::NoIcon);

				win.addComboBox("patchName", patchNames);
				win.addButton(L"OK", 1, KeyPress(KeyPress::returnKey));
				win.addButton(L"Cancel", 0, KeyPress(KeyPress::escapeKey));

				if(win.runModalLoop())
				{
					//Get the XmlElement the user selected.
					int index = win.getComboBoxComponent("patchName")->getSelectedItemIndex();
					forEachXmlChildElementWithTagName(*root, tempEl2, "Patch")
					{
						if(tempEl2->getStringAttribute("name") == patchNames[index])
						{
							tempEl = tempEl2;
							break;
						}
					}

					//Copy it to a new XmlElement *
					XmlElement *newPatch = new XmlElement(*tempEl);

					mainPanel->addPatch(newPatch);

					patchList->updateContent();
					repaint();
				}
			}
		}

        //[/UserButtonCode_importButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

//------------------------------------------------------------------------------
int PatchOrganiser::getNumRows()
{
	return patches.size();
}

//------------------------------------------------------------------------------
void PatchOrganiser::paintListBoxItem(int rowNumber,
									  Graphics &g,
									  int width,
									  int height,
									  bool rowIsSelected)
{
	map<String, Colour>& colours = ColourScheme::getInstance().colours;

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
	}
	else if(rowNumber%2)
		g.fillAll(Colour(0x10000000));
}

//------------------------------------------------------------------------------
Component *PatchOrganiser::refreshComponentForRow(int rowNumber,
												  bool isRowSelected,
												  Component *existingComponentToUpdate)
{
	String tempstr;
	Label *retval = dynamic_cast<Label *>(existingComponentToUpdate);

	if(rowNumber >= patches.size())
	{
		if(existingComponentToUpdate)
			delete existingComponentToUpdate;
		return 0;
	}

	if(!retval)
	{
		retval = new Label();
		retval->setEditable(false, true); //Double-click to edit, single-click to select.
		retval->addListener(this);
		retval->setInterceptsMouseClicks(false, true);
	}

	retval->setText(patches[rowNumber]->getStringAttribute("name"), dontSendNotification);
	tempstr << rowNumber;
	retval->setName(tempstr); //So we know which patch to update in labelTextChanged().

	if(isRowSelected)
		retval->setColour(Label::textColourId, Colour(0xFFFFFFFF));
	else
		retval->setColour(Label::textColourId, Colour(0xFF000000));

	return retval;
}

//------------------------------------------------------------------------------
void PatchOrganiser::listBoxItemClicked(int row, const MouseEvent &e)
{
	patchList->selectRow(row, false, !e.mods.isCtrlDown());

	if(e.getNumberOfClicks() == 2)
		((Label *)patchList->getComponentForRowNumber(row))->showEditor();
}

//------------------------------------------------------------------------------
void PatchOrganiser::backgroundClicked()
{
	patchList->deselectAllRows();
}

//------------------------------------------------------------------------------
void PatchOrganiser::labelTextChanged(Label *labelThatHasChanged)
{
	int index = labelThatHasChanged->getName().getIntValue();
	ComboBox *patchComboBox = mainPanel->getPatchComboBox();

	if((index > -1) && (index < patches.size()))
	{
		patches[index]->setAttribute("name", labelThatHasChanged->getText());
		patchComboBox->changeItemText(index+1, labelThatHasChanged->getText());
		if(patchComboBox->getSelectedItemIndex() == index)
			patchComboBox->setSelectedId(index+1);
	}
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PatchOrganiser" componentName=""
                 parentClasses="public Component, public ListBoxModel, public Label::Listener"
                 constructorParams="MainPanel *panel, Array&lt;XmlElement *&gt;&amp; patchArray"
                 variableInitialisers="mainPanel(panel), patches(patchArray)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ffeeece1">
    <RECT pos="89R 140 80 1" fill="solid: 40000000" hasStroke="0"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="patchList" id="3e27a75ef7faac5f" memberName="patchList"
                    virtualName="" explicitFocusOrder="0" pos="8 8 106M 15M" class="ListBox"
                    params="&quot;patchList&quot;, this"/>
  <TEXTBUTTON name="addButton" id="997ee36c9cdfc358" memberName="addButton"
              virtualName="" explicitFocusOrder="0" pos="90R 8 82 24" buttonText="Add"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="copyButton" id="bffd955ef9ba38b9" memberName="copyButton"
              virtualName="" explicitFocusOrder="0" pos="90R 40 82 24" buttonText="Copy"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="removeButton" id="6f6b296cd7a4761b" memberName="removeButton"
              virtualName="" explicitFocusOrder="0" pos="90R 72 82 24" buttonText="Remove"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="moveUpButton" id="992f47bf383c5905" memberName="moveUpButton"
              virtualName="" explicitFocusOrder="0" pos="90R 152 82 24" buttonText="Move Up"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="moveDownButton" id="d22a9df4cdb856de" memberName="moveDownButton"
              virtualName="" explicitFocusOrder="0" pos="90R 184 82 24" buttonText="Move Down"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="importButton" id="1c2c6c20053653c" memberName="importButton"
              virtualName="" explicitFocusOrder="0" pos="90R 104 82 24" buttonText="Import..."
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
