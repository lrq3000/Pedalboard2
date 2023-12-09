/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#include "FilterGraph.h"
#include "InternalFilters.h"
#include "OscMappingManager.h"
#include "BypassableInstance.h"
#include "MidiMappingManager.h"
#include "PropertiesSingleton.h"
#include "AudioSingletons.h"

#include <iostream>

using namespace std;


//==============================================================================
FilterConnection::FilterConnection (FilterGraph& owner_)
    : owner (owner_)
{
}

FilterConnection::FilterConnection (const FilterConnection& other)
    : sourceFilterID (other.sourceFilterID),
      sourceChannel (other.sourceChannel),
      destFilterID (other.destFilterID),
      destChannel (other.destChannel),
      owner (other.owner)
{
}

FilterConnection::~FilterConnection()
{
}


//==============================================================================
const int FilterGraph::midiChannelNumber = 0x1000;

FilterGraph::FilterGraph()
    : FileBasedDocument (filenameSuffix,
                         filenameWildcard,
                         "Load a filter graph",
                         "Save a filter graph"),
      lastUID (0)
{
    InternalPluginFormat internalFormat;
	bool audioInput = PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("AudioInput", true);
	bool midiInput = PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("MidiInput", true);

	if(audioInput)
	{
		addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::audioInputFilter),
				   10.0f, 10.0f);
	}

	if(midiInput)
	{
		addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::midiInputFilter),
				   10.0f, 120.0f);
	}

	addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::audioOutputFilter),
				892.0f, 10.0f);

    setChangedFlag (false);
}

FilterGraph::~FilterGraph()
{
    graph.clear();
}

uint32 FilterGraph::getNextUID() throw()
{
    return ++lastUID;
}

//==============================================================================
int FilterGraph::getNumFilters() const throw()
{
    return graph.getNumNodes();
}

const AudioProcessorGraph::Node::Ptr FilterGraph::getNode (const int index) const throw()
{
    return graph.getNode (index);
}

const AudioProcessorGraph::Node::Ptr FilterGraph::getNodeForId (const uint32 uid) const throw()
{
    return graph.getNodeForId (uid);
}

void FilterGraph::addFilter (const PluginDescription* desc, double x, double y)
{
    if (desc != 0)
    {
        String errorMessage;
		AudioPluginInstance* instance;
		AudioPluginInstance *tempInstance = AudioPluginFormatManagerSingleton::getInstance().createPluginInstance (*desc, errorMessage);

		if(dynamic_cast<AudioProcessorGraph::AudioGraphIOProcessor *>(tempInstance) ||
		   dynamic_cast<MidiInterceptor *>(tempInstance) ||
		   dynamic_cast<OscInput *>(tempInstance))
			instance = tempInstance;
		else
			instance = new BypassableInstance(tempInstance);

        AudioProcessorGraph::Node* node = 0;

        if (instance != 0)
            node = graph.addNode (instance);

        if (node != 0)
        {
            node->properties.set ("x", x);
            node->properties.set ("y", y);
            changed();
        }
        else
        {
            AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                         TRANS("Couldn't create filter"),
                                         errorMessage);
        }
    }
}

void FilterGraph::addFilter(AudioPluginInstance *plugin, double x, double y)
{
	if(plugin != 0)
    {
        String errorMessage;

		BypassableInstance* instance = new BypassableInstance(plugin);

        AudioProcessorGraph::Node* node = 0;

        if(plugin != 0)
            node = graph.addNode(instance);

        if(node != 0)
        {
            node->properties.set("x", x);
            node->properties.set("y", y);
            changed();
        }
        else
        {
            AlertWindow::showMessageBox(AlertWindow::WarningIcon,
                                        TRANS("Couldn't create filter"),
                                        errorMessage);
        }
    }
}

void FilterGraph::removeFilter (const uint32 id)
{
    //PluginWindow::closeCurrentlyOpenWindowsFor (id);

    if (graph.removeNode (id))
        changed();
}

void FilterGraph::disconnectFilter (const uint32 id)
{
    if (graph.disconnectNode (id))
        changed();
}

void FilterGraph::removeIllegalConnections()
{
    if (graph.removeIllegalConnections())
        changed();
}

void FilterGraph::setNodePosition (const int nodeId, double x, double y)
{
    const AudioProcessorGraph::Node::Ptr n (graph.getNodeForId (nodeId));

    if (n != 0)
    {
        n->properties.set ("x", jlimit (0.0, 1.0, x));
        n->properties.set ("y", jlimit (0.0, 1.0, y));
    }
}

void FilterGraph::getNodePosition (const int nodeId, double& x, double& y) const
{
    x = y = 0;

    const AudioProcessorGraph::Node::Ptr n (graph.getNodeForId (nodeId));

    if (n != 0)
    {
        //x = n->properties.getDoubleValue ("x");
        x = n->properties.getWithDefault ("x", 0.0);
        //y = n->properties.getDoubleValue ("y");
        y = n->properties.getWithDefault ("y", 0.0);
    }
}

//==============================================================================
int FilterGraph::getNumConnections() const throw()
{
    return graph.getNumConnections();
}

const AudioProcessorGraph::Connection* FilterGraph::getConnection (const int index) const throw()
{
    return graph.getConnection (index);
}

const AudioProcessorGraph::Connection* FilterGraph::getConnectionBetween (uint32 sourceFilterUID, int sourceFilterChannel,
                                                                          uint32 destFilterUID, int destFilterChannel) const throw()
{
    return graph.getConnectionBetween (sourceFilterUID, sourceFilterChannel,
                                       destFilterUID, destFilterChannel);
}

bool FilterGraph::canConnect (uint32 sourceFilterUID, int sourceFilterChannel,
                              uint32 destFilterUID, int destFilterChannel) const throw()
{
    return graph.canConnect (sourceFilterUID, sourceFilterChannel,
                             destFilterUID, destFilterChannel);
}

bool FilterGraph::addConnection (uint32 sourceFilterUID, int sourceFilterChannel,
                                 uint32 destFilterUID, int destFilterChannel)
{
    const bool result = graph.addConnection (sourceFilterUID, sourceFilterChannel,
                                             destFilterUID, destFilterChannel);

    if (result)
        changed();

    return result;
}

void FilterGraph::removeConnection (const int index)
{
    graph.removeConnection (index);
    changed();
}

void FilterGraph::removeConnection (uint32 sourceFilterUID, int sourceFilterChannel,
                                    uint32 destFilterUID, int destFilterChannel)
{
    if (graph.removeConnection (sourceFilterUID, sourceFilterChannel,
                                destFilterUID, destFilterChannel))
        changed();
}

void FilterGraph::clear(bool addAudioIn, bool addMidiIn, bool addAudioOut)
{
    InternalPluginFormat internalFormat;

    //PluginWindow::closeAllCurrentlyOpenWindows();

    graph.clear();

	if(addAudioIn)
	{
		addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::audioInputFilter),
				   10.0f, 10.0f);
	}

	if(addMidiIn)
	{
		addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::midiInputFilter),
				   10.0f, 120.0f);
	}

	if(addAudioOut)
	{
		addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::audioOutputFilter),
				   892.0f, 10.0f);
	}

    changed();
}

//==============================================================================
String FilterGraph::getDocumentTitle()
{
    if (! getFile().exists())
        return "Unnamed";

    return getFile().getFileNameWithoutExtension();
}

Result FilterGraph::loadDocument (const File& file)
{
    /*XmlDocument doc (file);
    XmlElement* xml = doc.getDocumentElement();

    if (xml == 0 || ! xml->hasTagName (L"FILTERGRAPH"))
    {
        delete xml;
		return Result::fail("Not a valid filter graph file");
    }

    restoreFromXml (*xml);
    delete xml;*/

	jassert(false);
	return Result::ok();
}

Result FilterGraph::saveDocument (const File& file)
{
    /*XmlElement* xml = createXml();

    String error;

    if (! xml->writeToFile (file, String::empty))
		return Result::fail("Couldn't write to the file");

    delete xml;*/
	jassert(false); //Pretty sure this is never used, but just to be sure...
	return Result::ok();
}

File FilterGraph::getLastDocumentOpened()
{
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (PropertiesSingleton::getInstance().getUserSettings()->getValue ("recentFilterGraphFiles"));

    return recentFiles.getFile (0);
}

void FilterGraph::setLastDocumentOpened (const File& file)
{
    RecentlyOpenedFilesList recentFiles;
    recentFiles.restoreFromString (PropertiesSingleton::getInstance().getUserSettings()->getValue ("recentFilterGraphFiles"));

    recentFiles.addFile (file);

    PropertiesSingleton::getInstance().getUserSettings()->setValue ("recentFilterGraphFiles", recentFiles.toString());
}

//==============================================================================
static XmlElement* createNodeXml(AudioProcessorGraph::Node* const node,
								 const OscMappingManager& oscManager) throw()
{
    AudioPluginInstance *plugin = dynamic_cast<AudioPluginInstance *>(node->getProcessor());
	BypassableInstance *bypassable = dynamic_cast<BypassableInstance *>(plugin);

    if (plugin == 0)
    {
        jassertfalse
        return 0;
    }

    XmlElement* e = new XmlElement ("FILTER");
    e->setAttribute(L"uid", (int) node->nodeId);
    /*e->setAttribute(L"x", node->properties.getDoubleValue("x"));
    e->setAttribute(L"y", node->properties.getDoubleValue("y"));
    e->setAttribute(L"uiLastX", node->properties.getIntValue("uiLastX"));
    e->setAttribute(L"uiLastY", node->properties.getIntValue("uiLastY"));*/
    e->setAttribute(L"x", (double)node->properties.getWithDefault("x", 0.0));
    e->setAttribute(L"y", (double)node->properties.getWithDefault("y", 0.0));
    e->setAttribute(L"uiLastX", (int)node->properties.getWithDefault("uiLastX", 0.0));
    e->setAttribute(L"uiLastY", (int)node->properties.getWithDefault("uiLastY", 0.0));
    e->setAttribute(L"windowOpen", (bool)node->properties.getWithDefault("windowOpen", false));
	e->setAttribute(L"program", node->getProcessor()->getCurrentProgram());
	if(bypassable)
	{
		e->setAttribute(L"oscMIDIAddress", oscManager.getMIDIProcessorAddress(bypassable));
		e->setAttribute(L"MIDIChannel", bypassable->getMIDIChannel());
	}

    PluginDescription pd;

    plugin->fillInPluginDescription (pd);

    e->addChildElement (pd.createXml());

    XmlElement* state = new XmlElement ("STATE");

    MemoryBlock m;
    node->getProcessor()->getStateInformation (m);
    state->addTextElement (m.toBase64Encoding());
    e->addChildElement (state);

    return e;
}

void FilterGraph::createNodeFromXml(const XmlElement& xml,
									OscMappingManager& oscManager)
{
	String midiAddress;
    String errorMessage;
    PluginDescription pd;
	AudioPluginInstance *instance = 0;
	BypassableInstance *bypassable = 0;
	AudioPluginInstance* tempInstance = 0;

    forEachXmlChildElement (xml, e)
    {
        if (pd.loadFromXml (*e))
            break;
    }

    tempInstance = AudioPluginFormatManagerSingleton::getInstance().createPluginInstance(pd, errorMessage);

	if(dynamic_cast<AudioProcessorGraph::AudioGraphIOProcessor *>(tempInstance) ||
		dynamic_cast<MidiInterceptor *>(tempInstance) ||
		dynamic_cast<OscInput *>(tempInstance))
		instance = tempInstance;
	else
	{
		bypassable = new BypassableInstance(tempInstance);
		instance = bypassable;
	}

    if (instance == 0)
    {
        // xxx handle ins + outs
    }

    if (instance == 0)
        return;

    AudioProcessorGraph::Node::Ptr node (graph.addNode (instance, xml.getIntAttribute (L"uid")));

    const XmlElement* const state = xml.getChildByName (L"STATE");

    if (state != 0)
    {
        MemoryBlock m;
        m.fromBase64Encoding (state->getAllSubText());

        node->getProcessor()->setStateInformation (m.getData(), m.getSize());
    }

    node->properties.set ("x", xml.getDoubleAttribute (L"x"));
    node->properties.set ("y", xml.getDoubleAttribute (L"y"));
    node->properties.set ("uiLastX", xml.getIntAttribute (L"uiLastX"));
    node->properties.set ("uiLastY", xml.getIntAttribute (L"uiLastY"));
    node->properties.set ("windowOpen", xml.getIntAttribute (L"windowOpen"));

	midiAddress = xml.getStringAttribute("oscMIDIAddress");
	if(bypassable)
	{
		if(!midiAddress.isEmpty())
			oscManager.registerMIDIProcessor(midiAddress, bypassable);

		bypassable->setMIDIChannel(xml.getIntAttribute("MIDIChannel"));
	}

	node->getProcessor()->setCurrentProgram(xml.getIntAttribute(L"program"));
}

XmlElement* FilterGraph::createXml(const OscMappingManager& oscManager) const
{
    XmlElement* xml = new XmlElement("FILTERGRAPH");

    int i;
    for(i=0;i<graph.getNumNodes();++i)
        xml->addChildElement(createNodeXml(graph.getNode(i), oscManager));

    for (i = 0; i < graph.getNumConnections(); ++i)
    {
        const AudioProcessorGraph::Connection* const fc = graph.getConnection(i);

        XmlElement* e = new XmlElement ("CONNECTION");

        e->setAttribute (L"srcFilter", (int) fc->sourceNodeId);
        e->setAttribute (L"srcChannel", fc->sourceChannelIndex);
        e->setAttribute (L"dstFilter", (int) fc->destNodeId);
        e->setAttribute (L"dstChannel", fc->destChannelIndex);

        xml->addChildElement (e);
    }

    return xml;
}

void FilterGraph::restoreFromXml(const XmlElement& xml,
								 OscMappingManager& oscManager)
{
    clear(false, false, false);

    forEachXmlChildElementWithTagName (xml, e, L"FILTER")
    {
        createNodeFromXml (*e, oscManager);
        changed();
    }

    forEachXmlChildElementWithTagName (xml, e2, L"CONNECTION")
    {
        addConnection ((uint32) e2->getIntAttribute (L"srcFilter"),
                       e2->getIntAttribute (L"srcChannel"),
                       (uint32) e2->getIntAttribute (L"dstFilter"),
                       e2->getIntAttribute (L"dstChannel"));
    }

    graph.removeIllegalConnections();
}
