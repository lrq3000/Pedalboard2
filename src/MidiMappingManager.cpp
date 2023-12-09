//	MidiMappingManager.cpp - Class which dispatches MIDI CC messages to
//						     MidiMappings.
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

#include "MidiMappingManager.h"
#include "PropertiesSingleton.h"
#include "MainPanel.h"
#include "LogFile.h"

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MidiMapping::MidiMapping(MidiMappingManager *manager,
						 FilterGraph *graph,
						 uint32 pluginId,
						 int param,
						 int midiCc,
						 bool latch,
						 int chan,
						 float lower,
						 float upper):
Mapping(graph, pluginId, param),
mappingManager(manager),
cc(midiCc),
latched(latch),
channel(chan),
lowerBound(lower),
upperBound(upper),
latchVal(0.0f),
latchToggle(false)
{
	//mappingManager->registerMapping(midiCc, this);
}

//------------------------------------------------------------------------------
MidiMapping::MidiMapping(MidiMappingManager *manager,
						 FilterGraph *graph,
						 XmlElement *e):
Mapping(graph, e),
mappingManager(manager),
channel(0),
latchVal(0.0f),
latchHi(1.0f),
latchLo(0.0f),
latchToggle(false)
{
	if(e)
	{
		cc = e->getIntAttribute("cc");
		latched = e->getBoolAttribute("latch");
		channel = e->getIntAttribute("channel");
		lowerBound = (float)e->getDoubleAttribute("lowerBound");
		upperBound = (float)e->getDoubleAttribute("upperBound");
	}
	//mappingManager->registerMapping(cc, this);
}

//------------------------------------------------------------------------------
MidiMapping::~MidiMapping()
{
	mappingManager->unregisterMapping(this);
}

//------------------------------------------------------------------------------
void MidiMapping::ccReceived(int val)
{
	float tempf;

	if(latched)
	{
		if(val == 0)
			return;

		latchToggle = !latchToggle;

		/*if(latchToggle)
		{
			latchHi = (float)val/127.0f;

			if(latchVal > latchLo)
				latchVal = latchLo;
			else
				latchVal = latchHi;
		}
		else
			latchLo = (float)val/127.0f;

		tempf = latchVal;*/
		if(latchToggle)
			tempf = 1.0f;
		else
			tempf = 0.0f;
	}
	else
		tempf = (float)val/127.0f;

	if(upperBound > lowerBound)
	{
		tempf *= upperBound-lowerBound;
		tempf += lowerBound;
	}
	else
	{
		tempf = 1.0f - tempf;
		tempf *= lowerBound-upperBound;
		tempf += upperBound;
	}

	updateParameter(tempf);
}

//------------------------------------------------------------------------------
XmlElement *MidiMapping::getXml() const
{
	XmlElement *retval = new XmlElement(L"MidiMapping");

	retval->setAttribute("pluginId", (int)getPluginId());
	retval->setAttribute("parameter", getParameter());
	retval->setAttribute("cc", cc);
	retval->setAttribute("latch", latched);
	retval->setAttribute("channel", channel);
	retval->setAttribute("lowerBound", lowerBound);
	retval->setAttribute("upperBound", upperBound);

	return retval;
}

//------------------------------------------------------------------------------
void MidiMapping::setCc(int val)
{
	cc = val;
	mappingManager->unregisterMapping(this);
	mappingManager->registerMapping(cc, this);
}

//------------------------------------------------------------------------------
void MidiMapping::setLatched(bool val)
{
	latched = val;
}

//------------------------------------------------------------------------------
void MidiMapping::setChannel(int val)
{
	channel = val;
}

//------------------------------------------------------------------------------
void MidiMapping::setLowerBound(float val)
{
	lowerBound = val;
}

//------------------------------------------------------------------------------
void MidiMapping::setUpperBound(float val)
{
	upperBound = val;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MidiAppMapping::MidiAppMapping(MidiMappingManager *manager,
							   int midiCc,
							   CommandID commandId):
midiManager(manager),
cc(midiCc),
id(commandId)
{
	//midiManager->registerAppMapping(this);
}

//------------------------------------------------------------------------------
MidiAppMapping::MidiAppMapping(MidiMappingManager *manager, XmlElement *e):
midiManager(manager)
{
	if(e)
	{
		cc = e->getIntAttribute("cc");
		id = e->getIntAttribute("commandId");
	}
	//midiManager->registerAppMapping(this);
}

//------------------------------------------------------------------------------
MidiAppMapping::~MidiAppMapping()
{
	midiManager->unregisterAppMapping(this);
}

//------------------------------------------------------------------------------
XmlElement *MidiAppMapping::getXml() const
{
	XmlElement *retval = new XmlElement("MidiAppMapping");

	retval->setAttribute("cc", cc);
	retval->setAttribute("commandId", id);

	return retval;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MidiMappingManager::MidiMappingManager(ApplicationCommandManager *manager):
appManager(manager),
midiLearnCallback(0)
{

}

//------------------------------------------------------------------------------
MidiMappingManager::~MidiMappingManager()
{
	unsigned int i;
	std::vector<MidiMapping *> tempMappings;
	std::vector<MidiAppMapping *> tempAppMappings;
	std::multimap<int, MidiMapping *>::iterator it;
	std::multimap<int, MidiAppMapping *>::iterator it2;

	for(it=mappings.begin();it!=mappings.end();++it)
		tempMappings.push_back(it->second);
	for(it2=appMappings.begin();it2!=appMappings.end();++it2)
		tempAppMappings.push_back(it2->second);

	for(i=0;i<tempMappings.size();++i)
		delete tempMappings[i];
	for(i=0;i<tempAppMappings.size();++i)
		delete tempAppMappings[i];
}

//------------------------------------------------------------------------------
void MidiMappingManager::midiCcReceived(const MidiMessage& message,
										double secondsSinceStart)
{
	if(LogFile::getInstance().getIsLogging())
	{
		String tempstr;

		if(message.isController())
		{
			tempstr << "MIDI CC message received: CC=" << message.getControllerNumber();
			tempstr << " val=" << message.getControllerValue();
			tempstr << " chan=" << message.getChannel();
		}
		else if(message.isNoteOn())
		{
			tempstr << "MIDI Note On message received: note=" << message.getNoteNumber();
			tempstr << " vel=" << (int)message.getVelocity();
			tempstr << " chan=" << message.getChannel();
		}
		else if(message.isNoteOff())
		{
			tempstr << "MIDI Note Off message received: note=" << message.getNoteNumber();
			tempstr << " vel=" << (int)message.getVelocity();
			tempstr << " chan=" << message.getChannel();
		}
		else if(message.isProgramChange())
		{
			tempstr << "MIDI Program Change message received: prog=" << message.getProgramChangeNumber();
		}
		else
		{
			int i;

			tempstr << "MIDI message received: ";
			for(i=0;i<message.getRawDataSize();++i)
				tempstr << String::toHexString(message.getRawData()[i]);
		}

		LogFile::getInstance().logEvent("MIDI", tempstr);
	}

	if(message.isController())
	{
		int mappingChan;
		multimap<int, MidiMapping *>::iterator it;
		multimap<int, MidiAppMapping *>::iterator it2;
		int cc = message.getControllerNumber();
		int value = message.getControllerValue();
		int messageChan = message.getChannel();

		if(midiLearnCallback)
		{
			midiLearnCallback->midiCcReceived(cc);
			midiLearnCallback = 0;
		}

		//Check if it matches any MidiMappings.
		for(it=mappings.lower_bound(cc);
			it!=mappings.upper_bound(cc);
			++it)
		{
			mappingChan = it->second->getChannel();
			if((mappingChan == 0) || (mappingChan == messageChan))
				it->second->ccReceived(value);
		}

		if(value > 64)
		{
			//Check if it matches any MidiAppMappings.
			for(it2=appMappings.lower_bound(cc);
				it2!=appMappings.upper_bound(cc);
				++it2)
			{
				CommandID id = it2->second->getId();
				MainPanel *panel = dynamic_cast<MainPanel *>(appManager->getFirstCommandTarget(MainPanel::TransportPlay));

				if(panel)
				{
					if(id != MainPanel::TransportTapTempo)
						panel->invokeCommandFromOtherThread(id);
					else
					{
						double tempo = tapHelper.updateTempo(secondsSinceStart);

						if(tempo > 0.0)
							panel->updateTempoFromOtherThread(tempo);
					}
				}
			}
		}
	}
	else if(message.isMidiMachineControlMessage())
	{
		if(PropertiesSingleton::getInstance().getUserSettings()->getBoolValue(L"mmcTransport", false))
		{
			CommandID id = -1;
			MainPanel *panel = dynamic_cast<MainPanel *>(appManager->getFirstCommandTarget(MainPanel::TransportPlay));

			switch(message.getMidiMachineControlCommand())
			{
				case MidiMessage::mmc_stop:
					id = MainPanel::TransportPlay;
					break;
				case MidiMessage::mmc_play:
					id = MainPanel::TransportPlay;
					break;
				case MidiMessage::mmc_rewind:
					id = MainPanel::TransportRtz;
					break;
				case MidiMessage::mmc_pause:
					id = MainPanel::TransportPlay;
					break;
				default:
					break;
			}
			if((id > -1) && panel)
				panel->invokeCommandFromOtherThread(id);
		}
	}
	else if(message.isProgramChange())
	{
		if(PropertiesSingleton::getInstance().getUserSettings()->getBoolValue(L"midiProgramChange", false))
		{
			int newPatch;
			MainPanel *panel = dynamic_cast<MainPanel *>(appManager->getFirstCommandTarget(MainPanel::TransportPlay));

			newPatch = message.getProgramChangeNumber();

			if(panel)
				panel->switchPatchFromProgramChange(newPatch);
		}
	}
}

//------------------------------------------------------------------------------
void MidiMappingManager::registerMapping(int midiCc, MidiMapping *mapping)
{
	jassert(mapping);

	mappings.insert(make_pair(midiCc, mapping));
}

//------------------------------------------------------------------------------
void MidiMappingManager::unregisterMapping(MidiMapping *mapping)
{
	multimap<int, MidiMapping *>::iterator it;

	jassert(mapping);

	for(it=mappings.begin();it!=mappings.end();)
	{
		if(it->second == mapping)
			mappings.erase(it++); //Pass the current iterator to erase, then increment it before erase() is executed, so it's not invalidated by the erase() call.
		else
			++it; //Pre-increment because it should be more efficient.
	}
}

//------------------------------------------------------------------------------
void MidiMappingManager::registerAppMapping(MidiAppMapping *mapping)
{
	jassert(mapping);

	appMappings.insert(make_pair(mapping->getCc(), mapping));
}

//------------------------------------------------------------------------------
void MidiMappingManager::unregisterAppMapping(MidiAppMapping *mapping)
{
	multimap<int, MidiAppMapping *>::iterator it;

	jassert(mapping);

	for(it=appMappings.begin();it!=appMappings.end();)
	{
		if(it->second == mapping)
			appMappings.erase(it++); //Pass the current iterator to erase, then increment it before erase() is executed, so it's not invalidated by the erase() call.
		else
			++it; //Pre-increment because it should be more efficient.
	}
}

//------------------------------------------------------------------------------
MidiAppMapping *MidiMappingManager::getAppMapping(int index)
{
	int i = 0;
	MidiAppMapping *retval = 0;
	multimap<int, MidiAppMapping *>::iterator it;

	for(it=appMappings.begin();it!=appMappings.end();++it)
	{
		if(i == index)
		{
			retval = it->second;
			break;
		}
		++i;
	}

	return retval;
}

//------------------------------------------------------------------------------
void MidiMappingManager::registerMidiLearnCallback(MidiLearnCallback *callback)
{
	midiLearnCallback = callback;
}

//------------------------------------------------------------------------------
void MidiMappingManager::unregisterMidiLearnCallback(MidiLearnCallback *callback)
{
	midiLearnCallback = 0;
}

//------------------------------------------------------------------------------
const StringArray MidiMappingManager::getCCNames()
{
	StringArray retval;

	retval.add(L"0: Bank Select");
    retval.add(L"1: Mod Wheel");
    retval.add(L"2: Breath");
    retval.add(L"3:");
    retval.add(L"4: Foot Pedal");
    retval.add(L"5: Portamento");
    retval.add(L"6: Data Entry");
    retval.add(L"7: Volume");
    retval.add(L"8: Balance");
    retval.add(L"9:");
    retval.add(L"10: Pan");
    retval.add(L"11: Expression");
    retval.add(L"12: Effect Control 1");
    retval.add(L"13: EffectControl 2");
    retval.add(L"14:");
    retval.add(L"15:");
    retval.add(L"16: General Purpose 1");
    retval.add(L"17: General Purpose 2");
    retval.add(L"18: General Purpose 3");
    retval.add(L"19: General Purpose 4");
    retval.add(L"20:");
    retval.add(L"21:");
    retval.add(L"22:");
    retval.add(L"23:");
    retval.add(L"24:");
    retval.add(L"25:");
    retval.add(L"26:");
    retval.add(L"27:");
    retval.add(L"28:");
    retval.add(L"29:");
    retval.add(L"30:");
    retval.add(L"31:");
    retval.add(L"32: Bank Select (fine)");
    retval.add(L"33: Mod Wheel (fine)");
    retval.add(L"34: Breath (fine)");
    retval.add(L"35:");
    retval.add(L"36: Foot Pedal (fine)");
    retval.add(L"37: Portamento (fine)");
    retval.add(L"38: Data Entry (fine)");
    retval.add(L"39: Volume (fine)");
    retval.add(L"40: Balance (fine)");
    retval.add(L"41:");
    retval.add(L"42: Pan (fine)");
    retval.add(L"43: Expression (fine)");
    retval.add(L"44: Effect Control 1 (fine)");
    retval.add(L"45: Effect Control 2 (fine)");
    retval.add(L"46:");
    retval.add(L"47:");
    retval.add(L"48:");
    retval.add(L"49:");
    retval.add(L"50:");
    retval.add(L"51:");
    retval.add(L"52:");
    retval.add(L"53:");
    retval.add(L"54:");
    retval.add(L"55:");
    retval.add(L"56:");
    retval.add(L"57:");
    retval.add(L"58:");
    retval.add(L"59:");
    retval.add(L"60:");
    retval.add(L"61:");
    retval.add(L"62:");
    retval.add(L"63:");
    retval.add(L"64: Hold Pedal");
    retval.add(L"65: Portamento (on/off)");
    retval.add(L"66: Sustenuto Pedal");
    retval.add(L"67: Soft Pedal");
    retval.add(L"68: Legato Pedal");
    retval.add(L"69: Hold 2 Pedal");
    retval.add(L"70: Sound Variation");
    retval.add(L"71: Sound Timbre");
    retval.add(L"72: Sound Release Time");
    retval.add(L"73: Sound Attack Time");
    retval.add(L"74: Sound Brightness");
    retval.add(L"75: Sound Control 6");
    retval.add(L"76: Sound Control 7");
    retval.add(L"77: Sound Control 8");
    retval.add(L"78: Sound Control 9");
    retval.add(L"79: Sound Control 10");
    retval.add(L"80: General Purpose Button 1");
    retval.add(L"81: General Purpose Button 2");
    retval.add(L"82: General Purpose Button 3");
    retval.add(L"83: General Purpose Button 4");
    retval.add(L"84:");
    retval.add(L"85:");
    retval.add(L"86:");
    retval.add(L"87:");
    retval.add(L"88:");
    retval.add(L"89:");
    retval.add(L"90:");
    retval.add(L"91: Effects Level");
    retval.add(L"92: Tremolo Level");
    retval.add(L"93: Chorus Level");
    retval.add(L"94: Celeste Level");
    retval.add(L"95: Phaser Level");
    retval.add(L"96: Data Button Inc");
    retval.add(L"97: Data Button Dec");
    retval.add(L"98: NRPN (fine)");
    retval.add(L"99: NRPN (coarse)");
    retval.add(L"100: RPN (fine)");
    retval.add(L"101: RPN (coarse)");
    retval.add(L"102:");
    retval.add(L"103:");
    retval.add(L"104:");
    retval.add(L"105:");
    retval.add(L"106:");
    retval.add(L"107:");
    retval.add(L"108:");
    retval.add(L"109:");
    retval.add(L"110:");
    retval.add(L"111:");
    retval.add(L"112:");
    retval.add(L"113:");
    retval.add(L"114:");
    retval.add(L"115:");
    retval.add(L"116:");
    retval.add(L"117:");
    retval.add(L"118:");
    retval.add(L"119:");
    retval.add(L"120: All Sound Off");
    retval.add(L"121: All Controllers Off");
    retval.add(L"122: Local Keyboard");
    retval.add(L"123: All Notes Off");
    retval.add(L"124: Omni Mode Off");
    retval.add(L"125: Omni Mode On");
    retval.add(L"126: Mono Operation");
    retval.add(L"127: Poly Operation");

	return retval;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MidiInterceptor::MidiInterceptor():
midiManager(0),
samplesSinceStart(0)
{

}

//------------------------------------------------------------------------------
MidiInterceptor::~MidiInterceptor()
{

}

//------------------------------------------------------------------------------
void MidiInterceptor::setManager(MidiMappingManager *manager)
{
	midiManager = manager;
}

//------------------------------------------------------------------------------
void MidiInterceptor::fillInPluginDescription(PluginDescription &description) const
{
	description.name = L"Midi Interceptor";
	description.descriptiveName = L"Hidden Midi Interceptor plugin for mapping MIDI CCs to parameters.";
	description.pluginFormatName = L"Internal";
	description.category = L"Internal";
	description.manufacturerName = L"Niall Moody";
	description.version = L"1.00";
	description.uid = description.name.hashCode();
	description.isInstrument = true; //?
	description.numInputChannels = 0;
	description.numOutputChannels = 0;
}

//------------------------------------------------------------------------------
void MidiInterceptor::processBlock(AudioSampleBuffer &buffer,
								   MidiBuffer &midiMessages)
{
	int samplePos;
	double seconds;
	MidiMessage tempMess(0xf0);
	MidiBuffer::Iterator it(midiMessages);
	const double sampleRate = getSampleRate();

	jassert(sampleRate > 0.0);

	if(midiManager)
	{
		int numMess = midiMessages.getNumEvents();

		if(numMess > 0)
			numMess = numMess;

		while(it.getNextEvent(tempMess, samplePos))
		{
			seconds = (double)(samplesSinceStart+samplePos)/sampleRate;
			midiManager->midiCcReceived(tempMess, seconds);
		}
	}

	samplesSinceStart += buffer.getNumSamples();

	midiMessages.clear();
}
