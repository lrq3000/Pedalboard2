//	PluginComponent.h - Component representing a plugin/filter in the
//						PluginField.
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

#ifndef PLUGINCOMPONENT_H_
#define PLUGINCOMPONENT_H_

#include "c0xHeaders.h"

#include "../JuceLibraryCode/JuceHeader.h"

#include <map>

class PresetBar;
class PluginEditorWindow;
class PluginPinComponent;

//------------------------------------------------------------------------------
///	Component representing a plugin/filter in the PluginField.
class PluginComponent : public Component,
					    public ChangeBroadcaster,
						public ButtonListener,
						public Label::Listener
{
  public:
	///	Constructor.
	PluginComponent(AudioProcessorGraph::Node *n);
	///	Destructor.
	~PluginComponent();

	///	Draws the component.
	void paint(Graphics& g);

	///	Used to redraw any connections to this component's pins.
	void moved();

	///	Used to update the bypass button if it needs it.
	void timerUpdate();

	///	Used to move the component about the PluginField.
	void mouseDown(const MouseEvent& e);
	///	Used to move the component about the PluginField.
	void mouseDrag(const MouseEvent& e);
	///	Used to move the component about the PluginField.
	void mouseUp(const MouseEvent& e);

	///	Used to open the plugin's editor/delete the plugin.
	void buttonClicked(Button *button);

	///	Called when the user changes the plugin's name.
	void labelTextChanged(Label *label);
	///	Sets the plugin's name.
	void setUserName(const String& val);

	///	Sets the plugin window for this plugin.
	void setWindow(PluginEditorWindow *val);
	///	Saves the plugin window's state.
	void saveWindowState();

	///	Opens the mappings window.
	void openMappingsWindow();

	///	Returns the node attached to this component.
	AudioProcessorGraph::Node *getNode() const {return node;};
	///	Returns the plugin's user name.
	const String& getUserName() const {return pluginName;};

	///	Used to cache the current program for this plugin.
	/*!
		Because of our preset management system, we need to be able to restore
		presets after the user has loaded one of their own presets. Then when
		the user switches back to one of the plugin's own presets, we can load
		it back, rather than being stuck with the preset we just loaded from
		our user preset dir.
	 */
	void cacheCurrentPreset();
	///	Returns a cached MemoryBlock for the indexed program.
	/*!
		\param index The indexed program to restore.
		\param memBlock The MemoryBlock to write the cached program into. Will
		be empty if we didn't have the indexed program cached.

		After retrieving the cached program, it will get deleted from the cache.
	 */
	void getCachedPreset(int index, MemoryBlock& memBlock);

	///	Returns the position for the indexed input pin.
	PluginPinComponent *getInputPin(int index) {return inputPins[index];};
	///	Returns the position for the indexed output pin.
	PluginPinComponent *getOutputPin(int index) {return outputPins[index];};
	///	Returns the position for the indexed parameter pin.
	PluginPinComponent *getParamPin(int index) {return paramPins[index];};

	///	Returns the number of input pins.
	int getNumInputPins() const {return inputPins.size();};
	///	Returns the number of output pins.
	int getNumOutputPins() const {return outputPins.size();};
	///	Returns the number of param pins.
	int getNumParamPins() const {return paramPins.size();};

	juce_UseDebuggingNewOperator
  private:
	///	Helper method to determine what size the component should be.
	/*!
		This is determined by the length of the component's name, and the length
		of its input and output channel names.
	 */
	void determineSize(bool onlyUpdateWidth = false);
	///	Helper method to create the necessary pins for the plugin.
	void createPins();

	///	The title label.
	Label *titleLabel;
	///	Button to open the plugin's editor.
	TextButton *editButton;
	///	Button to open the plugin's mappings editor.
	TextButton *mappingsButton;
	///	Button to bypass the plugin
	DrawableButton *bypassButton;
	///	Button to delete the plugin.
	DrawableButton *deleteButton;

	///	The plugin this component represents.
	//AudioProcessor *plugin;
	AudioProcessorGraph::Node *node;
	///	Window for the plugin.
	PluginEditorWindow *pluginWindow;

	///	The name of the plugin.
	String pluginName;

	///	Used to draw the name of the plugin.
	GlyphArrangement nameText;
	///	Used to draw the names of the input channels.
	OwnedArray<GlyphArrangement> inputText;
	///	Used to draw the names of the output channels.
	OwnedArray<GlyphArrangement> outputText;

	///	Keeps track of the positions of the input pins.
	Array<PluginPinComponent *> inputPins;
	///	Keeps track of the positions of the output pins.
	Array<PluginPinComponent *> outputPins;
	///	Keeps track of the positions of the parameter/midi pins.
	Array<PluginPinComponent *> paramPins;

	///	Used to move the component about the plugin field.
	bool beingDragged;
	///	Used to move the component about the plugin field.
	int dragX;
	///	Used to move the component about the plugin field.
	int dragY;

	///	Any cached presets we have.
	std::map<int, shared_ptr<MemoryBlock> > cachedPresets;
};

//------------------------------------------------------------------------------
///	Component representing an input or output pin for a plugin.
class PluginPinComponent : public Component
{
  public:
	///	Constructor.
	/*!
		\param dir Whether this is an input (false) or output (true) pin.
		\param id The uid of the plugin this pin is for.
		\param chan The input/output channel of the plugin this pin represents.
		\param param Whether this is an audio or parameter(/midi) pin.
	 */
	PluginPinComponent(bool dir, uint32 id, int chan, bool param);
	///	Destructor.
	~PluginPinComponent();

	///	Draws the pin.
	void paint(Graphics& g);

	///	Used to create a connection to another pin.
	void mouseDown(const MouseEvent& e);
	///	Used to drag a connection to another pin.
	void mouseDrag(const MouseEvent& e);
	///	Used to make a connection or delete the connection.
	void mouseUp(const MouseEvent& e);

	///	Returns this pin's direction.
	bool getDirection() const {return direction;};
	///	Returns this pin's plugin's uid.
	uint32 getUid() const {return uid;};
	///	Returns which channel this pin represents on the associated plugin.
	int getChannel() const {return channel;};
	///	Returns whether this is an audio or parameter(/midi) pin.
	bool getParameterPin() const {return parameterPin;};

	juce_UseDebuggingNewOperator
  private:
	///	Whether it's an input or an output pin.
	bool direction;
	///	Id of the plugin this pin is for.
	uint32 uid;
	///	Which channel of the plugin this pin represents.
	int channel;
	///	Whether this is an audio or parameter(/midi) pin.
	bool parameterPin;
};

//------------------------------------------------------------------------------
///	Window displaying a plugin's editor.
class PluginEditorWindow : public DocumentWindow
{
  public:
	///	Constructor.
	PluginEditorWindow(AudioProcessorEditor *editor, PluginComponent *c);
	///	Destructor.
	~PluginEditorWindow();

	///	Used to close the window.
	void closeButtonPressed();

	juce_UseDebuggingNewOperator
  private:
	///	The 'parent' PluginComponent.
	PluginComponent *component;

	///	Wrapper Component holding a PresetBar and an AudioProcessorEditor.
	class EditorWrapper : public Component
	{
	  public:
		///	Constructor.
		EditorWrapper(AudioProcessorEditor *ed, PluginComponent *comp);
		///	Destructor.
		~EditorWrapper();

		///	So we can resize our child components.
		void resized();

		///	So we know when the editor's bounds have changed.
		void childBoundsChanged(Component *child);
	  private:
		///	Our copy of the preset bar.
		PresetBar *presetBar;
		///	Our copy of the editor component.
		AudioProcessorEditor *editor;

		///	The 'parent' PluginComponent.
		PluginComponent *component;
	};
};

//------------------------------------------------------------------------------
///	Component representing a connection between two plugins.
class PluginConnection : public Component,
						 public ChangeListener
{
  public:
	///	Constructor.
	/*!
		\param The source plugin pin.
		\param The destination plugin pin.
		\param If true, the user was holding shift when they created this connection;
		when they release the mouse button, connections should be made for all the
		outputs following this one in its source, and all the inputs following it in
		its destination.
	 */
	PluginConnection(PluginPinComponent *s, PluginPinComponent *d = 0, bool allOutputs = false);
	///	Destructor.
	~PluginConnection();

	///	Draws the connection.
	void paint(Graphics& g);

	///	Used to select the connection (e.g. to delete it).
	void mouseDown(const MouseEvent& e);
	///	Used to ensure clicks only count when the mouse is over the actual curve.
	bool hitTest(int x, int y);

	///	Updates the component when the source or destination is moved.
	void changeListenerCallback(ChangeBroadcaster *changedObject);

	///	Used to drag the connection to another pin.
	void drag(int x, int y);
	///	Sets the destination pin of this connection.
	void setDestination(PluginPinComponent *d);
	///	Sets whether this connection is representing all outputs or not.
	void setRepresentsAllOutputs(bool val);

	///	Returns this connection's source pin.
	const PluginPinComponent *getSource() const {return source;};
	///	Returns this connection's destination pin.
	const PluginPinComponent *getDestination() const {return destination;};

	///	Returns whether this connection is selected or not.
	bool getSelected() const {return selected;};
	///	Returns true if the connection is a parameter(/midi) connection or not.
	bool getParameterConnection() const {return paramCon;};
	///	Returns true if the user was holding shift when they created this connection.
	bool getRepresentsAllOutputs() const {return representsAllOutputs;};

	juce_UseDebuggingNewOperator
  private:
	///	Helper method to help determine the component's size/bezier curve.
	/*!
		sX etc. are coordinates relative to the parent component.
	 */
	void getPoints(int& sX, int& sY, int& dX, int& dY);
	///	Helper method to work out (and update) the component's bounds.
	void updateBounds(int sX, int sY, int dX, int dY);

	///	The source plugin pin.
	PluginPinComponent *source;
	///	The destination plugin pin.
	PluginPinComponent *destination;

	///	The bezier curve that the connection is drawn as.
	Path drawnCurve;

	///	Whether the connection is selected or not.
	bool selected;
	///	Whether this connection is a parameter(/midi) connection or not.
	bool paramCon;
	///	Whether this connection is intended to represent all outputs on its source component or not.
	bool representsAllOutputs;
};

#endif
