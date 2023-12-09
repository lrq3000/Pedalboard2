//	BypassableInstance.cpp - Wrapper class to provide a bypass to
//							 AudioPluginInstance.
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

#include "BypassableInstance.h"

//------------------------------------------------------------------------------
BypassableInstance::BypassableInstance(AudioPluginInstance *plug):
plugin(plug),
tempBuffer(2, 4096),
bypass(false),
bypassRamp(0.0f),
midiChannel(0)
{
	jassert(plugin);

	setPlayConfigDetails(plugin->getNumInputChannels(),
						 plugin->getNumOutputChannels(),
						 plugin->getSampleRate(),
						 plugin->getBlockSize());
}

//------------------------------------------------------------------------------
BypassableInstance::~BypassableInstance()
{
	delete plugin;
}

//------------------------------------------------------------------------------
void BypassableInstance::prepareToPlay(double sampleRate,
									   int estimatedSamplesPerBlock)
{
	int numChannels;

	if(plugin->getNumInputChannels() > plugin->getNumOutputChannels())
		numChannels = plugin->getNumInputChannels();
	else
		numChannels = plugin->getNumOutputChannels();

	jassert(numChannels > 0);

	midiCollector.reset(sampleRate);

	//Since we only get an estimate of the number of samples per block, multiply
	//that number by 2 to ensure we don't run out of space.
	tempBuffer.setSize(numChannels, (estimatedSamplesPerBlock * 2));

	plugin->setPlayHead(getPlayHead());
	plugin->setPlayConfigDetails(plugin->getNumInputChannels(),
								 plugin->getNumOutputChannels(),
								 sampleRate,
								 estimatedSamplesPerBlock);
	plugin->prepareToPlay(sampleRate, estimatedSamplesPerBlock);
}

//------------------------------------------------------------------------------
void BypassableInstance::releaseResources()
{
	plugin->releaseResources();
}

//------------------------------------------------------------------------------
void BypassableInstance::processBlock(AudioSampleBuffer &buffer,
									  MidiBuffer &midiMessages)
{
	int i, j;
	float rampVal = bypassRamp;
	MidiBuffer tempMidi;
	MidiBuffer::Iterator it(midiMessages);

	jassert(buffer.getNumChannels() <= tempBuffer.getNumChannels());

	//Pass on any MIDI messages received via OSC.
	midiCollector.removeNextBlockOfMessages(tempMidi, buffer.getNumSamples());
	if(!midiMessages.isEmpty())
	{
		MidiMessage tempMess;
		int tempSample;

		while(it.getNextEvent(tempMess, tempSample))
		{
			//Filter out any messages on the wrong channel.
			if((midiChannel == 0) || (tempMess.getChannel() == midiChannel))
				tempMidi.addEvent(tempMess, tempSample);
		}
	}


	tempBuffer.setSize(buffer.getNumChannels(),
					   buffer.getNumSamples(),
					   false,
					   false,
					   true);

	//Fill out our temporary buffer correctly.
	for(i=0;i<buffer.getNumChannels();++i)
		tempBuffer.copyFrom(i, 0, buffer, i, 0, buffer.getNumSamples());

	//Get the plugin's audio.
	plugin->processBlock(buffer, tempMidi);

	//Add any new midi data to midiMessages.
	if(!tempMidi.isEmpty())
		midiMessages.swapWith(tempMidi);

	//Add the correct (bypassed or un-bypassed) audio back to the buffer.
	for(j=0;j<buffer.getNumChannels();++j)
	{
		float *origData = tempBuffer.getSampleData(j);
		float *newData = buffer.getSampleData(j);

		rampVal = bypassRamp;
		for(i=0;i<buffer.getNumSamples();++i)
		{
			newData[i] = (origData[i] * rampVal) + (newData[i] * (1.0f-rampVal));

			if(bypass && (rampVal < 1.0f))
			{
				rampVal += 0.001f;
				if(rampVal > 1.0f)
					rampVal = 1.0f;
			}
			else if(!bypass && (rampVal > 0.0f))
			{
				rampVal -= 0.001f;
				if(rampVal < 0.0f)
					rampVal = 0.0f;
			}
		}
	}
	bypassRamp = rampVal;
}

//------------------------------------------------------------------------------
void BypassableInstance::setBypass(bool val)
{
	bypass = val;
}

//------------------------------------------------------------------------------
void BypassableInstance::setMIDIChannel(int val)
{
	midiChannel = val;
}

//------------------------------------------------------------------------------
void BypassableInstance::addMidiMessage(const MidiMessage& message)
{
	if((midiChannel == 0) || (message.getChannel() == midiChannel))
		midiCollector.addMessageToQueue(message);
}
