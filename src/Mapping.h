//	Mapping.h - The various mapping classes.
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

#ifndef MAPPING_H_
#define MAPPING_H_

#include "../JuceLibraryCode/JuceHeader.h"

class FilterGraph;

//------------------------------------------------------------------------------
///	The base class for all parameter mappings.
class Mapping
{
  public:
	///	Constructor.
	/*!
		\param graph The FilterGraph this mapping exists in.
		\param pluginId The uid of the plugin in the FilterGraph that this
		mapping applies to.
		\param param The index of the plugin parameter which is being mapped to
		an input.
	 */
	Mapping(FilterGraph *graph, uint32 pluginId, int param);
	///	Constructor to load Mapping parameters from an XmlElement.
	Mapping(FilterGraph *graph, XmlElement *e);
	///	Destructor.
	virtual ~Mapping();

	///	Returns an XmlElement representing this Mapping.
	virtual XmlElement *getXml() const = 0;

	///	Returns the id of the plugin this mapping applies to.
	uint32 getPluginId() const {return plugin;};
	///	Returns the index of the plugin parameter this mapping applies to.
	int getParameter() const {return parameter;};

	///	Sets this mapping's parameter.
	void setParameter(int val);

	juce_UseDebuggingNewOperator
  protected:
	///	Called from subclasses to update their parameter.
	void updateParameter(float val);
  private:
	///	The FilterGraph this mapping exists in.
	FilterGraph *filterGraph;
	///	The uid of the plugin this mapping applies to.
	uint32 plugin;
	///	The index of the plugin parameter which is being mapped to an input.
	int parameter;
};

#endif
