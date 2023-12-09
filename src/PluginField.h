//	PluginField.h - Field representing the signal path through the app.
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

#ifndef PLUGINFIELD_H_
#define PLUGINFIELD_H_

#include "../JuceLibraryCode/JuceHeader.h"
#include "MidiMappingManager.h"
#include "OscMappingManager.h"

#include <map>

class Mapping;
class FilterGraph;
class PluginConnection;
class PluginPinComponent;

namespace OSC
{
class Message;
class Bundle;
};

///	Field representing the signal path through the app.
/*!
	It's a ChangeBroadcaster, and broadcasts a change message whenever
	something changes (i.e. so MainPanel can call FileBaseDocument::changed()
	accordingly).
 */
class PluginField : public Component,
					public ChangeBroadcaster,
					public ChangeListener,
					public FileDragAndDropTarget,
					public AudioPlayHead,
					public Timer
{
  public:
	///	Constructor.
	PluginField(FilterGraph *filterGraph,
				KnownPluginList *list,
				ApplicationCommandManager *appManager);
	///	Destructor.
	~PluginField();

	///	Fills in the background.
	void paint(Graphics& g);

	///	Used to add plugins with a double-click.
	void mouseDown(const MouseEvent& e);

	///	So we're informed when PluginComponents are moved around, and can update our bounds accordingly.
	void changeListenerCallback(ChangeBroadcaster *source);

	///	Used to periodically update PluginComponents etc.
	void timerCallback();

	///	Used to accept dragged plugin files.
	bool isInterestedInFileDrag(const StringArray& files);
	///	Used to accept dragged plugin files.
	void filesDropped(const StringArray& files, int x, int y);

	///	So plugins get info about the tempo etc.
	bool getCurrentPosition(CurrentPositionInfo &result);

	///	Enables/disables the audio input.
	void enableAudioInput(bool val);
	///	Enables/disables the MIDI input.
	void enableMidiInput(bool val);
	///	Enables/disables the OSC input.
	void enableOscInput(bool val);

	///	Sets whether to automatically open the mappings window or not.
	void setAutoMappingsWindow(bool val);

	///	Sets the current tempo.
	void setTempo(double val);
	///	Returns the current tempo.
	double getTempo() const {return tempo;};

	///	Adds a filter to the field.
	/*!
		\param index The index of the filter in the FilterGraph.
	 */
	void addFilter(int index, bool broadcastChangeMessage = true);
	///	Deletes a filter from the field.
	void deleteFilter(AudioProcessorGraph::Node *node);

	///	Lets us know the user's edited a processor name.
	void updateProcessorName(uint32 id, const String& val);

	///	Returns the FilterGraph.
	FilterGraph *getFilterGraph() {return signalPath;};

	///	Adds a connection to the field.
	void addConnection(PluginPinComponent *source, bool connectAll);
	///	Drags a connection between plugins.
	void dragConnection(int x, int y);
	///	Makes a connection between two plugins, or deletes it.
	void releaseConnection(int x, int y);
	///	Deletes any selected connections.
	void deleteConnection();

	///	Enables/disables the Midi Input->plugin connection for the passed-in Node.
	void enableMidiForNode(AudioProcessorGraph::Node *node, bool val);
	///	Returns true if the Node has a Midi Input->plugin connection.
	bool getMidiEnabledForNode(AudioProcessorGraph::Node *node) const;

	///	Adds a Mapping.
	void addMapping(Mapping *mapping);
	///	Removes a Mapping.
	/*!
		Also deletes mapping.
	 */
	void removeMapping(Mapping *mapping);
	///	Returns an Array of all the Mappings for the passed-in plugin id.
	Array<Mapping *> getMappingsForPlugin(uint32 id);

	///	Returns the MidiMappingManager;
	MidiMappingManager *getMidiManager() {return &midiManager;};
	///	Returns the OscMappingManager;
	OscMappingManager *getOscManager() {return &oscManager;};

	///	Called when the app receives data on its OSC port.
	void socketDataArrived(char *data, int32 dataSize);

	///	Returns the XML for the current patch.
	XmlElement *getXml() const;
	///	Loads a new patch from an XmlElement.
	void loadFromXml(XmlElement *patch);

	///	Clears the field.
	void clear();

	///	Clears the 'double-click...' message.
	void clearDoubleClickMessage();

	juce_UseDebuggingNewOperator
  private:
	///	Helper method. Clears mappings.
	void clearMappings();
	///	Helper method. Handles a single OSC bundle.
	void handleOscBundle(OSC::Bundle *bundle);
	///	Helper method. Handles a single OSC message.
	//void handleOscMessage(OSC::Message *message);

	///	Helper method. Makes sure PluginConnections are always behing PluginComponents.
	void moveConnectionsBehind();
	///	Used to find whether a Plugin Connection has been dragged to a PluginPinComponent.
	Component *getPinAt(const int x, const int y);

	///	Helper method. Connects all outputs from a source to all inputs of a destination.
	void connectAll(PluginConnection *connection);

	///	The signal path itself.
	FilterGraph *signalPath;
	///	The list of possible plugins.
	KnownPluginList *pluginList;
	///	The mappings for this field.
	std::multimap<uint32, Mapping *> mappings;
	///	The manager for any MidiMappings.
	MidiMappingManager midiManager;
	///	The manager for any OscMappings.
	OscMappingManager oscManager;

	///	Any user-edited processor names.
	std::map<uint32, String> userNames;

	///	Temporary PluginConnection for dragging.
	PluginConnection *draggingConnection;

	///	The current tempo in bpm.
	double tempo;

	///	Whether to display the 'double-click...' message or not.
	bool displayDoubleClickMessage;

	///	Whether the audio input is enabled or not.
	bool audioInputEnabled;
	///	Whether the MIDI input is enabled or not.
	bool midiInputEnabled;
	///	Whether the OSC input is enabled or not.
	bool oscInputEnabled;

	///	Whether to open the mappings window when a param connection is made.
	bool autoMappingsWindow;
};

#endif
