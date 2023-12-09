//	PedalboardProcessors.cpp - Internal processors provided by the app.
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

#include "PedalboardProcessorEditors.h"
#include "PedalboardProcessors.h"
#include "AudioRecorderControl.h"
#include "FilePlayerControl.h"
#include "MetronomeControl.h"
#include "AudioSingletons.h"
#include "MainTransport.h"
#include "LooperControl.h"
#include "LooperEditor.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LevelProcessor::LevelProcessor():
level(0.5f)
{
	setPlayConfigDetails(2, 2, 0, 0);
}

//------------------------------------------------------------------------------
LevelProcessor::~LevelProcessor()
{

}

//------------------------------------------------------------------------------
Component *LevelProcessor::getControls()
{
	LevelControl *retval = new LevelControl(this);

	return retval;
}

//------------------------------------------------------------------------------
void LevelProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
	editorBounds = bounds;
}

//------------------------------------------------------------------------------
void LevelProcessor::fillInPluginDescription(PluginDescription &description) const
{
	description.name = L"Level";
	description.descriptiveName = L"Simple level processor.";
	description.pluginFormatName = L"Internal";
	description.category = L"Pedalboard Processors";
	description.manufacturerName = L"Niall Moody";
	description.version = L"1.00";
	description.uid = description.name.hashCode();
	description.isInstrument = false;
	description.numInputChannels = 2;
	description.numOutputChannels = 2;
}

//------------------------------------------------------------------------------
void LevelProcessor::processBlock(AudioSampleBuffer &buffer,
								  MidiBuffer &midiMessages)
{
	int i;
	float *dataLeft;
	float *dataRight;

	jassert(buffer.getNumChannels() > 1);

	dataLeft = buffer.getSampleData(0);
	dataRight = buffer.getSampleData(1);

	for(i=0;i<buffer.getNumSamples();++i)
	{
		dataLeft[i] = dataLeft[i] * level * 2.0f;
		dataRight[i] = dataRight[i] * level * 2.0f;
	}
}

//------------------------------------------------------------------------------
AudioProcessorEditor *LevelProcessor::createEditor()
{
	return new LevelEditor(this, editorBounds);
}

//------------------------------------------------------------------------------
const String LevelProcessor::getParameterText(int parameterIndex)
{
	String retval;

	retval << (level*2.0f);

	return retval;
}

//------------------------------------------------------------------------------
void LevelProcessor::setParameter(int parameterIndex, float newValue)
{
	level = newValue;
}

//------------------------------------------------------------------------------
void LevelProcessor::getStateInformation(MemoryBlock &destData)
{
    XmlElement xml("Pedalboard2LevelSettings");

    xml.setAttribute("level", level);

    xml.setAttribute("editorX", editorBounds.getX());
    xml.setAttribute("editorY", editorBounds.getY());
    xml.setAttribute("editorW", editorBounds.getWidth());
    xml.setAttribute("editorH", editorBounds.getHeight());

    copyXmlToBinary(xml, destData);
}

//------------------------------------------------------------------------------
void LevelProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != 0)
    {
        if (xmlState->hasTagName("Pedalboard2LevelSettings"))
        {
            editorBounds.setX(xmlState->getIntAttribute("editorX"));
            editorBounds.setY(xmlState->getIntAttribute("editorY"));
            editorBounds.setWidth(xmlState->getIntAttribute("editorW"));
            editorBounds.setHeight(xmlState->getIntAttribute("editorH"));

            level = (float)xmlState->getDoubleAttribute("level", 0.5);
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
FilePlayerProcessor::FilePlayerProcessor():
looping(false),
syncToMainTransport(false),
justPaused(false)
{
	setPlayConfigDetails(0, 2, 0, 0);
	transportSource.addChangeListener(this);

	MainTransport::getInstance()->registerTransport(this);
}

//------------------------------------------------------------------------------
FilePlayerProcessor::FilePlayerProcessor(const File& phil):
looping(false),
syncToMainTransport(false),
justPaused(false)
{
	setPlayConfigDetails(0, 2, 0, 0);
	transportSource.addChangeListener(this);

	MainTransport::getInstance()->registerTransport(this);

	setFile(phil);
}

//------------------------------------------------------------------------------
FilePlayerProcessor::~FilePlayerProcessor()
{
	removeAllChangeListeners();
	MainTransport::getInstance()->unregisterTransport(this);
	transportSource.setSource(0);
}

//------------------------------------------------------------------------------
void FilePlayerProcessor::setFile(const File& phil)
{
	int readAheadSize;

	soundFile = phil;

	//Unload the previous file source and delete it.
    transportSource.stop();
    transportSource.setSource(0);
    soundFileSource = 0;

    //Get a format manager and set it up with the basic types (wav and aiff).
    /*AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    AudioFormatReader *reader = formatManager.createReaderFor(phil);*/
	AudioFormatReader *reader = AudioFormatManagerSingleton::getInstance().createReaderFor(phil);

    if(reader != 0)
    {
        soundFileSource = new AudioFormatReaderSource(reader, true);
		soundFileSource->setLooping(looping);

		if(soundFileSource->getTotalLength() < 32768)
			readAheadSize = 0;
		else
			readAheadSize = 32768;

        //Plug it into our transport source.
        transportSource.setSource(soundFileSource,
                                  readAheadSize, //Tells it to buffer this many samples ahead.
								  &(AudioThumbnailCacheSingleton::getInstance().getTimeSliceThread()));
    }
}

//------------------------------------------------------------------------------
Component *FilePlayerProcessor::getControls()
{
	FilePlayerControl *retval = new FilePlayerControl(this);

	return retval;
}

//------------------------------------------------------------------------------
void FilePlayerProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
	editorBounds = bounds;
}

//------------------------------------------------------------------------------
void FilePlayerProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
	if(source == MainTransport::getInstance())
	{
		if(syncToMainTransport)
		{
			//Play/pause the transport source.
			if(MainTransport::getInstance()->getState())
			{
				if(!transportSource.isPlaying())
					transportSource.start();
			}
			else
			{
				if(transportSource.isPlaying())
				{
					justPaused = true;
					transportSource.stop();
				}
			}

			//Return to zero if necessary.
			if(MainTransport::getInstance()->getReturnToZero())
			{
				transportSource.setPosition(0.0);
				sendChangeMessage();
			}
		}
	}
	else
	{
		if(!transportSource.isPlaying() && !justPaused)
		{
			transportSource.setPosition(0.0);
			MainTransport::getInstance()->transportFinished();
		}
		if(justPaused)
			justPaused = false;

		sendChangeMessage();
	}
}

//------------------------------------------------------------------------------
void FilePlayerProcessor::fillInPluginDescription(PluginDescription &description) const
{
	description.name = L"File Player";
	description.descriptiveName = L"Processor which plays back an audio file.";
	description.pluginFormatName = L"Internal";
	description.category = L"Pedalboard Processors";
	description.manufacturerName = L"Niall Moody";
	description.version = "1.00";
	description.uid = description.name.hashCode();
	description.isInstrument = false;
	description.numInputChannels = 0;
	description.numOutputChannels = 2;
}

//------------------------------------------------------------------------------
void FilePlayerProcessor::processBlock(AudioSampleBuffer &buffer,
									   MidiBuffer &midiMessages)
{
	AudioSourceChannelInfo bufferInfo;

	jassert(buffer.getNumChannels() > 0);

	//Fill out the transportSource's buffer.
	bufferInfo.buffer = &buffer;
	bufferInfo.startSample = 0;
	bufferInfo.numSamples = buffer.getNumSamples();

	//Write the transportSource's audio to the buffer.
	transportSource.getNextAudioBlock(bufferInfo);
}

//------------------------------------------------------------------------------
void FilePlayerProcessor::prepareToPlay(double sampleRate,
										int estimatedSamplesPerBlock)
{
	//Why are the arguments the opposite way round???
	transportSource.prepareToPlay(estimatedSamplesPerBlock, sampleRate);
}

//------------------------------------------------------------------------------
void FilePlayerProcessor::releaseResources()
{
	transportSource.releaseResources();
}

//------------------------------------------------------------------------------
AudioProcessorEditor *FilePlayerProcessor::createEditor()
{
	return new FilePlayerEditor(this, editorBounds);
}

//------------------------------------------------------------------------------
const String FilePlayerProcessor::getParameterName(int parameterIndex)
{
	String retval;

	switch(parameterIndex)
	{
		case Play:
			retval = L"Play";
			break;
		case ReturnToZero:
			retval = L"Return to Zero";
			break;
		case Looping:
			retval = L"Looping";
			break;
		case ReadPosition:
			retval = L"Read Position";
			break;
		case SyncToMainTransport:
			retval = L"Sync to Main Transport";
			break;
		case Trigger:
			retval = L"Trigger";
			break;
	}

	return retval;
}

//------------------------------------------------------------------------------
float FilePlayerProcessor::getParameter(int parameterIndex)
{
	float retval = 0.0f;

	switch(parameterIndex)
	{
		case Looping:
			retval = looping ? 1.0f : 0.0f;
			break;
		case SyncToMainTransport:
			retval = syncToMainTransport ? 1.0f : 0.0f;
			break;
	}

	return retval;
}

//------------------------------------------------------------------------------
const String FilePlayerProcessor::getParameterText(int parameterIndex)
{
	String retval;

	switch(parameterIndex)
	{
		case Looping:
			if(looping)
				retval = L"looping";
			else
				retval = L"not looping";
			break;
		case SyncToMainTransport:
			if(syncToMainTransport)
				retval = L"synced";
			else
				retval = L"not synced";
			break;
	}

	return retval;
}

//------------------------------------------------------------------------------
void FilePlayerProcessor::setParameter(int parameterIndex, float newValue)
{
	switch(parameterIndex)
	{
		case Play:
			if(newValue > 0.5f)
			{
				if(transportSource.isPlaying())
				{
					justPaused = true;
					transportSource.stop();
				}
				else
					transportSource.start();
			}
			sendChangeMessage();
			break;
		case ReturnToZero:
			transportSource.setPosition(0.0);
			sendChangeMessage();
			break;
		case Looping:
			if(newValue > 0.5f)
				looping = true;
			else
				looping = false;
			if(soundFileSource)
				soundFileSource->setLooping(looping);
			sendChangeMessage();
			break;
		case ReadPosition:
			transportSource.setPosition(newValue * transportSource.getLengthInSeconds());
			sendChangeMessage();
			break;
		case SyncToMainTransport:
			if(newValue > 0.5f)
				syncToMainTransport = true;
			else
				syncToMainTransport = false;
			sendChangeMessage();
			break;
		case Trigger:
			if(newValue > 0.5f)
			{
				transportSource.setPosition(0.0);

				if(!transportSource.isPlaying())
					transportSource.start();

				sendChangeMessage();
			}
			break;
	}
}

//------------------------------------------------------------------------------
void FilePlayerProcessor::getStateInformation(MemoryBlock &destData)
{
    XmlElement xml("Pedalboard2FilePlayerSettings");

	xml.setAttribute("file", soundFile.getFullPathName());
	xml.setAttribute("looping", looping);
	xml.setAttribute("syncToMainTransport", syncToMainTransport);

    xml.setAttribute("editorX", editorBounds.getX());
    xml.setAttribute("editorY", editorBounds.getY());
    xml.setAttribute("editorW", editorBounds.getWidth());
    xml.setAttribute("editorH", editorBounds.getHeight());

    copyXmlToBinary(xml, destData);
}

//------------------------------------------------------------------------------
void FilePlayerProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != 0)
    {
        if (xmlState->hasTagName("Pedalboard2FilePlayerSettings"))
        {
			setFile(xmlState->getStringAttribute("file"));
			looping = xmlState->getBoolAttribute("looping");
			if(soundFileSource)
				soundFileSource->setLooping(looping);
			syncToMainTransport = xmlState->getBoolAttribute("syncToMainTransport");

            editorBounds.setX(xmlState->getIntAttribute("editorX"));
            editorBounds.setY(xmlState->getIntAttribute("editorY"));
            editorBounds.setWidth(xmlState->getIntAttribute("editorW"));
            editorBounds.setHeight(xmlState->getIntAttribute("editorH"));
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
OutputToggleProcessor::OutputToggleProcessor():
toggle(false),
fade(0.0f)
{
	setPlayConfigDetails(1, 2, 0, 0);
}

//------------------------------------------------------------------------------
OutputToggleProcessor::~OutputToggleProcessor()
{

}

//------------------------------------------------------------------------------
Component *OutputToggleProcessor::getControls()
{
	OutputToggleControl *retval = new OutputToggleControl(this);

	return retval;
}

//------------------------------------------------------------------------------
void OutputToggleProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
	editorBounds = bounds;
}

//------------------------------------------------------------------------------
void OutputToggleProcessor::fillInPluginDescription(PluginDescription &description) const
{
	description.name = L"Output Toggle";
	description.descriptiveName = L"Simple output toggle processor.";
	description.pluginFormatName = L"Internal";
	description.category = L"Pedalboard Processors";
	description.manufacturerName = L"Niall Moody";
	description.version = L"1.00";
	description.uid = description.name.hashCode();
	description.isInstrument = false;
	description.numInputChannels = 1;
	description.numOutputChannels = 2;
}

//------------------------------------------------------------------------------
void OutputToggleProcessor::processBlock(AudioSampleBuffer &buffer,
										 MidiBuffer &midiMessages)
{
	int i;
	float tempf;
	float *data[2];

	jassert(buffer.getNumChannels() > 1);

	data[0] = buffer.getSampleData(0);
	data[1] = buffer.getSampleData(1);

	/*if(!toggle)
	{
		for(i=0;i<buffer.getNumSamples();++i)
		{
			data[0][i] = data[0][i];
			data[1][i] = 0.0f;
		}
	}
	else
	{
		for(i=0;i<buffer.getNumSamples();++i)
		{
			data[1][i] = data[0][i];
			data[0][i] = 0.0f;
		}
	}*/

	for(i=0;i<buffer.getNumSamples();++i)
	{
		if(!toggle && (fade > 0.0f))
			fade -= 0.001f;
		else if(toggle && (fade < 1.0f))
			fade += 0.001f;

		tempf = data[0][i];
		data[0][i] = tempf * (1.0f-fade);
		data[1][i] = tempf * fade;
	}
}

//------------------------------------------------------------------------------
AudioProcessorEditor *OutputToggleProcessor::createEditor()
{
	return new OutputToggleEditor(this, editorBounds);
}

//------------------------------------------------------------------------------
const String OutputToggleProcessor::getParameterText(int parameterIndex)
{
	String retval;

	if(toggle)
		retval = "Output 1";
	else
		retval = "Output 2";

	return retval;
}

//------------------------------------------------------------------------------
void OutputToggleProcessor::setParameter(int parameterIndex, float newValue)
{
	toggle = newValue > 0.5f;
}

//------------------------------------------------------------------------------
void OutputToggleProcessor::getStateInformation(MemoryBlock &destData)
{
    XmlElement xml("Pedalboard2OutputToggleSettings");

    xml.setAttribute("toggle", toggle);

    xml.setAttribute("editorX", editorBounds.getX());
    xml.setAttribute("editorY", editorBounds.getY());
    xml.setAttribute("editorW", editorBounds.getWidth());
    xml.setAttribute("editorH", editorBounds.getHeight());

    copyXmlToBinary(xml, destData);
}

//------------------------------------------------------------------------------
void OutputToggleProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != 0)
    {
        if (xmlState->hasTagName("Pedalboard2OutputToggleSettings"))
        {
            toggle = xmlState->getBoolAttribute("toggle", false);

            editorBounds.setX(xmlState->getIntAttribute("editorX"));
            editorBounds.setY(xmlState->getIntAttribute("editorY"));
            editorBounds.setWidth(xmlState->getIntAttribute("editorW"));
            editorBounds.setHeight(xmlState->getIntAttribute("editorH"));
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VuMeterProcessor::VuMeterProcessor():
levelLeft(0.0f),
levelRight(0.0f)
{
	setPlayConfigDetails(2, 0, 0, 0);
}

//------------------------------------------------------------------------------
VuMeterProcessor::~VuMeterProcessor()
{

}

//------------------------------------------------------------------------------
Component *VuMeterProcessor::getControls()
{
	VuMeterControl *retval = new VuMeterControl(this);

	return retval;
}

//------------------------------------------------------------------------------
void VuMeterProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
	editorBounds = bounds;
}

//------------------------------------------------------------------------------
void VuMeterProcessor::fillInPluginDescription(PluginDescription &description) const
{
	description.name = L"VU Meter";
	description.descriptiveName = L"Simple VU Meter.";
	description.pluginFormatName = L"Internal";
	description.category = L"Pedalboard Processors";
	description.manufacturerName = L"Niall Moody";
	description.version = L"1.00";
	description.uid = description.name.hashCode();
	description.isInstrument = false;
	description.numInputChannels = 2;
	description.numOutputChannels = 0;
}

//------------------------------------------------------------------------------
void VuMeterProcessor::processBlock(AudioSampleBuffer &buffer,
								    MidiBuffer &midiMessages)
{
	int i;
	float *dataLeft;
	float *dataRight;

	jassert(buffer.getNumChannels() > 1);

	dataLeft = buffer.getSampleData(0);
	dataRight = buffer.getSampleData(1);

	for(i=0;i<buffer.getNumSamples();++i)
	{
		if(fabsf(dataLeft[i]) > levelLeft)
			levelLeft = fabsf(dataLeft[i]);
		else if(levelLeft > 0.0f)
		{
			levelLeft -= 0.00001f;
			if(levelLeft < 0.0f)
				levelLeft = 0.0f;
		}

		if(fabsf(dataRight[i]) > levelRight)
			levelRight = fabsf(dataRight[i]);
		else if(levelRight > 0.0f)
		{
			levelRight -= 0.00001f;
			if(levelRight < 0.0f)
				levelRight = 0.0f;
		}
	}
}

//------------------------------------------------------------------------------
AudioProcessorEditor *VuMeterProcessor::createEditor()
{
	return new VuMeterEditor(this, editorBounds);
}

//------------------------------------------------------------------------------
void VuMeterProcessor::getStateInformation(MemoryBlock &destData)
{
    XmlElement xml("Pedalboard2VuMeterSettings");

    xml.setAttribute("editorX", editorBounds.getX());
    xml.setAttribute("editorY", editorBounds.getY());
    xml.setAttribute("editorW", editorBounds.getWidth());
    xml.setAttribute("editorH", editorBounds.getHeight());

    copyXmlToBinary(xml, destData);
}

//------------------------------------------------------------------------------
void VuMeterProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != 0)
    {
        if (xmlState->hasTagName("Pedalboard2VuMeterSettings"))
        {
            editorBounds.setX(xmlState->getIntAttribute("editorX"));
            editorBounds.setY(xmlState->getIntAttribute("editorY"));
            editorBounds.setWidth(xmlState->getIntAttribute("editorW"));
            editorBounds.setHeight(xmlState->getIntAttribute("editorH"));
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
RecorderProcessor::RecorderProcessor():
threadWriter(0),
thumbnail(512,
		  AudioFormatManagerSingleton::getInstance(),
		  AudioThumbnailCacheSingleton::getInstance()),
recording(false),
stopRecording(false),
syncToMainTransport(false),
currentRate(44100.0)
{
	setPlayConfigDetails(2, 0, 0, 0);

	MainTransport::getInstance()->registerTransport(this);
}

//------------------------------------------------------------------------------
RecorderProcessor::~RecorderProcessor()
{
	removeAllChangeListeners();
	MainTransport::getInstance()->unregisterTransport(this);

	if(threadWriter)
		delete threadWriter;
}

//------------------------------------------------------------------------------
void RecorderProcessor::setFile(const File& phil)
{
	WavAudioFormat wavFormat;
	StringPairArray metadata;
	AudioFormatWriter *writer;
	FileOutputStream *philStream;

	if(recording)
	{
		stopRecording = true;

		//Wait till the end of the current audio buffer.
		while(recording) {Thread::sleep(10);}
	}

	if(threadWriter)
	{
		delete threadWriter;
		threadWriter = 0;

		thumbnail.clear();
	}

	soundFile = phil;

	//So we overwrite any previous file.
	if(soundFile.existsAsFile())
	{
		if(!soundFile.deleteFile())
		{
			AlertWindow::showMessageBox(AlertWindow::WarningIcon,
										"Could not delete existing file",
										"Have you got the file open elsewhere? (e.g. in another File Player)");

			soundFile = File::nonexistent;
		}
	}

	if(soundFile != File::nonexistent)
	{
		philStream = new FileOutputStream(soundFile);

		writer = wavFormat.createWriterFor(philStream,
											currentRate,
											2,
											16,
											metadata,
											0);
		if(!writer)
		{
			delete philStream;
			soundFile = File::nonexistent;
			threadWriter = 0;
		}
		else
		{
			threadWriter = new AudioFormatWriter::ThreadedWriter(writer,
																 AudioThumbnailCacheSingleton::getInstance().getTimeSliceThread(),
																 16384);
			threadWriter->setDataReceiver(&thumbnail);
		}
	}
}

//------------------------------------------------------------------------------
void RecorderProcessor::cacheFile(const File& phil)
{
	soundFile = phil;
}

//------------------------------------------------------------------------------
Component *RecorderProcessor::getControls()
{
	AudioRecorderControl *retval = new AudioRecorderControl(this, thumbnail);

	return retval;
}

//------------------------------------------------------------------------------
void RecorderProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
	editorBounds = bounds;
}

//------------------------------------------------------------------------------
void RecorderProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
	if(source == MainTransport::getInstance())
	{
		if(syncToMainTransport)
		{
			//Play/pause the transport source.
			if(MainTransport::getInstance()->getState())
			{
				if(!recording)
					setFile(soundFile);

				if(!recording && !stopRecording && threadWriter)
					recording = true;
			}
			else
			{
				if(recording)
				{
					stopRecording = true;

					setFile(File::nonexistent);
				}
			}
			sendChangeMessage();
		}
	}
}

//------------------------------------------------------------------------------
void RecorderProcessor::fillInPluginDescription(PluginDescription &description) const
{
	description.name = L"Audio Recorder";
	description.descriptiveName = L"Simple audio recorder.";
	description.pluginFormatName = L"Internal";
	description.category = L"Pedalboard Processors";
	description.manufacturerName = L"Niall Moody";
	description.version = L"1.00";
	description.uid = description.name.hashCode();
	description.isInstrument = false;
	description.numInputChannels = 2;
	description.numOutputChannels = 0;
}

//------------------------------------------------------------------------------
void RecorderProcessor::processBlock(AudioSampleBuffer &buffer,
									 MidiBuffer &midiMessages)
{
	float *data[2];

	jassert(buffer.getNumChannels() > 1);

	data[0] = buffer.getSampleData(0);
	data[1] = buffer.getSampleData(1);

	if(recording && threadWriter)
	{
		threadWriter->write((const float **)data, buffer.getNumSamples());

		if(stopRecording)
		{
			recording = false;
			stopRecording = false;
		}
	}
	else if(recording)
	{
		recording = false;
		stopRecording = false;
	}
}

//------------------------------------------------------------------------------
AudioProcessorEditor *RecorderProcessor::createEditor()
{
	return new AudioRecorderEditor(this, editorBounds, thumbnail);
}

//------------------------------------------------------------------------------
void RecorderProcessor::prepareToPlay(double sampleRate,
									  int estimatedSamplesPerBlock)
{
	currentRate = sampleRate;
}

//------------------------------------------------------------------------------
const String RecorderProcessor::getParameterName(int parameterIndex)
{
	String retval;

	switch(parameterIndex)
	{
		case Record:
			retval = L"Record";
			break;
		case SyncToMainTransport:
			retval = L"Sync to Main Transport";
			break;
	}

	return retval;
}

//------------------------------------------------------------------------------
float RecorderProcessor::getParameter(int parameterIndex)
{
	float retval = 0.0f;

	switch(parameterIndex)
	{
		case SyncToMainTransport:
			retval = syncToMainTransport ? 1.0f : 0.0f;
			break;
	}

	return retval;
}

//------------------------------------------------------------------------------
const String RecorderProcessor::getParameterText(int parameterIndex)
{
	String retval;

	switch(parameterIndex)
	{
		case SyncToMainTransport:
			if(syncToMainTransport)
				retval = L"synced";
			else
				retval = L"not synced";
			break;
	}

	return retval;
}

//------------------------------------------------------------------------------
void RecorderProcessor::setParameter(int parameterIndex, float newValue)
{
	switch(parameterIndex)
	{
		case Record:
			if(newValue > 0.5f)
			{
				if(!recording)
					setFile(soundFile);

				if(!recording && !stopRecording && threadWriter)
					recording = true;
				else if(recording)
				{
					stopRecording = true;

					//Saves the file to disk.
					setFile(File::nonexistent);

					if(syncToMainTransport)
						MainTransport::getInstance()->transportFinished();
				}
				sendChangeMessage();
			}
			break;
		case SyncToMainTransport:
			if(newValue > 0.5f)
				syncToMainTransport = true;
			else
				syncToMainTransport = false;
			sendChangeMessage();
			break;
	}
}

//------------------------------------------------------------------------------
void RecorderProcessor::getStateInformation(MemoryBlock &destData)
{
    XmlElement xml("Pedalboard2RecorderSettings");

    xml.setAttribute("editorX", editorBounds.getX());
    xml.setAttribute("editorY", editorBounds.getY());
    xml.setAttribute("editorW", editorBounds.getWidth());
    xml.setAttribute("editorH", editorBounds.getHeight());
	
	xml.setAttribute("file", soundFile.getFullPathName());
	xml.setAttribute("syncToMainTransport", syncToMainTransport);

    copyXmlToBinary(xml, destData);
}

//------------------------------------------------------------------------------
void RecorderProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != 0)
    {
        if (xmlState->hasTagName("Pedalboard2RecorderSettings"))
        {
            editorBounds.setX(xmlState->getIntAttribute("editorX"));
            editorBounds.setY(xmlState->getIntAttribute("editorY"));
            editorBounds.setWidth(xmlState->getIntAttribute("editorW"));
            editorBounds.setHeight(xmlState->getIntAttribute("editorH"));

			setFile(xmlState->getStringAttribute("file"));
			syncToMainTransport = xmlState->getBoolAttribute("syncToMainTransport");
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MetronomeProcessor::MetronomeProcessor():
playing(false),
syncToMainTransport(false),
numerator(4),
denominator(4),
sineX0(1.0f),
sineX1(0.0f),
sineEnv(0.0f),
clickCount(0.0f),
clickDec(0.0f),
measureCount(0),
isAccent(true)
{
	setPlayConfigDetails(0, 1, 0, 0);

	MainTransport::getInstance()->registerTransport(this);
}

//------------------------------------------------------------------------------
MetronomeProcessor::~MetronomeProcessor()
{
	removeAllChangeListeners();
	MainTransport::getInstance()->unregisterTransport(this);
	transportSource[0].setSource(0);
	transportSource[1].setSource(0);
}

//------------------------------------------------------------------------------
void MetronomeProcessor::setAccentFile(const File& phil)
{
	int readAheadSize;

	files[0] = phil;

	//Unload the previous file source and delete it.
    transportSource[0].stop();
    transportSource[0].setSource(0);
    soundFileSource[0] = 0;

	AudioFormatReader *reader = AudioFormatManagerSingleton::getInstance().createReaderFor(phil);

    if(reader != 0)
    {
        soundFileSource[0] = new AudioFormatReaderSource(reader, true);
		soundFileSource[0]->setLooping(false);

		if(soundFileSource[0]->getTotalLength() < 32768)
			readAheadSize = 0;
		else
			readAheadSize = 32768;

        //Plug it into our transport source.
        transportSource[0].setSource(soundFileSource[0],
									 readAheadSize, //Tells it to buffer this many samples ahead.
									 &(AudioThumbnailCacheSingleton::getInstance().getTimeSliceThread()));
    }
	else
		files[0] = File::nonexistent;
}

//------------------------------------------------------------------------------
void MetronomeProcessor::setClickFile(const File& phil)
{
	int readAheadSize;

	files[1] = phil;

	//Unload the previous file source and delete it.
    transportSource[1].stop();
    transportSource[1].setSource(0);
    soundFileSource[1] = 0;

	AudioFormatReader *reader = AudioFormatManagerSingleton::getInstance().createReaderFor(phil);

    if(reader != 0)
    {
        soundFileSource[1] = new AudioFormatReaderSource(reader, true);
		soundFileSource[1]->setLooping(false);

		if(soundFileSource[1]->getTotalLength() < 32768)
			readAheadSize = 0;
		else
			readAheadSize = 32768;

        //Plug it into our transport source.
        transportSource[1].setSource(soundFileSource[1],
									 readAheadSize, //Tells it to buffer this many samples ahead.
									 &(AudioThumbnailCacheSingleton::getInstance().getTimeSliceThread()));
    }
	else
		files[1] = File::nonexistent;
}

//------------------------------------------------------------------------------
Component *MetronomeProcessor::getControls()
{
	MetronomeControl *retval = new MetronomeControl(this, false);

	return retval;
}

//------------------------------------------------------------------------------
void MetronomeProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
	editorBounds = bounds;
}

//------------------------------------------------------------------------------
void MetronomeProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
	if(source == MainTransport::getInstance())
	{
		if(syncToMainTransport)
		{
			//Play/pause the transport source.
			if(MainTransport::getInstance()->getState())
			{
				if(!playing)
				{
					clickCount = 0.0f;
					measureCount = 0;

					playing = true;
				}
			}
			else
			{
				if(playing)
					playing = false;
			}
			sendChangeMessage();
		}
	}
}

//------------------------------------------------------------------------------
void MetronomeProcessor::fillInPluginDescription(PluginDescription &description) const
{
	description.name = L"Metronome";
	description.descriptiveName = L"Simple metronome.";
	description.pluginFormatName = L"Internal";
	description.category = L"Pedalboard Processors";
	description.manufacturerName = L"Niall Moody";
	description.version = L"1.00";
	description.uid = description.name.hashCode();
	description.isInstrument = false;
	description.numInputChannels = 0;
	description.numOutputChannels = 1;
}

//------------------------------------------------------------------------------
void MetronomeProcessor::processBlock(AudioSampleBuffer &buffer,
									  MidiBuffer &midiMessages)
{
	int i;
	float *data;
	bool skipTransport = false;
	AudioSourceChannelInfo bufferInfo;
	const int numSamples = buffer.getNumSamples();

	jassert(buffer.getNumChannels() > 0);

	data = buffer.getSampleData(0);

	//Clear the buffer first.
	for(i=0;i<numSamples;++i)
		data[i] = 0.0f;

	for(i=0;i<numSamples;++i)
	{
		if(playing)
		{
			clickCount -= clickDec;
			if(clickCount <= 0.0f)
			{
				sineX0 = 1.0f;
				sineX1 = 0.0f;

				//The accent.
				if(!measureCount)
				{
					sineCoeff = 2.0f*sinf(3.1415926535897932384626433832795f*880.0f*2.0f/44100.0f);
					measureCount = numerator;
					isAccent = true;

					if(files[0] != File::nonexistent)
					{
						transportSource[0].setPosition(0.0);
						transportSource[0].start();

						//Fill out the transportSource's buffer.
						bufferInfo.buffer = &buffer;
						bufferInfo.startSample = i;
						bufferInfo.numSamples = numSamples-i;

						transportSource[0].getNextAudioBlock(bufferInfo);
						skipTransport = true;
					}
					else
						sineEnv = 1.0f;
				}
				else
				{
					sineCoeff = 2.0f*sinf(3.1415926535897932384626433832795f*440.0f*2.0f/44100.0f);
					isAccent = false;

					if(files[1] != File::nonexistent)
					{
						transportSource[1].setPosition(0.0);
						transportSource[1].start();

						//Fill out the transportSource's buffer.
						bufferInfo.buffer = &buffer;
						bufferInfo.startSample = i;
						bufferInfo.numSamples = numSamples-i;

						transportSource[1].getNextAudioBlock(bufferInfo);
						skipTransport = true;
					}
					else
						sineEnv = 1.0f;
				}

				--measureCount;

				//Update clickCount.
				{
					AudioPlayHead *playHead = getPlayHead();
					AudioPlayHead::CurrentPositionInfo posInfo;

					if(playHead)
						getPlayHead()->getCurrentPosition(posInfo);
					else
						posInfo.bpm = 120.0f;

					clickDec = (float)(1.0f/getSampleRate());
					clickCount += (float)(60.0/posInfo.bpm) * (4.0f/denominator);
				}
			}

			//if(isAccent && (files[0] != File::nonexistent))
			if(isAccent && (transportSource[0].isPlaying()))
			{
				
			}
			//else if(!isAccent && (files[1] !=File::nonexistent))
			else if(!isAccent && (transportSource[1].isPlaying()))
			{
				
			}
			else if(sineEnv > 0.0f)
			{
				sineX0 -= sineCoeff * sineX1;
				sineX1 += sineCoeff * sineX0;

				data[i] = sineX1 * sineEnv;

				sineEnv -= 0.0001f;
				if(sineEnv < 0.0f)
					sineEnv = 0.0f;
			}
		}
	}

	if(!skipTransport)
	{
		//Fill out the transportSource's buffer.
		bufferInfo.buffer = &buffer;
		bufferInfo.startSample = 0;
		bufferInfo.numSamples = numSamples;

		if(transportSource[0].isPlaying())
			transportSource[0].getNextAudioBlock(bufferInfo);
		if(transportSource[1].isPlaying())
			transportSource[1].getNextAudioBlock(bufferInfo);
	}
}

//------------------------------------------------------------------------------
AudioProcessorEditor *MetronomeProcessor::createEditor()
{
	return new MetronomeEditor(this, editorBounds);
}

//------------------------------------------------------------------------------
const String MetronomeProcessor::getParameterName(int parameterIndex)
{
	String retval;

	switch(parameterIndex)
	{
		case Play:
			retval = L"Play";
			break;
		case SyncToMainTransport:
			retval = L"Sync to Main Transport";
			break;
	}

	return retval;
}

//------------------------------------------------------------------------------
float MetronomeProcessor::getParameter(int parameterIndex)
{
	float retval = 0.0f;

	switch(parameterIndex)
	{
		case Numerator:
			retval = (float)numerator;
			break;
		case Denominator:
			retval = (float)denominator;
			break;
		case SyncToMainTransport:
			retval = syncToMainTransport ? 1.0f : 0.0f;
			break;
	}

	return retval;
}

//------------------------------------------------------------------------------
const String MetronomeProcessor::getParameterText(int parameterIndex)
{
	String retval;

	switch(parameterIndex)
	{
		case SyncToMainTransport:
			if(syncToMainTransport)
				retval = L"synced";
			else
				retval = L"not synced";
			break;
	}

	return retval;
}

//------------------------------------------------------------------------------
void MetronomeProcessor::setParameter(int parameterIndex, float newValue)
{
	switch(parameterIndex)
	{
		case Play:
			if(newValue > 0.5f)
			{
				if(!playing)
				{
					clickCount = 0.0f;
					measureCount = 0;

					playing = true;
				}
				else if(playing)
					playing = false;
				sendChangeMessage();
			}
			break;
		case Numerator:
			numerator = (int)newValue;
			break;
		case Denominator:
			denominator = (int)newValue;
			break;
		case SyncToMainTransport:
			if(newValue > 0.5f)
				syncToMainTransport = true;
			else
				syncToMainTransport = false;
			sendChangeMessage();
			break;
	}
}

//------------------------------------------------------------------------------
void MetronomeProcessor::getStateInformation(MemoryBlock &destData)
{
    XmlElement xml("Pedalboard2MetronomeSettings");

    xml.setAttribute("editorX", editorBounds.getX());
    xml.setAttribute("editorY", editorBounds.getY());
    xml.setAttribute("editorW", editorBounds.getWidth());
    xml.setAttribute("editorH", editorBounds.getHeight());
	
	xml.setAttribute("syncToMainTransport", syncToMainTransport);
	xml.setAttribute("numerator", numerator);
	xml.setAttribute("denominator", denominator);
	xml.setAttribute("accentFile", files[0].getFullPathName());
	xml.setAttribute("clickFile", files[1].getFullPathName());

    copyXmlToBinary(xml, destData);
}

//------------------------------------------------------------------------------
void MetronomeProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != 0)
    {
        if (xmlState->hasTagName("Pedalboard2MetronomeSettings"))
        {
            editorBounds.setX(xmlState->getIntAttribute("editorX"));
            editorBounds.setY(xmlState->getIntAttribute("editorY"));
            editorBounds.setWidth(xmlState->getIntAttribute("editorW"));
            editorBounds.setHeight(xmlState->getIntAttribute("editorH"));

			syncToMainTransport = xmlState->getBoolAttribute("syncToMainTransport");
			numerator = xmlState->getIntAttribute("numerator");
			denominator = xmlState->getIntAttribute("denominator");
			setAccentFile(File(xmlState->getStringAttribute("accentFile")));
			setClickFile(File(xmlState->getStringAttribute("clickFile")));
        }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LooperProcessor::LooperProcessor():
playing(false),
stopPlaying(false),
recording(false),
stopRecording(false),
syncToMainTransport(false),
stopAfterBar(false),
autoPlay(true),
inputLevel(1.0f),
loopLevel(1.0f),
threadWriter(0),
thumbnail(512,
		  AudioFormatManagerSingleton::getInstance(),
		  AudioThumbnailCacheSingleton::getInstance()),
numerator(4),
denominator(4),
clickCount(0.0f),
clickDec(0.0f),
measureCount(0),
currentRate(44100.0),
justPaused(false),
loopLength(0),
loopPos(0),
loopIndex(0),
deleteLastBuffer(false),
tempBufferWrite(0),
fadeOutCount(-1),
fadeInCount(0),
autoPlayFade(1.0f),
fileReader(0),
fileReaderPos(0),
fileReaderBufIndex(0),
newFileLoaded(false),
inputAudio(2, 2560)
{
	int i;

	loopBuffer.add(new AudioSampleBuffer(2, (44100 * 8)));
	loopBuffer[0]->clear();

	for(i=0;i<FadeBufferSize;++i)
	{
		tempBuffer[0][i] = 0.0f;
		tempBuffer[1][i] = 0.0f;

		fadeInBuffer[0][i] = 0.0f;
		fadeInBuffer[1][i] = 0.0f;

		fadeOutBuffer[0][i] = 0.0f;
		fadeOutBuffer[1][i] = 0.0f;
	}

	AudioThumbnailCacheSingleton::getInstance().getTimeSliceThread().addTimeSliceClient(this);

	setPlayConfigDetails(2, 2, 0, 0);

	MainTransport::getInstance()->registerTransport(this);
}

//------------------------------------------------------------------------------
LooperProcessor::~LooperProcessor()
{
	//Saves the file.
	setFile(File::nonexistent);

	AudioThumbnailCacheSingleton::getInstance().getTimeSliceThread().removeTimeSliceClient(this);

	removeAllChangeListeners();
	MainTransport::getInstance()->unregisterTransport(this);

	if(threadWriter)
		delete threadWriter;

	if(fileReader)
		delete fileReader;
}

//------------------------------------------------------------------------------
void LooperProcessor::setFile(const File& phil)
{
	int i;
	/*WavAudioFormat wavFormat;
	StringPairArray metadata;
	AudioFormatWriter *writer;
	FileOutputStream *philStream;*/

	if(recording)
	{
		stopRecording = true;

		//Wait till the end of the current audio buffer.
		while(recording) {Thread::sleep(10);}
	}

	if(threadWriter)
	{
		delete threadWriter;
		threadWriter = 0;
	}

	//So we don't delete a buffer while we're playing to it.
	while(playing) {Thread::sleep(10);};

	soundFile = phil;

	if(soundFile != File::nonexistent)
	{
		if(fileReader)
			delete fileReader;
		fileReader = AudioFormatManagerSingleton::getInstance().createReaderFor(soundFile);
		fileReaderPos = 0;
		fileReaderBufIndex = 0;
	}

	loopLength = 0;
	loopIndex = 0;
	loopPos = 0;

	//Remove all loop buffers except the 1st one.
	for(i=(loopBuffer.size()-1);i>0;--i)
		loopBuffer.remove(i);

	//So we overwrite any previous file.
	/*if(soundFile.existsAsFile())
	{
		if(!soundFile.deleteFile())
		{
			AlertWindow::showMessageBox(AlertWindow::WarningIcon,
										"Could not delete existing file",
										"Have you got the file open elsewhere? (e.g. in another File Player)");

			soundFile = File::nonexistent;
		}
	}

	if(soundFile != File::nonexistent)
	{
		philStream = new FileOutputStream(soundFile);

		writer = wavFormat.createWriterFor(philStream,
										   currentRate,
										   2,
										   16,
										   metadata,
										   0);
		if(!writer)
		{
			delete philStream;
			soundFile = File::nonexistent;
			threadWriter = 0;
		}
		else
		{
			threadWriter = new AudioFormatWriter::ThreadedWriter(writer,
																 AudioThumbnailCacheSingleton::getInstance(),
																 16384);
			//threadWriter->setDataReceiver(&thumbnail);
		}
	}*/
}

//------------------------------------------------------------------------------
double LooperProcessor::getReadPosition() const
{
	if(loopLength > 0)
		return (double)(((double)loopIndex * (double)LoopBufferSize) + (double)loopPos)/(double)loopLength;
	else
		return 0.0;
}

//------------------------------------------------------------------------------
void LooperProcessor::handleAsyncUpdate()
{
	int abortCounter = 2000; //== 2 seconds, assuming a perfect 1 millisecond wait from Thread::sleep().
	WavAudioFormat wavFormat;
	StringPairArray metadata;
	AudioFormatWriter *writer;
	FileOutputStream *philStream;

	//Just in case.
	if(playing)
		stopPlaying = true;
	while(playing)
	{
		Thread::sleep(1);
		--abortCounter;
		if(!abortCounter)
		{
			AlertWindow::showMessageBox(AlertWindow::WarningIcon,
										"Looper Error",
										"Unable to start recording; Looper still playing. Aborting.");

			return;
		}
	};

	/*if(control)
		control->clearDisplay();*/
	thumbnail.reset(2, getSampleRate());

	setFile(soundFile);

	if(fileReader)
	{
		delete fileReader;
		fileReader = 0;
	}

	if(soundFile.existsAsFile())
	{
		if(!soundFile.deleteFile())
		{
			AlertWindow::showMessageBox(AlertWindow::WarningIcon,
										"Could not delete existing file",
										"Have you got the file open elsewhere? (e.g. in another File Player)");

			soundFile = File::nonexistent;
		}
	}

	if(soundFile != File::nonexistent)
	{
		philStream = new FileOutputStream(soundFile);

		writer = wavFormat.createWriterFor(philStream,
											currentRate,
											2,
											16,
											metadata,
											0);
		if(!writer)
		{
			delete philStream;
			soundFile = File::nonexistent;
			threadWriter = 0;
		}
		else
		{
			threadWriter = new AudioFormatWriter::ThreadedWriter(writer,
																 AudioThumbnailCacheSingleton::getInstance().getTimeSliceThread(),
																 16384);
			threadWriter->setDataReceiver(&thumbnail);
		}
	}

	if(threadWriter)
	{
		if(playing)
			stopPlaying = true;
		while(playing) {Thread::sleep(10);}

		clickCount = 0.0f;
		clickDec = 0.0f;
		measureCount = 0;
		fadeOutCount = -1;
		if(autoPlay)
			autoPlayFade = 0.0f;

		recording = true;
	}
}

//------------------------------------------------------------------------------
int LooperProcessor::useTimeSlice()
{
	int retval = 250; //Wait 1/4 second before checking again.

	if(recording)
	{
		if(loopIndex == (loopBuffer.size()-1))
		{
			//16538 = ~3/8 of a second. Should be enough considering the wait
			//time we return from this method?
			if(loopPos > (LoopBufferSize-16538))
			{
				loopBuffer.add(new AudioSampleBuffer(2, LoopBufferSize));
				loopBuffer[loopBuffer.size()-1]->clear();
			}
		}
	}
	else if(deleteLastBuffer)
	{
		loopBuffer.remove(loopBuffer.size()-1);
		deleteLastBuffer = false;
	}
	else if(fileReader)
	{
		if(fileReaderBufIndex >= loopBuffer.size())
		{
			loopBuffer.add(new AudioSampleBuffer(2, LoopBufferSize));
			loopBuffer[loopBuffer.size()-1]->clear();

			jassert(fileReaderBufIndex < loopBuffer.size());
		}
		fileReader->read(loopBuffer[fileReaderBufIndex],
						 0,
						 LoopBufferSize,
						 fileReaderPos, true, true);
		++fileReaderBufIndex;
		fileReaderPos += LoopBufferSize;

		if(fileReaderPos >= fileReader->lengthInSamples)
		{
			loopLength = fileReader->lengthInSamples;
			delete fileReader;
			fileReader = 0;
		}

		//While we're reading the file, have a shorter delay between calls
		//so it gets done faster.
		retval = 20;
	}

	return retval;
}

//------------------------------------------------------------------------------
Component *LooperProcessor::getControls()
{
	return new LooperControl(this, &thumbnail);
}

//------------------------------------------------------------------------------
void LooperProcessor::updateEditorBounds(const Rectangle<int>& bounds)
{
	editorBounds = bounds;
}

//------------------------------------------------------------------------------
void LooperProcessor::changeListenerCallback(ChangeBroadcaster *source)
{
	if(source == MainTransport::getInstance())
	{
		if(syncToMainTransport)
		{
			//Play/pause the transport source.
			if(MainTransport::getInstance()->getState() && !playing)
				setParameter(Play, 1.0f);
			else if(playing && !stopPlaying)
				setParameter(Play, 1.0f);

			if(MainTransport::getInstance()->getReturnToZero())
				setParameter(ReturnToZero, 1.0f);

			sendChangeMessage();
		}
	}
}

//------------------------------------------------------------------------------
void LooperProcessor::fillInPluginDescription(PluginDescription &description) const
{
	description.name = L"Looper";
	description.descriptiveName = L"Simple looper processor.";
	description.pluginFormatName = L"Internal";
	description.category = L"Pedalboard Processors";
	description.manufacturerName = L"Niall Moody";
	description.version = L"1.00";
	description.uid = description.name.hashCode();
	description.isInstrument = false;
	description.numInputChannels = 2;
	description.numOutputChannels = 2;
}

//------------------------------------------------------------------------------
void LooperProcessor::processBlock(AudioSampleBuffer &buffer,
								   MidiBuffer &midiMessages)
{
	int i, j;
	float tempf;
	float *data[2];
	uint64_t end, end2;
	float *inputData[2];
	int fadeOutStart = 0;
	int samplesToRecord = buffer.getNumSamples();
	const int numSamples = buffer.getNumSamples();

	jassert(buffer.getNumChannels() > 1);

	data[0] = buffer.getSampleData(0);
	data[1] = buffer.getSampleData(1);

	inputAudio.copyFrom(0, 0, buffer, 0, 0, numSamples);
	inputAudio.copyFrom(1, 0, buffer, 1, 0, numSamples);
	inputData[0] = inputAudio.getSampleData(0);
	inputData[1] = inputAudio.getSampleData(1);

	if(recording && threadWriter)
	{
		//If we're stopping after a bar, need to keep track of where we are in
		//the bar.
		if(stopAfterBar)
		{
			for(i=0;i<samplesToRecord;++i)
			{
				clickCount -= clickDec;
				if(clickCount <= 0.0f)
				{
					AudioPlayHead *playHead = getPlayHead();
					AudioPlayHead::CurrentPositionInfo posInfo;

					++measureCount;
					if(measureCount == (numerator+1))
					{
						samplesToRecord = i;
						stopRecording = true;
						fadeInCount = (loopLength-1) + samplesToRecord;

						break;
					}

					if(playHead)
						getPlayHead()->getCurrentPosition(posInfo);
					else
						posInfo.bpm = 120.0f;

					clickDec = (float)(1.0f/getSampleRate());
					clickCount += (float)(60.0/posInfo.bpm) * (4.0f/denominator);
				}
			}
		}

		///Write the audio data to the file.
		threadWriter->write((const float **)data, samplesToRecord);

		//Copy fade in buffer if necessary.
		if((loopPos == 0) && (loopIndex == 0))
			fillFadeInBuffer();

		if(loopIndex < loopBuffer.size())
		{
			if((loopPos+samplesToRecord) < LoopBufferSize)
				i = samplesToRecord;
			else
				i = LoopBufferSize-loopPos;

			//Write the audio data to the current loop buffer.
			loopBuffer[loopIndex]->copyFrom(0, loopPos, buffer, 0, 0, i);
			loopBuffer[loopIndex]->copyFrom(1, loopPos, buffer, 1, 0, i);

			tempf = loopBuffer[loopIndex]->getSampleData(0, 0)[0];
			tempf = fadeInBuffer[0][127];
			if(i < samplesToRecord)
			{
				++loopIndex;
				loopPos = 0;
				if(loopIndex >= loopBuffer.size())
				{
					AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
													 "Looper Error",
													 "Not enough memory to continue. Recording stopped.");
					stopRecording = true;
					loopLength += i;
					fadeOutStart = i;
				}
				else
				{
					loopBuffer[loopIndex]->copyFrom(0, 0, buffer, 0, i, (samplesToRecord-i));
					loopBuffer[loopIndex]->copyFrom(1, 0, buffer, 1, i, (samplesToRecord-i));

					loopPos = samplesToRecord-i;
					loopLength += samplesToRecord;
				}
			}
			else
			{
				loopPos += samplesToRecord;
				loopLength += samplesToRecord;
			}
		}
		else
		{
			AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
											 "Looper Error",
											 "Not enough memory to continue. Recording stopped.");
			stopRecording = true;
		}

		if(stopRecording)
		{
			recording = false;
			stopRecording = false;
			if(autoPlay)
				playing = true;
			sendChangeMessage();

			if(loopIndex < (loopBuffer.size()-1))
				deleteLastBuffer = true;

			loopPos = 0;
			loopIndex = 0;
			fadeInCount = (loopLength-1);

			//Starts the count of [NumBufferSize] samples so we can store the
			//fade out buffer.
			fadeOutStart = samplesToRecord;
			fadeOutCount = 0;
		}
	}
	else if(recording)
	{
		recording = false;
		stopRecording = false;
		if(autoPlay)
			playing = true;
		sendChangeMessage();

		loopPos = 0;
		loopIndex = 0;
		//fadeInCount = loopLength;
	}

	//Apply input level gain change.
	buffer.applyGain(0, numSamples, inputLevel);

	if(playing && (loopLength > 0))
	{
		if(loopBuffer.size() == 1)
			end = (uint64_t)loopLength;
		else if(loopIndex < (loopBuffer.size()-1))
			end = (uint64_t)LoopBufferSize;
		else
			end = loopLength - (uint64_t)(LoopBufferSize * (loopBuffer.size()-1));

		if((loopPos+numSamples) < end)
			i = numSamples;
		else
			i = (int)(end-(uint64_t)loopPos);

		//Output the fade out buffer.
		if((loopPos < FadeBufferSize) && (fadeOutCount == -1))
		{
			end2 = FadeBufferSize - loopPos;
			if(end2 > numSamples)
				end2 = numSamples;

			for(j=0;j<end2;++j)
			{
				tempf = 1.0f - ((float)(loopPos+j)/(float)FadeBufferSize);
				data[0][j] += fadeOutBuffer[0][loopPos+j] * tempf * loopLevel;
				data[1][j] += fadeOutBuffer[1][loopPos+j] * tempf * loopLevel;
			}
		}

		//Output the fade in buffer.
		for(j=0;j<numSamples;++j,--fadeInCount)
		{
			if(fadeInCount <= (FadeBufferSize-1))
			{
				tempf = 1.0f - ((float)fadeInCount/(float)(FadeBufferSize-1));
				data[0][j] += fadeInBuffer[0][FadeBufferSize-1-fadeInCount] * tempf * loopLevel;
				data[1][j] += fadeInBuffer[1][FadeBufferSize-1-fadeInCount] * tempf * loopLevel;

				if((FadeBufferSize-1-fadeInCount) == 127)
					tempf = fadeInBuffer[0][127];
			}

			if(fadeInCount == 0)
				fadeInCount = loopLength;
		}

		tempf = loopBuffer[loopIndex]->getSampleData(0, 0)[0];
		if(autoPlayFade < 1.0f)
		{
			buffer.addFromWithRamp(0, 0, loopBuffer[loopIndex]->getSampleData(0, loopPos), i, 0.0f, loopLevel);
			buffer.addFromWithRamp(1, 0, loopBuffer[loopIndex]->getSampleData(1, loopPos), i, 0.0f, loopLevel);
			autoPlayFade = 1.0f;
		}
		else
		{
			buffer.addFrom(0, 0, *(loopBuffer[loopIndex]), 0, loopPos, i, loopLevel);
			buffer.addFrom(1, 0, *(loopBuffer[loopIndex]), 1, loopPos, i, loopLevel);
		}
		if(i < numSamples)
		{
			++loopIndex;
			if(loopIndex >= loopBuffer.size())
				loopIndex = 0;
			loopPos = 0;

			if(fadeOutCount == -1)
			{
				end = FadeBufferSize - loopPos;
				if(end > (numSamples-i))
					end = (numSamples-i);

				for(j=0;j<end;++j)
				{
					tempf = 1.0f - ((float)(loopPos+j)/(float)FadeBufferSize);
					data[0][i+j] += fadeOutBuffer[0][loopPos+j] * tempf * loopLevel;
					data[1][i+j] += fadeOutBuffer[1][loopPos+j] * tempf * loopLevel;
				}
			}

			buffer.addFrom(0, i, *(loopBuffer[loopIndex]), 0, loopPos, (numSamples-i), loopLevel);
			buffer.addFrom(1, i, *(loopBuffer[loopIndex]), 1, loopPos, (numSamples-i), loopLevel);
			tempf = buffer.getSampleData(0, 0)[0];

			loopPos = numSamples-i;
		}
		else
			loopPos += i;

		if(stopPlaying)
		{
			playing = false;
			stopPlaying = false;
		}
	}
	else if(stopPlaying)
	{
		playing = false;
		stopPlaying = false;
	}

	for(i=0;i<numSamples;++i)
	{
		if(fadeOutCount >= FadeBufferSize)
		{
			fadeOutCount = -1;
			fillFadeOutBuffer();
		}

		tempBuffer[0][tempBufferWrite] = inputData[0][i];
		tempBuffer[1][tempBufferWrite] = inputData[1][i];

		++tempBufferWrite;
		tempBufferWrite %= FadeBufferSize;

		if((i >= fadeOutStart) && (fadeOutCount > -1))
			++fadeOutCount;
	}
}

//------------------------------------------------------------------------------
void LooperProcessor::prepareToPlay(double sampleRate,
									int estimatedSamplesPerBlock)
{
	currentRate = sampleRate;

	inputAudio.setSize(2, estimatedSamplesPerBlock);
}

//------------------------------------------------------------------------------
AudioProcessorEditor *LooperProcessor::createEditor()
{
	return new LooperEditor(this, &thumbnail);
}

//------------------------------------------------------------------------------
const String LooperProcessor::getParameterName(int parameterIndex)
{
	String retval;

	switch(parameterIndex)
	{
		case Play:
			retval = "Play/Pause";
			break;
		case ReturnToZero:
			retval = "Return to Zero";
			break;
		case Record:
			retval = "Record";
			break;
		case ReadPosition:
			retval = "Read Position";
			break;
		case SyncToMainTransport:
			retval = "Sync to Main Transport";
			break;
		case StopAfterBar:
			retval = "Stop After Bar";
			break;
		case AutoPlay:
			retval = "Auto-play";
			break;
		case BarNumerator:
			retval = "Bar Numerator";
			break;
		case BarDenominator:
			retval = "Bar Denominator";
			break;
		case InputLevel:
			retval = "Input Level";
			break;
		case LoopLevel:
			retval = "Loop Level";
			break;
	}

	return retval;
}

//------------------------------------------------------------------------------
float LooperProcessor::getParameter(int parameterIndex)
{
	float retval = 0.0f;

	switch(parameterIndex)
	{
		case SyncToMainTransport:
			retval = syncToMainTransport ? 1.0f : 0.0f;
			break;
		case StopAfterBar:
			retval = stopAfterBar ? 1.0f : 0.0f;
			break;
		case AutoPlay:
			retval = autoPlay ? 1.0f : 0.0f;
			break;
		case BarNumerator:
			retval = (float)numerator;
			break;
		case BarDenominator:
			retval = (float)denominator;
			break;
		case InputLevel:
			retval = inputLevel * 0.5f;
			break;
		case LoopLevel:
			retval = loopLevel * 0.5f;
			break;
	}

	return retval;
}

//------------------------------------------------------------------------------
const String LooperProcessor::getParameterText(int parameterIndex)
{
	String retval;

	return retval;
}

//------------------------------------------------------------------------------
void LooperProcessor::setParameter(int parameterIndex, float newValue)
{
	switch(parameterIndex)
	{
		case Play:
			if(newValue > 0.5f)
			{
				if(!playing)
					playing = true;
				else
					stopPlaying = true;
			}
			sendChangeMessage();
			break;
		case ReturnToZero:
			loopIndex = 0;
			loopPos = 0;
			fadeInCount = loopLength-1;
			break;
		case Record:
			if(newValue > 0.5f)
			{
				if(playing)
					stopPlaying = true;

				if(!recording && !stopRecording)// && threadWriter)
				{
					triggerAsyncUpdate();
				}
				else if(recording)
				{
					stopRecording = true;

					//Saves the file to disk.
					//setFile(File::nonexistent);

					//Wait till the end of the current audio buffer.
					//while(recording) {Thread::sleep(10);}

					if(syncToMainTransport)
						MainTransport::getInstance()->transportFinished();
				}
				sendChangeMessage();
			}
			break;
		case ReadPosition:
			{
				int tempindex = 0;
				uint64_t tempint = 0;
				uint64_t pos = (uint64_t)(newValue * (float)loopLength);

				fadeInCount = loopLength-1-pos;

				//This is a stupid way of doing this, but my brain's not working
				//right now...
				while(tempint < pos)
				{
					++tempindex;
					tempint += LoopBufferSize;
				}
				loopIndex = tempindex-1;
				loopPos = pos % LoopBufferSize;

				sendChangeMessage();
			}
			break;
		case SyncToMainTransport:
			syncToMainTransport = (newValue > 0.5f) ? true : false;
			sendChangeMessage();
			break;
		case StopAfterBar:
			stopAfterBar = (newValue > 0.5f) ? true : false;
			break;
		case AutoPlay:
			autoPlay = (newValue > 0.5f) ? true : false;
			break;
		case BarNumerator:
			numerator = (int)newValue;
			break;
		case BarDenominator:
			denominator = (int)newValue;
			break;
		case InputLevel:
			inputLevel = newValue * 2.0f;
			break;
		case LoopLevel:
			loopLevel = newValue * 2.0f;
			break;
	}
}

//------------------------------------------------------------------------------
void LooperProcessor::getStateInformation(MemoryBlock &destData)
{
    XmlElement xml("Pedalboard2LooperSettings");

	xml.setAttribute("soundFile", soundFile.getFullPathName());
	xml.setAttribute("syncToMainTransport", syncToMainTransport);
	xml.setAttribute("stopAfterBar", stopAfterBar);
	xml.setAttribute("autoPlay", autoPlay);
	xml.setAttribute("barNumerator", numerator);
	xml.setAttribute("barDenominator", denominator);
	xml.setAttribute("inputLevel", inputLevel);
	xml.setAttribute("loopLevel", loopLevel);

    xml.setAttribute("editorX", editorBounds.getX());
    xml.setAttribute("editorY", editorBounds.getY());
    xml.setAttribute("editorW", editorBounds.getWidth());
    xml.setAttribute("editorH", editorBounds.getHeight());

    copyXmlToBinary(xml, destData);
}

//------------------------------------------------------------------------------
void LooperProcessor::setStateInformation(const void *data, int sizeInBytes)
{
	ScopedPointer<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != 0)
    {
        if (xmlState->hasTagName("Pedalboard2LooperSettings"))
        {
            editorBounds.setX(xmlState->getIntAttribute("editorX"));
            editorBounds.setY(xmlState->getIntAttribute("editorY"));
            editorBounds.setWidth(xmlState->getIntAttribute("editorW"));
            editorBounds.setHeight(xmlState->getIntAttribute("editorH"));

			//Load the file, indicate to change listeners a new file's been
			//loaded.
			setFile(File(xmlState->getStringAttribute("soundFile")));
			newFileLoaded = true;
			sendChangeMessage();

			syncToMainTransport = xmlState->getBoolAttribute("syncToMainTransport", false);
			stopAfterBar = xmlState->getBoolAttribute("stopAfterBar", false);
			autoPlay = xmlState->getBoolAttribute("autoPlay", true);
			numerator = xmlState->getIntAttribute("barNumerator", 4);
			denominator = xmlState->getIntAttribute("barDenominator", 4);
			inputLevel = (float)xmlState->getDoubleAttribute("inputLevel", 0.5);
			loopLevel = (float)xmlState->getDoubleAttribute("loopLevel", 0.5);
        }
    }
}

//------------------------------------------------------------------------------
void LooperProcessor::fillFadeInBuffer()
{
	int i, j;

	for(i=0,j=tempBufferWrite;i<FadeBufferSize;++i,++j)
	{
		j %= FadeBufferSize;

		fadeInBuffer[0][i] = tempBuffer[0][j];
		fadeInBuffer[1][i] = tempBuffer[1][j];
	}
}

//------------------------------------------------------------------------------
void LooperProcessor::fillFadeOutBuffer()
{
	int i, j;

	for(i=0,j=tempBufferWrite;i<FadeBufferSize;++i,++j)
	{
		j %= FadeBufferSize;

		fadeOutBuffer[0][i] = tempBuffer[0][j];
		fadeOutBuffer[1][i] = tempBuffer[1][j];
	}
}
