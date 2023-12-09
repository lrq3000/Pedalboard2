//	OscMappingManager.cpp - Class which dispatches OSC messages to OscMappings.
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
#include "OscMappingManager.h"
#include "MainPanel.h"
#include "LogFile.h"

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
OscMapping::OscMapping(OscMappingManager *manager,
					   FilterGraph *graph,
					   uint32 pluginId,
					   int param,
					   const String& oscAddress,
					   int oscParam):
Mapping(graph, pluginId, param),
mappingManager(manager),
address(oscAddress),
parameter(oscParam)
{
	//mappingManager->registerMapping(address, this);
}

//------------------------------------------------------------------------------
OscMapping::OscMapping(OscMappingManager *manager,
					   FilterGraph *graph,
					   XmlElement *e):
Mapping(graph, e),
mappingManager(manager)
{
	if(e)
	{
		address = e->getStringAttribute("address");
		parameter = e->getIntAttribute("parameterIndex");
	}
	//mappingManager->registerMapping(address, this);
}

//------------------------------------------------------------------------------
OscMapping::~OscMapping()
{
	mappingManager->unregisterMapping(this);
}

//------------------------------------------------------------------------------
void OscMapping::messageReceived(float val)
{
	updateParameter(val);
}

//------------------------------------------------------------------------------
XmlElement *OscMapping::getXml() const
{
	XmlElement *retval = new XmlElement("OscMapping");

	retval->setAttribute("pluginId", (int)getPluginId());
	retval->setAttribute("parameter", getParameter());
	retval->setAttribute("address", address);
	retval->setAttribute("parameterIndex", parameter);

	return retval;
}

//------------------------------------------------------------------------------
void OscMapping::setAddress(const String& oscAddress)
{
	address = oscAddress;
	mappingManager->unregisterMapping(this);
	mappingManager->registerMapping(address, this);
}

//------------------------------------------------------------------------------
void OscMapping::setParameterIndex(int val)
{
	parameter = val;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
OscAppMapping::OscAppMapping(OscMappingManager *manager,
						     const String& oscAddress,
							 int oscParam,
							 CommandID commandId):
oscManager(manager),
address(oscAddress),
parameter(oscParam),
id(commandId)
{
	//oscManager->registerAppMapping(this);
}

//------------------------------------------------------------------------------
OscAppMapping::OscAppMapping(OscMappingManager *manager, XmlElement *e):
oscManager(manager)
{
	if(e)
	{
		address = e->getStringAttribute("address");
		parameter = e->getIntAttribute("parameterIndex");
		id = e->getIntAttribute("commandId");
	}
	//oscManager->registerAppMapping(this);
}

//------------------------------------------------------------------------------
OscAppMapping::~OscAppMapping()
{
	oscManager->unregisterAppMapping(this);
}

//------------------------------------------------------------------------------
XmlElement *OscAppMapping::getXml() const
{
	XmlElement *retval = new XmlElement("OscAppMapping");

	retval->setAttribute("address", address);
	retval->setAttribute("parameterIndex", parameter);
	retval->setAttribute("commandId", id);

	return retval;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
OscMappingManager::OscMappingManager(ApplicationCommandManager *manager):
appManager(manager)
{

}

//------------------------------------------------------------------------------
OscMappingManager::~OscMappingManager()
{
	unsigned int i;
	std::vector<OscMapping *> tempMappings;
	std::vector<OscAppMapping *> tempAppMappings;
	std::multimap<String, OscMapping *>::iterator it;
	std::multimap<String, OscAppMapping *>::iterator it2;

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
void OscMappingManager::messageReceived(OSC::Message *message)
{
	int i;
	ScopedPointer<MidiMessage> tempMess;
	String address(message->getAddress().c_str());
	multimap<String, BypassableInstance *>::iterator it;

	if(LogFile::getInstance().getIsLogging())
	{
		String tempstr;

		tempstr << "OSC Message received. address=" << address;
		tempstr << " typetag=" << message->getTypeTag().c_str();
		if(message->getNumFloats() > 0)
		{
			tempstr << " float value(s)=";
			for(i=0;i<message->getNumFloats();++i)
				tempstr << message->getFloat(i) << " ";
		}
		if(message->getNumInts() > 0)
		{
			tempstr << " int value(s)=";
			for(i=0;i<message->getNumInts();++i)
				tempstr << message->getInt(i) << " ";
		}
		if(message->getNumStrings() > 0)
		{
			tempstr << " string value(s)=";
			for(i=0;i<message->getNumStrings();++i)
				tempstr << message->getString(i).c_str() << " ";
		}
		if(message->getNumMIDI() > 0)
		{
			tempstr << " MIDI value(s)=";
			for(i=0;i<message->getNumMIDI();++i)
			{
				tempstr << String::toHexString(message->getMIDI(i).bytes.byte0);
				tempstr << String::toHexString(message->getMIDI(i).bytes.byte1);
				tempstr << String::toHexString(message->getMIDI(i).bytes.byte2);
				tempstr << String::toHexString(message->getMIDI(i).bytes.byte3);
			}
		}

		LogFile::getInstance().logEvent("OSC", tempstr);
	}

	//Standard OSC mappings are all treated as floats (0->1).
	for(i=0;i<message->getNumFloats();++i)
		handleFloatMessage(address, i, message->getFloat(i));

	//...but we may also have MIDI over OSC, which could be various types, so...
	it = midiProcessors.lower_bound(address);
	if(it != midiProcessors.end())
	{
		if(message->getNumMIDI() > 0)
		{
			OSC::MIDIMessage tempMess2 = message->getMIDI(0);

			tempMess = new MidiMessage(tempMess2.bytes.byte1,
									   tempMess2.bytes.byte2,
									   tempMess2.bytes.byte3);
		}
		else if(message->getNumInts() > 2)
		{
			tempMess = new MidiMessage(message->getInt(0),
									   message->getInt(1),
									   message->getInt(2));
		}
		else if(message->getNumFloats() > 2)
		{
			tempMess = new MidiMessage((int)message->getFloat(0),
									   (int)message->getFloat(1),
									   (int)message->getFloat(2));
		}
	}

	if(tempMess)
	{
		tempMess->setTimeStamp(Time::getMillisecondCounter()/1000.0);

		for(it;
			it!=midiProcessors.upper_bound(address);
			++it)
		{
			if(it->second)
				it->second->addMidiMessage(*tempMess);
		}
	}
}

//------------------------------------------------------------------------------
void OscMappingManager::handleFloatMessage(const String& address,
										   int index,
										   float val)
{
	multimap<String, OscMapping *>::iterator it;
	multimap<String, OscAppMapping *>::iterator it2;

	//Check against any OscMappings.
	for(it=mappings.lower_bound(address);
		it!=mappings.upper_bound(address);
		++it)
	{
		if(it->second->getParameterIndex() == index)
			it->second->messageReceived(val);
	}

	//Check against any OscAppMappings.
	if(val > 0.5f)
	{
		for(it2=appMappings.lower_bound(address);
			it2!=appMappings.upper_bound(address);
			++it2)
		{
			if(it2->second->getParameterIndex() == index)
			{
				CommandID id = it2->second->getId();
				MainPanel *panel = dynamic_cast<MainPanel *>(appManager->getFirstCommandTarget(MainPanel::TransportPlay));

				if(panel)
				{
					if(id != MainPanel::TransportTapTempo)
						panel->invokeCommandFromOtherThread(id);
					else
					{
						double tempo = tapHelper.updateTempo(Time::highResolutionTicksToSeconds(Time::getHighResolutionTicks()));

						if(tempo > 0.0)
							panel->updateTempoFromOtherThread(tempo);
					}
				}
			}
		}
	}

	//Add it to uniqueAddresses if necessary.
	uniqueAddresses.addIfNotAlreadyThere(address);
}

//------------------------------------------------------------------------------
void OscMappingManager::handleMIDIMessage(const String& address,
										  OSC::MIDIMessage val)
{
	multimap<String, BypassableInstance *>::iterator it;
	MidiMessage tempMess(val.bytes.byte1, val.bytes.byte2, val.bytes.byte3);

	for(it=midiProcessors.lower_bound(address);
		it!=midiProcessors.upper_bound(address);
		++it)
	{
		if(it->second)
			it->second->addMidiMessage(tempMess);
	}
}

//------------------------------------------------------------------------------
void OscMappingManager::registerMapping(const String& address, OscMapping *mapping)
{
	jassert(mapping);

	mappings.insert(make_pair(address, mapping));
}

//------------------------------------------------------------------------------
void OscMappingManager::unregisterMapping(OscMapping *mapping)
{
	multimap<String, OscMapping *>::iterator it;

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
void OscMappingManager::registerAppMapping(OscAppMapping *mapping)
{
	jassert(mapping);

	appMappings.insert(make_pair(mapping->getAddress(), mapping));
}

//------------------------------------------------------------------------------
void OscMappingManager::unregisterAppMapping(OscAppMapping *mapping)
{
	multimap<String, OscAppMapping *>::iterator it;

	for(it=appMappings.begin();it!=appMappings.end();)
	{
		if(it->second == mapping)
			appMappings.erase(it++); //Pass the current iterator to erase, then increment it before erase() is executed, so it's not invalidated by the erase() call.
		else
			++it; //Pre-increment because it should be more efficient.
	}
}

//------------------------------------------------------------------------------
void OscMappingManager::registerMIDIProcessor(const String& address,
											  BypassableInstance *processor)
{
	jassert(processor);

	//So we don't get multiple entries for a single processor.
	unregisterMIDIProcessor(processor);

	midiProcessors.insert(make_pair(address, processor));
}

//------------------------------------------------------------------------------
void OscMappingManager::unregisterMIDIProcessor(BypassableInstance *processor)
{
	multimap<String, BypassableInstance *>::iterator it;

	for(it=midiProcessors.begin();it!=midiProcessors.end();++it)
	{
		if(it->second == processor)
		{
			midiProcessors.erase(it);
			break;
		}
	}
}

//------------------------------------------------------------------------------
const String OscMappingManager::getMIDIProcessorAddress(BypassableInstance *processor) const
{
	String retval;
	multimap<String, BypassableInstance *>::const_iterator it;

	for(it=midiProcessors.begin();it!=midiProcessors.end();++it)
	{
		if(it->second == processor)
		{
			retval = it->first;
			break;
		}
	}

	return retval;
}

//------------------------------------------------------------------------------
OscAppMapping *OscMappingManager::getAppMapping(int index)
{
	int i = 0;
	OscAppMapping *retval = 0;
	multimap<String, OscAppMapping *>::iterator it;

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
//------------------------------------------------------------------------------
OscInput::OscInput()
{

}

//------------------------------------------------------------------------------
OscInput::~OscInput()
{

}

//------------------------------------------------------------------------------
void OscInput::fillInPluginDescription(PluginDescription &description) const
{
	description.name = L"OSC Input";
	description.descriptiveName = L"Dummy AudioProcessor so we can see at a glance which plugins have OSC mappings.";
	description.pluginFormatName = L"Internal";
	description.category = L"Internal";
	description.manufacturerName = L"Niall Moody";
	description.version = L"1.00";
	description.uid = description.name.hashCode();
	description.isInstrument = false; //?
	description.numInputChannels = 0;
	description.numOutputChannels = 0;
}

//------------------------------------------------------------------------------
void OscInput::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
	
}
