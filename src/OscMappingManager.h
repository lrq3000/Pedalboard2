//	OscMappingManager.h - Class which dispatches OSC messages to OscMappings.
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

#ifndef OSCMAPPINGMANAGER_H_
#define OSCMAPPINGMANAGER_H_

#include "Mapping.h"
#include "TapTempoHelper.h"
#include "NiallsOSCLib/OSCMessage.h"

#include <map>

class OscInput;
class OscMappingManager;
class BypassableInstance;

//------------------------------------------------------------------------------
///	Class representing a mapping between OSC address and plugin parameter.
class OscMapping : public Mapping
{
  public:
	///	Constructor.
	/*!
		\param manager The OscMappingManager to register and unregister with.
		\param graph The FilterGraph this Mapping exists in.
		\param pluginId The uid of the plugin whose parameter is being mapped
		to.
		\param param The plugin parameter which is being mapped to.
		\param oscAddress The OSC address which is being mapped to the above
		parameter.
		\param oscParam The index of the parameter in the OSC message which we are
		interested in.
	 */
	OscMapping(OscMappingManager *manager,
			   FilterGraph *graph,
			   uint32 pluginId,
			   int param,
			   const String& oscAddress,
			   int oscParam);
	///	Constructor to load Mapping parameters from an XmlElement.
	OscMapping(OscMappingManager *manager, FilterGraph *graph, XmlElement *e);
	///	Destructor.
	~OscMapping();

	///	Called from OscMappingManager when it receives an OSC message which matches this mapping's address.
	void messageReceived(float val);

	///	Returns an XmlElement representing this Mapping.
	XmlElement *getXml() const;

	///	Returns the OSC address this mapping applies to.
	const String& getAddress() const {return address;};
	///	Returns the index of the OSC parameter this mapping applies to.
	int getParameterIndex() const {return parameter;};

	///	Sets the mapping's OSC address.
	void setAddress(const String& oscAddress);
	///	Sets the mapping's OSC parameter index.
	void setParameterIndex(int val);

	juce_UseDebuggingNewOperator
  private:
	///	The OscMappingManager to register with.
	OscMappingManager *mappingManager;

	///	The OSC address this mapping applies to.
	String address;
	///	The index of the OSC parameter this mapping applies to.
	int parameter;
};

//------------------------------------------------------------------------------
///	Class representing a mapping between OSC message and ApplicationCommandTarget.
class OscAppMapping
{
  public:
	///	Constructor.
	/*!
		\param manager The OscMappingManager to register and unregister with.
		\param oscAddress The OSC address which is being mapped to the above
		parameter.
		\param oscParameter The index of the OSC parameter which is being
		mapped.
		\param commandId The ApplicationCommand ID to be mapped to the OSC
		address.
	 */
	OscAppMapping(OscMappingManager *manager,
			      const String& oscAddress,
				  int oscParam,
				  CommandID commandId);
	///	Constructor to load Mapping parameters from an XmlElement.
	OscAppMapping(OscMappingManager *manager, XmlElement *e);
	///	Destructor.
	~OscAppMapping();

	///	Returns an XmlElement representing this Mapping.
	XmlElement *getXml() const;

	///	Returns the OSC address this mapping applies to.
	const String& getAddress() const {return address;};
	///	Returns the index of the OSC parameter this mapping applies to.
	int getParameterIndex() const {return parameter;};
	///	Returns the CommandId this mapping applies to.
	CommandID getId() const {return id;};

	juce_UseDebuggingNewOperator
  private:
	///	The main MidiMappingManager.
	OscMappingManager *oscManager;

	///	This mapping's OSC address.
	String address;
	///	This mapping's OSC parameter index.
	int parameter;
	///	This mapping's command ID.
	CommandID id;
};

//------------------------------------------------------------------------------
///	Class which dispatches OSC messages to OscMappings.
class OscMappingManager
{
  public:
	///	Constructor.
	OscMappingManager(ApplicationCommandManager *manager);
	///	Destructor.
	~OscMappingManager();

	///	Called when an OSC message is received.
	void messageReceived(OSC::Message *message);
	///	Called when a OSC message is received.
	void handleFloatMessage(const String& address, int index, float val);
	///	Called when a OSC MIDI message is received.
	void handleMIDIMessage(const String& address, OSC::MIDIMessage val);

	///	Registers a OscMapping with the manager.
	void registerMapping(const String& address, OscMapping *mapping);
	///	Unregisters a OscMapping with the manager.
	void unregisterMapping(OscMapping *mapping);

	///	Registers a MidiAppMapping with the manager.
	void registerAppMapping(OscAppMapping *mapping);
	///	Unregisters a MidiAppMapping with the manager.
	void unregisterAppMapping(OscAppMapping *mapping);

	///	Registers a node which is interested in MIDI over OSC messages with the manager.
	void registerMIDIProcessor(const String& address,
							   BypassableInstance *processor);
	///	Unregisters the passed-in node from the MIDI processors map.
	void unregisterMIDIProcessor(BypassableInstance *processor);
	///	Returns the OSC address for the passed-in plugin, if there is one.
	const String getMIDIProcessorAddress(BypassableInstance *processor) const;

	///	Returns the number of OscAppMappings.
	int getNumAppMappings() const {return appMappings.size();};
	///	Returns the indexed OscAppMapping.
	OscAppMapping *getAppMapping(int i);

	///	Returns an array of all unique OSC addresses the manager has received so far.
	const StringArray& getReceivedAddresses() const {return uniqueAddresses;};

	juce_UseDebuggingNewOperator
  private:
	///	Holds all the OscMappings to dispatch messages to.
	std::multimap<String, OscMapping *> mappings;

	///	Holds any MIDI CC->ApplicationCommand mappings.
	std::multimap<String, OscAppMapping *> appMappings;
	///	Holds a copy of the app's ApplicationCommandManager so we can invoke ApplicationCommands.
	ApplicationCommandManager *appManager;

	///	Keeps track of all the processors which want MIDI messages over OSC.
	std::multimap<String, BypassableInstance *> midiProcessors;

	///	Keeps a note of any OSC addresses from messages sent to us.
	StringArray uniqueAddresses;

	///	Used for tap tempo.
	TapTempoHelper tapHelper;
};

//------------------------------------------------------------------------------
///	Dummy AudioProcessor so we can see at a glance which plugins have OSC mappings.
class OscInput : public AudioPluginInstance
{
  public:
	///	Constructor.
	OscInput();
	///	Destructor.
	~OscInput();

	///	Provides a description of the processor to the filter graph.
	void fillInPluginDescription(PluginDescription &description) const;

	///	Passes any Midi messages onto MidiMappingManager.
	void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	///	Returns the name of the processor.
	const String getName() const {return L"OSC Input";};
	///	Ignored.
	void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock) {};
	///	Ignored.
	void releaseResources() {};
	///	We have no audio inputs.
	const String getInputChannelName(int channelIndex) const {return L"";};
	///	We have no audio outputs.
	const String getOutputChannelName(int channelIndex) const {return L"";};
	///	We have no audio inputs.
	bool isInputChannelStereoPair(int index) const {return false;};
	///	We have no audio outputs.
	bool isOutputChannelStereoPair(int index) const {return false;};
	///	Returns true if there's no tail.
	bool silenceInProducesSilenceOut() const {return true;};
	///	Returns the length of the plugin's tail.
	double getTailLengthSeconds() const {return 0.0;};
	///	We don't want any Midi input.
	bool acceptsMidi() const {return false;};
	///	And we don't need to output it.
	bool producesMidi() const {return false;};
	///	We have no editor.
	AudioProcessorEditor *createEditor() {return 0;};
	///	We have no editor.
	bool hasEditor() const {return false;};
	///	We have no parameters.
	int getNumParameters() {return 0;};
	///	We have no parameters.
	const String getParameterName(int parameterIndex) {return L"";};
	///	We have no parameters.
	float getParameter(int parameterIndex) {return 0.0f;};
	///	We have no parameters.
	const String getParameterText(int parameterIndex) {return L"";};
	///	We have no parameters.
	void setParameter(int parameterIndex, float newValue) {};
	///	We have no programs.
	int getNumPrograms() {return 0;};
	///	We have no programs.
	int getCurrentProgram() {return 0;};
	///	We have no programs.
	void setCurrentProgram(int index) {};
	///	We have no programs.
	const String getProgramName(int index) {return L"";};
	///	We have no programs.
	void changeProgramName(int index, const String &newName) {};
	///	We have no state information.
	void getStateInformation(MemoryBlock &destData) {};
	///	We have no state information.
	void setStateInformation(const void *data, int sizeInBytes) {};

	juce_UseDebuggingNewOperator
};

#endif
