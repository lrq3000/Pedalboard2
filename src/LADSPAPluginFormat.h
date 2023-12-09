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

#ifndef LADSPAPLUGINFORMAT_H_
#define LADSPAPLUGINFORMAT_H_

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
    Implements a plugin format manager for DirectX plugins.
*/
class LADSPAPluginFormat   : public AudioPluginFormat
{
public:
    //==============================================================================
    LADSPAPluginFormat();
    ~LADSPAPluginFormat();

    //==============================================================================
    const String getName() const                { return "LADSPA"; }
    void findAllTypesForFile (OwnedArray <PluginDescription>& results, const String& fileOrIdentifier);
    AudioPluginInstance* createInstanceFromDescription (const PluginDescription& desc);
    bool fileMightContainThisPluginType (const String& fileOrIdentifier);
    const String getNameOfPluginFromIdentifier (const String& fileOrIdentifier);
    const StringArray searchPathsForPlugins (const FileSearchPath& directoriesToSearch, bool recursive);
    bool doesPluginStillExist (const PluginDescription& desc);
    const FileSearchPath getDefaultLocationsToSearch();

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    LADSPAPluginFormat (const LADSPAPluginFormat&);
    const LADSPAPluginFormat& operator= (const LADSPAPluginFormat&);
    
    void recursiveFileSearch (StringArray& results, const File& dir, const bool recursive);
};

#endif
