//	BypassableInstance.h - Wrapper class to provide a bypass to
//						   AudioPluginInstance.
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

#ifndef BYPASSABLEINSTANCE_H_
#define BYPASSABLEINSTANCE_H_

#include "../JuceLibraryCode/JuceHeader.h"

///	Wrapper class to provide a bypass to AudioPluginInstance.
class BypassableInstance : public AudioPluginInstance
{
  public:
	///	Constructor.
	BypassableInstance(AudioPluginInstance *plug);
	///	Destructor.
	~BypassableInstance();

	///	Sets up our buffer.
	void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock);
	///	Clears our buffer.
	void releaseResources();
	///	Handles the audio.
	void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	///	Sets the bypass state.
	void setBypass(bool val);
	///	Returns the bypass state.
	bool getBypass() const {return bypass;};

	///	Sets the MIDI channel the plugin responds to.
	void setMIDIChannel(int val);
	///	Returns the plugin's MIDI channel (-1 == omni).
	int getMIDIChannel() const {return midiChannel;};
	///	Passes a MIDI message to the plugin from the OSC input.
	void addMidiMessage(const MidiMessage& message);

	///	Returns the plugin instance we're wrapping.
	AudioPluginInstance *getPlugin() {return plugin;};

	///	Returns the plugin's name.
	const String getName() const {return plugin->getName();};
	///	Returns the indexed channel name.
	const String getInputChannelName(int channelIndex) const {return plugin->getInputChannelName(channelIndex);};
	///	Returns the indexed channel name.
	const String getOutputChannelName(int channelIndex) const {return plugin->getOutputChannelName(channelIndex);};
	///	Returns true if the channel's a stereo pair.
	bool isInputChannelStereoPair(int index) const {return plugin->isInputChannelStereoPair(index);};
	///	Returns true if the channel's a stereo pair.
	bool isOutputChannelStereoPair(int index) const {return plugin->isOutputChannelStereoPair(index);};
	///	Returns true if the plugin has no tail.
	bool silenceInProducesSilenceOut() const {return plugin->silenceInProducesSilenceOut();};
	///	Returns the length of the plugin's tail.
	double getTailLengthSeconds() const {return plugin->getTailLengthSeconds();};
	///	Returns true if the plugin wants MIDI input.
	bool acceptsMidi() const {return plugin->acceptsMidi();};
	///	Returns true if the plugin produces MIDI output.
	bool producesMidi() const {return plugin->producesMidi();};
	///	Can be used to reset the plugin.
	void reset() {plugin->reset();};
	///	Creates the plugin's editor.
	AudioProcessorEditor *createEditor() {return plugin->createEditor();};
	///	Returns true if the plugin has an editor.
	bool hasEditor() const {return plugin->hasEditor();};
	///	Returns the number of parameters the plugin has.
	int getNumParameters() {return plugin->getNumParameters();};
	///	Returns the indexed parameter's name.
	const String getParameterName(int parameterIndex) {return plugin->getParameterName(parameterIndex);};
	///	Returns the indexed parameter's value.
	float getParameter(int parameterIndex) {return plugin->getParameter(parameterIndex);};
	///	Returns the indexed parameter's value as a string.
	const String getParameterText(int parameterIndex) {return plugin->getParameterText(parameterIndex);};
	///	Sets the indexed parameter.
	void setParameter(int parameterIndex, float newValue) {plugin->setParameter(parameterIndex, newValue);};
	///	Returns true if the indexed parameter is automatable.
	bool isParameterAutomatable(int parameterIndex) const {return plugin->isParameterAutomatable(parameterIndex);};
	///	Returns true if the indexed parameter is meta.
	bool isMetaParameter(int parameterIndex) const {return plugin->isMetaParameter(parameterIndex);};
	///	Returns the number of programs the plugin has.
	int getNumPrograms() {return plugin->getNumPrograms();};
	///	Returns the index of the current program.
	int getCurrentProgram() {return plugin->getCurrentProgram();};
	///	Sets the current program.
	void setCurrentProgram(int index) {plugin->setCurrentProgram(index);};
	///	Returns the indexed program's name.
	const String getProgramName(int index) {return plugin->getProgramName(index);};
	///	Changes the indexed program's name.
	void changeProgramName(int index, const String &newName) {plugin->changeProgramName(index, newName);};
	///	Used to save the plugin's internal state.
	void getStateInformation(juce::MemoryBlock &destData) {plugin->getStateInformation(destData);};
	///	Used to save the state of the current program.
	void getCurrentProgramStateInformation(juce::MemoryBlock &destData) {plugin->getCurrentProgramStateInformation(destData);};
	///	Restores the plugin's internal state.
	void setStateInformation(const void *data, int sizeInBytes) {plugin->setStateInformation(data, sizeInBytes);};
	///	Sets the state of the current program.
	void setCurrentProgramStateInformation(const void *data, int sizeInBytes) {plugin->setCurrentProgramStateInformation(data, sizeInBytes);};
	///	Fills in the plugin's description.
	void fillInPluginDescription(PluginDescription &description) const {plugin->fillInPluginDescription(description);};
	///	Returns platform-specific data about the plugin.
	void *getPlatformSpecificData() {return plugin->getPlatformSpecificData();};
  private:
	///	The plugin instance we're wrapping.
	AudioPluginInstance *plugin;

	///	Buffer used to store the plugin's audio.
	AudioSampleBuffer tempBuffer;

	///	Whether we are currently bypassing the plugin or not.
	bool bypass;
	///	Used to ramp the bypass audio.
	float bypassRamp;

	///	The MIDI channel the plugin responds to.
	int midiChannel;
	///	Used to pass OSC MIDI messages to the plugin.
	MidiMessageCollector midiCollector;
};

#endif
