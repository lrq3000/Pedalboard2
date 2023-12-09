//	TapTempoBox.cpp - Simple component letting the user tap the tempo.
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

#include "TapTempoBox.h"
#include "PluginField.h"
#include "ColourScheme.h"

#include <sstream>

//------------------------------------------------------------------------------
TapTempoBox::TapTempoBox(PluginField *field, TextEditor *tempoEd):
tempo(120.0),
pluginField(field),
tempoEditor(tempoEd)/*,
lastTicks(0)*/
{
	setSize(300, 120);

	startTimer(30);
}

//------------------------------------------------------------------------------
TapTempoBox::~TapTempoBox()
{

}

//------------------------------------------------------------------------------
void TapTempoBox::paint(Graphics& g)
{
	String tempstr;
	std::wstringstream converterString; //Because I can't figure out how to do this with juce' String class.
	Font smallFont(24.0f);
	Font bigFont(48.0f, Font::bold);
	
	g.setColour(ColourScheme::getInstance().colours[L"Text Colour"]);

	g.setFont(smallFont);
	g.drawText(L"Tap to set tempo:", 0, 0, 300, 50, Justification(Justification::centred), false);

	converterString.precision(2);
	converterString.fill(L'0');
	converterString << std::fixed << tempo;
	tempstr << converterString.str().c_str() << L" bpm";
	g.setFont(bigFont);
	g.drawText(tempstr, 0, 50, 300, 50, Justification(Justification::centred), false);
}

//------------------------------------------------------------------------------
void TapTempoBox::mouseDown(const MouseEvent &e)
{
	//int64 delta;
	//double seconds;
	double tempTempo;
	std::wstringstream converterString;
	int64 ticks = Time::getHighResolutionTicks();

	tempTempo = tapHelper.updateTempo(Time::highResolutionTicksToSeconds(ticks));
	if(tempTempo > 0.0)
	{
		tempo = tempTempo;
		pluginField->setTempo(tempo);

		converterString.precision(2);
		converterString.fill(L'0');
		converterString << std::fixed << tempo;
		tempoEditor->setText(converterString.str().c_str(), false);

		repaint();
	}

	/*if(lastTicks > 0)
	{
		delta = ticks - lastTicks;

		seconds = Time::highResolutionTicksToSeconds(delta);
		if(seconds > 0.0)
		{
			tempo = (1.0f/seconds) * 60.0;
			pluginField->setTempo(tempo);

			converterString.precision(2);
			converterString.fill(L'0');
			converterString << std::fixed << tempo;
			tempoEditor->setText(converterString.str().c_str(), false);

			repaint();
		}
	}
	lastTicks = ticks;*/
}

//------------------------------------------------------------------------------
void TapTempoBox::timerCallback()
{
	double newTempo = pluginField->getTempo();

	if(tempo != newTempo)
	{
		tempo = newTempo;
		repaint();
	}
}
