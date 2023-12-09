//	ColourScheme.cpp - Singleton struct handling colour schemes.
//	----------------------------------------------------------------------------
//	This file is part of Pedalboard2, an audio plugin host.
//	Copyright (c) 2012 Niall Moody.
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

#include "ColourScheme.h"
#include "JuceHelperStuff.h"

using namespace std;

//------------------------------------------------------------------------------
ColourScheme& ColourScheme::getInstance()
{
	static ColourScheme retval;

	return retval;
}

//------------------------------------------------------------------------------
const StringArray ColourScheme::getPresets() const
{
	int i;
	Array<File> files;
	StringArray retval;
	File settingsDir = JuceHelperStuff::getAppDataFolder();

	settingsDir.findChildFiles(files, File::findFiles, false, L"*.colourscheme");
	for(i=0;i<files.size();++i)
		retval.add(files[i].getFileNameWithoutExtension());

	return retval;
}

//------------------------------------------------------------------------------
void ColourScheme::loadPreset(const String& name)
{
	String filename;
	File settingsDir = JuceHelperStuff::getAppDataFolder();

	filename << name << L".colourscheme";

	File presetFile = settingsDir.getChildFile(filename);

	if(presetFile.existsAsFile())
	{
		ScopedPointer<XmlElement> rootXml(XmlDocument::parse(presetFile));

		if(rootXml)
		{
			if(rootXml->hasTagName(L"Pedalboard2ColourScheme"))
			{
				forEachXmlChildElement(*rootXml, colour)
				{
					if(colour->hasTagName("Colour"))
					{
						String colName;
						String tempstr;

						colName = colour->getStringAttribute(L"name", L"NoName");
						tempstr = colour->getStringAttribute(L"value", L"FFFFFFFF");
						if(colName != L"NoName")
							colours[colName] = Colour(tempstr.getHexValue32());
					}
				}
				presetName = name;
			}
		}
	}
}

//------------------------------------------------------------------------------
void ColourScheme::savePreset(const String& name)
{
	String filename;
	map<String, Colour>::iterator it;
	XmlElement rootXml(L"Pedalboard2ColourScheme");
	File settingsDir = JuceHelperStuff::getAppDataFolder();

	filename << name << L".colourscheme";

	File presetFile = settingsDir.getChildFile(filename);

	for(it=colours.begin();it!=colours.end();++it)
	{
		XmlElement *colour = new XmlElement(L"Colour");

		colour->setAttribute(L"name", it->first);
		colour->setAttribute(L"value", it->second.toString());

		rootXml.addChildElement(colour);
	}

	presetName = name;
	rootXml.writeToFile(presetFile, L"");
}

//------------------------------------------------------------------------------
bool ColourScheme::doesColoursMatchPreset(const String& name)
{
	String tempstr;
	File presetFile;
	bool retval = true;
	File settingsDir = JuceHelperStuff::getAppDataFolder();

	tempstr << name << L".colourscheme";
	presetFile = settingsDir.getChildFile(tempstr);

	if(presetFile.existsAsFile())
	{
		ScopedPointer<XmlElement> rootXml(XmlDocument::parse(presetFile));

		if(rootXml)
		{
			if(rootXml->hasTagName(L"Pedalboard2ColourScheme"))
			{
				forEachXmlChildElement(*rootXml, colour)
				{
					if(colour->hasTagName("Colour"))
					{
						String colName;
						String value;

						colName = colour->getStringAttribute(L"name", L"NoName");
						value = colour->getStringAttribute(L"value", L"FFFFFFFF");

						if(colours[colName] != Colour(value.getHexValue32()))
						{
							retval = false;
							break;
						}
					}
				}
				presetName = name;
			}
		}
	}
	else
		retval = false;

	return retval;
}

//------------------------------------------------------------------------------
ColourScheme::ColourScheme()
{
	File defaultFile = JuceHelperStuff::getAppDataFolder().getChildFile(L"default.colourscheme");

	if(defaultFile.existsAsFile())
		loadPreset(L"default");
	else
	{
		presetName = L"default";

		colours[L"Window Background"] = Colour(0xFFEEECE1);
		colours[L"Field Background"] = Colour(Colour(0xFFEEECE1).brighter(0.5f));
		colours[L"Text Colour"] = Colour(0xFF000000);
		colours[L"Plugin Border"] = Colour(0xFFB0B0FF);
		colours[L"Plugin Background"] = Colour(0xFFFFFFFF);
		colours[L"Audio Connection"] = Colour(0xFFB0B0FF).darker(0.25f);
		colours[L"Parameter Connection"] = Colour(0xFFFFD3B3).darker(0.25f);
		colours[L"Button Colour"] = Colour(0xFFEEECE1);
		colours[L"Button Highlight"] = Colour(0xB0B0B0FF);
		colours[L"Text Editor Colour"] = Colour(0xFFFFFFFF);
		colours[L"Dialog Inner Background"] = Colour(0xFFFFFFFF);
		colours[L"CPU Meter Colour"] = Colour(0xB0B0B0FF);
		colours[L"Slider Colour"] = Colour(0xFF9A9181);
		colours[L"List Selected Colour"] = Colour(0xFFB0B0FF);
		colours[L"VU Meter Lower Colour"] = Colour(0x7F00BF00);
		colours[L"VU Meter Upper Colour"] = Colour(0x7FFFFF00);
		colours[L"VU Meter Over Colour"] = Colour(0x7FFF0000);
		colours[L"Vector Colour"] = Colour(0x80000000);
		colours[L"Menu Selection Colour"] = Colour(0x40000000);
		colours[L"Waveform Colour"] = Colour(0xFFB0B0FF);
		colours[L"Level Dial Colour"] = Colour(0xFFB0B0FF).darker(0.25f);
		colours[L"Tick Box Colour"] = Colour(0x809A9181);

		savePreset(L"default");
	}
}

//------------------------------------------------------------------------------
ColourScheme::~ColourScheme()
{

}
