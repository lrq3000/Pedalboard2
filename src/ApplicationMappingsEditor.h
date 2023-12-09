//	ApplicationMappingsEditor.h - Component used to edit the application
//								  mappings.
//	----------------------------------------------------------------------------
//	This file is part of Pedalboard2, an audio plugin host.
//	Copyright (c) 2011 Niall Moody.
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//	----------------------------------------------------------------------------

#ifndef APPLICATIONMAPPINGSEDITOR_H_
#define APPLICATIONMAPPINGSEDITOR_H_

#include "../JuceLibraryCode/JuceHeader.h"
#include "MidiMappingManager.h"
#include "MidiCcAlertWindow.h"
#include "OscMappingManager.h"
#include "JuceHelperStuff.h"
#include "ColourScheme.h"
#include "Vectors.h"

class OscMappingManager;
class MidiMappingManager;

///	Component used to edit the application mappings.
class ApplicationMappingsEditor : public Component,
								  public Button::Listener
{
  public:
	///	Constructor.
	ApplicationMappingsEditor(ApplicationCommandManager *app,
							  MidiMappingManager *midi,
							  OscMappingManager *osc);
	///	Destructor.
	~ApplicationMappingsEditor();

	///	Fills the background the correct colour.
	void paint(Graphics& g);

	///	So we can resize the child components accordingly.
	void resized();

	///	So we can reset the mappings to their defaults.
	void buttonClicked(Button *button);

	juce_UseDebuggingNewOperator
  private:
	///	The app's ApplicationCommandManager.
	ApplicationCommandManager *appManager;
	///	The app's MidiMappingManager.
	MidiMappingManager *midiManager;
	///	The app's OscMappingManager.
	OscMappingManager *oscManager;

	///	The main tree view.
	TreeView *mappingsTree;
	///	Button to reset all the mappings.
	TextButton *resetButton;

	///	The actual mapping item in the mappingsTree.
	class MappingItem : public TreeViewItem
	{
	  private:
		class MappingItemButtons;
	  public:
		///	Constructor.
		MappingItem(const String& commandName,
					CommandID commandId,
					ApplicationCommandManager *app,
					MidiMappingManager *midi,
					OscMappingManager *osc):
		name(commandName),
		id(commandId),
		appManager(app),
		midiManager(midi),
		oscManager(osc)
		{
			
		};
		///	Destructor.
		~MappingItem() {};

		///	Returns false, obviously.
		bool mightContainSubItems() {return false;};

		///	Draws the item.
		void paintItem(Graphics &g, int width, int height)
		{
			g.setColour(ColourScheme::getInstance().colours[L"Text Colour"]);
			g.setFont(14.0f);
			g.drawText(name, 0, 0, width, height, Justification::centredLeft, false);
		};

		///	Adds the buttons used to edit the command's mappings.
		Component *createItemComponent()
		{
			return new MappingItemButtons(id,
										  appManager,
										  midiManager,
										  oscManager);
		};
	  private:
		///	The command's name.
		String name;
		///	The command's id.
		CommandID id;

		///	The app's ApplicationCommandManager.
		ApplicationCommandManager *appManager;
		///	The app's MidiMappingManager.
		MidiMappingManager *midiManager;
		///	The app's OscMappingManager.
		OscMappingManager *oscManager;

		///	Component holding the buttons to edit the command's mappings.
		class MappingItemButtons : public Component,
								   public Button::Listener,
								   public KeyListener
		{
		  public:
			///	Constructor.
			MappingItemButtons(CommandID commandId,
							   ApplicationCommandManager *app,
							   MidiMappingManager *midi,
							   OscMappingManager *osc):
			id(commandId),
			appManager(app),
			midiManager(midi),
			oscManager(osc)
			{
				int i;

				//Setup the addMapping button.
				ScopedPointer<Drawable> addImage(JuceHelperStuff::loadSVGFromMemory(Vectors::addmappingbutton_svg,
																					Vectors::addmappingbutton_svgSize));
				ScopedPointer<Drawable> addImageOver(JuceHelperStuff::loadSVGFromMemory(Vectors::addmappingbuttonover_svg,
																						Vectors::addmappingbuttonover_svgSize));
				addMapping = new DrawableButton(L"Add Mapping Buttton",
												DrawableButton::ImageFitted);
				addMapping->setImages(addImage, addImageOver);
				addMapping->addListener(this);
				addAndMakeVisible(addMapping);

				//Setup all the mapping buttons.
				const Array<KeyPress> keyMappings = appManager->getKeyMappings()->getKeyPressesAssignedToCommand(id);

				//KeyPresses.
				for(i=0;i<keyMappings.size();++i)
				{
					TextButton *tempB = new TextButton(keyMappings[i].getTextDescription());
					tempB->getProperties().set("type", L"KeyPress");
					tempB->addListener(this);
					mappingButtons.add(tempB);
					addAndMakeVisible(tempB);
				}

				//MIDI CCs.
				for(i=0;i<midiManager->getNumAppMappings();++i)
				{
					if(midiManager->getAppMapping(i)->getId() == id)
					{
						String tempstr;

						tempstr << "MIDI CC: " << midiManager->getAppMapping(i)->getCc();
						TextButton *tempB = new TextButton(tempstr);
						tempB->getProperties().set("type", L"MIDI CC");
						tempB->addListener(this);
						mappingButtons.add(tempB);
						addAndMakeVisible(tempB);
					}
				}

				//OSC.
				for(i=0;i<oscManager->getNumAppMappings();++i)
				{
					if(oscManager->getAppMapping(i)->getId() == id)
					{
						TextButton *tempB = new TextButton(oscManager->getAppMapping(i)->getAddress());
						tempB->getProperties().set("type", L"OSC");
						tempB->addListener(this);
						mappingButtons.add(tempB);
						addAndMakeVisible(tempB);
					}
				}
			};
			///	Destructor.
			~MappingItemButtons()
			{
				deleteAllChildren();
			};

			///	So the buttons get re-positioned correctly.
			void resized()
			{
				int i;
				const int buttonWidth = 90;
				const int addMappingX = getWidth()-getHeight();

				addMapping->setBounds(addMappingX,
									  0,
									  getHeight(),
									  getHeight());

				for(i=0;i<mappingButtons.size();++i)
				{
					mappingButtons[i]->setBounds(addMappingX-((i+1)*(buttonWidth+2)),
												 0,
												 buttonWidth,
												 getHeight());
				}
			};

			///	So we can respond to any button clicks.
			void buttonClicked(Button *button)
			{
				if(button == addMapping)
				{
					PopupMenu popeye;

					popeye.addItem(1, "Add Key Mapping");
					popeye.addItem(2, "Add MIDI CC Mapping");
					popeye.addItem(3, "Add Open Sound Control Mapping");

					switch(popeye.showAt(button))
					{
						case 1:
							{
								AlertWindow win("Keypress mapping",
												"Enter the key combination to map this command to:\n\n",
												AlertWindow::NoIcon);

								win.addButton("OK", 1);
								win.addButton("Cancel", 0);
								win.setWantsKeyboardFocus(true);
								win.grabKeyboardFocus();
								win.addKeyListener(this);

								if(win.runModalLoop())
								{
									KeyPressMappingSet *keyMappings = appManager->getKeyMappings();

									keyMappings->addKeyPress(id, tempKeyPress);

									TextButton *tempB = new TextButton(tempKeyPress.getTextDescription());
									tempB->getProperties().set("type", "KeyPress");
									tempB->addListener(this);
									mappingButtons.add(tempB);
									addAndMakeVisible(tempB);
									resized();
								}
							}
							break;
						case 2:
							{
								/*AlertWindow win("MIDI CC mapping", 
												"Select a MIDI CC to map this command to:",
												AlertWindow::NoIcon);

								win.addComboBox("midiCc", MidiMappingManager::getCCNames());
								win.addButton("OK", 1, KeyPress(KeyPress::returnKey));
								win.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));*/
								MidiCcAlertWindow win(midiManager);

								if(win.runModalLoop())
								{
									int index;

									index = win.getComboBoxComponent("midiCc")->getSelectedId();
									if(index > 1)
									{
										String tempstr;
										MidiAppMapping *mapping = new MidiAppMapping(midiManager,
																					 index-2,
																					 id);
										midiManager->registerAppMapping(mapping);
										tempstr << "MIDI CC: " << index-2;
										TextButton *tempB = new TextButton(tempstr);
										tempB->getProperties().set("type", "MIDI CC");
										tempB->addListener(this);
										mappingButtons.add(tempB);
										addAndMakeVisible(tempB);
										resized();
									}
								}
							}
							break;
						case 3:
							{
								AlertWindow win("Open Sound Control mapping", 
												"Enter OSC address to map this command to:",
												AlertWindow::NoIcon);

								//win.addTextEditor("oscAddress", "");
								win.addComboBox("oscAddress", oscManager->getReceivedAddresses());
								win.getComboBoxComponent("oscAddress")->setEditableText(true);
								win.addTextEditor("oscParam", "0", "Parameter:");
								win.getTextEditor("oscParam")->setInputRestrictions(3, "0123456789");
								win.addButton("OK", 1, KeyPress(KeyPress::returnKey));
								win.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));

								if(win.runModalLoop())
								{
									int param;
									String tempstr, tempstr2;

									//tempstr = win.getTextEditorContents("oscAddress");
									tempstr = win.getComboBoxComponent("oscAddress")->getText();
									tempstr2 = win.getTextEditorContents("oscParam");
									param = tempstr2.getIntValue();
									if(!tempstr.isEmpty())
									{
										OscAppMapping *mapping = new OscAppMapping(oscManager,
																				   tempstr,
																				   param,
																				   id);
										oscManager->registerAppMapping(mapping);
										TextButton *tempB = new TextButton(tempstr);
										tempB->getProperties().set("type", "OSC");
										tempB->addListener(this);
										mappingButtons.add(tempB);
										addAndMakeVisible(tempB);
										resized();
									}
								}
							}
							break;
					}
				}
				else
				{
					PopupMenu popeye;

					popeye.addItem(1, "Remove Mapping");

					if(popeye.showAt(button))
					{
						String typeString = button->getProperties()["type"];

						if(typeString == "KeyPress")
						{
							int i;
							String buttonText = button->getName();
							KeyPressMappingSet *mappings = appManager->getKeyMappings();
							const Array<KeyPress> keys = mappings->getKeyPressesAssignedToCommand(id);

							for(i=0;i<keys.size();++i)
							{
								if(keys[i].getTextDescription() == buttonText)
								{
									mappings->removeKeyPress(keys[i]);
									removeChildComponent(button);
									mappingButtons.removeFirstMatchingValue(dynamic_cast<TextButton *>(button));
									delete button;

									break;
								}
							}
						}
						else if(typeString == "MIDI CC")
						{
							int i;
							String tempstr = button->getName().substring(9);
							int cc = tempstr.getIntValue();

							for(i=0;i<midiManager->getNumAppMappings();++i)
							{
								MidiAppMapping *mapping = midiManager->getAppMapping(i);
								if(mapping->getCc() == cc)
								{
									delete mapping;
									removeChildComponent(button);
									mappingButtons.removeFirstMatchingValue(dynamic_cast<TextButton *>(button));
									delete button;

									break;
								}
							}
						}
						else if(typeString == "OSC")
						{
							int i;

							for(i=0;i<oscManager->getNumAppMappings();++i)
							{
								OscAppMapping *mapping = oscManager->getAppMapping(i);
								if(mapping->getAddress() == button->getName())
								{
									delete mapping;
									removeChildComponent(button);
									mappingButtons.removeFirstMatchingValue(dynamic_cast<TextButton *>(button));
									delete button;

									break;
								}
							}
						}
						resized();
					}
				}
			};

			///	Used to respond to the KeyPress AlertWindow.
			bool keyPressed(const KeyPress &key, Component *originatingComponent)
			{
				AlertWindow *win = dynamic_cast<AlertWindow *>(originatingComponent);

				if(win)
				{
					String tempstr;

					tempKeyPress = key;

					tempstr << L"Enter the key combination to map this command to:\n\n" << key.getTextDescription();
					win->setMessage(tempstr);
					win->repaint();
				}

				return true;
			};
		  private:
			///	The command's id.
			CommandID id;

			///	The app's ApplicationCommandManager.
			ApplicationCommandManager *appManager;
			///	The app's MidiMappingManager.
			MidiMappingManager *midiManager;
			///	The app's OscMappingManager.
			OscMappingManager *oscManager;

			///	The add mapping button.
			DrawableButton *addMapping;
			///	The various individual mapping buttons.
			Array<TextButton *> mappingButtons;

			///	Where we store the KeyPress from the KeyPress AlertWindow.
			KeyPress tempKeyPress;
		};
	};
	///	The mapping category item in the mappingsTree.
	class CategoryItem : public TreeViewItem
	{
	  public:
		///	Constructor.
		CategoryItem(const String& category,
					 ApplicationCommandManager *app,
					 MidiMappingManager *midi,
					 OscMappingManager *osc):
		name(category),
		appManager(app),
		midiManager(midi),
		oscManager(osc)
		{
			setLinesDrawnForSubItems(true);
			setOpen(true);
		};
		///	Destructor.
		~CategoryItem() {};

		///	Returns true, obviously.
		bool mightContainSubItems() {return true;};

		///	Adds all the sub-items (mappings in this category).
		void itemOpennessChanged(bool isNowOpen)
		{
			clearSubItems();
			//We only need to add the mappings once.
			//if(getNumSubItems() == 0)
			{
				int i;
				const Array<CommandID> commands = appManager->getCommandsInCategory(name);

				for(i=0;i<commands.size();++i)
				{
					addSubItem(new MappingItem(appManager->getNameOfCommand(commands[i]),
											   commands[i],
											   appManager,
											   midiManager,
											   oscManager));
				}
			}
		};

		///	Draws the item.
		void paintItem(Graphics &g, int width, int height)
		{
			g.setColour(ColourScheme::getInstance().colours[L"Text Colour"]);
			g.setFont(Font(16.0f, Font::bold));
			g.drawText(name, 0, 0, width, height, Justification::centredLeft, false);
		};
	  private:
		///	The name of this category.
		String name;

		///	The app's ApplicationCommandManager.
		ApplicationCommandManager *appManager;
		///	The app's MidiMappingManager.
		MidiMappingManager *midiManager;
		///	The app's OscMappingManager.
		OscMappingManager *oscManager;
	};
	///	The root item in the mappingsTree.
	class RootItem : public TreeViewItem
	{
	  public:
		///	Constructor.
		RootItem(ApplicationCommandManager *app,
				 MidiMappingManager *midi,
				 OscMappingManager *osc):
		appManager(app),
		midiManager(midi),
		oscManager(osc)
		{
			setLinesDrawnForSubItems(false);
		};
		///	Destructor.
		~RootItem() {};

		///	Returns true, obviously.
		bool mightContainSubItems() {return true;};

		///	Adds all the sub-items (mapping categories).
		void itemOpennessChanged(bool isNowOpen)
		{
			//We only need to add the categories once.
			if(getNumSubItems() == 0)
			{
				int i;
				StringArray categories = appManager->getCommandCategories();

				for(i=0;i<categories.size();++i)
				{
					addSubItem(new CategoryItem(categories[i],
												appManager,
												midiManager,
												oscManager));
				}
			}
		};
	  private:
		///	The app's ApplicationCommandManager.
		ApplicationCommandManager *appManager;
		///	The app's MidiMappingManager.
		MidiMappingManager *midiManager;
		///	The app's OscMappingManager.
		OscMappingManager *oscManager;
	};
};

#endif
