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

#ifndef __JUCE_MAINHOSTWINDOW_JUCEHEADER__xxxxx
#define __JUCE_MAINHOSTWINDOW_JUCEHEADER__xxxxx

#include "FilterGraph.h"

#include "../JuceLibraryCode/JuceHeader.h"
using juce::uint32;


//==============================================================================
/**
    Manages the internal plugin types.
*/
class InternalPluginFormat   : public AudioPluginFormat
{
public:
    //==============================================================================
    InternalPluginFormat();
    ~InternalPluginFormat() {}

    //==============================================================================
    enum InternalFilterType
    {
        audioInputFilter = 0,
        audioOutputFilter,
        midiInputFilter,

		midiInterceptorFilter,
		oscInputFilter,
		levelProcFilter,
		filePlayerProcFilter,
		outputToggleProcFilter,
		vuMeterProcFilter,
		recorderProcFilter,
		metronomeProcFilter,
		looperProcFilter,

        endOfFilterTypes
    };

    const PluginDescription* getDescriptionFor (const InternalFilterType type);

    void getAllTypes (OwnedArray <PluginDescription>& results);

	bool canScanForPlugins() const {return false;};

    //==============================================================================
    String getName() const                                { return "Internal"; }
    bool fileMightContainThisPluginType (const String&)           { return false; }
    FileSearchPath getDefaultLocationsToSearch()          { return FileSearchPath(); }
    void findAllTypesForFile (OwnedArray <PluginDescription>&, const String&)     {}
    AudioPluginInstance* createInstanceFromDescription (const PluginDescription& desc);
    String getNameOfPluginFromIdentifier(const String& fileOrIdentifier) {return "Internal";}
    bool doesPluginStillExist (const PluginDescription& desc) {return true;}
    StringArray searchPathsForPlugins (const FileSearchPath& directoriesToSearch,
                                                     bool recursive) {StringArray retval; return retval;}

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    PluginDescription audioInDesc;
    PluginDescription audioOutDesc;
    PluginDescription midiInDesc;

	PluginDescription midiInterceptorDesc;
	PluginDescription oscInputDesc;
	PluginDescription levelProcDesc;
	PluginDescription filePlayerProcDesc;
	PluginDescription outputToggleProcDesc;
	PluginDescription vuMeterProcDesc;
	PluginDescription recorderProcDesc;
	PluginDescription metronomeProcDesc;
	PluginDescription looperProcDesc;
};


#endif
