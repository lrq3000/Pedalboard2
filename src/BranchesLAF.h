//	BranchesLAF.h - LookAndFeel class implementing some different buttons.
//	----------------------------------------------------------------------------
//	This file is part of Branches, a branching story editor.
//	Copyright (c) 2008 Niall Moody.
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

#ifndef BRANCHESLAF_H_
#define BRANCHESLAF_H_

#include "../JuceLibraryCode/JuceHeader.h"

///	LookAndFeel class implementing some different buttons.
class BranchesLAF : public LookAndFeel
{
  public:
	///	Constructor.
	BranchesLAF();
	///	Destructor.
	~BranchesLAF();

	///	Draws the buttons.
	void drawButtonBackground(Graphics &g,
							  Button &button,
							  const Colour &backgroundColour,
							  bool isMouseOverButton,
							  bool isButtonDown);
	///	Draws button text.
	void drawButtonText(Graphics &g,
						TextButton &button,
						bool isMouseOverButton,
						bool isButtonDown);

	///	Draws the scrollbar buttons.
	void drawScrollbarButton(Graphics &g,
							 ScrollBar &scrollbar,
							 int width,
							 int height,
							 int buttonDirection,
							 bool isScrollbarVertical,
							 bool isMouseOverButton,
							 bool isButtonDown);
	///	Draws the scrollbar.
	void drawScrollbar(Graphics &g,
					   ScrollBar &scrollbar,
					   int x,
					   int y,
					   int width,
					   int height,
					   bool isScrollbarVertical,
					   int thumbStartPosition,
					   int thumbSize,
					   bool isMouseOver,
					   bool isMouseDown);

	///	Draws the menubar.
	void drawMenuBarBackground(Graphics &g,
							   int width,
							   int height,
							   bool isMouseOverBar,
							   MenuBarComponent &menuBar);
	///	Returns the menubar font.
	Font getMenuBarFont(MenuBarComponent &menuBar,
						int itemIndex,
						const String &itemText);
	///	Draws the menubar items.
	void drawMenuBarItem(Graphics &g,
						 int width,
						 int height,
						 int itemIndex,
						 const String &itemText,
						 bool isMouseOverItem,
						 bool isMenuOpen,
						 bool isMouseOverBar,
						 MenuBarComponent &menuBar);
	///	The width of a menubar item.
	int getMenuBarItemWidth(MenuBarComponent &menuBar,
							int itemIndex,
							const String &itemText);
	///	Returns the popup meun font.
	Font getPopupMenuFont() {return Font(15.0f);};
	///	Draws the popup menu background.
	void drawPopupMenuBackground(Graphics &g, int width, int height);
	///	Cancels menus' drop shadow.
	int getMenuWindowFlags() {return 0;};

	///	Returns the image of a folder for the file chooser.
	const Drawable *getDefaultFolderImage();
	///	Draws a combobox (used in the file chooser).
	void drawComboBox(Graphics& g,
					  int width,
					  int height,
                      bool isButtonDown,
                      int buttonX,
					  int buttonY,
                      int buttonW,
					  int buttonH,
                      ComboBox& box);

	///	Draws the ProgressBar.
	void drawProgressBar(Graphics& g,
						 ProgressBar& progressBar,
                         int width,
						 int height,
						 double progress,
						 const String& textToShow);

	///	Draws the KeymapChange button.
	void drawKeymapChangeButton(Graphics &g,
								int width,
								int height,
								Button &button,
								const String &keyDescription);

	///	Draws a Label.
	void drawLabel(Graphics& g, Label& label);

	///	Draws a ToggleButton.
	void drawToggleButton(Graphics& g,
                          ToggleButton& button,
                          bool isMouseOverButton,
                          bool isButtonDown);

	///	Drwas a tick box.
	void drawTickBox(Graphics& g,
                     Component& component,
                     float x, float y, float w, float h,
                     bool ticked,
                     bool isEnabled,
                     bool isMouseOverButton,
                     bool isButtonDown);

	///	Fills in the TextEditor background.
	void fillTextEditorBackground(Graphics& g, int width, int height,
                                  TextEditor& textEditor);

	///	Draws the callout box.
	void drawCallOutBoxBackground(CallOutBox &box,
								  Graphics &g,
								  const Path &path,
								  Image &cachedImage);

	juce_UseDebuggingNewOperator
};

#endif
