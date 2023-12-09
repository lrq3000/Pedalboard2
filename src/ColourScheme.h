//	ColourScheme.h - Singleton struct handling colour schemes.
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

#ifndef COLOURSCHEME_H_
#define COLOURSCHEME_H_

#include "../JuceLibraryCode/JuceHeader.h"

#include <map>

///	Singleton struct handling colour schemes.
struct ColourScheme
{
  public:
	///	The map of all the available colours.
	std::map<String, Colour> colours;
	///	The name of the current colour scheme preset.
	String presetName;

	///	Returns the sole instance of the singleton.
	static ColourScheme& getInstance();

	///	Returns a StringArray of all the available colour scheme presets.
	const StringArray getPresets() const;
	///	Loads a colour scheme preset.
	void loadPreset(const String& name);
	///	Saves a colour scheme preset.
	void savePreset(const String& name);

	///	Returns true if colours == the named preset.
	bool doesColoursMatchPreset(const String& name);
  private:
	///	Constructor.
	ColourScheme();
	///	Destructor.
	~ColourScheme();
};

#endif
