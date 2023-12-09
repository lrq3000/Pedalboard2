//	PedalboardProcessorEditors.h - The various editors for the app's internal
//								   processors.
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

#ifndef PEDALBOARDPROCESSOREDITORS_H_
#define PEDALBOARDPROCESSOREDITORS_H_

#include "../JuceLibraryCode/JuceHeader.h"

class LevelProcessor;
class FilePlayerProcessor;
class FilePlayerControl;
class OutputToggleProcessor;
class VuMeterProcessor;
class RecorderProcessor;
class MetronomeProcessor;
class AudioRecorderControl;
class MetronomeControl;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///	The PluginComponent control for LevelProcessor.
class LevelControl : public Component,
					 public Timer,
					 public Slider::Listener
{
  public:
	///	Constructor.
	LevelControl(LevelProcessor *proc);
	///	Destructor.
	~LevelControl();

	///	So we can update the slider when its value changes.
	void timerCallback();
	///	Sets the processor's level from the slider.
	void sliderValueChanged(Slider *slider);

	juce_UseDebuggingNewOperator
  private:
	///	Our copy of the associated LevelProcessor.
	LevelProcessor *processor;

	///	The rotary slider.
	Slider *slider;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///	Proper big editor for the LevelProcessor.
class LevelEditor : public AudioProcessorEditor,
					public Timer,
					public Slider::Listener
{
  public:
	///	Constructor.
	LevelEditor(AudioProcessor *processor, const Rectangle<int>& windowBounds);
	///	Destructor.
	~LevelEditor();

	///	So we can resize the slider to match the size of the window.
	void resized();
	///	So we can paint the background a nice colour.
	void paint(Graphics& g);

	///	So we can update the slider when its value changes.
	void timerCallback();
	///	Sets the processor's level from the slider.
	void sliderValueChanged(Slider *slider);

	juce_UseDebuggingNewOperator
  private:
	///	The rotary slider.
	Slider *slider;

	///	The bounds of the parent window.
	Rectangle<int> parentBounds;

	///	True if we've updated the position of the window to its previous position.
	bool setPos;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///	Proper big editor for the FilePlayerProcessor.
class FilePlayerEditor : public AudioProcessorEditor,
						 public Timer
{
  public:
	///	Constructor.
	FilePlayerEditor(FilePlayerProcessor *processor,
					 const Rectangle<int>& windowBounds);
	///	Destructor.
	~FilePlayerEditor();

	///	So we can resize the slider to match the size of the window.
	void resized();
	///	So we can paint the background a nice colour.
	void paint(Graphics& g);

	///	So we can update the window's bounds.
	void timerCallback();

	juce_UseDebuggingNewOperator
  private:
	///	The actual file player controls.
	FilePlayerControl *controls;

	///	The bounds of the parent window.
	Rectangle<int> parentBounds;

	///	True if we've updated the position of the window to its previous position.
	bool setPos;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///	The PluginComponent control for OutputToggleProcessor.
class OutputToggleControl : public Component,
							public Timer,
							public ButtonListener
{
  public:
	///	Constructor.
	OutputToggleControl(OutputToggleProcessor *proc);
	///	Destructor.
	~OutputToggleControl();

	///	So we can update the slider when its value changes.
	void timerCallback();
	///	To toggle between outputs.
	void buttonClicked(Button *button);

	juce_UseDebuggingNewOperator
  private:
	///	Our copy of the associated OutputToggleProcessor.
	OutputToggleProcessor *processor;

	///	The toggle button.
	DrawableButton *toggleButton;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///	Proper big editor for the OutputToggleProcessor.
class OutputToggleEditor : public AudioProcessorEditor,
						   public Timer,
						   public ButtonListener
{
  public:
	///	Constructor.
	OutputToggleEditor(AudioProcessor *processor,
					   const Rectangle<int>& windowBounds);
	///	Destructor.
	~OutputToggleEditor();

	///	So we can resize the button to match the size of the window.
	void resized();
	///	So we can paint the background a nice colour.
	void paint(Graphics& g);

	///	So we can update the slider when its value changes.
	void timerCallback();
	///	To toggle between outputs.
	void buttonClicked(Button *button);

	juce_UseDebuggingNewOperator
  private:
	///	The toggle button.
	DrawableButton *toggleButton;

	///	The bounds of the parent window.
	Rectangle<int> parentBounds;

	///	True if we've updated the position of the window to its previous position.
	bool setPos;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///	The PluginComponent control for the VuMeter processor.
class VuMeterControl : public Component,
					   public Timer
{
  public:
	///	Constructor.
	VuMeterControl(VuMeterProcessor *proc);
	///	Destructor.
	~VuMeterControl();

	///	Draws the meter.
	void paint(Graphics& g);
	///	Resizes the meter.
	void resized();

	///	Updates the meter.
	void timerCallback();

	juce_UseDebuggingNewOperator
  private:
	///	Our copy of the associated VuMeterProcessor.
	VuMeterProcessor *processor;

	///	The current left level.
	float levelLeft;
	///	The current right level.
	float levelRight;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///	Proper big editor for the VuMeterProcessor.
class VuMeterEditor : public AudioProcessorEditor
{
  public:
	///	Constructor.
	VuMeterEditor(AudioProcessor *processor,
				  const Rectangle<int>& windowBounds);
	///	Destructor.
	~VuMeterEditor();

	///	So we can resize the button to match the size of the window.
	void resized();
	///	So we can paint the background a nice colour.
	void paint(Graphics& g);

	juce_UseDebuggingNewOperator
  private:
	///	The vu meter control.
	VuMeterControl *meter;

	///	The bounds of the parent window.
	Rectangle<int> parentBounds;

	///	True if we've updated the position of the window to its previous position.
	bool setPos;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///	Proper big editor for the RecorderProcessor.
class AudioRecorderEditor : public AudioProcessorEditor,
							public Timer
{
  public:
	///	Constructor.
	AudioRecorderEditor(RecorderProcessor *processor,
					    const Rectangle<int>& windowBounds,
						AudioThumbnail& thumbnail);
	///	Destructor.
	~AudioRecorderEditor();

	///	So we can resize the slider to match the size of the window.
	void resized();
	///	So we can paint the background a nice colour.
	void paint(Graphics& g);

	///	So we can update the window's bounds.
	void timerCallback();

	juce_UseDebuggingNewOperator
  private:
	///	The actual file player controls.
	AudioRecorderControl *controls;

	///	The bounds of the parent window.
	Rectangle<int> parentBounds;

	///	True if we've updated the position of the window to its previous position.
	bool setPos;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///	Proper big editor for the MetronomeProcessor.
class MetronomeEditor : public AudioProcessorEditor, public Timer
{
  public:
	///	Constructor.
	MetronomeEditor(MetronomeProcessor *processor,
					const Rectangle<int>& windowBounds);
	///	Destructor.
	~MetronomeEditor();

	///	So we can resize the slider to match the size of the window.
	void resized();
	///	So we can paint the background a nice colour.
	void paint(Graphics& g);

	///	So we can update the window's bounds.
	void timerCallback();

	juce_UseDebuggingNewOperator
  private:
	///	The actual metronome controls.
	MetronomeControl *controls;

	///	The bounds of the parent window.
	Rectangle<int> parentBounds;

	///	True if we've updated the position of the window to its previous position.
	bool setPos;
};

#endif
