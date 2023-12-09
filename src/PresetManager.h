//	PresetManager.h - A class used to keep track of user-saved plugin presets.
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

#ifndef PRESETMANAGER_H_
#define PRESETMANAGER_H_

#include "../JuceLibraryCode/JuceHeader.h"

///	A class used to keep track of user-saved plugin presets.
class PresetManager
{
  public:
	///	Constructor.
	PresetManager();
	///	Destructor.
	~PresetManager();

	///	Loads a preset from an .fxp file.
	void importPreset(const File& inFile, AudioProcessor *plugin);
	///	Attempts to load a named preset from the user-saved presets dir for this plugin.
	/*!
		This is essentially a wrapper for the above importPreset(); it just
		calculates a file path from the presetName passed in, like so:
		<Pedalboard2 user data dir>/presets/<plugin name>/<presetName>.fxp
	 */
	void importPreset(const String& presetName, AudioProcessor *plugin);
	///	Saves the contents of a MemoryBlock to a .fxp file.
	void savePreset(const MemoryBlock& block,
					const String& presetName,
					const String& pluginName);

	///	Returns a StringArray of the user-saved presets for the named plugin.
	static void getListOfUserPresets(const String& pluginName, StringArray& presetList);
  private:
	///	Helper method. Returns the location of a plugin's presets.
	static File getPluginPresetDir(const String& pluginName);
};

#endif
