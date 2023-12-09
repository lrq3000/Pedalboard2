/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  3 Feb 2012 3:10:41pm

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
//[/Headers]

#include "UserPresetWindow.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
UserPresetWindow::UserPresetWindow (KnownPluginList *knownPlugins)
    : knownPluginList(knownPlugins),
      presetList (0),
      copyButton (0),
      removeButton (0),
      importButton (0),
      exportButton (0),
      renameButton (0)
{
    addAndMakeVisible (presetList = new TreeView());
    presetList->setName (L"presetList");

    addAndMakeVisible (copyButton = new TextButton (L"copyButton"));
    copyButton->setTooltip (L"Duplicate the selected preset");
    copyButton->setButtonText (L"Copy");
    copyButton->addListener (this);

    addAndMakeVisible (removeButton = new TextButton (L"removeButton"));
    removeButton->setTooltip (L"Delete the selected preset");
    removeButton->setButtonText (L"Remove");
    removeButton->addListener (this);

    addAndMakeVisible (importButton = new TextButton (L"importButton"));
    importButton->setTooltip (L"Import a preset from an .fxp file");
    importButton->setButtonText (L"Import...");
    importButton->addListener (this);

    addAndMakeVisible (exportButton = new TextButton (L"exportButton"));
    exportButton->setTooltip (L"Export the selected preset to an .fxp file");
    exportButton->setButtonText (L"Export...");
    exportButton->addListener (this);

    addAndMakeVisible (renameButton = new TextButton (L"renameButton"));
    renameButton->setTooltip (L"Rename the selected preset");
    renameButton->setButtonText (L"Rename");
    renameButton->addListener (this);


    //[UserPreSize]

	presetList->setColour(TreeView::backgroundColourId, Colours::white);
	presetList->setRootItem(&treeRoot);
	presetList->setRootItemVisible(false);

    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

UserPresetWindow::~UserPresetWindow()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

	presetList->setRootItem(0);

    //[/Destructor_pre]

    deleteAndZero (presetList);
    deleteAndZero (copyButton);
    deleteAndZero (removeButton);
    deleteAndZero (importButton);
    deleteAndZero (exportButton);
    deleteAndZero (renameButton);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void UserPresetWindow::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffeeece1));

    g.setColour (Colour (0x40000000));
    g.fillRect (7, 7, getWidth() - 104, getHeight() - 13);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void UserPresetWindow::resized()
{
    presetList->setBounds (8, 8, getWidth() - 106, getHeight() - 15);
    copyButton->setBounds (getWidth() - 90, 40, 82, 24);
    removeButton->setBounds (getWidth() - 90, 72, 82, 24);
    importButton->setBounds (getWidth() - 90, 104, 82, 24);
    exportButton->setBounds (getWidth() - 90, 136, 82, 24);
    renameButton->setBounds (getWidth() - 90, 8, 82, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void UserPresetWindow::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == copyButton)
    {
        //[UserButtonCode_copyButton] -- add your button handler code here..

		PresetItem *selected = dynamic_cast<PresetItem *>(presetList->getSelectedItem(0));

		if(selected)
		{
			AlertWindow win(L"Copy Preset",
							L"Enter a name for the duplicate preset:",
							AlertWindow::NoIcon);

			win.addTextEditor(L"presetName", L"");

			win.addButton(L"Cancel", 0);
			win.addButton(L"Ok", 1, KeyPress(KeyPress::returnKey));

			if(win.runModalLoop())
			{
				String tempstr;
				File srcPreset = selected->getFile();

				tempstr = win.getTextEditorContents(L"presetName");
				tempstr << L".fxp";

				srcPreset.copyFileTo(srcPreset.getParentDirectory().getChildFile(tempstr));

				//A bit hacky, but treeHasChanged() doesn't seem to do anything...
				selected->getParentItem()->itemOpennessChanged(true);
				presetList->repaint();

				//treeRoot.treeHasChanged();
			}
		}

        //[/UserButtonCode_copyButton]
    }
    else if (buttonThatWasClicked == removeButton)
    {
        //[UserButtonCode_removeButton] -- add your button handler code here..

		PresetItem *selected = dynamic_cast<PresetItem *>(presetList->getSelectedItem(0));

		if(selected)
		{
			File parentDir = selected->getFile().getParentDirectory();

			if(!selected->getFile().deleteFile())
			{
				AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
												 L"Preset Deletion Error",
												 L"Could not delete preset from the filesystem. Check your permissions.");
			}
			else
			{
				//If that plugin's directory is now empty, delete it too.
				if(!parentDir.getNumberOfChildFiles(File::findFilesAndDirectories))
					parentDir.deleteFile();

				treeRoot.itemOpennessChanged(true);
				presetList->repaint();
			}
		}

        //[/UserButtonCode_removeButton]
    }
    else if (buttonThatWasClicked == importButton)
    {
        //[UserButtonCode_importButton] -- add your button handler code here..

		int i;
		StringArray plugins;

		for(i=0;i<knownPluginList->getNumTypes();++i)
			plugins.add(knownPluginList->getType(i)->name);

		//First get the user to select which plugin the preset is for.
		AlertWindow whichPlugin(L"Import Preset",
								L"Select which plugin this preset is intended for:",
								AlertWindow::NoIcon);

		whichPlugin.addComboBox(L"plugins", plugins);
		whichPlugin.addButton(L"Cancel", 0);
		whichPlugin.addButton(L"Ok", 1, KeyPress(KeyPress::returnKey));

		if(whichPlugin.runModalLoop())
		{
			FileChooser phil(L"Import preset",
							 File::nonexistent,
							 L"*.fxp");

			if(phil.browseForFileToOpen())
			{
				String pluginName = whichPlugin.getComboBoxComponent(L"plugins")->getText();
				File presetDir = File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile(L"Pedalboard2").getChildFile(L"presets");
				File pluginDir = presetDir.getChildFile(pluginName);
				File srcPreset = phil.getResult();

				if(!pluginDir.exists())
				{
					if(!pluginDir.createDirectory())
					{
						AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
														 L"Preset Import Error",
														 L"Could not create a directory for this plugin. Check your permissions.");
					}

					phil.getResult().copyFileTo(pluginDir.getChildFile(srcPreset.getFileName()));

					treeRoot.itemOpennessChanged(true);
					presetList->repaint();
				}
			}
		}

        //[/UserButtonCode_importButton]
    }
    else if (buttonThatWasClicked == exportButton)
    {
        //[UserButtonCode_exportButton] -- add your button handler code here..

		PresetItem *selected = dynamic_cast<PresetItem *>(presetList->getSelectedItem(0));

		if(selected)
		{
			FileChooser phil(L"Export preset",
							 File::nonexistent,
							 L"*.fxp");

			if(phil.browseForFileToSave(true))
				selected->getFile().copyFileTo(phil.getResult().withFileExtension(L"fxp"));
		}

        //[/UserButtonCode_exportButton]
    }
    else if (buttonThatWasClicked == renameButton)
    {
        //[UserButtonCode_renameButton] -- add your button handler code here..

		PresetItem *selected = dynamic_cast<PresetItem *>(presetList->getSelectedItem(0));

		if(selected)
		{
			AlertWindow win(L"Rename Preset",
							L"Enter a new name for the selected preset:",
							AlertWindow::NoIcon);

			win.addTextEditor(L"presetName", L"");

			win.addButton(L"Cancel", 0);
			win.addButton(L"Ok", 1, KeyPress(KeyPress::returnKey));

			if(win.runModalLoop())
			{
				File newFile = selected->getFile().getParentDirectory();

				newFile = newFile.getChildFile(win.getTextEditorContents(L"presetName")).withFileExtension(L"fxp");

				selected->getFile().moveFileTo(newFile);

				selected->getParentItem()->itemOpennessChanged(true);
				presetList->repaint();
			}
		}

        //[/UserButtonCode_renameButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="UserPresetWindow" componentName=""
                 parentClasses="public Component" constructorParams="KnownPluginList *knownPlugins"
                 variableInitialisers="knownPluginList(knownPlugins)" snapPixels="8"
                 snapActive="1" snapShown="1" overlayOpacity="0.330000013" fixedSize="0"
                 initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ffeeece1">
    <RECT pos="7 7 104M 13M" fill="solid: 40000000" hasStroke="0"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="presetList" id="edbeb49ad8d8692f" memberName="presetList"
                    virtualName="" explicitFocusOrder="0" pos="8 8 106M 15M" class="TreeView"
                    params=""/>
  <TEXTBUTTON name="copyButton" id="6f6bc4046dd45dbe" memberName="copyButton"
              virtualName="" explicitFocusOrder="0" pos="90R 40 82 24" tooltip="Duplicate the selected preset"
              buttonText="Copy" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="removeButton" id="84f0f249f8f4f48b" memberName="removeButton"
              virtualName="" explicitFocusOrder="0" pos="90R 72 82 24" tooltip="Delete the selected preset"
              buttonText="Remove" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="importButton" id="10d95fb20ceb0d60" memberName="importButton"
              virtualName="" explicitFocusOrder="0" pos="90R 104 82 24" tooltip="Import a preset from an .fxp file"
              buttonText="Import..." connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="exportButton" id="430e592d5f7332cc" memberName="exportButton"
              virtualName="" explicitFocusOrder="0" pos="90R 136 82 24" tooltip="Export the selected preset to an .fxp file"
              buttonText="Export..." connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="renameButton" id="35d44514604265cc" memberName="renameButton"
              virtualName="" explicitFocusOrder="0" pos="90R 8 82 24" tooltip="Rename the selected preset"
              buttonText="Rename" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
