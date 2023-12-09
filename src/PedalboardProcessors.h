//	PedalboardProcessors.h - Internal processors provided by the app.
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

#ifndef PEDALBOARDPROCESSORS_H_
#define PEDALBOARDPROCESSORS_H_

#include "../JuceLibraryCode/JuceHeader.h"

#include <stdint.h>

class LooperControl;

//------------------------------------------------------------------------------
///	Abstract base of all the internal processors.
class PedalboardProcessor : public AudioPluginInstance
{
  public:
	///	Destructor.
	virtual ~PedalboardProcessor() {};

	///	Returns the component which is added to the instance's PluginComponent.
	/*!
		\return Will be deleted by the caller.
	 */
	virtual Component *getControls() = 0;
	///	Returns the size of the controls component.
	virtual Point<int> getSize() = 0;

	juce_UseDebuggingNewOperator
};

//------------------------------------------------------------------------------
///	Simple processor which provides a level control.
class LevelProcessor : public PedalboardProcessor
{
  public:
	///	Constructor.
	LevelProcessor();
	///	Destructor.
	~LevelProcessor();

	///	Returns the component which is added to the instance's PluginComponent.
	Component *getControls();
	///	Returns the size of the controls component.
	Point<int> getSize() {return Point<int>(64, 64);};

	///	Updates the bounds of our editor window.
	void updateEditorBounds(const Rectangle<int>& bounds);

	///	Provides a description of the processor to the filter graph.
	void fillInPluginDescription(PluginDescription &description) const;

	///	Alters the input audio's level accordingly.
	void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	///	Returns the name of the processor.
	const String getName() const {return L"Level";};
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
	bool acceptsMidi() const {return false;};
	///	But we don't need to output it.
	bool producesMidi() const {return false;};
	///	We have no editor.
	AudioProcessorEditor *createEditor();
	///	We have no editor.
	bool hasEditor() const {return true;};
	///	We have no parameters.
	int getNumParameters() {return 1;};
	///	We have no parameters.
	const String getParameterName(int parameterIndex) {return L"Level";};
	///	We have no parameters.
	float getParameter(int parameterIndex) {return level;};
	///	We have no parameters.
	const String getParameterText(int parameterIndex);
	///	We have no parameters.
	void setParameter(int parameterIndex, float newValue);
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
	///	Loads the position of the slider and the size and position of the editor.
	void getStateInformation(MemoryBlock &destData);
	///	Saves the position of the slider and the size and position of the editor.
	void setStateInformation(const void *data, int sizeInBytes);

	juce_UseDebuggingNewOperator
  private:
	///	The level parameter.
	float level;

	///	The editor's bounds.
	Rectangle<int> editorBounds;
};

//------------------------------------------------------------------------------
///	Processor which plays back an audio file.
class FilePlayerProcessor : public PedalboardProcessor,
							public ChangeListener,
							public ChangeBroadcaster
{
  public:
	///	Constructor.
	FilePlayerProcessor();
	///	Constructor which also sets the processor's sound file.
	FilePlayerProcessor(const File& phil);
	///	Destructor.
	~FilePlayerProcessor();

	///	Sets the sound file to play.
	void setFile(const File& phil);
	///	Returns the sound file.
	const File& getFile() const {return soundFile;};
	///	Returns the current read position within the file.
	/*!
		\return 0->1
	 */
	double getReadPosition() const {return transportSource.getCurrentPosition()/transportSource.getLengthInSeconds();};
	///	Returns whether the file is currently playing.
	bool isPlaying() const {return transportSource.isPlaying();};

	///	Returns the component which is added to the instance's PluginComponent.
	Component *getControls();
	///	Returns the size of the controls component.
	Point<int> getSize() {return Point<int>(300, 100);};

	///	Updates the bounds of our editor window.
	void updateEditorBounds(const Rectangle<int>& bounds);

	///	So we can reset the play position when we reach the end of the file.
	void changeListenerCallback(ChangeBroadcaster *source);

	///	Provides a description of the processor to the filter graph.
	void fillInPluginDescription(PluginDescription &description) const;

	///	Alters the input audio's level accordingly.
	void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	///	Parameter constants.
	enum
	{
		Play = 0,
		ReturnToZero,
		Looping,
		ReadPosition,
		SyncToMainTransport,
		Trigger,

		NumParameters
	};

	///	Returns the name of the processor.
	const String getName() const {return L"File Player";};
	///	Passed to transportSource.
	void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock);
	///	Passed to transportSource.
	void releaseResources();
	///	We have no audio inputs.
	const String getInputChannelName(int channelIndex) const {return L"";};
	///	We have no audio outputs.
	const String getOutputChannelName(int channelIndex) const {return L"";};
	///	We have no audio inputs.
	bool isInputChannelStereoPair(int index) const {return false;};
	///	We have no audio outputs.
	bool isOutputChannelStereoPair(int index) const {return true;};
	///	Returns true if there's no tail.
	bool silenceInProducesSilenceOut() const {return false;};
	///	Returns the length of the plugin's tail.
	double getTailLengthSeconds() const {return 0.0;};
	///	We definitely want Midi input.
	bool acceptsMidi() const {return false;};
	///	But we don't need to output it.
	bool producesMidi() const {return false;};
	///	We have no editor.
	AudioProcessorEditor *createEditor();
	///	We have no editor.
	bool hasEditor() const {return true;};
	///	We have no parameters.
	int getNumParameters() {return NumParameters;};
	///	We have no parameters.
	const String getParameterName(int parameterIndex);
	///	We have no parameters.
	float getParameter(int parameterIndex);
	///	We have no parameters.
	const String getParameterText(int parameterIndex);
	///	We have no parameters.
	void setParameter(int parameterIndex, float newValue);
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
	///	Loads the position of the slider and the size and position of the editor.
	void getStateInformation(MemoryBlock &destData);
	///	Saves the position of the slider and the size and position of the editor.
	void setStateInformation(const void *data, int sizeInBytes);

	juce_UseDebuggingNewOperator
  private:
	///	The transport source which plays the file.
	AudioTransportSource transportSource;
	///	The actual sound file source.
	ScopedPointer<AudioFormatReaderSource> soundFileSource;

	///	The file we're playing.
	File soundFile;
	///	Whether or not we're looping this file.
	bool looping;
	///	Whether or not we're syncing to the main transport.
	bool syncToMainTransport;

	///	The editor's bounds.
	Rectangle<int> editorBounds;

	///	Used to ensure we don't reset the play position to zero when the user clicks pause.
	bool justPaused;
};

//------------------------------------------------------------------------------
///	Simple processor which lets you toggle between outputs.
class OutputToggleProcessor : public PedalboardProcessor
{
  public:
	///	Constructor.
	OutputToggleProcessor();
	///	Destructor.
	~OutputToggleProcessor();

	///	Returns the component which is added to the instance's PluginComponent.
	Component *getControls();
	///	Returns the size of the controls component.
	Point<int> getSize() {return Point<int>(48, 48);};

	///	Updates the bounds of our editor window.
	void updateEditorBounds(const Rectangle<int>& bounds);

	///	Provides a description of the processor to the filter graph.
	void fillInPluginDescription(PluginDescription &description) const;

	///	Alters the input audio's level accordingly.
	void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	///	Returns the name of the processor.
	const String getName() const {return L"Output Toggle";};
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
	bool acceptsMidi() const {return false;};
	///	But we don't need to output it.
	bool producesMidi() const {return false;};
	///	We have no editor.
	AudioProcessorEditor *createEditor();
	///	We have no editor.
	bool hasEditor() const {return true;};
	///	We have no parameters.
	int getNumParameters() {return 1;};
	///	We have no parameters.
	const String getParameterName(int parameterIndex) {return L"Output";};
	///	We have no parameters.
	float getParameter(int parameterIndex) {return toggle ? 1.0f : 0.0f;};
	///	We have no parameters.
	const String getParameterText(int parameterIndex);
	///	We have no parameters.
	void setParameter(int parameterIndex, float newValue);
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
	///	Loads the position of the slider and the size and position of the editor.
	void getStateInformation(MemoryBlock &destData);
	///	Saves the position of the slider and the size and position of the editor.
	void setStateInformation(const void *data, int sizeInBytes);

	juce_UseDebuggingNewOperator
  private:
	///	The toggle parameter.
	bool toggle;
	///	Used to fade between outputs.
	float fade;

	///	The editor's bounds.
	Rectangle<int> editorBounds;
};

//------------------------------------------------------------------------------
///	Simple processor which provides a VU meter.
class VuMeterProcessor : public PedalboardProcessor
{
  public:
	///	Constructor.
	VuMeterProcessor();
	///	Destructor.
	~VuMeterProcessor();

	///	Returns the component which is added to the instance's PluginComponent.
	Component *getControls();
	///	Returns the size of the controls component.
	Point<int> getSize() {return Point<int>(64, 128);};

	///	Returns the current left level.
	float getLeftLevel() const {return levelLeft;};
	///	Returns the current right level.
	float getRightLevel() const {return levelRight;};

	///	Updates the bounds of our editor window.
	void updateEditorBounds(const Rectangle<int>& bounds);

	///	Provides a description of the processor to the filter graph.
	void fillInPluginDescription(PluginDescription &description) const;

	///	Alters the input audio's level accordingly.
	void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	///	Returns the name of the processor.
	const String getName() const {return L"VU Meter";};
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
	bool acceptsMidi() const {return false;};
	///	But we don't need to output it.
	bool producesMidi() const {return false;};
	///	We have no editor.
	AudioProcessorEditor *createEditor();
	///	We have no editor.
	bool hasEditor() const {return true;};
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
	///	Loads the position of the slider and the size and position of the editor.
	void getStateInformation(MemoryBlock &destData);
	///	Saves the position of the slider and the size and position of the editor.
	void setStateInformation(const void *data, int sizeInBytes);

	juce_UseDebuggingNewOperator
  private:
	///	The current level.
	float levelLeft;
	///	The current level.
	float levelRight;

	///	The editor's bounds.
	Rectangle<int> editorBounds;
};

//------------------------------------------------------------------------------
///	Basic processor used to record audio.
class RecorderProcessor : public PedalboardProcessor,
						  public ChangeListener,
						  public ChangeBroadcaster
{
  public:
	///	Constructor.
	RecorderProcessor();
	///	Destructor.
	~RecorderProcessor();

	///	Sets the sound file to play.
	void setFile(const File& phil);
	///	Stores the sound file to record to.
	void cacheFile(const File& phil);
	///	Returns the sound file.
	const File& getFile() const {return soundFile;};
	///	Returns whether or not we're currently recording.
	bool isRecording() const {return recording;};

	///	Returns the component which is added to the instance's PluginComponent.
	Component *getControls();
	///	Returns the size of the controls component.
	Point<int> getSize() {return Point<int>(300, 100);};

	///	Updates the bounds of our editor window.
	void updateEditorBounds(const Rectangle<int>& bounds);

	///	So we can listen to the main transport.
	void changeListenerCallback(ChangeBroadcaster *source);

	///	Provides a description of the processor to the filter graph.
	void fillInPluginDescription(PluginDescription &description) const;

	///	Alters the input audio's level accordingly.
	void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	///	Parameter constants.
	enum
	{
		Record = 0,
		SyncToMainTransport,

		NumParameters
	};

	///	Returns the name of the processor.
	const String getName() const {return L"Audio Recorder";};
	///	Ignored.
	void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock);
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
	bool acceptsMidi() const {return false;};
	///	But we don't need to output it.
	bool producesMidi() const {return false;};
	///	We have no editor.
	AudioProcessorEditor *createEditor();
	///	We have no editor.
	bool hasEditor() const {return true;};
	///	We have no parameters.
	int getNumParameters() {return NumParameters;};
	///	We have no parameters.
	const String getParameterName(int parameterIndex);
	///	We have no parameters.
	float getParameter(int parameterIndex);
	///	We have no parameters.
	const String getParameterText(int parameterIndex);
	///	We have no parameters.
	void setParameter(int parameterIndex, float newValue);
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
	///	Loads the position of the slider and the size and position of the editor.
	void getStateInformation(MemoryBlock &destData);
	///	Saves the position of the slider and the size and position of the editor.
	void setStateInformation(const void *data, int sizeInBytes);

	juce_UseDebuggingNewOperator
  private:
	///	The file being recorded to.
	File soundFile;
	///	Used to record the audio input.
	AudioFormatWriter::ThreadedWriter *threadWriter;

	///	The thumbnail image which gets passed to the Controls and Editor.
	AudioThumbnail thumbnail;

	///	If we're currently recording or not.
	bool recording;
	///	Safeguard in case the user tries to change the file while we're recording.
	bool stopRecording;
	///	Whether or not we're syncing to the main transport.
	bool syncToMainTransport;

	///	The editor's bounds.
	Rectangle<int> editorBounds;

	///	The samplerate passed to prepareToPlay().
	double currentRate;
};

//------------------------------------------------------------------------------
///	Simple metronome processor.
class MetronomeProcessor : public PedalboardProcessor,
						   public ChangeListener,
						   public ChangeBroadcaster
{
  public:
	///	Constructor.
	MetronomeProcessor();
	///	Destructor.
	~MetronomeProcessor();

	///	Returns true if the metronome is currently playing.
	bool isPlaying() const {return playing;};
	///	Sets the accent sound file to play.
	void setAccentFile(const File& phil);
	///	Returns the accent sound file.
	const File& getAccentFile() const {return files[0];};
	///	Sets the click sound file to play.
	void setClickFile(const File& phil);
	///	Returns the click sound file.
	const File& getClickFile() const {return files[1];};

	///	Returns the component which is added to the instance's PluginComponent.
	Component *getControls();
	///	Returns the size of the controls component.
	Point<int> getSize() {return Point<int>(170, 100);};

	///	Updates the bounds of our editor window.
	void updateEditorBounds(const Rectangle<int>& bounds);

	///	So we can listen to the main transport.
	void changeListenerCallback(ChangeBroadcaster *source);

	///	Provides a description of the processor to the filter graph.
	void fillInPluginDescription(PluginDescription &description) const;

	///	Alters the input audio's level accordingly.
	void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	///	Parameter constants.
	enum
	{
		Play = 0,
		Numerator,
		Denominator,
		SyncToMainTransport,

		NumParameters
	};

	///	Returns the name of the processor.
	const String getName() const {return L"Metronome";};
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
	bool silenceInProducesSilenceOut() const {return false;};
	///	Returns the length of the plugin's tail.
	double getTailLengthSeconds() const {return 0.0;};
	///	We definitely want Midi input.
	bool acceptsMidi() const {return false;};
	///	But we don't need to output it.
	bool producesMidi() const {return false;};
	///	We have no editor.
	AudioProcessorEditor *createEditor();
	///	We have no editor.
	bool hasEditor() const {return true;};
	///	We have no parameters.
	int getNumParameters() {return NumParameters;};
	///	We have no parameters.
	const String getParameterName(int parameterIndex);
	///	We have no parameters.
	float getParameter(int parameterIndex);
	///	We have no parameters.
	const String getParameterText(int parameterIndex);
	///	We have no parameters.
	void setParameter(int parameterIndex, float newValue);
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
	///	Loads the position of the slider and the size and position of the editor.
	void getStateInformation(MemoryBlock &destData);
	///	Saves the position of the slider and the size and position of the editor.
	void setStateInformation(const void *data, int sizeInBytes);

	juce_UseDebuggingNewOperator
  private:
	///	The transport sources which play the accent and click files.
	AudioTransportSource transportSource[2];
	///	The accent and click sound file sources.
	ScopedPointer<AudioFormatReaderSource> soundFileSource[2];
	///	The files we're playing.
	File files[2];

	///	If we're currently playing or not.
	bool playing;
	///	Whether or not we're syncing to the main transport.
	bool syncToMainTransport;
	///	The time signature numerator.
	int numerator;
	///	The time signature denominator.
	int denominator;

	///	Default click sound.
	float sineX0;
	///	Default click sound.
	float sineX1;
	///	Default click sound.
	float sineCoeff;
	///	The amplitude envelope for the default click.
	float sineEnv;

	///	Used to count down to the next click.
	float clickCount;
	///	Used to decrement clickCount.
	float clickDec;
	///	Used to count down to the next start of the measure.
	int measureCount;
	///	Whether we're currently playing the accent or the click.
	bool isAccent;

	///	The editor's bounds.
	Rectangle<int> editorBounds;
};

//------------------------------------------------------------------------------
///	A basic looper processor
/*!
	How this works:

	When the user hits record, the input audio is saved to disk via an
	AudioFormatWriter, *and* stored in memory for playback. While this is
	happening the input audio is also streamed to the output.

	Because the looper is intended to handle any length of loop, the memory
	buffer is a little complicated:
	
	To avoid reallocating memory in the audio thread when the buffer needs to
	be resized we actually store an array of fixed-size buffers (8 seconds long
	@ 44.1k samplerate; that's 4 bars @ 4/4 120bpm). We have a separate thread
	which monitors how close we are to the end of the current buffer, and
	allocates a new buffer if we are getting close to the end. Then when the
	audio thread gets to the end of the current buffer it can just start
	writing to the next buffer which should be allocated and ready for it.
 */
class LooperProcessor : public PedalboardProcessor,
					    public ChangeListener,
					    public ChangeBroadcaster,
						public TimeSliceClient,
						public AsyncUpdater
{
  public:
	///	Constructor.
	LooperProcessor();
	///	Destructor.
	~LooperProcessor();

	///	Sets the sound file to play.
	void setFile(const File& phil);
	///	Returns the sound file.
	const File& getFile() {newFileLoaded = false; return soundFile;};
	///	Returns whether or not we're currently playing.
	bool isPlaying() const {return (playing && !stopPlaying);};
	///	Returns whether or not we're currently recording.
	bool isRecording() const {return (recording && !stopRecording);};
	///	Returns the current read position within the file.
	/*!
		\return 0->1
	 */
	double getReadPosition() const;

	///	Returns true if we've just loaded a new sound file.
	/*!
		This is necessary so that any ChangeListeners (e.g. LooperControl) can
		update their elements accordingly.
	 */
	bool getNewFileLoaded() const {return newFileLoaded;};

	///	Used to start recording.
	/*!
		This is necessary because recording may be started via a MidiMessage,
		which will happen in the audio thread. If we do our setup in the audio
		thread we get glitches in the audio (from file deletion, memory
		allocation etc.), so we need to make sure recording is only ever started
		from the main thread. Hence the use of an AsyncUpdater.
	 */
	void handleAsyncUpdate();
	///	Used to allocate new loop buffers if necessary.
	int useTimeSlice();

	///	Returns the component which is added to the instance's PluginComponent.
	Component *getControls();
	///	Returns the size of the controls component.
	Point<int> getSize() {return Point<int>(300, 100);};

	///	Updates the bounds of our editor window.
	void updateEditorBounds(const Rectangle<int>& bounds);

	///	So we can listen to the main transport.
	void changeListenerCallback(ChangeBroadcaster *source);

	///	Provides a description of the processor to the filter graph.
	void fillInPluginDescription(PluginDescription &description) const;

	///	Alters the input audio's level accordingly.
	void processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages);

	///	Ignored.
	void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock);

	///	Parameter constants.
	enum
	{
		Play = 0,
		ReturnToZero,
		Record,
		ReadPosition,
		SyncToMainTransport,
		StopAfterBar,
		AutoPlay,
		BarNumerator,
		BarDenominator,
		InputLevel,
		LoopLevel,

		NumParameters
	};

	///	Returns the name of the processor.
	const String getName() const {return L"Looper";};
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
	bool silenceInProducesSilenceOut() const {return false;};
	///	Returns the length of the plugin's tail.
	double getTailLengthSeconds() const {return 0.0;};
	///	We definitely want Midi input.
	bool acceptsMidi() const {return false;};
	///	But we don't need to output it.
	bool producesMidi() const {return false;};
	///	We have no editor.
	AudioProcessorEditor *createEditor();
	///	We have no editor.
	bool hasEditor() const {return true;};
	///	We have no parameters.
	int getNumParameters() {return NumParameters;};
	///	We have no parameters.
	const String getParameterName(int parameterIndex);
	///	We have no parameters.
	float getParameter(int parameterIndex);
	///	We have no parameters.
	const String getParameterText(int parameterIndex);
	///	We have no parameters.
	void setParameter(int parameterIndex, float newValue);
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
	///	Loads the position of the slider and the size and position of the editor.
	void getStateInformation(MemoryBlock &destData);
	///	Saves the position of the slider and the size and position of the editor.
	void setStateInformation(const void *data, int sizeInBytes);

	juce_UseDebuggingNewOperator
  private:
	///	Helper method. Copies the contents of tempBuffer into fadeInBuffer.
	void fillFadeInBuffer();
	///	Helper method. Copies the contents of tempBuffer into fadeOutBuffer.
	void fillFadeOutBuffer();

	///	The size of each memory buffer we use.
	enum
	{
		LoopBufferSize = (44100 * 8),
		FadeBufferSize = 128
	};

	///	The file we're playing.
	File soundFile;
	///	If we're currently playing or not.
	bool playing;
	///	Safeguard in case the user tries to record a new loop while we're playing.
	bool stopPlaying;
	///	If we're currently recording or not.
	bool recording;
	///	Safeguard in case the user tries to change the file while we're recording.
	bool stopRecording;
	///	Whether or not we're syncing to the main transport.
	bool syncToMainTransport;
	///	Whether or not recording should stop after a bar.
	bool stopAfterBar;
	///	True if playback should start immediately after recording has stopped.
	bool autoPlay;
	///	The output level of the looper's input.
	float inputLevel;
	///	The output level of the looper's loop.
	float loopLevel;

	///	Used to record the audio input.
	AudioFormatWriter::ThreadedWriter *threadWriter;

	///	The thumbnail image which gets passed to the Controls and Editor.
	AudioThumbnail thumbnail;

	///	The editor's bounds.
	Rectangle<int> editorBounds;

	///	The time signature numerator.
	int numerator;
	///	The time signature denominator.
	int denominator;

	///	Used to count down to the next click.
	float clickCount;
	///	Used to decrement clickCount.
	float clickDec;
	///	Used to count down to the next start of the measure.
	int measureCount;

	///	The samplerate passed to prepareToPlay().
	double currentRate;
	///	Used to ensure we don't reset the play position to zero when the user clicks pause.
	bool justPaused;

	///	The length of the loop in samples.
	uint64_t loopLength;
	///	The loop buffer in memory.
	OwnedArray<AudioSampleBuffer> loopBuffer;
	///	Our playback position in the loop buffer.
	int loopPos;
	///	Which loop buffer we're currently playing back.
	int loopIndex;
	///	Used to ensure the last loop buffer is deleted.
	/*!
		This is necessary for when we stop recording after a new loop buffer
		has been created, but before we've actually started writing into it.
	 */
	bool deleteLastBuffer;

	///	The temporary buffer which is copied into the two fade buffers.
	float tempBuffer[2][FadeBufferSize];
	///	The current write position into tempBuffer.
	int tempBufferWrite;
	///	The fade-in buffer (for seamless looping).
	float fadeInBuffer[2][FadeBufferSize];
	///	The fade-out buffer (for seamless looping).
	float fadeOutBuffer[2][FadeBufferSize];
	///	Used to determine when the fade-out buffer can be filled from the temp buffer.
	int fadeOutCount;
	///	Used to determine when to start the fade-in at the end.
	uint64_t fadeInCount;
	///	Used to fade in the loop buffer after recording stops (& autoPlay is on).
	float autoPlayFade;

	///	Used to read a file into the loopBuffers.
	AudioFormatReader *fileReader;
	///	Used to read a file into the loopBuffers.
	int64 fileReaderPos;
	///	Which loopBuffer we are writing to next.
	int fileReaderBufIndex;

	///	Used to let our ChangeListeners know that we've loaded a new file.
	bool newFileLoaded;

	///	The input audio data to processBlock().
	AudioSampleBuffer inputAudio;
};

#endif
