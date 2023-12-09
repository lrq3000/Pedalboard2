//	NiallsAudioPluginFormat.cpp - Describes the NiallsAudioPlugin format.
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

#include "NiallsAudioPluginFormat.h"
#include "NiallsAudioPlugin.h"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
NiallsAudioPluginFormat::NiallsAudioPluginFormat():
AudioPluginFormat()
{
	
}

//------------------------------------------------------------------------------
NiallsAudioPluginFormat::~NiallsAudioPluginFormat()
{
	
}

//------------------------------------------------------------------------------
void NiallsAudioPluginFormat::findAllTypesForFile(OwnedArray<PluginDescription>& results,
												  const String& fileOrIdentifier)
{
	if(!fileMightContainThisPluginType(fileOrIdentifier))
		return;

	PluginDescription desc;
	desc.fileOrIdentifier = fileOrIdentifier;
	desc.uid = 0;

	NAPInstance* instance = dynamic_cast<NAPInstance*>(createInstanceFromDescription(desc));

	if(instance == 0)
		return;

	try
	{
		instance->fillInPluginDescription(desc);

		if(instance->getModule()->pluginCreator != 0)
			results.add(new PluginDescription(desc));
	}
	catch (...)
	{
		cout << "NiallsAudioPluginFormat: Caught exception interrogating plugin." << endl;
	}

	deleteAndZero(instance);
}

//------------------------------------------------------------------------------
AudioPluginInstance *NiallsAudioPluginFormat::createInstanceFromDescription(const PluginDescription& desc)
{
	NAPInstance *result = 0;

	if(fileMightContainThisPluginType(desc.fileOrIdentifier))
	{
		//NAPModuleHandle *m = 0;
		File file(desc.fileOrIdentifier);

		const File previousWorkingDirectory(File::getCurrentWorkingDirectory());
		file.getParentDirectory().setAsCurrentWorkingDirectory();

		const ReferenceCountedObjectPtr<NAPModuleHandle> module(NAPModuleHandle::findOrCreateModule(file));

		if(module != 0)
		{
			result = new NAPInstance(module);

			if(result)
			{
				if(!result->getPluginLoadedOkay())
				{
					cout << "NiallsAudioPluginFormat: Could not load plugin." << endl;
					deleteAndZero(result);
				}
			}
		}

		previousWorkingDirectory.setAsCurrentWorkingDirectory();
	}

	return result;
}

//------------------------------------------------------------------------------
bool NiallsAudioPluginFormat::fileMightContainThisPluginType(const String& fileOrIdentifier)
{
	const File f (fileOrIdentifier);

	return f.existsAsFile() && f.hasFileExtension(T(".so"));
}

//------------------------------------------------------------------------------
const String NiallsAudioPluginFormat::getNameOfPluginFromIdentifier(const String& fileOrIdentifier)
{
	return fileOrIdentifier;
}

//------------------------------------------------------------------------------
bool NiallsAudioPluginFormat::doesPluginStillExist(const PluginDescription& desc)
{
	return File(desc.fileOrIdentifier).exists();
}

//------------------------------------------------------------------------------
const StringArray NiallsAudioPluginFormat::searchPathsForPlugins(const FileSearchPath& directoriesToSearch,
																 const bool recursive)
{
	int j;
	StringArray results;

    for(j=0;j<directoriesToSearch.getNumPaths();++j)
        recursiveFileSearch(results, directoriesToSearch[j], recursive);

    return results;
}

//------------------------------------------------------------------------------
const FileSearchPath NiallsAudioPluginFormat::getDefaultLocationsToSearch()
{
	return FileSearchPath ("/usr/lib/nap;/usr/local/lib/nap");
}

//------------------------------------------------------------------------------
void NiallsAudioPluginFormat::recursiveFileSearch(StringArray& results,
												  const File& dir, 
												  const bool recursive)
{
	DirectoryIterator iter(dir, false, "*", File::findFilesAndDirectories);

	while(iter.next())
	{
		const File f(iter.getFile());
		bool isPlugin = false;

		if(fileMightContainThisPluginType(f.getFullPathName()))
		{
			isPlugin = true;
			results.add(f.getFullPathName());
		}

		if(recursive && (!isPlugin) && f.isDirectory())
			recursiveFileSearch(results, f, true);
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
NAPInstance::NAPInstance(const ReferenceCountedObjectPtr<NAPModuleHandle>& mod):
AudioPluginInstance(),
plugin(0),
module(mod)
{
	try
	{
		if(module->pluginCreator != 0)
		{
			plugin = module->pluginCreator();
			if(plugin == 0)
			{
				cout << "NAPInstance: Could not create plugin." << endl;
				return;
			}
			else
			{
				name = plugin->getName().c_str();

				//So the app knows how many channels the plugin has.
				setPlayConfigDetails(plugin->getNumAudioInputs()+plugin->getNumParameters(),
									 plugin->getNumAudioOutputs(),
									 44100.0,
									 4096);
			}
		}
		else
		{
			cout << "NAPInstance: No createPlugin() function in this library." << endl;
			return;
		}

		//uint32 sampleRate = (uint32)(getSampleRate() > 0 ? getSampleRate() : 44100.0f);
    }
    catch (...)
    {
        cout << "NAPInstance: Caught exception when creating plugin." << endl;
    }
}

//------------------------------------------------------------------------------
NAPInstance::~NAPInstance()
{
	try
	{
		if(plugin)
			delete plugin;
	}
	catch(...)
	{
		cout << "NAPInstance: Exception deleting plugin." << endl;
	}
}

//------------------------------------------------------------------------------
void NAPInstance::fillInPluginDescription(PluginDescription& description) const
{
	description.name = name;
	description.pluginFormatName = T("NAP");
	description.category = T("effect");
	description.manufacturerName = T("Niall");
	description.version = T("1.0");
	description.fileOrIdentifier = module->file.getFullPathName();
	description.lastFileModTime = module->file.getLastModificationTime();
	description.uid = 0;
	description.isInstrument = false;
	if(plugin)
	{
		description.numInputChannels = plugin->getNumAudioInputs() +
									   plugin->getNumParameters();
		description.numOutputChannels = plugin->getNumAudioOutputs();
	}
	else
	{
		description.numInputChannels = 0;
		description.numOutputChannels = 0;
	}
}

//------------------------------------------------------------------------------
void NAPInstance::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
	if(!plugin)
		return;

	setPlayConfigDetails(plugin->getNumAudioInputs()+plugin->getNumParameters(),
						 plugin->getNumAudioOutputs(),
						 sampleRate,
						 estimatedSamplesPerBlock);
}

//------------------------------------------------------------------------------
void NAPInstance::releaseResources()
{
	
}

//------------------------------------------------------------------------------
void NAPInstance::processBlock(AudioSampleBuffer& buffer,
							   MidiBuffer& midiMessages)
{
	if(plugin)
	{
		plugin->processAudio(buffer.getArrayOfChannels(),
							 buffer.getArrayOfChannels(),
							 buffer.getNumSamples());
	}
}

//------------------------------------------------------------------------------
const String NAPInstance::getInputChannelName(const int channelIndex) const
{
	String retval;

	if(plugin)
	{
		if(channelIndex < plugin->getNumAudioInputs())
			retval = plugin->getInputName(channelIndex).c_str();
		else
			retval = plugin->getParameterName(channelIndex - plugin->getNumAudioInputs()).c_str();
	}

	return retval;
}

//------------------------------------------------------------------------------
const String NAPInstance::getOutputChannelName(const int channelIndex) const
{
	String retval;

	if(plugin)
		retval = plugin->getOutputName(channelIndex).c_str();

	return retval;
}

//------------------------------------------------------------------------------
AudioProcessorEditor *NAPInstance::createEditor()
{
	AudioProcessorEditor *retval = 0;

	if(plugin)
		retval = plugin->createEditor();

	return retval;
}

//------------------------------------------------------------------------------
bool NAPInstance::hasEditor() const
{
	bool retval = false;

	if(plugin)
		retval = plugin->hasEditor();

	return retval;
}

//------------------------------------------------------------------------------
int NAPInstance::getNumParameters()
{
	int retval = 0;

	if(plugin)
		retval = plugin->getNumParameters();

	return retval;
}

//------------------------------------------------------------------------------
const String NAPInstance::getParameterName(int parameterIndex)
{
	String retval;

	if(plugin)
		retval = plugin->getParameterName(parameterIndex).c_str();

	return retval;
}

//------------------------------------------------------------------------------
float NAPInstance::getParameter(int parameterIndex)
{
	return 0.0f;
}

//------------------------------------------------------------------------------
const String NAPInstance::getParameterText(int parameterIndex)
{
	return T("");
}

//------------------------------------------------------------------------------
void NAPInstance::setParameter(int parameterIndex, float newValue)
{
	
}

//------------------------------------------------------------------------------
void NAPInstance::getStateInformation(MemoryBlock &destData)
{
	
}

//------------------------------------------------------------------------------
void NAPInstance::setStateInformation(const void *data, int sizeInBytes)
{
	
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static Array<NAPModuleHandle *> activeNAPModules;

//------------------------------------------------------------------------------
NAPModuleHandle::NAPModuleHandle(const File& file_):
ReferenceCountedObject(),
file(file_),
pluginCreator(0),
hModule(0)
{
	activeNAPModules.add(this);

	fullParentDirectoryPathName = file_.getParentDirectory().getFullPathName();
}

//------------------------------------------------------------------------------
NAPModuleHandle::~NAPModuleHandle()
{
	activeNAPModules.removeValue(this);

	close();
}

//------------------------------------------------------------------------------
bool NAPModuleHandle::open()
{
	pluginName = file.getFileNameWithoutExtension();

	hModule = PlatformUtilities::loadDynamicLibrary(file.getFullPathName());

	if(hModule)
	{
		pluginCreator = (NAPCreatePlugin)PlatformUtilities::getProcedureEntryPoint(hModule,
																				   "createPlugin");
	}

	return pluginCreator != 0;
}

//------------------------------------------------------------------------------
void NAPModuleHandle::close()
{
	if(hModule)
		PlatformUtilities::freeDynamicLibrary(hModule);
}

//------------------------------------------------------------------------------
NAPModuleHandle *NAPModuleHandle::findOrCreateModule(const File& file)
{
	int i;

	for(i=(activeNAPModules.size()-1);i>=0;--i)
	{
		NAPModuleHandle* const module = (NAPModuleHandle *)activeNAPModules.getUnchecked(i);

		if(module->file == file)
			return module;
	}

	Logger::writeToLog("Attempting to load NAP: " + file.getFullPathName());

	NAPModuleHandle* m = 0;
	m = new NAPModuleHandle(file);

	if(m)
	{
		if(!m->open())
		{
			/*delete m;
			m = 0;*/
			deleteAndZero(m);
		}
	}

	return m;
}

