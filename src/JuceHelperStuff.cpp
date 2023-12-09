//	JuceHelperStuff.cpp - Some useful helper functions.
//	----------------------------------------------------------------------------
//	This file is part of Pedalboard2, an audio plugin host.
//	Copyright (c) 2012 Niall Moody.
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

#include "JuceHelperStuff.h"
#include "Images.h"

//------------------------------------------------------------------------------
Drawable *JuceHelperStuff::loadSVGFromMemory(const void *dataToInitialiseFrom,
											 size_t sizeInBytes)
{
	Drawable *retval = 0;

	MemoryBlock memBlock(dataToInitialiseFrom, sizeInBytes);
	XmlDocument doc(memBlock.toString());
	ScopedPointer<XmlElement> svgData(doc.getDocumentElement());

	retval = Drawable::createFromSVG(*svgData);

	return retval;
}

//------------------------------------------------------------------------------
class TempDialogWindow : public DialogWindow
{
public:
    TempDialogWindow (const String& title,
                      Component* contentComponent_,
                      Component* componentToCentreAround,
                      const Colour& colour,
                      const bool escapeKeyTriggersCloseButton_,
                      const bool shouldBeResizable,
                      const bool useBottomRightCornerResizer,
					  const bool deleteContent = false)
        : DialogWindow (title, colour, escapeKeyTriggersCloseButton_, true),
		deleteDialog(deleteContent)
    {
        if (! JUCEApplication::isStandaloneApp())
            setAlwaysOnTop (true); // for a plugin, make it always-on-top because the host windows are often top-level

		if(deleteContent)
			setContentOwned (contentComponent_, true);
		else
			setContentNonOwned (contentComponent_, true);
        centreAroundComponent (componentToCentreAround, getWidth(), getHeight());
        setResizable (shouldBeResizable, useBottomRightCornerResizer);
    }

    void closeButtonPressed()
    {
        setVisible (false);

		if(deleteDialog)
			delete this;
    }

private:
    JUCE_DECLARE_NON_COPYABLE (TempDialogWindow);

	///	To make sure it's only deleted when it should be.
	bool deleteDialog;
};

//------------------------------------------------------------------------------
int JuceHelperStuff::showModalDialog(const String& dialogTitle,
									 Component *contentComponent,
									 Component *componentToCentreAround,
									 const Colour& backgroundColour,
									 bool escapeKeyTriggersCloseButton,
									 bool shouldBeResizable,
									 bool useBottomRightCornerResizer)
{
	TempDialogWindow dw (dialogTitle, contentComponent, componentToCentreAround,
                         backgroundColour, escapeKeyTriggersCloseButton,
                         shouldBeResizable, useBottomRightCornerResizer);
	dw.setUsingNativeTitleBar(true);
	dw.getPeer()->setIcon(ImageCache::getFromMemory(Images::icon512_png,
													Images::icon512_pngSize));

    return dw.runModalLoop();
}

//------------------------------------------------------------------------------
void JuceHelperStuff::showNonModalDialog(const String& dialogTitle,
										 Component *contentComponent,
										 Component *componentToCentreAround,
										 const Colour& backgroundColour,
										 bool escapeKeyTriggersCloseButton,
										 bool shouldBeResizable,
										 bool useBottomRightCornerResizer,
										 bool stayOnTop)
{
	TempDialogWindow* dw = new TempDialogWindow (dialogTitle, contentComponent, componentToCentreAround,
                                                 backgroundColour, escapeKeyTriggersCloseButton,
                                                 shouldBeResizable, useBottomRightCornerResizer, true);
	dw->setUsingNativeTitleBar(true);

	dw->addToDesktop();
	dw->setVisible(true);
	dw->setAlwaysOnTop(stayOnTop);
	dw->getPeer()->setIcon(ImageCache::getFromMemory(Images::icon512_png,
													 Images::icon512_pngSize));
}

//------------------------------------------------------------------------------
File JuceHelperStuff::getAppDataFolder()
{
#ifdef __APPLE__
	File retval = File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile("Application Support").getChildFile("Pedalboard2");

	if(!retval.exists())
		retval.createDirectory();

	return retval;
#else
	return File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile("Pedalboard2");
#endif
}
