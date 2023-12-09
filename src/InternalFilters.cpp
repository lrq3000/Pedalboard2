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

#include "PedalboardProcessors.h"
#include "MidiMappingManager.h"
#include "OscMappingManager.h"
#include "InternalFilters.h"
#include "FilterGraph.h"

//==============================================================================
InternalPluginFormat::InternalPluginFormat()
{
    {
        AudioProcessorGraph::AudioGraphIOProcessor p (AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
        p.fillInPluginDescription (audioOutDesc);
    }

    {
        AudioProcessorGraph::AudioGraphIOProcessor p (AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);
        p.fillInPluginDescription (audioInDesc);
    }

    {
        AudioProcessorGraph::AudioGraphIOProcessor p (AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode);
        p.fillInPluginDescription (midiInDesc);
    }

    {
        MidiInterceptor p;
        p.fillInPluginDescription (midiInterceptorDesc);
    }

    {
        OscInput p;
        p.fillInPluginDescription (oscInputDesc);
    }

    {
        LevelProcessor p;
        p.fillInPluginDescription (levelProcDesc);
    }

    {
        FilePlayerProcessor p;
        p.fillInPluginDescription (filePlayerProcDesc);
    }

    {
        OutputToggleProcessor p;
        p.fillInPluginDescription (outputToggleProcDesc);
    }

    {
        VuMeterProcessor p;
        p.fillInPluginDescription (vuMeterProcDesc);
    }

    {
        RecorderProcessor p;
        p.fillInPluginDescription (recorderProcDesc);
    }

    {
        MetronomeProcessor p;
        p.fillInPluginDescription (metronomeProcDesc);
    }

    {
        LooperProcessor p;
        p.fillInPluginDescription (looperProcDesc);
    }
}

AudioPluginInstance* InternalPluginFormat::createInstanceFromDescription (const PluginDescription& desc)
{
    if (desc.name == audioOutDesc.name)
    {
        return new AudioProcessorGraph::AudioGraphIOProcessor (AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
    }
    else if (desc.name == audioInDesc.name)
    {
        return new AudioProcessorGraph::AudioGraphIOProcessor (AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);
    }
    else if (desc.name == midiInDesc.name)
    {
        return new AudioProcessorGraph::AudioGraphIOProcessor (AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode);
    }
    else if (desc.name == midiInterceptorDesc.name)
    {
        return new MidiInterceptor();
    }
    else if (desc.name == oscInputDesc.name)
    {
        return new OscInput();
    }
    else if (desc.name == levelProcDesc.name)
    {
        return new LevelProcessor();
    }
    else if (desc.name == filePlayerProcDesc.name)
    {
        return new FilePlayerProcessor();
    }
    else if (desc.name == outputToggleProcDesc.name)
    {
        return new OutputToggleProcessor();
    }
    else if (desc.name == vuMeterProcDesc.name)
    {
        return new VuMeterProcessor();
    }
    else if (desc.name == recorderProcDesc.name)
    {
        return new RecorderProcessor();
    }
    else if (desc.name == metronomeProcDesc.name)
    {
        return new MetronomeProcessor();
    }
    else if (desc.name == looperProcDesc.name)
    {
        return new LooperProcessor();
    }

    return 0;
}

const PluginDescription* InternalPluginFormat::getDescriptionFor (const InternalFilterType type)
{
    switch (type)
    {
    case audioInputFilter:
        return &audioInDesc;
    case audioOutputFilter:
        return &audioOutDesc;
    case midiInputFilter:
        return &midiInDesc;
	case midiInterceptorFilter:
		return &midiInterceptorDesc;
	case oscInputFilter:
		return &oscInputDesc;
	case levelProcFilter:
		return &levelProcDesc;
	case filePlayerProcFilter:
		return &filePlayerProcDesc;
	case outputToggleProcFilter:
		return &outputToggleProcDesc;
	case vuMeterProcFilter:
		return &vuMeterProcDesc;
	case recorderProcFilter:
		return &recorderProcDesc;
	case metronomeProcFilter:
		return &metronomeProcDesc;
	case looperProcFilter:
		return &looperProcDesc;
	default:
		return 0;
    }
}

void InternalPluginFormat::getAllTypes (OwnedArray <PluginDescription>& results)
{
    for (int i = 0; i < (int) endOfFilterTypes; ++i)
        results.add (new PluginDescription (*getDescriptionFor ((InternalFilterType) i)));
}
