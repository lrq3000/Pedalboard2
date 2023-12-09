//	PresetManager.cpp - A class used to keep track of user-saved plugin presets.
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

#include "PresetManager.h"

//------------------------------------------------------------------------------
PresetManager::PresetManager()
{
	File presetDir = File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile(L"Pedalboard2").getChildFile(L"presets");

	if(!presetDir.exists())
		presetDir.createDirectory();
}

//------------------------------------------------------------------------------
PresetManager::~PresetManager()
{

}

//------------------------------------------------------------------------------
void PresetManager::importPreset(const File& inFile, AudioProcessor *plugin)
{
	FileInputStream inStream(inFile);
	MemoryBlock memBlock((const size_t)inStream.getTotalLength());

	inStream.read(memBlock.getData(), (const size_t)inStream.getTotalLength());

	plugin->setCurrentProgramStateInformation(memBlock.getData(),
											  memBlock.getSize());
}

//------------------------------------------------------------------------------
void PresetManager::importPreset(const String& presetName,
								 AudioProcessor *plugin)
{
	File presetPath;

	presetPath = getPluginPresetDir(plugin->getName()).getChildFile(presetName).withFileExtension(L"fxp");
	importPreset(presetPath, plugin);
}

//------------------------------------------------------------------------------
void PresetManager::savePreset(const MemoryBlock& block,
							   const String& presetName,
							   const String& pluginName)
{
	File presetDir = getPluginPresetDir(pluginName);

	if(!presetDir.exists())
		presetDir.createDirectory();

	FileOutputStream outStream(presetDir.getChildFile(presetName).withFileExtension(L".fxp"));

	outStream.write(block.getData(), block.getSize());
}

//------------------------------------------------------------------------------
void PresetManager::getListOfUserPresets(const String& pluginName,
										 StringArray& presetList)
{
	int i;
	File presetDir = getPluginPresetDir(pluginName);
	Array<File> presets;

	if(presetDir.exists())
	{
		presetDir.findChildFiles(presets, File::findFiles, false, "*.fxp");
		for(i=0;i<presets.size();++i)
			presetList.add(presets[i].getFileNameWithoutExtension());
	}
}

//------------------------------------------------------------------------------
File PresetManager::getPluginPresetDir(const String& pluginName)
{
	return File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile(L"Pedalboard2").getChildFile(L"presets").getChildFile(pluginName);
}
