//	WaveformDisplay.cpp - A component that displays a waveform.
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

#include "WaveformDisplay.h"
#include "AudioSingletons.h"
#include "ColourScheme.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
WaveformDisplay::WaveformDisplay(AudioThumbnail *thumb, bool deleteThumb):
/*thumbnail(512,
		  AudioFormatManagerSingleton::getInstance(),
		  AudioThumbnailCacheSingleton::getInstance()),*/
readPointer(0.0f),
newReadPointer(0.0f),
backgroundColour(ColourScheme::getInstance().colours[L"Window Background"]),
deleteThumbnail(deleteThumb)
{
	if(thumb)
		thumbnail = thumb;
	else
	{
		thumbnail = new AudioThumbnail(512,
									   AudioFormatManagerSingleton::getInstance(),
									   AudioThumbnailCacheSingleton::getInstance());
	}
	thumbnail->addChangeListener(this);
}

//------------------------------------------------------------------------------
WaveformDisplay::~WaveformDisplay()
{
	//Necessary?
	thumbnail->removeChangeListener(this);

	if(deleteThumbnail)
		delete thumbnail;
}

//------------------------------------------------------------------------------
void WaveformDisplay::paint(Graphics& g)
{
	float x;
	String tempstr;
	const double length = thumbnail->getTotalLength();
	const Rectangle<int> tempRect(1, 1, getWidth()-2, getHeight()-2);

	g.fillAll(ColourScheme::getInstance().colours[L"Window Background"]);

	g.setColour(Colours::black.withAlpha(0.25f));
	g.drawRect(0, 0, getWidth(), getHeight());

	if(length > 0)
	{
		//Draw the thumbnail.
		g.setColour(ColourScheme::getInstance().colours[L"Waveform Colour"]);
		thumbnail->drawChannels(g, tempRect, 0.0, length, 1.0f);

		//Draw the readPointer.
		g.setColour(ColourScheme::getInstance().colours[L"Text Colour"].withAlpha(0.25f));
		x = 1.0f + (readPointer * (float)(getWidth()-2));
		g.drawLine(x, 1.0f, x, (float)(getHeight()-1), 1.0f);

		//Draw the length of the file.
		g.setColour(ColourScheme::getInstance().colours[L"Text Colour"].withAlpha(0.25f));
		g.setFont(10.0f);
		tempstr << (int)(length/60.0) << ":" << String(((int)length)%60).paddedLeft('0', 2);
		g.drawText(tempstr,
				   tempRect.getRight()-52,
				   tempRect.getY(),
				   50,
				   12,
				   Justification(Justification::right),
				   false);
	}
}

//------------------------------------------------------------------------------
void WaveformDisplay::changeListenerCallback(ChangeBroadcaster *source)
{
	repaint();
}

//------------------------------------------------------------------------------
void WaveformDisplay::mouseDown(const MouseEvent &e)
{
	float x = (float)e.x;

	if((x > 0.0f) && (x < (getWidth()-1)) && (thumbnail->getTotalLength() > 0))
	{
		newReadPointer = x/(float)(getWidth()-2);
		sendChangeMessage();
	}
}

//------------------------------------------------------------------------------
void WaveformDisplay::mouseDrag(const MouseEvent &e)
{
	float x = (float)e.x;

	if((x > 0.0f) && (x < (getWidth()-1)) && (thumbnail->getTotalLength() > 0))
	{
		newReadPointer = x/(float)(getWidth()-2);
		sendChangeMessage();
	}
}

//------------------------------------------------------------------------------
void WaveformDisplay::setFile(const File& file)
{
	AudioThumbnailCacheSingleton::getInstance().clear();
	thumbnail->setSource(new FileInputSource(file));
	if(file == File::nonexistent)
		thumbnail->reset(2, 44100.0);
	readPointer = 0.0f;
}

//------------------------------------------------------------------------------
void WaveformDisplay::setReadPointer(float val)
{
	readPointer = val;
	repaint();
}

//------------------------------------------------------------------------------
void WaveformDisplay::setBackgroundColour(const Colour& col)
{
	backgroundColour = col;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
WaveformDisplayLite::WaveformDisplayLite(AudioThumbnail& thumb):
thumbnail(thumb),
backgroundColour(ColourScheme::getInstance().colours[L"Window Background"])
{
	thumbnail.addChangeListener(this);
}

//------------------------------------------------------------------------------
WaveformDisplayLite::~WaveformDisplayLite()
{
	thumbnail.removeChangeListener(this);
}

//------------------------------------------------------------------------------
void WaveformDisplayLite::paint(Graphics& g)
{
	String tempstr;
	const double length = thumbnail.getTotalLength();
	const Rectangle<int> tempRect(1, 1, getWidth()-2, getHeight()-2);

	g.fillAll(ColourScheme::getInstance().colours[L"Window Background"]);

	g.setColour(Colours::black.withAlpha(0.25f));
	g.drawRect(0, 0, getWidth(), getHeight());

	if(length > 0)
	{
		//Draw the thumbnail.
		g.setColour(ColourScheme::getInstance().colours[L"Waveform Colour"]);
		thumbnail.drawChannels(g, tempRect, 0.0, length, 1.0f);

		//Draw the length of the file.
		g.setColour(ColourScheme::getInstance().colours[L"Text Colour"].withAlpha(0.25f));
		g.setFont(10.0f);
		tempstr << (int)(length/60.0) << ":" << String(((int)length)%60).paddedLeft('0', 2);
		g.drawText(tempstr,
				   tempRect.getRight()-52,
				   tempRect.getY(),
				   50,
				   12,
				   Justification(Justification::right),
				   false);
	}
}

//------------------------------------------------------------------------------
void WaveformDisplayLite::changeListenerCallback(ChangeBroadcaster *source)
{
	repaint();
}

//------------------------------------------------------------------------------
void WaveformDisplayLite::setBackgroundColour(const Colour& col)
{
	backgroundColour = col;
}
