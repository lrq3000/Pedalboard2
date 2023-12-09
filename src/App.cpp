//	App.cpp - Main application stuff.
//	----------------------------------------------------------------------------
//	This file is part of Pedalboard2, an audio plugin host.
//	Copyright (c) 2009 Niall Moody.
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

#include "MainPanel.h"
#include "InternalFilters.h"
//#include "LADSPAPluginFormat.h"
#include "NiallsAudioPluginFormat.h"
#include "App.h"
#include "MainTransport.h"
#include "MidiMappingManager.h"
#include "OscMappingManager.h"
#include "PropertiesSingleton.h"
#include "AudioSingletons.h"
#include "Images.h"
#include "ColourScheme.h"
#include "JuceHelperStuff.h"
#include "TrayIcon.h"
#include "AudioSingletons.h"

//------------------------------------------------------------------------------
void App::initialise(const String& commandLine)
{
	bool useTrayIcon, startInTray;
	PropertiesFile::Options opts;

	//Initialise our settings file.
	opts.applicationName = L"Pedalboard2";
	opts.filenameSuffix = L"Settings";
	opts.osxLibrarySubFolder = L"Application Support";
#ifdef __APPLE__
	opts.folderName = L"Pedalboard2";
#endif
	opts.commonToAllUsers = false;
	opts.ignoreCaseOfKeyNames = false;
	opts.millisecondsBeforeSaving = 1000;
	opts.storageFormat = PropertiesFile::storeAsXML;
	PropertiesSingleton::getInstance().setStorageParameters(opts);

#ifndef __APPLE__
	useTrayIcon = PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("useTrayIcon");
#else
	useTrayIcon = false;
#endif
	startInTray = PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("startInTray");

	win = new StupidWindow(commandLine, (useTrayIcon && startInTray));

#ifndef __APPLE__
	if(useTrayIcon)
		trayIcon = new TrayIcon(win);
#endif

#ifdef __APPLE__
	MainPanel *pan = dynamic_cast<MainPanel *>(win->getContentComponent());
	if(pan)
		MenuBarModel::setMacMainMenu(pan);
#endif
}

//------------------------------------------------------------------------------
void App::shutdown()
{
#ifdef __APPLE__
	MenuBarModel::setMacMainMenu(0);
#endif

	delete win;

#ifndef __APPLE__
	if(trayIcon)
		delete trayIcon;
#endif

	MainTransport::deleteInstance();
}

//------------------------------------------------------------------------------
void App::anotherInstanceStarted(const String& commandLine)
{
#ifdef __APPLE__
	File initialFile(commandLine);

	if(initialFile.existsAsFile())
	{
		MainPanel *pan = dynamic_cast<MainPanel *>(win->getContentComponent());

		if(initialFile.getFileExtension() == L".pdl" && pan)
			pan->loadDocument(initialFile);
	}
#endif
}

//------------------------------------------------------------------------------
void App::showTrayIcon(bool val)
{
#ifndef __APPLE__
	if(val && !trayIcon)
		trayIcon = new TrayIcon(win);
	else if(!val && trayIcon)
	{
		delete trayIcon;
		trayIcon = 0;
	}
#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StupidWindow::StupidWindow(const String& commandLine, bool startHidden):
DocumentWindow(L"Pedalboard 2", Colours::black, DocumentWindow::allButtons)
{
	//Make sure we've loaded all the available plugin formats before we create
	//the main panel.
	{
		InternalPluginFormat *internalFormat = new InternalPluginFormat;
		//LADSPAPluginFormat *ladspaFormat = new LADSPAPluginFormat;
		//VSTPluginFormat *vstFormat = new VSTPluginFormat;
		//NiallsAudioPluginFormat *napFormat = new NiallsAudioPluginFormat;

		AudioPluginFormatManagerSingleton::getInstance().addFormat(internalFormat);
		//AudioPluginFormatManager::getInstance()->addFormat(napFormat);
		//AudioPluginFormatManager::getInstance()->addFormat(vstFormat);
		//AudioPluginFormatManager::getInstance()->addFormat(ladspaFormat);
	}

	//Load correct colour scheme.
	{
		String scheme = PropertiesSingleton::getInstance().getUserSettings()->getValue(L"colourScheme");

		if(scheme != String::empty)
			ColourScheme::getInstance().loadPreset(scheme);
	}

	LookAndFeel::setDefaultLookAndFeel(laf = new BranchesLAF());
	setResizable(true, false);
	setContentOwned(mainPanel = new MainPanel(&commandManager), true);
	//mainPanel->setCommandManager(&commandManager);
	centreWithSize(1024, 580);
	setUsingNativeTitleBar(true);
	//setDropShadowEnabled(false);
	if(!startHidden)
		setVisible(true);
#ifndef __APPLE__
	setMenuBar(mainPanel);
#endif

	//Attempts to associate our icon with the window's titlebar.
	getPeer()->setIcon(ImageCache::getFromMemory(Images::icon512_png,
												 Images::icon512_pngSize));

	commandManager.registerAllCommandsForTarget(mainPanel);
    commandManager.registerAllCommandsForTarget(JUCEApplication::getInstance());

	commandManager.getKeyMappings()->resetToDefaultMappings();

	loadKeyMappings();

	addKeyListener(commandManager.getKeyMappings());

	restoreWindowStateFromString(PropertiesSingleton::getInstance().getUserSettings()->getValue("WindowState"));

	//See if we can load a .pdl file from the commandline.
	File initialFile(commandLine);

	if(initialFile.existsAsFile())
	{
		if(initialFile.getFileExtension() == L".pdl")
		{
			mainPanel->loadDocument(initialFile);
			mainPanel->setLastDocumentOpened(initialFile);
			mainPanel->setFile(initialFile);
			mainPanel->setChangedFlag(false);
		}
	}
}

//------------------------------------------------------------------------------
StupidWindow::~StupidWindow()
{
	saveKeyMappings();
	PropertiesSingleton::getInstance().getUserSettings()->setValue("WindowState",
																	getWindowStateAsString());

	setMenuBar(0);
	setContentOwned(0, true);
	LookAndFeel::setDefaultLookAndFeel(0);
	
	AudioPluginFormatManagerSingleton::killInstance();
	AudioFormatManagerSingleton::killInstance();
	AudioThumbnailCacheSingleton::killInstance();
	PropertiesSingleton::killInstance();
}

//------------------------------------------------------------------------------
void StupidWindow::closeButtonPressed()
{
	if(PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("useTrayIcon"))
	{
		if(isVisible())
			setVisible(false);
		else
		{
			FileBasedDocument::SaveResult result = mainPanel->saveIfNeededAndUserAgrees();

			if(result == FileBasedDocument::savedOk)
				JUCEApplication::quit();
		}
	}
	else
	{
		FileBasedDocument::SaveResult result = mainPanel->saveIfNeededAndUserAgrees();

		if(result == FileBasedDocument::savedOk)
			JUCEApplication::quit();
	}
}

//------------------------------------------------------------------------------
void StupidWindow::loadKeyMappings()
{
	int i;
	OscMappingManager *oscManager = mainPanel->getOscMappingManager();
	MidiMappingManager *midiManager = mainPanel->getMidiMappingManager();
	File mappingsFile = JuceHelperStuff::getAppDataFolder().getChildFile("AppMappings.xml");

	if(mappingsFile.existsAsFile())
	{
		ScopedPointer<XmlElement> rootXml(XmlDocument::parse(mappingsFile));

		if(rootXml)
		{
			XmlElement *keyMappings = rootXml->getChildByName("KEYMAPPINGS");
			XmlElement *midiMappings = rootXml->getChildByName("MidiMappings");
			XmlElement *oscMappings = rootXml->getChildByName("OscMappings");

			if(keyMappings)
				commandManager.getKeyMappings()->restoreFromXml(*keyMappings);
			if(midiMappings)
			{
				for(i=0;i<midiMappings->getNumChildElements();++i)
				{
					XmlElement *tempEl = midiMappings->getChildElement(i);

					if(tempEl->hasTagName("MidiAppMapping"))
					{
						MidiAppMapping *newMapping = new MidiAppMapping(midiManager, tempEl);
						midiManager->registerAppMapping(newMapping);
					}
				}
			}
			if(oscMappings)
			{
				for(i=0;i<oscMappings->getNumChildElements();++i)
				{
					XmlElement *tempEl = oscMappings->getChildElement(i);

					if(tempEl->hasTagName("OscAppMapping"))
					{
						OscAppMapping *newMapping = new OscAppMapping(oscManager, tempEl);
						oscManager->registerAppMapping(newMapping);
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
void StupidWindow::saveKeyMappings()
{
	int i;
	File mappingsFile = JuceHelperStuff::getAppDataFolder().getChildFile("AppMappings.xml");
	XmlElement *mappingsXml = commandManager.getKeyMappings()->createXml(false);
	XmlElement rootXml("Pedalboard2AppMappings");
	XmlElement *midiXml = new XmlElement("MidiMappings");
	XmlElement *oscXml = new XmlElement("OscMappings");
	MidiMappingManager *midiManager = mainPanel->getMidiMappingManager();
	OscMappingManager *oscManager = mainPanel->getOscMappingManager();

	//Add the KeyPress mappings.
	rootXml.addChildElement(mappingsXml);

	//Add the MIDI CC mappings.
	for(i=0;i<midiManager->getNumAppMappings();++i)
		midiXml->addChildElement(midiManager->getAppMapping(i)->getXml());
	rootXml.addChildElement(midiXml);

	//Add the OSC mappings.
	for(i=0;i<oscManager->getNumAppMappings();++i)
		oscXml->addChildElement(oscManager->getAppMapping(i)->getXml());
	rootXml.addChildElement(oscXml);

	//Save to the file.
	rootXml.writeToFile(mappingsFile, "");
}

START_JUCE_APPLICATION(App)
