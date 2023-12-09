//	PluginField.cpp - Field representing the signal path through the app.
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

#include "PluginField.h"
#include "FilterGraph.h"
#include "PluginComponent.h"
#include "Mapping.h"
#include "LogFile.h"
#include "InternalFilters.h"
#include "PropertiesSingleton.h"
#include "ColourScheme.h"
#include "MainTransport.h"
#include "PedalboardProcessors.h"
#include "BypassableInstance.h"
#include "AudioSingletons.h"

#include "NiallsOSCLib/OSCMessage.h"
#include "NiallsOSCLib/OSCBundle.h"

using namespace std;

//------------------------------------------------------------------------------
PluginField::PluginField(FilterGraph *filterGraph,
						 KnownPluginList *list,
						 ApplicationCommandManager *appManager):
signalPath(filterGraph),
pluginList(list),
midiManager(appManager),
oscManager(appManager),
draggingConnection(0),
tempo(120.0),
displayDoubleClickMessage(true)
{
	int i;

	audioInputEnabled = PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("AudioInput", true);
	midiInputEnabled = PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("MidiInput", true);
	oscInputEnabled = PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("OscInput", true);

	autoMappingsWindow = PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("AutoMappingsWindow", true);

	//Inform the signal path about our AudioPlayHead.
	signalPath->getGraph().setPlayHead(this);

	//Add OSC input.
	if(oscInputEnabled)
	{
		OscInput p;
		PluginDescription desc;

        p.fillInPluginDescription(desc);

		signalPath->addFilter(&desc, 10, 215);
	}

	//Setup gui.
	for(i=0;i<signalPath->getNumFilters();++i)
		addFilter(i);

	//Add MidiInterceptor.
	if(midiInputEnabled)
	{
		MidiInterceptor p;
		PluginDescription desc;

        p.fillInPluginDescription(desc);

		signalPath->addFilter(&desc, 100, 100);

		//And connect it up to the midi input.
		{
			uint32 midiInput = 0;
			uint32 midiInterceptor = 0;

			for(i=0;i<signalPath->getNumFilters();++i)
			{
				if(signalPath->getNode(i)->getProcessor()->getName() == "Midi Input")
					midiInput = signalPath->getNode(i)->nodeId;
				else if(signalPath->getNode(i)->getProcessor()->getName() == "Midi Interceptor")
				{
					midiInterceptor = signalPath->getNode(i)->nodeId;
					dynamic_cast<MidiInterceptor *>(signalPath->getNode(i)->getProcessor())->setManager(&midiManager);
				}
			}
			signalPath->addConnection(midiInput,
									  AudioProcessorGraph::midiChannelIndex,
									  midiInterceptor,
									  AudioProcessorGraph::midiChannelIndex);
		}
	}

	setWantsKeyboardFocus(true);

	startTimer(50);
}

//------------------------------------------------------------------------------
PluginField::~PluginField()
{
	int i;
	multimap<uint32, Mapping *>::iterator it;

	//If we don't do this, the connections will try to contact their pins, which
	//may have already been deleted.
	for(i=(getNumChildComponents()-1);i>=0;--i)
	{
		PluginConnection *connection = dynamic_cast<PluginConnection *>(getChildComponent(i));
		
		if(connection)
		{
			removeChildComponent(connection);
			delete connection;
		}
	}

	for(it=mappings.begin();it!=mappings.end();++it)
		delete it->second;

	deleteAllChildren();
}

//------------------------------------------------------------------------------
void PluginField::paint(Graphics& g)
{
	g.fillAll(ColourScheme::getInstance().colours[L"Field Background"]);

	if(displayDoubleClickMessage)
	{
		g.setColour(ColourScheme::getInstance().colours[L"Text Colour"].withAlpha(0.5f));
		g.drawText("<double-click to add processor>",
				   400,
				   230,
				   300,
				   30,
				   Justification(Justification::centredLeft),
				   false);
	}
}

//------------------------------------------------------------------------------
void PluginField::mouseDown(const MouseEvent& e)
{
	if(e.getNumberOfClicks() == 2)
	{
		int result = 0;
		PopupMenu menu;

		pluginList->addToMenu(menu, KnownPluginList::sortAlphabetically);

		//result = menu.showAt(Rectangle<int>(e.x, e.y, 0, 0));
		result = menu.show();

		if(result > 0)
		{
			int pluginIndex = signalPath->getNumFilters()-1;

			signalPath->addFilter(pluginList->getType(pluginList->getIndexChosenByMenu(result)),
								  (double)e.x,
								  (double)e.y);

			//Make sure the plugin got created before we add a component for it.
			if((signalPath->getNumFilters()-1) > pluginIndex)
			{
				pluginIndex = signalPath->getNumFilters()-1;

				addFilter(pluginIndex);

				sendChangeMessage();

				clearDoubleClickMessage();
			}
		}
	}
}

//------------------------------------------------------------------------------
void PluginField::changeListenerCallback(ChangeBroadcaster *source)
{
	PluginComponent *pluginComp = dynamic_cast<PluginComponent *>(source);

	if(pluginComp)
	{
		Point<int> fieldSize(getWidth(), getHeight());
		Point<int> pluginPos = pluginComp->getPosition();
		Point<int> pluginSize(pluginComp->getWidth(), pluginComp->getHeight());

		if((pluginPos.getX()+pluginSize.getX()) > fieldSize.getX())
			fieldSize.setX((pluginPos.getX()+pluginSize.getX()));
		if((pluginPos.getY()+pluginSize.getY()) > fieldSize.getY())
			fieldSize.setY((pluginPos.getY()+pluginSize.getY()));

		setSize(fieldSize.getX(), fieldSize.getY());
		repaint();
	}
}

//------------------------------------------------------------------------------
void PluginField::timerCallback()
{
	int i;

	for(i=0;i<getNumChildComponents();++i)
	{
		PluginComponent *comp = dynamic_cast<PluginComponent *>(getChildComponent(i));

		if(comp)
			comp->timerUpdate();
	}
}

//------------------------------------------------------------------------------
bool PluginField::isInterestedInFileDrag(const StringArray& files)
{
	int i;
	bool retval = false;

	for(i=0;i<files.size();++i)
	{
		//If they're plugins.
#ifdef WIN32
		if(files[i].endsWith(".dll"))
		{
			retval = true;
			break;
		}
#elif defined(__APPLE__)
		if(files[i].endsWith(".vst") || files[i].endsWith(".component"))
		{
			retval = true;
			break;
		}
#endif
		//If they're sound files.
		else if(files[i].endsWith(".wav") ||
				files[i].endsWith(".aif") ||
				files[i].endsWith(".aiff") ||
				files[i].endsWith(".ogg")||
				files[i].endsWith(".flac")||
				files[i].endsWith(".wma"))
		{
			retval = true;
			break;
		}
	}

	return retval;
}

//------------------------------------------------------------------------------
void PluginField::filesDropped(const StringArray& files, int x, int y)
{
	int i;
	bool soundsInArray = false;
	bool pluginsInArray = false;
	OwnedArray<PluginDescription> foundPlugins;

	for(i=0;i<files.size();++i)
	{
		//If they're plugins.
#ifdef WIN32
		if(files[i].endsWith(".dll"))
			pluginsInArray = true;
#elif defined(__APPLE__)
		if(files[i].endsWith(".vst") || files[i].endsWith(".component"))
			pluginsInArray = true;
#endif
		//If they're sound files.
		else if(files[i].endsWith(".wav") ||
				files[i].endsWith(".aif") ||
				files[i].endsWith(".aiff") ||
				files[i].endsWith(".ogg")||
				files[i].endsWith(".flac")||
				files[i].endsWith(".wma"))
		{
			soundsInArray = true;
		}
	}

	if(pluginsInArray)
	{
		pluginList->scanAndAddDragAndDroppedFiles(AudioPluginFormatManagerSingleton::getInstance(),
												  files,
												  foundPlugins);

		for(i=0;i<foundPlugins.size();++i)
		{
			int pluginIndex = signalPath->getNumFilters()-1;

			signalPath->addFilter(foundPlugins[i],
								  (double)x,
								  (double)y);

			//Make sure the plugin got created before we add a component for it.
			if((signalPath->getNumFilters()-1) > pluginIndex)
			{
				pluginIndex = signalPath->getNumFilters()-1;

				addFilter(pluginIndex);

				sendChangeMessage();
			}

			x += 100;
			y += 100;
		}
	}

	if(soundsInArray)
	{
		for(i=0;i<files.size();++i)
		{
			int pluginIndex = signalPath->getNumFilters()-1;

			signalPath->addFilter(new FilePlayerProcessor(File(files[i])),
								  (double)x,
								  (double)y);

			//Make sure the plugin got created before we add a component for it.
			if((signalPath->getNumFilters()-1) > pluginIndex)
			{
				pluginIndex = signalPath->getNumFilters()-1;

				addFilter(pluginIndex);

				sendChangeMessage();

				clearDoubleClickMessage();
			}
		}
	}
}

//------------------------------------------------------------------------------
bool PluginField::getCurrentPosition(CurrentPositionInfo &result)
{
	result.bpm = tempo;
	result.timeSigNumerator = 4;
	result.timeSigDenominator = 4;
	result.timeInSeconds = 0.0;
	result.editOriginTime = 0.0;
	result.ppqPosition = 0.0;
	result.ppqPositionOfLastBarStart = 0.0;
	result.frameRate = AudioPlayHead::fpsUnknown;
	result.isPlaying = MainTransport::getInstance()->getState();
	result.isRecording = false;

	return true;
}

//------------------------------------------------------------------------------
void PluginField::enableAudioInput(bool val)
{
	int i;

	audioInputEnabled = val;

	if(!val)
	{
		//Delete the filter(s) in the signal path.
		for(i=(signalPath->getNumFilters()-1);i>=0;--i)
		{
			if(signalPath->getNode(i)->getProcessor()->getName() == "Audio Input")
				deleteFilter(signalPath->getNode(i));
		}

		//Delete the associated "Audio Input" PluginComponent(s).
		for(i=(getNumChildComponents()-1);i>=0;--i)
		{
			PluginComponent *comp = dynamic_cast<PluginComponent *>(getChildComponent(i));

			if(comp)
			{
				if(comp->getNode()->getProcessor()->getName() == "Audio Input")
					delete removeChildComponent(i);
			}
		}
	}
	else
	{
		InternalPluginFormat internalFormat;

		//Add the filter to the signal path.
		signalPath->addFilter(internalFormat.getDescriptionFor(InternalPluginFormat::audioInputFilter),
				              10.0f, 10.0f);

		//Add the associated PluginComponent.
		addFilter(signalPath->getNumFilters()-1);
	}
}

//------------------------------------------------------------------------------
void PluginField::enableMidiInput(bool val)
{
	int i;
	AudioProcessorGraph::Node *tempNode = 0;
	multimap<uint32, Mapping *>::iterator it;

	midiInputEnabled = val;

	if(!val)
	{
		//Delete mappings.
		for(it=mappings.begin();it!=mappings.end();)
		{
			MidiMapping *midiMapping = dynamic_cast<MidiMapping *>(it->second);

			if(midiMapping)
			{
				delete it->second;
				mappings.erase(it++);
			}
			else
				++it;
		}

		//Midi Input filter.
		{
			//Delete filter.
			for(i=(signalPath->getNumFilters()-1);i>=0;--i)
			{
				if(signalPath->getNode(i)->getProcessor()->getName() == "Midi Input")
				{
					tempNode = signalPath->getNode(i);
					deleteFilter(tempNode);
				}
			}

			//Delete PluginComponent.
			for(i=(getNumChildComponents()-1);i>=0;--i)
			{
				PluginComponent *comp = dynamic_cast<PluginComponent *>(getChildComponent(i));

				if(comp)
				{
					if(comp->getNode() == tempNode)
						delete removeChildComponent(i);
				}
			}
		}
		//Midi Interceptor filter.
		{
			//Delete filter.
			for(i=(signalPath->getNumFilters()-1);i>=0;--i)
			{
				if(signalPath->getNode(i)->getProcessor()->getName() == "Midi Interceptor")
					deleteFilter(signalPath->getNode(i));
			}
		}
	}
	else
	{
		InternalPluginFormat internalFormat;

		//Add the filter to the signal path.
		signalPath->addFilter(internalFormat.getDescriptionFor(InternalPluginFormat::midiInputFilter),
				              10.0f, 120.0f);

		//Add the associated PluginComponent.
		addFilter(signalPath->getNumFilters()-1);

		//Add the Midi Interceptor too.
		{
			MidiInterceptor p;
			PluginDescription desc;

			p.fillInPluginDescription(desc);

			signalPath->addFilter(&desc, 100, 100);

			//And connect it up to the midi input.
			{
				uint32 midiInput = 0;
				uint32 midiInterceptor = 0;

				for(i=0;i<signalPath->getNumFilters();++i)
				{
					if(signalPath->getNode(i)->getProcessor()->getName() == "Midi Input")
						midiInput = signalPath->getNode(i)->nodeId;
					else if(signalPath->getNode(i)->getProcessor()->getName() == "Midi Interceptor")
					{
						midiInterceptor = signalPath->getNode(i)->nodeId;
						dynamic_cast<MidiInterceptor *>(signalPath->getNode(i)->getProcessor())->setManager(&midiManager);
					}
				}
				signalPath->addConnection(midiInput,
										  AudioProcessorGraph::midiChannelIndex,
										  midiInterceptor,
										  AudioProcessorGraph::midiChannelIndex);
			}
		}
	}
}

//------------------------------------------------------------------------------
void PluginField::enableOscInput(bool val)
{
	int i;
	multimap<uint32, Mapping *>::iterator it;
	AudioProcessorGraph::Node *tempNode = 0;

	oscInputEnabled = val;

	if(!val)
	{
		//Delete mappings.
		for(it=mappings.begin();it!=mappings.end();)
		{
			OscMapping *oscMapping = dynamic_cast<OscMapping *>(it->second);

			if(oscMapping)
			{
				delete it->second;
				mappings.erase(it++);
			}
			else
				++it;
		}

		//Delete filter.
		for(i=(signalPath->getNumFilters()-1);i>=0;--i)
		{
			if(signalPath->getNode(i)->getProcessor()->getName() == "OSC Input")
			{
				tempNode = signalPath->getNode(i);
				deleteFilter(tempNode);
			}
		}

		//Delete PluginComponent.
		for(i=(getNumChildComponents()-1);i>=0;--i)
		{
			PluginComponent *comp = dynamic_cast<PluginComponent *>(getChildComponent(i));

			if(comp)
			{
				if(comp->getNode() == tempNode)
					delete removeChildComponent(i);
			}
		}
	}
	else
	{
		OscInput p;
		PluginDescription desc;

        p.fillInPluginDescription(desc);

		signalPath->addFilter(&desc, 10, 215);

		addFilter(signalPath->getNumFilters()-1);
	}
}

//------------------------------------------------------------------------------
void PluginField::setAutoMappingsWindow(bool val)
{
	autoMappingsWindow = val;
}

//------------------------------------------------------------------------------
void PluginField::setTempo(double val)
{
	tempo = val;
}

//------------------------------------------------------------------------------
void PluginField::addFilter(int index, bool broadcastChangeMessage)
{
	int x, y;
	PluginComponent *plugin;
	AudioProcessorGraph::Node *node;

	if(index < signalPath->getNumFilters())
	{
		node = signalPath->getNode(index);

		if(node->getProcessor()->getName() != "Midi Interceptor")
		{
			//Make sure the plugin knows about the AudioPlayHead.
			node->getProcessor()->setPlayHead(this);

			plugin = new PluginComponent(node);
			x = signalPath->getNode(index)->properties.getWithDefault("x", 0);
			y = signalPath->getNode(index)->properties.getWithDefault("y", 0);
			plugin->setTopLeftPosition(x, y);
			plugin->addChangeListener(this);
			addAndMakeVisible(plugin);

			if(LogFile::getInstance().getIsLogging())
			{
				String tempstr;

				tempstr << "Added plugin to signal path: " << node->getProcessor()->getName();
				LogFile::getInstance().logEvent("Pedalboard", tempstr);
			}

			//To make sure the plugin field bounds are correct.
			changeListenerCallback(plugin);

			if(broadcastChangeMessage)
				sendChangeMessage();
		}
	}
}

//------------------------------------------------------------------------------
void PluginField::deleteFilter(AudioProcessorGraph::Node *node)
{
	int i;
	PluginConnection *connection;
	const uint32 uid = node->nodeId;
	multimap<uint32, Mapping *>::iterator it;
	String pluginName = node->getProcessor()->getName();

	//Disconnect any PluginConnections.
	for(i=getNumChildComponents();i>=0;--i)
	{
		connection = dynamic_cast<PluginConnection *>(getChildComponent(i));

		if(connection)
		{
			const PluginPinComponent *src = connection->getSource();
			const PluginPinComponent *dest = connection->getDestination();
			uint32 srcId, destId;

			//Had a crash here once, where dest was 0. Not exactly sure why that
			//happened, but the following will at least prevent it happening
			//again...
			if(src)
				srcId = src->getUid();
			else
				srcId = 4294967295u;
			if(dest)
				destId = dest->getUid();
			else
				destId = 4294967295u;

			if((uid == srcId) || (uid == destId))
			{
				removeChildComponent(connection);
				delete connection;
			}
		}
	}

	//Delete any associated mappings.
	for(it=mappings.lower_bound(uid);
		it!=mappings.upper_bound(uid);
		++it)
	{
		delete it->second;
	}
	mappings.erase(uid);

	//Unregister the filter from wanting MIDI over OSC.
	{
		BypassableInstance *proc = dynamic_cast<BypassableInstance *>(node->getProcessor());

		if(proc)
			oscManager.unregisterMIDIProcessor(proc);
	}

	signalPath->disconnectFilter(uid);
	signalPath->removeFilter(uid);

	if(LogFile::getInstance().getIsLogging())
	{
		String tempstr;

		tempstr << "Deleted plugin from signal path: " << pluginName;
		LogFile::getInstance().logEvent("Pedalboard", tempstr);
	}

	sendChangeMessage();
}

//------------------------------------------------------------------------------
void PluginField::updateProcessorName(uint32 id, const String& val)
{
	userNames[id] = val;
}

//------------------------------------------------------------------------------
void PluginField::addConnection(PluginPinComponent *source, bool connectAll)
{
	if(source)
	{
		PluginConnection *connection = new PluginConnection(source,
															0,
															connectAll);

		connection->setSize(10, 12);
		addAndMakeVisible(connection);
		draggingConnection = connection;

		sendChangeMessage();
	}
}

//------------------------------------------------------------------------------
void PluginField::dragConnection(int x, int y)
{
	if(draggingConnection)
	{
		Component *c = getPinAt(x+5, y);
		PluginPinComponent *p = dynamic_cast<PluginPinComponent *>(c);

		if(p)
		{
			if(p->getParameterPin() == draggingConnection->getParameterConnection())
			{
				Point<int> tempPoint(p->getX()+5, p->getY()+6);

				tempPoint = getLocalPoint(p->getParentComponent(), tempPoint);
				draggingConnection->drag(tempPoint.getX(), tempPoint.getY());
			}
			else
				draggingConnection->drag(x, y);
		}
		else
			draggingConnection->drag(x, y);
	}
}

//------------------------------------------------------------------------------
void PluginField::releaseConnection(int x, int y)
{
	if(draggingConnection)
	{
		//PluginPinComponent *p = dynamic_cast<PluginPinComponent *>(getComponentAt(x, y));
		Component *c = getPinAt(x, y);
		PluginPinComponent *p = dynamic_cast<PluginPinComponent *>(c);

		/*testX = x;
		testY = y;*/
		repaint();

		if(p)
		{
			if(!p->getDirection())
			{
				//draggingConnection->setDestination(p);

				const PluginPinComponent *s = draggingConnection->getSource();
				//const PluginPinComponent *d = draggingConnection->getDestination();

				if((s->getParameterPin() && p->getParameterPin()) ||
				   (!s->getParameterPin() && !p->getParameterPin()))
				{
					signalPath->addConnection(s->getUid(),
											  s->getChannel(),
											  p->getUid(),
											  p->getChannel());
					draggingConnection->setDestination(p);

					//If we should be connecting all the outputs and inputs of the two
					//plugins (user holding down shift).
					if(draggingConnection->getRepresentsAllOutputs())
					{
						connectAll(draggingConnection);
						draggingConnection->setRepresentsAllOutputs(false);
					}

					if(p->getParameterPin())
					{
						PluginComponent *pComp = dynamic_cast<PluginComponent *>(p->getParentComponent());

						if(pComp && autoMappingsWindow)
							pComp->openMappingsWindow();

					}
					moveConnectionsBehind();

					sendChangeMessage();
				}
				else
				{
					removeChildComponent(draggingConnection);
					delete draggingConnection;
				}
			}
		}
		else
		{
			removeChildComponent(draggingConnection);
			delete draggingConnection;
		}
		draggingConnection = 0;
	}
}

//------------------------------------------------------------------------------
void PluginField::deleteConnection()
{
	int i;

	for(i=(getNumChildComponents()-1);i>=0;--i)
	{
		PluginConnection *c = dynamic_cast<PluginConnection *>(getChildComponent(i));

		if(c)
		{
			if(c->getSelected())
			{
				const PluginPinComponent *s = c->getSource();
				const PluginPinComponent *d = c->getDestination();

				signalPath->removeConnection(s->getUid(),
												s->getChannel(),
												d->getUid(),
												d->getChannel());
				removeChildComponent(c);
				delete c;

				//If it's a param connection, delete any MIDI or OSC mappings.
				if(c->getParameterConnection())
				{
					uint32 sourceId = s->getUid();
					uint32 destId = d->getUid();
					String tempstr = signalPath->getNodeForId(sourceId)->getProcessor()->getName();

					//It's a Midi connection, so delete any associated Midi
					//mappings for the destination plugin.
					if(tempstr == "Midi Input")
					{
						multimap<uint32, Mapping *>::iterator it;

						for(it=mappings.lower_bound(destId);
							it!=mappings.upper_bound(destId);)
						{
							MidiMapping *mapping = dynamic_cast<MidiMapping *>(it->second);

							if(mapping)
							{
								delete mapping;
								mappings.erase(it++);
							}
							else
								++it;
						}
					}
					else if(tempstr == "OSC Input")
					{
						multimap<uint32, Mapping *>::iterator it;

						for(it=mappings.lower_bound(destId);
							it!=mappings.upper_bound(destId);)
						{
							OscMapping *mapping = dynamic_cast<OscMapping *>(it->second);

							if(mapping)
							{
								delete it->second;
								mappings.erase(it++);
							}
							else
								++it;
						}
					}
				}
				sendChangeMessage();

				repaint();
			}
		}
	}
}

//------------------------------------------------------------------------------
void PluginField::enableMidiForNode(AudioProcessorGraph::Node *node, bool val)
{
	int i;
	bool connection = false;
	AudioProcessorGraph::Node *midiInput = 0;

	//Find the Midi Input node.
	for(i=0;i<signalPath->getNumFilters();++i)
	{
		midiInput = signalPath->getNode(i);

		if(midiInput->getProcessor()->getName() == "Midi Input")
			break;
	}
	//Just in case.
	if(midiInput)
	{
		if(midiInput->getProcessor()->getName() != "Midi Input")
			return;
	}
	else
		return;

	//Check if there's a connection.
	connection = (signalPath->getConnectionBetween(midiInput->nodeId,
												   AudioProcessorGraph::midiChannelIndex,
												   node->nodeId,
												   AudioProcessorGraph::midiChannelIndex) != 0);
	if(val)
	{
		//If there's a connection, remove it.
		signalPath->removeConnection(midiInput->nodeId,
									 AudioProcessorGraph::midiChannelIndex,
									 node->nodeId,
									 AudioProcessorGraph::midiChannelIndex);
	}
	else
	{
		//If there's not a connection, add it.
		signalPath->addConnection(midiInput->nodeId,
								  AudioProcessorGraph::midiChannelIndex,
								  node->nodeId,
								  AudioProcessorGraph::midiChannelIndex);
	}
}

//------------------------------------------------------------------------------
bool PluginField::getMidiEnabledForNode(AudioProcessorGraph::Node *node) const
{
	int i;
	AudioProcessorGraph::Node *midiInput = 0;

	//Find the Midi Input node.
	for(i=0;i<signalPath->getNumFilters();++i)
	{
		midiInput = signalPath->getNode(i);

		if(midiInput->getProcessor()->getName() == "Midi Input")
			break;
		else
			midiInput = 0;
	}

	if(!midiInput)
		return false;
	else
		return signalPath->getConnectionBetween(midiInput->nodeId,
												AudioProcessorGraph::midiChannelIndex,
												node->nodeId,
												AudioProcessorGraph::midiChannelIndex) != 0;
}

//------------------------------------------------------------------------------
void PluginField::addMapping(Mapping *mapping)
{
	mappings.insert(make_pair(mapping->getPluginId(), mapping));
	sendChangeMessage();
}

//------------------------------------------------------------------------------
void PluginField::removeMapping(Mapping *mapping)
{
	multimap<uint32, Mapping *>::iterator it;

	for(it=mappings.begin();it!=mappings.end();++it)
	{
		if(it->second == mapping)
		{
			delete it->second;
			mappings.erase(it);
			break;
		}
	}
	sendChangeMessage();
}

//------------------------------------------------------------------------------
Array<Mapping *> PluginField::getMappingsForPlugin(uint32 id)
{
	Array<Mapping *> retval;
	multimap<uint32, Mapping *>::iterator it;

	for(it=mappings.lower_bound(id);
		it!=mappings.upper_bound(id);
		++it)
	{
		retval.add(it->second);
	}

	return retval;
}

//------------------------------------------------------------------------------
void PluginField::socketDataArrived(char *data, int32 dataSize)
{
	if(OSC::Bundle::isBundle(data, dataSize))
	{
		OSC::Bundle bundle(data, dataSize);

		handleOscBundle(&bundle);
	}
	else if(OSC::Message::isMessage(data, dataSize))
	{
		OSC::Message message(data, dataSize);

		oscManager.messageReceived(&message);
	}
}

//------------------------------------------------------------------------------
XmlElement *PluginField::getXml() const
{
	int i;
	map<uint32, String>::const_iterator it2;
	multimap<uint32, Mapping *>::const_iterator it;
	XmlElement *retval = new XmlElement("Patch");
	XmlElement *mappingsXml = new XmlElement("Mappings");
	XmlElement *userNamesXml = new XmlElement("UserNames");

	//Update saved window positions.
	for(i=0;i<getNumChildComponents();++i)
	{
		PluginComponent *plugin = dynamic_cast<PluginComponent *>(getChildComponent(i));

		if(plugin)
			plugin->saveWindowState();
	}

	//Set the patch tempo.
	retval->setAttribute(L"tempo", tempo);

	//Add FilterGraph.
	retval->addChildElement(signalPath->createXml(oscManager));

	//Add Mappings.
	for(it=mappings.begin();it!=mappings.end();++it)
		mappingsXml->addChildElement(it->second->getXml());
	retval->addChildElement(mappingsXml);

	//Add user names.
	for(it2=userNames.begin();it2!=userNames.end();++it2)
	{
		XmlElement *nameXml = new XmlElement("Name");

		nameXml->setAttribute("id", (int)it2->first);
		nameXml->setAttribute("val", it2->second);

		userNamesXml->addChildElement(nameXml);
	}
	retval->addChildElement(userNamesXml);

	return retval;
}

#ifdef __APPLE__
struct NodeAndId
{
	AudioProcessorGraph::Node *node;
	uint32 id;
};
#endif

//------------------------------------------------------------------------------
void PluginField::loadFromXml(XmlElement *patch)
{
	int i, j;
	Array<uint32> paramConnections;

	//Delete all the filter and connection components.
	{
		//If we don't do this, the connections will try to contact their pins, which
		//may have already been deleted.
		for(i=(getNumChildComponents()-1);i>=0;--i)
		{
			PluginConnection *connection = dynamic_cast<PluginConnection *>(getChildComponent(i));
		
			if(connection)
			{
				removeChildComponent(connection);
				delete connection;
			}
		}
	}
	deleteAllChildren();
	repaint();

	//Wipe userNames.
	userNames.clear();

	//Clear and possibly load the signal path.
	clearMappings();
	if(patch)
	{
		tempo = patch->getDoubleAttribute(L"tempo", 120.0);

		signalPath->clear(false, false, false);
		signalPath->restoreFromXml(*(patch->getChildByName("FILTERGRAPH")),
								   oscManager);
	}
	else
		signalPath->clear(audioInputEnabled, midiInputEnabled);

	//Add the filter components.
	for(i=0;i<signalPath->getNumFilters();++i)
		addFilter(i, false);

	//Update any plugin names.
	{
		XmlElement *userNamesXml = patch->getChildByName("UserNames");

		if(userNamesXml)
		{
			forEachXmlChildElement(*userNamesXml, e)
			{
				if(e->hasTagName("Name"))
				{
					uint32 id = e->getIntAttribute("id");
					String name = e->getStringAttribute("val");

					for(i=0;i<getNumChildComponents();++i)
					{
						PluginComponent *pluginComp = dynamic_cast<PluginComponent *>(getChildComponent(i));

						if(pluginComp)
						{
							if(pluginComp->getNode()->nodeId == id)
							{
								pluginComp->setUserName(name);
								userNames[id] = name;
								break;
							}
						}
					}
				}
			}
		}
	}

	//Add the audio/midi connections.
	{
#ifndef __APPLE__ //Stupid gcc...
		struct NodeAndId
		{
			AudioProcessorGraph::Node *node;
			uint32 id;
		};
#endif
		Array<NodeAndId> tempNodes;
		
		//Stick all the nodes and their ids in an array.
		for(i=0;i<signalPath->getNumFilters();++i)
		{
			NodeAndId n;

			n.node = signalPath->getNode(i);
			n.id = n.node->nodeId;
			tempNodes.add(n);
		}

		//Add the audio/midi connections.
		for(i=0;i<signalPath->getNumConnections();++i)
		{
			AudioProcessorGraph::Node *sourceNode = 0;
			AudioProcessorGraph::Node *destNode = 0;
			PluginComponent *sourceComp = 0;
			PluginComponent *destComp = 0;
			PluginPinComponent *sourcePin;
			PluginPinComponent *destPin;
			const AudioProcessorGraph::Connection *connection = signalPath->getConnection(i);

			//Fill out sourceNode and destNode.
			for(j=0;j<tempNodes.size();++j)
			{
				if(tempNodes[j].id == connection->sourceNodeId)
					sourceNode = tempNodes[j].node;
				else if(tempNodes[j].id == connection->destNodeId)
					destNode = tempNodes[j].node;
			}

			if(!sourceNode || !destNode)
			{
				jassertfalse;
				continue;
			}
			else if(destNode->getProcessor()->getName() == "Midi Interceptor")
				continue;

			//Now get the source and destination components.
			for(j=0;j<getNumChildComponents();++j)
			{
				PluginComponent *pluginComp = dynamic_cast<PluginComponent *>(getChildComponent(j));

				if(pluginComp)
				{
					if(pluginComp->getNode() == sourceNode)
						sourceComp = pluginComp;
					else if(pluginComp->getNode() == destNode)
						destComp = pluginComp;
				}
			}

			if(!sourceComp || !destComp)
			{
				jassertfalse;
				continue;
			}

			if((connection->sourceChannelIndex == connection->destChannelIndex) &&
			   (connection->sourceChannelIndex == AudioProcessorGraph::midiChannelIndex))
			{
				sourcePin = sourceComp->getParamPin(0);
				destPin = destComp->getParamPin(0);

				paramConnections.add(connection->destNodeId);
			}
			else
			{
				sourcePin = sourceComp->getOutputPin(connection->sourceChannelIndex);
				destPin = destComp->getInputPin(connection->destChannelIndex);
			}

			if(!sourcePin || !destPin)
			{
				jassertfalse;
				continue;
			}

			addAndMakeVisible(new PluginConnection(sourcePin, destPin));
		}
	}

	//Add the mappings.
	{
		XmlElement *mappingsXml = patch->getChildByName("Mappings");

		if(mappingsXml)
		{
			forEachXmlChildElement(*mappingsXml, e)
			{
				if(e->hasTagName("MidiMapping"))
				{
					MidiMapping *mapping = new MidiMapping(&midiManager,
														   signalPath,
														   e);
					midiManager.registerMapping(mapping->getCc(), mapping);

					mappings.insert(make_pair(mapping->getPluginId(), mapping));
				}
				else if(e->hasTagName("OscMapping"))
				{
					OscMapping *mapping = new OscMapping(&oscManager,
														 signalPath,
														 e);
					oscManager.registerMapping(mapping->getAddress(), mapping);

					mappings.insert(make_pair(mapping->getPluginId(), mapping));
				}
			}
		}
	}

	//Connect the Midi Interceptor to the MidiMappingManager.
	if(midiInputEnabled)
	{
		MidiInterceptor *interceptor = 0;

		for(i=0;i<signalPath->getNumFilters();++i)
		{
			interceptor = dynamic_cast<MidiInterceptor *>(signalPath->getNode(i)->getProcessor());

			if(interceptor)
			{
				interceptor->setManager(&midiManager);
				break;
			}
		}
	}

	//Add in any parameter mapping connections.
	{
		multimap<uint32, Mapping *>::iterator it;
		PluginPinComponent *midiInput = 0;
		PluginPinComponent *oscInput = 0;

		//Get the Midi Input and OSC Input pins.
		for(i=0;i<getNumChildComponents();++i)
		{
			PluginComponent *comp = dynamic_cast<PluginComponent *>(getChildComponent(i));

			if(comp)
			{
				String tempstr = comp->getNode()->getProcessor()->getName();

				if(tempstr == "Midi Input")
					midiInput = comp->getParamPin(0);
				else if(tempstr == "OSC Input")
					oscInput = comp->getParamPin(0);
			}
		}

		/*jassert(midiInput);
		jassert(oscInput);*/

		if(midiInputEnabled && midiInput)
		{
			//Add a connection for each Midi mapping, checking that we don't already
			//have an identical one.
			for(it=mappings.begin();it!=mappings.end();++it)
			{
				MidiMapping *midiMapping = dynamic_cast<MidiMapping *>(it->second);

				if(midiMapping)
				{
					uint32 uid = midiMapping->getPluginId();

					if(!paramConnections.contains(uid))
					{
						//Find the PluginComponent matching this uid.
						for(i=0;i<getNumChildComponents();++i)
						{
							PluginComponent *comp = dynamic_cast<PluginComponent *>(getChildComponent(i));

							if(comp)
							{
								if(comp->getNode()->nodeId == uid)
								{
									PluginPinComponent *paramInput = 0;

									for(j=0;j<comp->getNumParamPins();++j)
									{
										if(!comp->getParamPin(j)->getDirection())
										{
											paramInput = comp->getParamPin(j);
											break;
										}
									}

									jassert(paramInput);

									addAndMakeVisible(new PluginConnection(midiInput,
																		   paramInput));
									paramConnections.add(uid);

									break;
								}
							}
						}
					}
				}
			}
		}

		if(oscInputEnabled && oscInput)
		{
			//Ditto for the osc mappings.
			paramConnections.clear();
			for(it=mappings.begin();it!=mappings.end();++it)
			{
				OscMapping *oscMapping = dynamic_cast<OscMapping *>(it->second);

				if(oscMapping)
				{
					uint32 uid = oscMapping->getPluginId();

					if(!paramConnections.contains(uid))
					{
						//Find the PluginComponent matching this uid.
						for(i=0;i<getNumChildComponents();++i)
						{
							PluginComponent *comp = dynamic_cast<PluginComponent *>(getChildComponent(i));

							if(comp)
							{
								if(comp->getNode()->nodeId == uid)
								{
									PluginPinComponent *paramInput = 0;

									for(j=0;j<comp->getNumParamPins();++j)
									{
										if(!comp->getParamPin(j)->getDirection())
										{
											paramInput = comp->getParamPin(j);
											break;
										}
									}

									jassert(paramInput);

									addAndMakeVisible(new PluginConnection(oscInput,
																		   paramInput));
									paramConnections.add(uid);

									break;
								}
							}
						}
					}
				}
			}
		}
	}

	//Make sure any disabled inputs/outputs don't get accidentally re-enabled.
	if(!audioInputEnabled)
		enableAudioInput(audioInputEnabled);
	if(!midiInputEnabled)
		enableMidiInput(midiInputEnabled);
	if(!oscInputEnabled)
		enableOscInput(oscInputEnabled);
	
	moveConnectionsBehind();
	repaint();
}

//------------------------------------------------------------------------------
void PluginField::clear()
{
	int i;

	//Delete all the filter and connection components.
	{
		//If we don't do this, the connections will try to contact their pins, which
		//may have already been deleted.
		for(i=(getNumChildComponents()-1);i>=0;--i)
		{
			PluginConnection *connection = dynamic_cast<PluginConnection *>(getChildComponent(i));
		
			if(connection)
			{
				removeChildComponent(connection);
				delete connection;
			}
		}
	}
	deleteAllChildren();
	repaint();

	//Wipe userNames.
	userNames.clear();

	//Clear any mappings.
	clearMappings();

	//Clear the signal path.
	signalPath->clear(audioInputEnabled, midiInputEnabled);

	//Add OSC input.
	if(oscInputEnabled)
	{
		OscInput p;
		PluginDescription desc;

        p.fillInPluginDescription(desc);

		signalPath->addFilter(&desc, 10, 215);
	}

	//Setup gui.
	for(i=0;i<signalPath->getNumFilters();++i)
		addFilter(i);

	//Add MidiInterceptor.
	if(midiInputEnabled)
	{
		MidiInterceptor p;
		PluginDescription desc;

        p.fillInPluginDescription(desc);

		signalPath->addFilter(&desc, 100, 100);

		//And connect it up to the midi input.
		{
			uint32 midiInput = 0;
			uint32 midiInterceptor = 0;

			for(i=0;i<signalPath->getNumFilters();++i)
			{
				if(signalPath->getNode(i)->getProcessor()->getName() == "Midi Input")
					midiInput = signalPath->getNode(i)->nodeId;
				else if(signalPath->getNode(i)->getProcessor()->getName() == "Midi Interceptor")
				{
					midiInterceptor = signalPath->getNode(i)->nodeId;
					dynamic_cast<MidiInterceptor *>(signalPath->getNode(i)->getProcessor())->setManager(&midiManager);
				}
			}
			signalPath->addConnection(midiInput,
									  AudioProcessorGraph::midiChannelIndex,
									  midiInterceptor,
									  AudioProcessorGraph::midiChannelIndex);
		}
	}

	repaint();
}

//------------------------------------------------------------------------------
void PluginField::clearDoubleClickMessage()
{
	displayDoubleClickMessage = false;
	repaint();
}

//------------------------------------------------------------------------------
void PluginField::clearMappings()
{
	multimap<uint32, Mapping *>::iterator it;

	for(it=mappings.begin();it!=mappings.end();++it)
		delete it->second;

	mappings.clear();
}

//------------------------------------------------------------------------------
void PluginField::handleOscBundle(OSC::Bundle *bundle)
{
	int i;

	for(i=0;i<bundle->getNumBundles();++i)
		handleOscBundle(bundle->getBundle(i));

	for(i=0;i<bundle->getNumMessages();++i)
		oscManager.messageReceived(bundle->getMessage(i));
		//handleOscMessage(bundle->getMessage(i));
}

//------------------------------------------------------------------------------
/*void PluginField::handleOscMessage(OSC::Message *message)
{
	oscManager.messageReceived(message);
}*/

//------------------------------------------------------------------------------
void PluginField::moveConnectionsBehind()
{
	int i;

	for(i=(getNumChildComponents()-1);i>=0;--i)
	{
		PluginConnection *connection = dynamic_cast<PluginConnection *>(getChildComponent(i));

		if(connection)
			connection->toBack();
		else
			getChildComponent(i)->toFront(false);
	}
}

//------------------------------------------------------------------------------
Component *PluginField::getPinAt(const int x, const int y)
{
	Point<int> pos(x, y);

	if (isVisible() &&
        ((unsigned int)x) < (unsigned int)getWidth() &&
        ((unsigned int)y) < (unsigned int)getHeight() &&
        hitTest(x, y))
    {
        for(int i = getNumChildComponents(); --i >= 0;)
        {
            Component* const child = getChildComponent(i);

			if(!dynamic_cast<PluginConnection *>(child))
			{
				Rectangle<int> updatedRect(child->getX()-16,
										   child->getY()-16,
										   child->getWidth()+32,
										   child->getHeight()+32);

				if(updatedRect.contains(pos))
				{
					if(pos.getX() < child->getX())
						pos.addXY((child->getX()-pos.getX()), 0);
					if(pos.getY() < child->getY())
						pos.addXY(0, (child->getY()-pos.getY()));

					if(pos.getX() > (child->getX()+child->getWidth()))
						pos.addXY(-(pos.getX()-(child->getX()+child->getWidth())), 0);
					if(pos.getY() > (child->getY()+child->getHeight()))
						pos.addXY(0, -(pos.getY()-(child->getY()+child->getHeight())));

					Component* const c = child->getComponentAt (pos.getX() - child->getX(),
																pos.getY() - child->getY());

					if (c != 0)
						return c;
				}
			}
        }

        return this;
    }

    return 0;
}

//------------------------------------------------------------------------------
void PluginField::connectAll(PluginConnection *connection)
{
	PluginComponent *source = dynamic_cast<PluginComponent *>(connection->getSource()->getParentComponent());
	PluginComponent *dest = dynamic_cast<PluginComponent *>(connection->getDestination()->getParentComponent());

	if(source && dest)
	{
		int left, right;
		int numSources = source->getNumOutputPins();
		int numDests = dest->getNumInputPins();
		PluginPinComponent *sourcePin;
		PluginPinComponent *destPin;

		for(left=0;left<numSources;++left)
		{
			if(source->getOutputPin(left) == connection->getSource())
			{
				++left;
				break;
			}
		}
		for(right=0;right<numDests;++right)
		{
			if(dest->getInputPin(right) == connection->getDestination())
			{
				++right;
				break;
			}
		}

		for(;(left<numSources) && (right<numDests);++left,++right)
		{
			sourcePin = source->getOutputPin(left);
			destPin = dest->getInputPin(right);
			signalPath->addConnection(sourcePin->getUid(),
									  sourcePin->getChannel(),
									  destPin->getUid(),
									  destPin->getChannel());
			addAndMakeVisible(new PluginConnection(sourcePin, destPin));
		}
	}
}
