//	PluginComponent.cpp - Component representing a plugin/filter in the
//						  PluginField.
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

#include "PedalboardProcessors.h"
#include "PropertiesSingleton.h"
#include "BypassableInstance.h"
#include "JuceHelperStuff.h"
#include "PluginComponent.h"
#include "MappingsDialog.h"
#include "ColourScheme.h"
#include "PluginField.h"
#include "PresetBar.h"
#include "Vectors.h"
#include "Images.h"

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class NiallsGenericEditor : public GenericAudioProcessorEditor
{
  public:
	///	Constructor.
	NiallsGenericEditor(AudioProcessor * const owner):
	GenericAudioProcessorEditor(owner)
	{
		
	};

	///	Fill the background the correct colour.
	void paint(Graphics& g)
	{
		g.fillAll(ColourScheme::getInstance().colours[L"Window Background"]);
	};
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PluginComponent::PluginComponent(AudioProcessorGraph::Node *n):
Component(),
//plugin(p),
editButton(0),
mappingsButton(0),
bypassButton(0),
deleteButton(0),
node(n),
pluginWindow(0),
beingDragged(false),
dragX(0),
dragY(0)
{
	BypassableInstance *bypassable = dynamic_cast<BypassableInstance *>(node->getProcessor());
	PedalboardProcessor *proc = 0;

	if(bypassable)
		proc = dynamic_cast<PedalboardProcessor *>(bypassable->getPlugin());

	pluginName = node->getProcessor()->getName();

	determineSize();

	titleLabel = new Label("titleLabel", pluginName);
	titleLabel->setBounds(5, 0, getWidth()-10, 20);
	titleLabel->setInterceptsMouseClicks(false, false);
	titleLabel->setFont(Font(14.0f, Font::bold));
	titleLabel->addListener(this);
	addAndMakeVisible(titleLabel);

	if((pluginName != "Audio Input") &&
	   (pluginName != "Midi Input") &&
	   (pluginName != "Audio Output") &&
	   (pluginName != "OSC Input"))
	{
		ScopedPointer<Drawable> closeUp(JuceHelperStuff::loadSVGFromMemory(Vectors::closefilterbutton_svg,
																		   Vectors::closefilterbutton_svgSize));
		ScopedPointer<Drawable> closeOver(JuceHelperStuff::loadSVGFromMemory(Vectors::closefilterbuttonover_svg,
																			 Vectors::closefilterbuttonover_svgSize));
		ScopedPointer<Drawable> closeDown(JuceHelperStuff::loadSVGFromMemory(Vectors::closefilterbuttondown_svg,
																			 Vectors::closefilterbuttondown_svgSize));
		ScopedPointer<Drawable> bypassOff(JuceHelperStuff::loadSVGFromMemory(Vectors::bypassbuttonoff_svg,
																		     Vectors::bypassbuttonoff_svgSize));
		ScopedPointer<Drawable> bypassOn(JuceHelperStuff::loadSVGFromMemory(Vectors::bypassbuttonon_svg,
																		    Vectors::bypassbuttonon_svgSize));

		//So the audio I/O etc. don't get their titles squeezed by the
		//non-existent close button.
		titleLabel->setBounds(5, 0, getWidth()-17, 20);

		editButton = new TextButton("e", "Open plugin editor");
		editButton->setBounds(10, getHeight()-30, 20, 20);
		editButton->addListener(this);
		addAndMakeVisible(editButton);

		mappingsButton = new TextButton("m", "Open mappings editor");
		mappingsButton->setBounds(32, getHeight()-30, 24, 20);
		mappingsButton->addListener(this);
		addAndMakeVisible(mappingsButton);

		bypassButton = new DrawableButton("BypassFilterButton",
										  DrawableButton::ImageOnButtonBackground);
		bypassButton->setImages(bypassOff, nullptr, nullptr, nullptr, bypassOn);
		bypassButton->setClickingTogglesState(true);
		bypassButton->setBounds(getWidth()-30, getHeight()-30, 20, 20);
		bypassButton->addListener(this);
		addAndMakeVisible(bypassButton);

		deleteButton = new DrawableButton("DeleteFilterButton",
										  DrawableButton::ImageRaw);
		deleteButton->setImages(closeUp, closeOver, closeDown);
		deleteButton->setEdgeIndent(0);
		deleteButton->setBounds(getWidth()-17, 5, 12, 12);
		deleteButton->addListener(this);
		addAndMakeVisible(deleteButton);
	}

	if(proc)
	{
		int tempint;
		Component *comp = proc->getControls();
		Point<int> compSize = proc->getSize();

		tempint = (getWidth()/2)-(compSize.getX()/2);
		comp->setTopLeftPosition(tempint, 24);
		addAndMakeVisible(comp);
	}

	createPins();

	if(node->properties.getWithDefault("windowOpen", false))
		buttonClicked(editButton);
}

//------------------------------------------------------------------------------
PluginComponent::~PluginComponent()
{
	deleteAllChildren();
	if(pluginWindow)
		delete pluginWindow;
}

//------------------------------------------------------------------------------
void PluginComponent::paint(Graphics& g)
{
	int i;
	map<String, Colour>& colours = ColourScheme::getInstance().colours;

	//Draw slight black outline.
	g.setColour(Colours::black);
	g.drawRoundedRectangle(1.0f,
						   1.0f,
						   (float)getWidth()-2.0f,
						   (float)getHeight()-2.0f,
						   5.0f,
						   1.0f);

	//Fill Component background.
	g.setColour(colours[L"Plugin Background"]);
	g.fillRoundedRectangle(2.0f,
						   2.0f,
						   (float)getWidth()-4.0f,
						   (float)getHeight()-4.0f,
						   5.0f);

	//Fill Component outline and text background.
	g.setColour(colours[L"Plugin Border"]);
	g.drawRoundedRectangle(3.0f,
						   3.0f,
						   (float)getWidth()-6.0f,
						   (float)getHeight()-6.0f,
						   5.0f,
						   4.0f);
	g.fillRoundedRectangle(1.0f,
						   1.0f,
						   (float)getWidth()-2.0f,
						   16.0f,
						   5.0f);
	g.fillRect(1.0f, 16.0f, (float)getWidth()-2.0f, 5.0f);

	//Draw the plugin name.
	g.setColour(colours[L"Text Colour"]);
	//nameText.draw(g);

	//Draw the input channels.
	for(i=0;i<inputText.size();++i)
		inputText[i]->draw(g);

	//Draw the output channels.
	for(i=0;i<outputText.size();++i)
		outputText[i]->draw(g);
}

//------------------------------------------------------------------------------
void PluginComponent::moved()
{
	sendChangeMessage();
}

//------------------------------------------------------------------------------
void PluginComponent::timerUpdate()
{
	BypassableInstance *bypassable = dynamic_cast<BypassableInstance *>(node->getProcessor());

	if(bypassable)
		bypassButton->setToggleState(bypassable->getBypass(), false);
}

//------------------------------------------------------------------------------
void PluginComponent::mouseDown(const MouseEvent& e)
{
	if(e.y < 21)
	{
		if(e.getNumberOfClicks() == 2)
			titleLabel->showEditor();
		else
		{
			beginDragAutoRepeat(30);
			beingDragged = true;
			dragX = e.getPosition().getX();
			dragY = e.getPosition().getY();
			toFront(true);
		}
	}
}

//------------------------------------------------------------------------------
void PluginComponent::mouseDrag(const MouseEvent& e)
{
	if(beingDragged)
	{
		MouseEvent eField = e.getEventRelativeTo(getParentComponent());

		//parent = PluginField => parent = Viewport's contentHolder => parent = Viewport.
		Viewport *viewport = dynamic_cast<Viewport *>(getParentComponent()->getParentComponent()->getParentComponent());

		if(viewport)
		{
			MouseEvent tempEv = e.getEventRelativeTo(viewport);

			viewport->autoScroll(tempEv.x, tempEv.y, 20, 4);
		}

		setTopLeftPosition(eField.x - dragX, eField.y - dragY);
		if(getX() < 0)
			setTopLeftPosition(0, getY());
		if(getY() < 0)
			setTopLeftPosition(getX(), 0);
		node->properties.set("x", getX());
		node->properties.set("y", getY());
		sendChangeMessage();
	}
}

//------------------------------------------------------------------------------
void PluginComponent::mouseUp(const MouseEvent& e)
{
	beingDragged = false;
}

//------------------------------------------------------------------------------
void PluginComponent::buttonClicked(Button *button)
{
	if((button == editButton) && !pluginWindow)
	{
		AudioProcessorEditor *editor;

		editor = node->getProcessor()->createEditorIfNeeded();

		//Create generic ui.
		if(!editor)
			editor = new NiallsGenericEditor(node->getProcessor());

		if(editor)
		{
			editor->setName(node->getProcessor()->getName());
			pluginWindow = new PluginEditorWindow(editor, this);
		}

		if(pluginWindow)
			node->properties.set("windowOpen", true);
	}
	else if(button == mappingsButton)
		openMappingsWindow();
	else if(button == bypassButton)
	{
		BypassableInstance *bypassable = dynamic_cast<BypassableInstance *>(node->getProcessor());

		if(bypassable)
			bypassable->setBypass(bypassButton->getToggleState());
	}
	else if(button == deleteButton)
	{
		PluginField *parent = dynamic_cast<PluginField *>(getParentComponent());

		if(pluginWindow)
			pluginWindow->closeButtonPressed();

		if(parent)
			parent->deleteFilter(node);
		//else
			//????

		delete this;
	}
}

//------------------------------------------------------------------------------
void PluginComponent::labelTextChanged(Label *label)
{
	int i, y;
	PluginField *parent = dynamic_cast<PluginField *>(getParentComponent());

	pluginName = label->getText();

	parent->updateProcessorName(node->nodeId, pluginName);

	//Reset the Component's size/layout.
	determineSize(true);
	titleLabel->setBounds(5, 0, getWidth()-17, 20);
	if(deleteButton)
		deleteButton->setBounds(getWidth()-17, 5, 12, 12);
	if(bypassButton)
		bypassButton->setBounds(getWidth()-30, getHeight()-30, 20, 20);

	y = 25;
	for(i=0;i<outputPins.size();++i)
	{
		Point<int> pinPos;

		pinPos.setXY(getWidth()-5, y);
		outputPins[i]->setTopLeftPosition(pinPos.getX(), pinPos.getY());
		
		y += 12;
	}
	
	for(i=0;i<paramPins.size();++i)
	{
		Point<int> pinPos;
		
		pinPos.setXY(getWidth()-5, y);
		if(paramPins[i]->getX() > 0)
		{
			paramPins[i]->setTopLeftPosition(pinPos.getX(), pinPos.getY());
		
			y += 12;
		}
	}
}

//------------------------------------------------------------------------------
void PluginComponent::setUserName(const String& val)
{
	titleLabel->setText(val, sendNotification);
}

//------------------------------------------------------------------------------
void PluginComponent::setWindow(PluginEditorWindow *val)
{
	pluginWindow = val;
	if(pluginWindow)
		node->properties.set("windowOpen", true);
	else
		node->properties.set("windowOpen", false);
}

//------------------------------------------------------------------------------
void PluginComponent::saveWindowState()
{
	if(pluginWindow)
	{
		node->properties.set("uiLastX", pluginWindow->getX());
		node->properties.set("uiLastY", pluginWindow->getY());
		node->properties.set("windowOpen", true);
	}
	else
		node->properties.set("windowOpen", false);
}

//------------------------------------------------------------------------------
void PluginComponent::openMappingsWindow()
{
	String tempstr;
	PluginField *parent = dynamic_cast<PluginField *>(getParentComponent());
	MappingsDialog dlg(parent->getMidiManager(),
					   parent->getOscManager(),
					   node,
					   parent->getMappingsForPlugin(node->nodeId),
					   parent);

	tempstr << node->getProcessor()->getName() << " Mappings";
	JuceHelperStuff::showModalDialog(tempstr,
									 &dlg,
									 getParentComponent(),
									 Colour(0xFFEEECE1),
									 false,
									 true);
}

//------------------------------------------------------------------------------
void PluginComponent::cacheCurrentPreset()
{
	MemoryBlock *preset = new MemoryBlock();

	node->getProcessor()->getCurrentProgramStateInformation(*preset);

	cachedPresets.insert(make_pair(node->getProcessor()->getCurrentProgram(),
								   shared_ptr<MemoryBlock>(preset)));
}

//------------------------------------------------------------------------------
void PluginComponent::getCachedPreset(int index, MemoryBlock& memBlock)
{
	map<int, shared_ptr<MemoryBlock> >::iterator it;

	it = cachedPresets.find(index);

	//Make sure the cached preset actually exists.
	if(it != cachedPresets.end())
	{
		it->second->swapWith(memBlock);
		cachedPresets.erase(it);
	}
}

//------------------------------------------------------------------------------
void PluginComponent::determineSize(bool onlyUpdateWidth)
{
	int i;
	//float l, r, t, b;
	Rectangle<float> bounds;
	float nameWidth;
	float inputWidth = 0.0f;
	float outputWidth = 0.0f;
	int w = 150;
	int h = 100;
	float x;
	float y = 15.0f;
	int numInputPins = 0;
	int numOutputPins = 0;
	PedalboardProcessor *proc = 0;
	Font tempFont(14.0f, Font::bold);
	AudioProcessor *plugin = node->getProcessor();
	BypassableInstance *bypassable = dynamic_cast<BypassableInstance *>(plugin);
	bool ignorePinNames = PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("IgnorePinNames", false);

	if(bypassable)
		proc = dynamic_cast<PedalboardProcessor *>(bypassable->getPlugin());

	nameText.clear();

	//Determine plugin name bounds.
	nameText.addLineOfText(tempFont, pluginName, 10.0f, y);
	//nameText.getBoundingBox(0, -1, l, t, r, b, true);
	bounds = nameText.getBoundingBox(0, -1, true);
	nameWidth = bounds.getWidth();

	//Add on space for the close button if necessary.
	if((pluginName != "Audio Input") &&
	   (pluginName != "Midi Input") &&
	   (pluginName != "Audio Output") &&
	   (pluginName != "OSC Input"))
	{
		nameWidth += 20.0f;
	}
	else
		nameWidth += 4.0f;

	inputText.clear();
	outputText.clear();

	if(!proc)
	{
		//Determine plugin input channel name bounds.
		y = 35.0f;
		tempFont.setHeight(12.0f);
		tempFont.setStyleFlags(Font::plain);
		for(i=0;i<plugin->getNumInputChannels();++i)
		{
			if(!ignorePinNames)
			{
				GlyphArrangement *g = new GlyphArrangement;

				g->addLineOfText(tempFont, plugin->getInputChannelName(i), 10.0f, y);
				bounds = g->getBoundingBox(0, -1, true);

				inputText.add(g);

				if(bounds.getWidth() > inputWidth)
					inputWidth = bounds.getWidth();
			}
			else
			{
				String tempstr;
				GlyphArrangement *g = new GlyphArrangement;

				tempstr << L"Input " << i+1;
				g->addLineOfText(tempFont, tempstr, 10.0f, y);
				bounds = g->getBoundingBox(0, -1, true);

				inputText.add(g);

				if(bounds.getWidth() > inputWidth)
					inputWidth = bounds.getWidth();
			}

			y += 13.0f;
			++numInputPins;
		}

		//Add input parameter/midi name.
		if((plugin->acceptsMidi() ||
		   (plugin->getNumInputChannels() > 0) ||
		   (plugin->getNumOutputChannels() > 0)) &&
		   ((pluginName != "Audio Input") && (pluginName != "Audio Output")))
		{
			//if(!ignorePinNames)
			{
				GlyphArrangement *g = new GlyphArrangement;

				g->addLineOfText(tempFont, "param", 10.0f, y);
				bounds = g->getBoundingBox(0, -1, true);

				inputText.add(g);

				if(bounds.getWidth() > inputWidth)
					inputWidth = bounds.getWidth();
			}

			y += 13.0f;
			++numInputPins;
		}

		//Determine plugin output channel name bounds.
		y = 35.0f;
		for(i=0;i<plugin->getNumOutputChannels();++i)
		{
			if(!ignorePinNames)
			{
				GlyphArrangement *g = new GlyphArrangement;

				g->addLineOfText(tempFont,
								 plugin->getOutputChannelName(i),
								 0.0f,//(inputWidth + 20.0f),
								 y);
				bounds = g->getBoundingBox(0, -1, true);

				outputText.add(g);

				if(bounds.getWidth() > outputWidth)
					outputWidth = bounds.getWidth();
			}
			else
			{
				String tempstr;
				GlyphArrangement *g = new GlyphArrangement;

				tempstr << L"Output " << i+1;
				g->addLineOfText(tempFont,
								 tempstr,
								 0.0f,//(inputWidth + 20.0f),
								 y);
				bounds = g->getBoundingBox(0, -1, true);

				outputText.add(g);

				if(bounds.getWidth() > outputWidth)
					outputWidth = bounds.getWidth();
			}

			y += 13.0f;
			++numOutputPins;
		}

		//Add output parameter/midi name.
		if(plugin->producesMidi() || (plugin->getName() == "OSC Input"))
		{
			//if(!ignorePinNames)
			{
				GlyphArrangement *g = new GlyphArrangement;

				g->addLineOfText(tempFont,
								 "param",
								 0.0f,//(inputWidth + 20.0f),
								 y);
				bounds = g->getBoundingBox(0, -1, true);

				outputText.add(g);

				if(bounds.getWidth() > outputWidth)
					outputWidth = bounds.getWidth();
			}

			y += 13.0f;
			++numOutputPins;
		}

		if(nameWidth > (inputWidth + outputWidth + 30.0f))
			w = (int)(nameWidth + 12.0f);
		else
			w = (int)(inputWidth + outputWidth + 30.0f);

		//Shift output texts to where they should be.
		{
			x = (w - outputWidth - 10.0f);

			for(i=0;i<outputText.size();++i)
				outputText[i]->moveRangeOfGlyphs(0, -1, x, 0.0f);
		}

		h = jmax(numInputPins, numOutputPins);
		h *= 13;

		if((pluginName != "Audio Input") &&
		   (pluginName != "Midi Input") &&
		   (pluginName != "Audio Output") &&
		   (pluginName != "OSC Input"))
		{
			h += 60;
		}
		else
			h += 34;
	}
	else
	{
		Point<int> compSize = proc->getSize();

		if(nameWidth > (compSize.getX() + 24.0f))
			w = (int)(nameWidth + 20.0f);
		else
			w = (int)(compSize.getX() + 24.0f);
		
		h = compSize.getY() + 52;
	}

	if(onlyUpdateWidth)
		setSize(w, getHeight());
	else
		setSize(w, h);
}

//------------------------------------------------------------------------------
void PluginComponent::createPins()
{
	int i;
	int y;
	PluginPinComponent *pin;
	AudioProcessor *plugin = node->getProcessor();
	const uint32 uid = node->nodeId;

	y = 25;
	for(i=0;i<plugin->getNumInputChannels();++i)
	{
		Point<int> pinPos;

		pin = new PluginPinComponent(false, uid, i, false);
		pinPos.setXY(-5, y);
		pin->setTopLeftPosition(pinPos.getX(), pinPos.getY());
		addAndMakeVisible(pin);

		inputPins.add(pin);

		y += 12;
	}

	if((plugin->acceptsMidi() ||
	    (plugin->getNumInputChannels() > 0) ||
	    (plugin->getNumOutputChannels() > 0)) &&
	   ((pluginName != "Audio Input") && (pluginName != "Audio Output")))
	{
		Point<int> pinPos;

		pin = new PluginPinComponent(false, uid, AudioProcessorGraph::midiChannelIndex, true);
		pinPos.setXY(-5, y);
		pin->setTopLeftPosition(pinPos.getX(), pinPos.getY());
		addAndMakeVisible(pin);

		paramPins.add(pin);

		y += 12;
	}

	y = 25;
	for(i=0;i<plugin->getNumOutputChannels();++i)
	{
		Point<int> pinPos;

		pin = new PluginPinComponent(true, uid, i, false);
		pinPos.setXY(getWidth()-5, y);
		pin->setTopLeftPosition(pinPos.getX(), pinPos.getY());
		addAndMakeVisible(pin);

		outputPins.add(pin);

		y += 12;
	}

	if(plugin->producesMidi() || (plugin->getName() == "OSC Input"))
	{
		Point<int> pinPos;

		pin = new PluginPinComponent(true, uid, AudioProcessorGraph::midiChannelIndex, true);
		pinPos.setXY(getWidth()-5, y);
		pin->setTopLeftPosition(pinPos.getX(), pinPos.getY());
		addAndMakeVisible(pin);

		paramPins.add(pin);

		y += 12;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PluginPinComponent::PluginPinComponent(bool dir, uint32 id, int chan, bool param):
Component(),
direction(dir),
uid(id),
channel(chan),
parameterPin(param)
{
	setSize(10, 12);
}

//------------------------------------------------------------------------------
PluginPinComponent::~PluginPinComponent()
{
	
}

//------------------------------------------------------------------------------
void PluginPinComponent::paint(Graphics& g)
{
	const float w = (float)getWidth()-2;
	const float h = (float)getHeight()-2;

	g.setColour(Colours::black);
	g.drawEllipse(1, 1, w, h, 1.0f);

	if(!parameterPin)
		g.setColour(ColourScheme::getInstance().colours[L"Audio Connection"]);
	else
		g.setColour(ColourScheme::getInstance().colours[L"Parameter Connection"]);
	g.fillEllipse(1, 1, w, h);
}

//------------------------------------------------------------------------------
void PluginPinComponent::mouseDown(const MouseEvent& e)
{
	if(direction)
	{
		PluginField *field = findParentComponentOfClass<PluginField>();

		field->addConnection(this, (e.mods.isShiftDown() && !parameterPin));
	}
}

//------------------------------------------------------------------------------
void PluginPinComponent::mouseDrag(const MouseEvent& e)
{
	PluginField *field = findParentComponentOfClass<PluginField>();
	MouseEvent e2 = e.getEventRelativeTo(field);

	//field->dragConnection(e2.x+5, e2.y+6);
	field->dragConnection(e2.x-5, e2.y);
}

//------------------------------------------------------------------------------
void PluginPinComponent::mouseUp(const MouseEvent& e)
{
	if(e.mods.testFlags(ModifierKeys::leftButtonModifier))
	{
		PluginField *field = findParentComponentOfClass<PluginField>();
		MouseEvent e2 = e.getEventRelativeTo(field);

		field->releaseConnection(e2.x, e2.y);
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PluginEditorWindow::PluginEditorWindow(AudioProcessorEditor *editor,
									   PluginComponent *c):
DocumentWindow(c->getUserName(),
			   ColourScheme::getInstance().colours[L"Window Background"],
			   DocumentWindow::minimiseButton|DocumentWindow::maximiseButton|DocumentWindow::closeButton),
component(c)
{
	int x, y;

	centreWithSize(400, 300);
	
	setResizeLimits(396, 32, 10000, 10000);
	setUsingNativeTitleBar(true);
    setContentOwned(new EditorWrapper(editor, c), true);
	setAlwaysOnTop(PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("WindowsOnTop", false));
	//setDropShadowEnabled(false);
	//Fix for my favourite synth being unable to handle being resizable :(
	if((c->getNode()->getProcessor()->getName() != "VAZPlusVSTi") &&
	   !PropertiesSingleton::getInstance().getUserSettings()->getBoolValue("fixedSizeWindows", true))
	{
/*#ifdef __APPLE__
		//Most OSX AudioUnits/VSTs do not like being put in resizable windows, so
		//we only put our internal processors in them.
		PedalboardProcessor *p = dynamic_cast<PedalboardProcessor *>(editor->getAudioProcessor());
		if(p)
#endif*/
		setResizable(true, false);
	}

	x = component->getNode()->properties.getWithDefault("uiLastX", getX());
	if(x < 10)
		x = 10;
	y = component->getNode()->properties.getWithDefault("uiLastY", getY());
	if(y < 10)
		y = 10;
	setTopLeftPosition(x, y);

    setVisible(true);
	getPeer()->setIcon(ImageCache::getFromMemory(Images::icon512_png,
												 Images::icon512_pngSize));
}

//------------------------------------------------------------------------------
PluginEditorWindow::~PluginEditorWindow()
{
	component->getNode()->properties.set("uiLastX", getX());
	component->getNode()->properties.set("uiLastY", getY());
}

//------------------------------------------------------------------------------
void PluginEditorWindow::closeButtonPressed()
{
	component->setWindow(0);
    delete this;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PluginEditorWindow::EditorWrapper::EditorWrapper(AudioProcessorEditor *ed,
												 PluginComponent *comp):
editor(ed),
component(comp)
{
	presetBar = new PresetBar(component);

	presetBar->setBounds(0, 0, 396, 32);
	addAndMakeVisible(presetBar);

	editor->setTopLeftPosition(0, 32);
	addAndMakeVisible(editor);

	if(editor->getWidth() < 396)
		setSize(396, 32 + editor->getHeight());
	else
		setSize(editor->getWidth(), 32 + editor->getHeight());
}

//------------------------------------------------------------------------------
PluginEditorWindow::EditorWrapper::~EditorWrapper()
{
	deleteAllChildren();
}

//------------------------------------------------------------------------------
void PluginEditorWindow::EditorWrapper::resized()
{
	presetBar->setSize(getWidth(), 32);
	editor->setSize(getWidth(), getHeight()-32);
}

//------------------------------------------------------------------------------
void PluginEditorWindow::EditorWrapper::childBoundsChanged(Component *child)
{
	if(child == editor)
	{
		if(editor->getWidth() < 396)
			setSize(396, 32 + editor->getHeight());
		else
			setSize(editor->getWidth(), 32 + editor->getHeight());
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PluginConnection::PluginConnection(PluginPinComponent *s,
								   PluginPinComponent *d,
								   bool allOutputs):
Component(),
source(s),
selected(false),
representsAllOutputs(allOutputs)
{
	if(source)
	{
		Point<int> tempPoint(source->getX()+5, source->getY()+6);
		PluginField *field = source->findParentComponentOfClass<PluginField>();

		tempPoint = field->getLocalPoint(source->getParentComponent(), tempPoint);
		setTopLeftPosition(tempPoint.getX(), tempPoint.getY());

		((PluginComponent *)source->getParentComponent())->addChangeListener(this);

		paramCon = source->getParameterPin();
	}

	if(d)
		setDestination(d);
	else
		destination = 0;
}

//------------------------------------------------------------------------------
PluginConnection::~PluginConnection()
{
	if(source)
	{
		PluginComponent *sourceComp = dynamic_cast<PluginComponent *>(source->getParentComponent());
		if(sourceComp)
			sourceComp->removeChangeListener(this);
	}
	if(destination)
	{
		PluginComponent *destComp = dynamic_cast<PluginComponent *>(destination->getParentComponent());
		if(destComp)
			destComp->removeChangeListener(this);
	}
}

//------------------------------------------------------------------------------
void PluginConnection::paint(Graphics& g)
{
	Colour tempCol;

	if(representsAllOutputs)
	{
		g.setColour(Colours::red);
		g.strokePath(drawnCurve, PathStrokeType(4.0f));
	}
	else
	{
		g.setColour(Colours::black);
		g.strokePath(drawnCurve, PathStrokeType(1.0f));
	}

	if(!paramCon)
		tempCol = ColourScheme::getInstance().colours[L"Audio Connection"];
	else
		tempCol = ColourScheme::getInstance().colours[L"Parameter Connection"];

	if(selected)
		g.setColour(tempCol.brighter(0.75f));
	else
		g.setColour(tempCol);
	g.fillPath(drawnCurve);
}

//------------------------------------------------------------------------------
void PluginConnection::mouseDown(const MouseEvent& e)
{
	selected = !selected;
	repaint();
}

//------------------------------------------------------------------------------
bool PluginConnection::hitTest(int x, int y)
{
	bool retval = false;

	if(drawnCurve.contains((float)x, (float)y))
	{
		//Make sure clicking the source pin doesn't select this connection.
		if(x > 10)
			retval = true;
	}

	return retval;
}

//------------------------------------------------------------------------------
void PluginConnection::changeListenerCallback(ChangeBroadcaster *changedObject)
{
	Component *field = getParentComponent();

	if(source && destination)
	{
		Point<int> sourcePoint(source->getX()+5, source->getY()+6);
		Point<int> destPoint(destination->getX()+5, destination->getY()+6);
		sourcePoint = field->getLocalPoint(source->getParentComponent(), sourcePoint);
		destPoint = field->getLocalPoint(destination->getParentComponent(), destPoint);

		updateBounds(sourcePoint.getX(),
					 sourcePoint.getY(),
					 destPoint.getX(),
					 destPoint.getY());
	}
}

//------------------------------------------------------------------------------
void PluginConnection::drag(int x, int y)
{
	Component *field = getParentComponent();

	if(source)
	{
		Point<int> sourcePoint(source->getX()+5, source->getY()+6);
		sourcePoint = field->getLocalPoint(source->getParentComponent(), sourcePoint);

		updateBounds(sourcePoint.getX(), sourcePoint.getY(), x, y);
	}
}

//------------------------------------------------------------------------------
void PluginConnection::setDestination(PluginPinComponent *d)
{
	PluginField *field = source->findParentComponentOfClass<PluginField>();

	destination = d;
	if(destination)
		((PluginComponent *)destination->getParentComponent())->addChangeListener(this);

	if(source && destination)
	{
		Point<int> sourcePoint(source->getX()+5, source->getY()+6);
		Point<int> destPoint(destination->getX()+5, destination->getY()+6);
		sourcePoint = field->getLocalPoint(source->getParentComponent(), sourcePoint);
		destPoint = field->getLocalPoint(destination->getParentComponent(), destPoint);

		if(destPoint.getX() > sourcePoint.getX())
			updateBounds(sourcePoint.getX(), sourcePoint.getY(), destPoint.getX(), destPoint.getY());
		else
			updateBounds(destPoint.getX(), destPoint.getY(), sourcePoint.getX(), sourcePoint.getY());
	}
}

//------------------------------------------------------------------------------
void PluginConnection::setRepresentsAllOutputs(bool val)
{
	representsAllOutputs = val;
}

//------------------------------------------------------------------------------
void PluginConnection::getPoints(int& sX, int& sY, int& dX, int&dY)
{
	int tX, tY;

	if(dY < sY)
	{
		if(sX < dX)
		{
			dX -= sX;
			sX = 5;
			dX += 5;

			tX = dX;
			dX = sX;
			sX = tX;
		}
		else
		{
			sX -= dX;
			dX = 5;
			sX += 5;

			tX = dX;
			dX = sX;
			sX = tX;
		}

		sY -= dY;
		dY = 5;
		sY += 5;

		tY = dY;
		dY = sY;
		sY = tY;
	}
	else if(sX < dX)
	{
		dX -= sX;
		sX = 5;
		dX += 5;

		dY -= sY;
		sY = 5;
		dY += 5;
	}
	else
	{
		sX -= dX;
		dX = 5;
		sX += 5;

		tX = dX;
		dX = sX;
		sX = tX;

		sY -= dY;
		dY = 5;
		sY += 5;

		tY = dY;
		dY = sY;
		sY = tY;
	}
}

//------------------------------------------------------------------------------
void PluginConnection::updateBounds(int sX, int sY, int dX, int dY)
{
	int left, top, width, height;

	Path tempPath;
	PathStrokeType drawnType(9.0f, PathStrokeType::mitered, PathStrokeType::rounded);

	if(sX < dX)
	{
		left = sX;
		width = dX - sX;
	}
	else
	{
		left = dX;
		width = sX - dX;
	}
	if(sY < dY)
	{
		top = sY;
		height = dY - sY;
	}
	else
	{
		top = dY;
		height = sY - dY;
	}

	getPoints(sX, sY, dX, dY);

	tempPath.startNewSubPath((float)sX, (float)sY);
	tempPath.cubicTo(((float)width*0.5f)+jmin(sX, dX),
					 (float)sY,
					 ((float)width*0.5f)+jmin(sX, dX),
					 (float)dY,
					 (float)dX,
					 (float)dY);
	drawnType.createStrokedPath(drawnCurve, tempPath);

	setBounds(left-5, top-5, width+10, height+10);
}
