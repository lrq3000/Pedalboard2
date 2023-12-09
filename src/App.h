//	App.h - Main application stuff.
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

#ifndef APP_H_
#define APP_H_

#include "BranchesLAF.h"

#include "../JuceLibraryCode/JuceHeader.h"

class MainPanel;
class TrayIcon;

//------------------------------------------------------------------------------
///	Handles the main application stuff.
class App : public JUCEApplication
{
  public:
	///	Constructor.
	App() {win = 0; trayIcon = 0;};
	///	Destructor.
	~App() {};

	///	Where all the important stuff happens for this class.
	void initialise(const String& commandLine);
	///	Not used by us?
	void shutdown();

	///	Returns the name of the application (duh...).
	const String getApplicationName() {return L"Pedalboard 2";};
	///	Returns the current version.
	const String getApplicationVersion() {return L"2.14";};
	///	Whether or not we want to allow more than one instance to run.
	bool moreThanOneInstanceAllowed() {return true;};
	///	Not used right now?
	void anotherInstanceStarted(const String& commandLine);

	///	Returns the main window.
	DocumentWindow *getWindow() {return win;};
	///	Used to show/hide the tray icon.
	void showTrayIcon(bool val);

	juce_UseDebuggingNewOperator
  private:
	///	Pointer to our main window.
	DocumentWindow *win;
	///	Pointer to our tray icon.
	TrayIcon *trayIcon;
};

//------------------------------------------------------------------------------
///	Pointless waste of code, just to receive messages from the close button.
class StupidWindow : public DocumentWindow
{
  public:
	///	Constructor.
	StupidWindow(const String& commandLine, bool startHidden);
	///	Constructor.
	~StupidWindow();

	///	To handle the close button being pressed.
	void closeButtonPressed();

	///	Returns the mapping set for the window.
	KeyPressMappingSet *getMappingSet() {return commandManager.getKeyMappings();};
	///	Returns the application command manager for the window.
	ApplicationCommandManager *getAppManager() {return &commandManager;};
  private:
	///	Helper method. Attempts to load the key mappings from their file.
	void loadKeyMappings();
	///	Helper method. Attempts to save the key mappings to their file.
	void saveKeyMappings();

	///	The main panel of the main window.
	MainPanel *mainPanel;
	///	Used to handle menus and shortcuts etc.
	ApplicationCommandManager commandManager;
	///	Our custom LookAndFeel.
	ScopedPointer<BranchesLAF> laf;
};

#endif
