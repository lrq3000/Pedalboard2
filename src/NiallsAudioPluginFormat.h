//	NiallsAudioPluginFormat.h - Describes the NiallsAudioPlugin format.
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

#ifndef NIALLSAUDIOPLUGINFORMAT_H_
#define NIALLSAUDIOPLUGINFORMAT_H_

#include "../JuceLibraryCode/JuceHeader.h"

class NiallsAudioPlugin;
class NAPModuleHandle;

//------------------------------------------------------------------------------
///	Describes the NiallsAudioPlugin format.
class NiallsAudioPluginFormat : public AudioPluginFormat
{
  public:
	///	Constructor.
	NiallsAudioPluginFormat();
	///	Destructor.
	~NiallsAudioPluginFormat();

	///	Returns the format name.
	String getName() const {return "NiallsAudioPlugin";};
	///	Tries to create descriptions for all the plugins in a particular file.
	void findAllTypesForFile(OwnedArray<PluginDescription>& results,
							 const String& fileOrIdentifier);
	///	Tries to create an instance of a NAP from a PluginDescription.
	AudioPluginInstance *createInstanceFromDescription(const PluginDescription& desc);
	///	Quick check to see whether this file may be a NAP.
	bool fileMightContainThisPluginType(const String& fileOrIdentifier);
	///	Returns the name of the plugin.
	String getNameOfPluginFromIdentifier(const String& fileOrIdentifier);
	///	Checks whether this plugin could be loaded.
	bool doesPluginStillExist(const PluginDescription& desc);
	///	Searchs a set of directories for any NAPs.
	StringArray searchPathsForPlugins(const FileSearchPath& directoriesToSearch,
											bool recursive);
	///	Returns the typical places to look for NAPs.
	FileSearchPath getDefaultLocationsToSearch();
  private:
  	///	Used by searchPathsForPlugins().
	void recursiveFileSearch(StringArray& results,
							 const File& dir, 
							 const bool recursive);
};

//------------------------------------------------------------------------------
///	Used to host NAPs.
class NAPInstance : public AudioPluginInstance
{
  public:
	///	Constructor.
	NAPInstance(const ReferenceCountedObjectPtr<NAPModuleHandle>& mod);
	///	Destructor.
	~NAPInstance();

	///	Fills in a PluginDescription for this plugin.
	void fillInPluginDescription(PluginDescription& description) const;

	///	Returns this plugin's name.
	const String getName() const {return name;};

	///	Called before playback starts.
	void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock);
	///	Called after playback has stopped.
	void releaseResources();

	///	Where the audio processing takes place.
	void processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

	///	Returns the name of the indexed input channel.
	const String getInputChannelName(int channelIndex) const;
	///	Returns the name of the indexed output channel.
	const String getOutputChannelName(int channelIndex) const;
	///	Returns false.
	bool isInputChannelStereoPair(int index) const {return false;};
	///	Returns false.
	bool isOutputChannelStereoPair(int index) const {return false;};

	///	Returns false.
	bool acceptsMidi() const {return false;};
	///	Returns false.
	bool producesMidi() const {return false;};

	///	Creates the plugin's editor.
	AudioProcessorEditor *createEditor();
	///	Returns true if the plugin has an editor.
	bool hasEditor() const;

	///	Returns the number of parameters.
	int getNumParameters();
	///	Returns the indexed parameter's name.
	const String getParameterName(int parameterIndex);
	///	Returns the value of the indexed parameter.
	float getParameter(int parameterIndex);
	///	Returns the value of the indexed parameter as a string.
	const String getParameterText(int parameterIndex);
	///	Sets the indexed parameter.
	void setParameter(int parameterIndex, float newValue);

	///	Returns 1.
	int getNumPrograms() {return 1;};
	///	Returns 0.
	int getCurrentProgram() {return 0;};
	///	Does nothing.
	void setCurrentProgram(int index) {};
	///	Returns nothing.
	const String getProgramName(int index) {return L"";};
	///	Does nothing.
	void changeProgramName(int index, const String &newName) {};

	///	Saves the plugin's state.
	void getStateInformation(MemoryBlock &destData);
	///	Loads the plugin's state.
	void setStateInformation(const void *data, int sizeInBytes);

	///	Returns whether the plugin was loaded or not.
	bool getPluginLoadedOkay() const {return plugin != 0;};
	///	Returns this plugin's module.
	const ReferenceCountedObjectPtr<NAPModuleHandle>& getModule() const {return module;};
  private:
	///	Pointer to the actual plugin.
	NiallsAudioPlugin *plugin;
	///	The plugin library.
	ReferenceCountedObjectPtr<NAPModuleHandle> module;
	///	The name of the plugin.
	String name;
};

//------------------------------------------------------------------------------
typedef NiallsAudioPlugin *(*NAPCreatePlugin)();

//------------------------------------------------------------------------------
///	Represents a NAP library.
class NAPModuleHandle : public ReferenceCountedObject
{
  public:
	///	Constructor.
    NAPModuleHandle(const File& file_);
	///	Destructor.
    ~NAPModuleHandle();

	///	Tries to load a NAP library.
    bool open();
	///	Closes a NAP library.
    void close();

	///	Creates a new NAPModuleHandle, or returns the existing one.
    static NAPModuleHandle *findOrCreateModule(const File& file);

    juce_UseDebuggingNewOperator
  public:
	///	The library file.
    File file;
    ///	The name of the plugin.
    String pluginName;

	///	The library's createPlugin() function.
    NAPCreatePlugin pluginCreator;

	///	Pointer to the module.
    void* hModule;
    ///	Path to the parent directory.
    String fullParentDirectoryPathName;
};

#endif

