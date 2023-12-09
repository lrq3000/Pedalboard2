//	MidiMappingManager.h - Class which dispatches MIDI CC messages to
//						   MidiMappings.
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

#ifndef MIDIMAPPINGMANAGER_H_
#define MIDIMAPPINGMANAGER_H_

#include "Mapping.h"
#include "TapTempoHelper.h"

#include <map>

class MidiMappingManager;

//------------------------------------------------------------------------------
///	Class representing a mapping between MIDI CC and plugin parameter.
class MidiMapping : public Mapping
{
  public:
	///	Constructor.
	/*!
		\param manager The MidiMappingManager to register and unregister with.
		\param graph The FilterGraph this Mapping exists in.
		\param pluginId The uid of the plugin whose parameter is being mapped
		to.
		\param param The plugin parameter which is being mapped to.
		\param midiCc The MIDI CC which is being mapped to the above parameter.
		\param latch Whether the CC should be latched or not.
		\param chan The MIDI channel to respond to (0 == omni).
		\param float The lower bound of the plugin parameter mapping.
		\param float The upper bound of the plugin parameter mapping.
	 */
	MidiMapping(MidiMappingManager *manager,
				FilterGraph *graph,
				uint32 pluginId,
				int param,
				int midiCc,
				bool latch,
				int chan = 0,
				float lower = 0.0f,
				float upper = 1.0f);
	///	Constructor to load Mapping parameters from an XmlElement.
	MidiMapping(MidiMappingManager *manager, FilterGraph *graph, XmlElement *e);
	///	Destructor.
	~MidiMapping();

	///	Called from MidiMappingManager when it receives a MIDI CC message which matches this mapping's CC.
	void ccReceived(int val);

	///	Returns an XmlElement representing this Mapping.
	XmlElement *getXml() const;

	///	Returns the MIDI CC this mapping applies to.
	int getCc() const {return cc;};
	///	Returns whether the CC should be latched or not.
	bool getLatched() const {return latched;};
	///	Returns the MIDI channel the mapping applies to.
	int getChannel() const {return channel;};
	///	Returns the lower bound of the parameter mapping.
	float getLowerBound() const {return lowerBound;};
	///	Returns the upper bound of the parameter mapping.
	float getUpperBound() const {return upperBound;};

	///	Sets the mapping's MIDI CC.
	void setCc(int val);
	///	Sets the mapping's latch value.
	void setLatched(bool val);
	///	Sets the MIDI channel the mapping applies to.
	void setChannel(int val);
	///	Sets the mapping's lower bound.
	void setLowerBound(float val);
	///	Sets the mapping's upper bound.
	void setUpperBound(float val);

	juce_UseDebuggingNewOperator
  private:
	///	The MidiMappingManager to register with.
	MidiMappingManager *mappingManager;

	///	The CC this mapping applies to.
	int cc;
	///	If the CC should be latched or not (useful for buttons).
	bool latched;
	///	The MIDI channel this mapping applies to.
	int channel;
	///	The lower bound of the plugin parameter mapping.
	float lowerBound;
	///	The upper bound of the plugin parameter mapping.
	float upperBound;

	///The currently-latched value.
	float latchVal;
	///	The current high latched value.
	float latchHi;
	///	The current low latched value.
	float latchLo;
	///	Whether to toggle the latch.
	bool latchToggle;
};

//------------------------------------------------------------------------------
///	Class representing a mapping between MIDI CC and an ApplicationCommandTarget.
class MidiAppMapping
{
  public:
	///	Constructor.
	/*!
		\param manager The MidiMappingManager to register and unregister with.
		\param midiCc The MIDI CC which is being mapped to the above parameter.
		\param commandId The ID of the ApplicationCommandTarget to invoke.
	 */
	MidiAppMapping(MidiMappingManager *manager, int midiCc, CommandID commandId);
	///	Constructor to load Mapping parameters from an XmlElement.
	MidiAppMapping(MidiMappingManager *manager, XmlElement *e);
	///	Destructor.
	~MidiAppMapping();

	///	Returns an XmlElement representing this Mapping.
	XmlElement *getXml() const;

	///	Returns the MIDI CC this mapping applies to.
	int getCc() const {return cc;};
	///	Returns the CommandId this mapping applies to.
	CommandID getId() const {return id;};

	juce_UseDebuggingNewOperator
  private:
	///	The main MidiMappingManager.
	MidiMappingManager *midiManager;

	///	This mapping's MIDI CC.
	int cc;
	///	This mapping's command ID.
	CommandID id;
};

//------------------------------------------------------------------------------
///	Class which dispatches MIDI CC messages to MidiMappings.
class MidiMappingManager
{
  public:
	///	Constructor.
	MidiMappingManager(ApplicationCommandManager *manager);
	///	Destructor.
	~MidiMappingManager();

	///	Called when a MIDI CC message is received.
	void midiCcReceived(const MidiMessage& message, double secondsSinceStart);

	///	Registers a MidiMapping with the manager.
	void registerMapping(int midiCc, MidiMapping *mapping);
	///	Unregisters a MidiMapping with the manager.
	void unregisterMapping(MidiMapping *mapping);

	///	Registers a MidiAppMapping with the manager.
	void registerAppMapping(MidiAppMapping *mapping);
	///	Unregisters a MidiAppMapping with the manager.
	void unregisterAppMapping(MidiAppMapping *mapping);

	///	Returns the number of MidiAppMappings.
	int getNumAppMappings() const {return appMappings.size();};
	///	Returns the indexed MidiAppMapping.
	MidiAppMapping *getAppMapping(int index);

	///	Callback used by the MIDI learn functions.
	class MidiLearnCallback
	{
	  public:
		///	Destructor.
		virtual ~MidiLearnCallback() {};

		///	Called when the manager receives a MIDI CC message.
		virtual void midiCcReceived(int val) = 0;
	};
	///	Registers a callback to call when the manager receives the next MIDI CC message.
	/*!
		This callback will be automatically unregistered after it has been
		called once.
	 */
	void registerMidiLearnCallback(MidiLearnCallback *callback);
	///	Unregisters a midi learn callback.
	/*!
		This should only need to be called if the callback has been deleted
		before it received a MIDI CC.
	 */
	void unregisterMidiLearnCallback(MidiLearnCallback *callback);

	///	Returns a StringArray with the full range of named MIDI CCs.
	static const StringArray getCCNames();

	juce_UseDebuggingNewOperator
  private:
	///	Holds all the MidiMappings to dispatch messages to.
	/*!
		Stores mappings with their cc as the key.
	 */
	std::multimap<int, MidiMapping *> mappings;

	///	Holds any MIDI CC->ApplicationCommand mappings.
	std::multimap<int, MidiAppMapping *> appMappings;
	///	Holds a copy of the app's ApplicationCommandManager so we can invoke ApplicationCommands.
	ApplicationCommandManager *appManager;

	///	Used for tap tempo.
	TapTempoHelper tapHelper;

	///	The midi learn callback to call for the next received MIDI CC message.
	/*!
		0 if there's no callback currently registered.
	 */
	MidiLearnCallback *midiLearnCallback;
};

//------------------------------------------------------------------------------
///	Class which intercepts Midi messages so they can be sent to the MidiMappingManager.
class MidiInterceptor : public AudioPluginInstance
{
  public:
	///	Constructor.
	MidiInterceptor();
	///	Destructor.
	~MidiInterceptor();

	///	Registers the current MidiMappingManager with this instance.
	void setManager(MidiMappingManager *manager);

	///	Provides a description of the processor to the filter graph.
	void fillInPluginDescription(PluginDescription &description) const;

	///	Passes any Midi messages onto MidiMappingManager.
	void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	///	Returns the name of the processor.
	const String getName() const {return L"Midi Interceptor";};
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
	///	We definitely want Midi input.
	bool acceptsMidi() const {return true;};
	///	But we don't need to output it.
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
  private:
	///	The MidiMappingManager to pass Midi messages to.
	MidiMappingManager *midiManager;

	///	Used to help calculate tempo.
	int64 samplesSinceStart;
};

#endif
