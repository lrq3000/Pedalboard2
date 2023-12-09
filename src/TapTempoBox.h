

#ifndef TAPTEMPOBOX_H_
#define TAPTEMPOBOX_H_

#include "TapTempoHelper.h"

#include "../JuceLibraryCode/JuceHeader.h"

class PluginField;

///	Simple component letting the user tap the tempo.
class TapTempoBox : public Component,
					public Timer
{
  public:
	///	Constructor.
	TapTempoBox(PluginField *field, TextEditor *tempoEd);
	///	Destructor.
	~TapTempoBox();

	///	Draws the box.
	void paint(Graphics& g);

	///	Used to tap the tempo.
	void mouseDown(const MouseEvent &e);

	///	Updates the current tempo.
	void timerCallback();
  private:
	///	The current tempo.
	double tempo;

	///	The PluginField we get the tempo from.
	PluginField *pluginField;
	///	The tempo TextEditor to update with the new tempo.
	TextEditor *tempoEditor;

	///	The previous number of ticks.
	//int64 lastTicks;

	///	Used to calculate the tempo.
	TapTempoHelper tapHelper;
};

#endif
