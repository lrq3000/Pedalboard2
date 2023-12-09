//	PedalboardProcessorEditors.cpp - The various editors for the app's internal
//									 processors.
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
#include "JuceHelperStuff.h"
#include "ColourScheme.h"
#include "Vectors.h"

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LevelControl::LevelControl(LevelProcessor *proc):
processor(proc)
{
	slider = new Slider();

	slider->setSliderStyle(Slider::RotaryVerticalDrag);
	slider->setTextBoxStyle(Slider::NoTextBox, true, 64, 20);
	slider->setRange(0.0, 2.0);
	slider->setValue(processor->getParameter(0) * 2.0f);
	slider->setDoubleClickReturnValue(true, 1.0);
	slider->addListener(this);
	slider->setTopLeftPosition(0, 0);
	slider->setSize(64, 64);
	slider->setColour(Slider::rotarySliderFillColourId,
					  ColourScheme::getInstance().colours[L"Level Dial Colour"]);
	addAndMakeVisible(slider);

	startTimer(60);

	setSize(64, 64);
}

//------------------------------------------------------------------------------
LevelControl::~LevelControl()
{
	deleteAllChildren();
}

//------------------------------------------------------------------------------
void LevelControl::timerCallback()
{
	slider->setValue(processor->getParameter(0) * 2.0f);
}

//------------------------------------------------------------------------------
void LevelControl::sliderValueChanged(Slider *slider)
{
	processor->setParameter(0, (float)slider->getValue() * 0.5f);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LevelEditor::LevelEditor(AudioProcessor *processor,
						 const Rectangle<int>& windowBounds):
AudioProcessorEditor(processor),
parentBounds(windowBounds),
setPos(false)
{
	slider = new Slider();

	slider->setSliderStyle(Slider::RotaryVerticalDrag);
	slider->setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
	slider->setRange(0.0, 2.0);
	slider->setValue(processor->getParameter(0) * 2.0f);
	slider->setDoubleClickReturnValue(true, 1.0);
	slider->addListener(this);
	slider->setTopLeftPosition(0, 0);
	slider->setSize(192, 192);
	slider->setColour(Slider::rotarySliderFillColourId,
					  ColourScheme::getInstance().colours[L"Level Dial Colour"]);
	addAndMakeVisible(slider);

	setSize(192, 192);

	startTimer(60);
}

//------------------------------------------------------------------------------
LevelEditor::~LevelEditor()
{
	LevelProcessor *proc = dynamic_cast<LevelProcessor *>(getAudioProcessor());

	if(proc && getParentComponent())
	{
		parentBounds = getTopLevelComponent()->getBounds();
		proc->updateEditorBounds(parentBounds);
	}

	deleteAllChildren();
	getAudioProcessor()->editorBeingDeleted(this);
}

//------------------------------------------------------------------------------
void LevelEditor::resized()
{
	//LevelProcessor *proc = dynamic_cast<LevelProcessor *>(getAudioProcessor());
	const int h = getHeight();
	const int deskH = (int)((float)getParentMonitorArea().getHeight()/1.5f);

	//Resize the slider.
	slider->setSize(getWidth(), h);

	//Resize its drag area.
	if(h > 250)
	{
		if(h < deskH)
			slider->setMouseDragSensitivity(h);
		else
			slider->setMouseDragSensitivity(deskH);
	}
	else
		slider->setMouseDragSensitivity(250);
}

//------------------------------------------------------------------------------
void LevelEditor::paint(Graphics& g)
{
	g.fillAll(ColourScheme::getInstance().colours[L"Window Background"]);
}

//------------------------------------------------------------------------------
void LevelEditor::timerCallback()
{
	slider->setValue(getAudioProcessor()->getParameter(0) * 2.0f);

	if(!setPos)
	{
		ComponentPeer *peer = getPeer();

		if(parentBounds.isEmpty())
			setPos = true;
		else if(peer)
		{
			peer->setBounds(parentBounds.getX(),
							parentBounds.getY(),
							parentBounds.getWidth(),
							parentBounds.getHeight(),
							false);
			setPos = true;
		}
	}
}

//------------------------------------------------------------------------------
void LevelEditor::sliderValueChanged(Slider *slider)
{
	getAudioProcessor()->setParameter(0, (float)slider->getValue() * 0.5f);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
FilePlayerEditor::FilePlayerEditor(FilePlayerProcessor *processor,
								   const Rectangle<int>& windowBounds):
AudioProcessorEditor(processor),
parentBounds(windowBounds),
setPos(false)
{
	controls = new FilePlayerControl(processor);
	controls->setWaveformBackground(Colour(0xFFEEECE1).darker(0.05f));
	controls->setTopLeftPosition(4, 4);
	controls->setSize(getWidth()-8, getHeight()-8);
	addAndMakeVisible(controls);

	setSize(600, 200);

	startTimer(60);
}

//------------------------------------------------------------------------------
FilePlayerEditor::~FilePlayerEditor()
{
	FilePlayerProcessor *proc = dynamic_cast<FilePlayerProcessor *>(getAudioProcessor());

	if(proc && getParentComponent())
	{
		parentBounds = getTopLevelComponent()->getBounds();
		proc->updateEditorBounds(parentBounds);
	}

	deleteAllChildren();
	getAudioProcessor()->editorBeingDeleted(this);
}

//------------------------------------------------------------------------------
void FilePlayerEditor::resized()
{
	//Resize the controls.
	controls->setSize(getWidth()-8, getHeight()-8);
}

//------------------------------------------------------------------------------
void FilePlayerEditor::paint(Graphics& g)
{
	g.fillAll(ColourScheme::getInstance().colours[L"Window Background"]);
}

//------------------------------------------------------------------------------
void FilePlayerEditor::timerCallback()
{
	if(!setPos)
	{
		ComponentPeer *peer = getPeer();

		if(parentBounds.isEmpty())
			setPos = true;
		else if(peer)
		{
			peer->setBounds(parentBounds.getX(),
							parentBounds.getY(),
							parentBounds.getWidth(),
							parentBounds.getHeight(),
							false);
			setPos = true;
			stopTimer();
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
OutputToggleControl::OutputToggleControl(OutputToggleProcessor *proc):
processor(proc)
{
	ScopedPointer<Drawable> im1(JuceHelperStuff::loadSVGFromMemory(Vectors::outputtoggle1_svg,
																   Vectors::outputtoggle1_svgSize));
	ScopedPointer<Drawable> im2(JuceHelperStuff::loadSVGFromMemory(Vectors::outputtoggle2_svg,
																   Vectors::outputtoggle2_svgSize));

	toggleButton = new DrawableButton("toggleButton",
									  DrawableButton::ImageFitted);
	toggleButton->setImages(im1, 0, 0, 0, im2);
	toggleButton->setColour(DrawableButton::backgroundColourId,
							Colours::transparentBlack);
	toggleButton->setColour(DrawableButton::backgroundOnColourId,
							Colours::transparentBlack);
	toggleButton->setClickingTogglesState(true);
	toggleButton->setTopLeftPosition(0, 0);
	toggleButton->setSize(48, 48);
	toggleButton->addListener(this);
	addAndMakeVisible(toggleButton);

	startTimer(60);

	setSize(48, 48);
}

//------------------------------------------------------------------------------
OutputToggleControl::~OutputToggleControl()
{
	deleteAllChildren();
}

//------------------------------------------------------------------------------
void OutputToggleControl::timerCallback()
{
	toggleButton->setToggleState(processor->getParameter(0) > 0.5f, false);
}

//------------------------------------------------------------------------------
void OutputToggleControl::buttonClicked(Button *button)
{
	processor->setParameter(0, toggleButton->getToggleState() ? 1.0f : 0.0f);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
OutputToggleEditor::OutputToggleEditor(AudioProcessor *processor,
									   const Rectangle<int>& windowBounds):
AudioProcessorEditor(processor),
parentBounds(windowBounds),
setPos(false)
{
	ScopedPointer<Drawable> im1(JuceHelperStuff::loadSVGFromMemory(Vectors::outputtoggle1_svg,
																   Vectors::outputtoggle1_svgSize));
	ScopedPointer<Drawable> im2(JuceHelperStuff::loadSVGFromMemory(Vectors::outputtoggle2_svg,
																   Vectors::outputtoggle2_svgSize));

	toggleButton = new DrawableButton("toggleButton",
									  DrawableButton::ImageFitted);
	toggleButton->setImages(im1, 0, 0, 0, im2);
	toggleButton->setColour(DrawableButton::backgroundColourId,
							Colours::transparentBlack);
	toggleButton->setColour(DrawableButton::backgroundOnColourId,
							Colours::transparentBlack);
	toggleButton->setClickingTogglesState(true);
	toggleButton->setTopLeftPosition(0, 0);
	toggleButton->setSize(48, 48);
	toggleButton->addListener(this);
	addAndMakeVisible(toggleButton);

	startTimer(60);

	setSize(192, 192);
}

//------------------------------------------------------------------------------
OutputToggleEditor::~OutputToggleEditor()
{
	LevelProcessor *proc = dynamic_cast<LevelProcessor *>(getAudioProcessor());

	if(proc && getParentComponent())
	{
		parentBounds = getTopLevelComponent()->getBounds();
		proc->updateEditorBounds(parentBounds);
	}

	deleteAllChildren();
	getAudioProcessor()->editorBeingDeleted(this);
}

//------------------------------------------------------------------------------
void OutputToggleEditor::resized()
{
	//Resize the button.
	toggleButton->setSize(getWidth(), getHeight());
}

//------------------------------------------------------------------------------
void OutputToggleEditor::paint(Graphics& g)
{
	g.fillAll(ColourScheme::getInstance().colours[L"Window Background"]);
}

//------------------------------------------------------------------------------
void OutputToggleEditor::timerCallback()
{
	toggleButton->setToggleState(getAudioProcessor()->getParameter(0) > 0.5f, false);

	if(!setPos)
	{
		ComponentPeer *peer = getPeer();

		if(parentBounds.isEmpty())
			setPos = true;
		else if(peer)
		{
			peer->setBounds(parentBounds.getX(),
							parentBounds.getY(),
							parentBounds.getWidth(),
							parentBounds.getHeight(),
							false);
			setPos = true;
		}
	}
}

//------------------------------------------------------------------------------
void OutputToggleEditor::buttonClicked(Button *button)
{
	getAudioProcessor()->setParameter(0,
									  toggleButton->getToggleState() ? 1.0f : 0.0f);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VuMeterControl::VuMeterControl(VuMeterProcessor *proc):
processor(proc),
levelLeft(0.0f),
levelRight(0.0f)
{
	startTimer(60);

	setSize(64, 128);
}

//------------------------------------------------------------------------------
VuMeterControl::~VuMeterControl()
{

}

//------------------------------------------------------------------------------
void VuMeterControl::paint(Graphics& g)
{
	float textSize;
	float levLeft = (levelLeft < 0.0f) ? (levelLeft/60.0f)+1.0f : 1.0f;
	float levRight = (levelRight < 0.0f) ? (levelRight/60.0f)+1.0f : 1.0f;
	float width = (float)getWidth();
	float height = (float)getHeight();
	float redSize = (height < 128) ? 10.0f : (height * (10.0f/128.0f));
	float heightLeft = (height-redSize-4) * levLeft;
	float heightRight = (height-redSize-4) * levRight;
	const float sixDb = redSize + ((6.0f/60.0f) * (height-redSize-4));
	const float twelveDb = redSize + ((12.0f/60.0f) * (height-redSize-4));
	const float twentyFourDb = redSize + ((24.0f/60.0f) * (height-redSize-4));
	const float fortyEightDb = redSize + ((48.0f/60.0f) * (height-redSize-4));
	map<String, Colour>& colours = ColourScheme::getInstance().colours;

	/*Colour topColour1((uint8)((255 * levLeft) + (0 * (1.0f-levLeft))),
					  (uint8)((255 * levLeft) + (191 * (1.0f-levLeft))),
					  (uint8)((0 * levLeft) + (0 * (1.0f-levLeft))),
					  (uint8)(127 * levLeft));
	Colour topColour2((uint8)((255 * levRight) + (0 * (1.0f-levRight))),
					  (uint8)((255 * levRight) + (191 * (1.0f-levRight))),
					  (uint8)((0 * levRight) + (0 * (1.0f-levRight))),
					  (uint8)(127 * levRight));*/
	Colour topColour1 = colours[L"VU Meter Upper Colour"].withMultipliedBrightness(levLeft);
	Colour topColour2 = colours[L"VU Meter Upper Colour"].withMultipliedBrightness(levRight);
	Colour bottomColour = colours[L"VU Meter Lower Colour"];
	ColourGradient grad1(topColour1,
						 0,
						 0,
						 bottomColour,
						 0,
						 heightLeft,
						 false);
	ColourGradient grad2(topColour2,
						 0,
						 0,
						 bottomColour,
						 0,
						 heightRight,
						 false);

	if(levelLeft >= 0.0f)
	{
		g.setColour(colours[L"VU Meter Over Colour"]);

		g.fillRect(0.0f, 0.0f, (width*0.5f)-2.0f, redSize);
	}
	if(levelLeft > -60.0f)
	{
		g.setGradientFill(grad1);
		g.fillRect(0.0f,
				   height - heightLeft - 4,
				   (width*0.5f)-2.0f,
				   heightLeft);
	}

	if(levelRight >= 0.0f)
	{
		g.setColour(colours[L"VU Meter Over Colour"]);

		g.fillRect((width*0.5f)+2.0f, 0.0f, (width*0.5f)-2.0f, redSize);
	}
	if(levelRight > -60.0f)
	{
		g.setGradientFill(grad2);
		g.fillRect((width*0.5f)+2.0f,
				   height - heightRight - 4,
				   (width*0.5f)-2.0f,
				   heightRight);
	}

	g.setColour(colours[L"Text Colour"].withAlpha(0.25f));
	g.drawLine(0.0f, redSize, width, redSize);
	g.drawLine(0.0f, sixDb, width, sixDb);
	g.drawLine(0.0f, twelveDb, width, twelveDb);
	g.drawLine(0.0f, twentyFourDb, width, twentyFourDb);
	g.drawLine(0.0f, fortyEightDb, width, fortyEightDb);

	if(width <= 64.0f)
		textSize = 8.0f;
	else if(width > 192.0f)
		textSize = 24.0f;
	else
		textSize = width/8.0f;
	g.setFont(textSize);
	g.setColour(colours[L"Text Colour"].withAlpha(0.5f));
	g.drawText(L"0dB",
			   (int)((width/2)-textSize),
			   (int)(redSize-textSize),
			   (int)(textSize*2),
			   (int)(textSize*2),
			   Justification(Justification::centred),
			   false);
	g.drawText(L"6dB",
			   (int)((width/2)-textSize),
			   (int)(sixDb-textSize),
			   (int)(textSize*2),
			   (int)(textSize*2),
			   Justification(Justification::centred),
			   false);
	g.drawText(L"12dB",
			   (int)((width/2)-(textSize*2)),
			   (int)(twelveDb-textSize),
			   (int)(textSize*4),
			   (int)(textSize*2),
			   Justification(Justification::centred),
			   false);
	g.drawText(L"24dB",
			   (int)((width/2)-(textSize*2)),
			   (int)(twentyFourDb-textSize),
			   (int)(textSize*4),
			   (int)(textSize*2),
			   Justification(Justification::centred),
			   false);
	g.drawText(L"48dB",
			   (int)((width/2)-(textSize*2)),
			   (int)(fortyEightDb-textSize),
			   (int)(textSize*4),
			   (int)(textSize*2),
			   Justification(Justification::centred),
			   false);
}

//------------------------------------------------------------------------------
void VuMeterControl::resized()
{

}

//------------------------------------------------------------------------------
void VuMeterControl::timerCallback()
{
	if(processor)
	{
		float levLeft = processor->getLeftLevel();
		float levRight = processor->getRightLevel();
		const float minus60 = powf(10.0f, (-60.0f/20.0f));

		if(levLeft > minus60)
			levelLeft = 20.0f * log10f(levLeft);
		else
			levelLeft = -60.0f;

		if(levRight > minus60)
			levelRight = 20.0f * log10f(levRight);
		else
			levelRight = -60.0f;

		repaint();
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
VuMeterEditor::VuMeterEditor(AudioProcessor *processor,
							 const Rectangle<int>& windowBounds):
AudioProcessorEditor(processor),
parentBounds(windowBounds),
setPos(false)
{
	meter = new VuMeterControl(dynamic_cast<VuMeterProcessor *>(processor));
	addAndMakeVisible(meter);

	setSize(128, 256);
}

//------------------------------------------------------------------------------
VuMeterEditor::~VuMeterEditor()
{
	VuMeterProcessor *proc = dynamic_cast<VuMeterProcessor *>(getAudioProcessor());

	if(proc && getParentComponent())
	{
		parentBounds = getTopLevelComponent()->getBounds();
		proc->updateEditorBounds(parentBounds);
	}

	deleteAllChildren();
	getAudioProcessor()->editorBeingDeleted(this);
}

//------------------------------------------------------------------------------
void VuMeterEditor::resized()
{
	//Resize the meter.
	meter->setSize(getWidth(), getHeight());
}

//------------------------------------------------------------------------------
void VuMeterEditor::paint(Graphics& g)
{
	g.fillAll(ColourScheme::getInstance().colours[L"Window Background"]);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AudioRecorderEditor::AudioRecorderEditor(RecorderProcessor *processor,
										 const Rectangle<int>& windowBounds,
										 AudioThumbnail& thumbnail):
AudioProcessorEditor(processor),
parentBounds(windowBounds),
setPos(false)
{
	controls = new AudioRecorderControl(processor, thumbnail);
	controls->setWaveformBackground(Colour(0xFFEEECE1).darker(0.05f));
	controls->setTopLeftPosition(4, 4);
	controls->setSize(getWidth()-8, getHeight()-8);
	addAndMakeVisible(controls);

	setSize(600, 200);

	startTimer(60);
}

//------------------------------------------------------------------------------
AudioRecorderEditor::~AudioRecorderEditor()
{
	RecorderProcessor *proc = dynamic_cast<RecorderProcessor *>(getAudioProcessor());

	if(proc && getParentComponent())
	{
		parentBounds = getTopLevelComponent()->getBounds();
		proc->updateEditorBounds(parentBounds);
	}

	deleteAllChildren();
	getAudioProcessor()->editorBeingDeleted(this);
}

//------------------------------------------------------------------------------
void AudioRecorderEditor::resized()
{
	//Resize the controls.
	controls->setSize(getWidth()-8, getHeight()-8);
}

//------------------------------------------------------------------------------
void AudioRecorderEditor::paint(Graphics& g)
{
	g.fillAll(ColourScheme::getInstance().colours[L"Window Background"]);
}

//------------------------------------------------------------------------------
void AudioRecorderEditor::timerCallback()
{
	if(!setPos)
	{
		ComponentPeer *peer = getPeer();

		if(parentBounds.isEmpty())
			setPos = true;
		else if(peer)
		{
			peer->setBounds(parentBounds.getX(),
							parentBounds.getY(),
							parentBounds.getWidth(),
							parentBounds.getHeight(),
							false);
			setPos = true;
			stopTimer();
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
MetronomeEditor::MetronomeEditor(MetronomeProcessor *processor,
								 const Rectangle<int>& windowBounds):
AudioProcessorEditor(processor),
parentBounds(windowBounds),
setPos(false)
{
	controls = new MetronomeControl(processor, true);
	controls->setTopLeftPosition(4, 4);
	controls->setSize(getWidth()-8, getHeight()-8);
	addAndMakeVisible(controls);

	setSize(400, 200);

	startTimer(60);
}

//------------------------------------------------------------------------------
MetronomeEditor::~MetronomeEditor()
{
	MetronomeProcessor *proc = dynamic_cast<MetronomeProcessor *>(getAudioProcessor());

	if(proc && getParentComponent())
	{
		parentBounds = getTopLevelComponent()->getBounds();
		proc->updateEditorBounds(parentBounds);
	}

	deleteAllChildren();
	getAudioProcessor()->editorBeingDeleted(this);
}

//------------------------------------------------------------------------------
void MetronomeEditor::resized()
{
	//Resize the controls.
	controls->setSize(getWidth()-8, getHeight()-8);
}

//------------------------------------------------------------------------------
void MetronomeEditor::paint(Graphics& g)
{
	g.fillAll(ColourScheme::getInstance().colours[L"Window Background"]);
}

//------------------------------------------------------------------------------
void MetronomeEditor::timerCallback()
{
	if(!setPos)
	{
		ComponentPeer *peer = getPeer();

		if(parentBounds.isEmpty())
			setPos = true;
		else if(peer)
		{
			peer->setBounds(parentBounds.getX(),
							parentBounds.getY(),
							parentBounds.getWidth(),
							parentBounds.getHeight(),
							false);
			setPos = true;
			stopTimer();
		}
	}
}
