//	JuceHelperStuff.h - Some useful helper functions.
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

#ifndef JUCEHELPERSTUFF_H_
#define JUCEHELPERSTUFF_H_

#include "../JuceLibraryCode/JuceHeader.h"

namespace JuceHelperStuff
{

///	Helper method to load an SVG image from memory into a drawable.
Drawable *loadSVGFromMemory(const void *dataToInitialiseFrom,
							size_t sizeInBytes);

///	Helper method to create a native window with the proper icon.
int showModalDialog(const String& dialogTitle,
					Component *contentComponent,
					Component *componentToCentreAround,
					const Colour& backgroundColour,
					bool escapeKeyTriggersCloseButton,
					bool shouldBeResizable = false,
					bool useBottomRightCornerResizer = false);

///	Helper method to create a native window with the proper icon.
void showNonModalDialog(const String& dialogTitle,
						Component *contentComponent,
						Component *componentToCentreAround,
						const Colour& backgroundColour,
						bool escapeKeyTriggersCloseButton,
						bool shouldBeResizable = false,
						bool useBottomRightCornerResizer = false,
						bool stayOnTop = false);
	
///	To get around the fact that the current version of JUCE has File::userApplicationDataDirectory pointing to the wrong place on OSX.
File getAppDataFolder();

};

#endif
