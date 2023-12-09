//	PropertiesSingleton.h - Because Jules changed ApplicationProperties to no
//							longer be a singleton, and that causes big problems.
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

#ifndef PROPERTIESSINGLETON_H_
#define PROPERTIESSINGLETON_H_

#include "../JuceLibraryCode/JuceHeader.h"

///	Because Jules changed ApplicationProperties to no longer be a singleton, and that causes big problems.
class PropertiesSingleton
{
  public:
	///	Returns the sole instance of ApplicationProperties.
	static ApplicationProperties& getInstance();
	///	Kills the ApplicationProperties instance.
	/*!
		****ONLY CALL THIS ONCE, WHEN YOU'RE CLOSING THE PROGRAM.****
	 */
	static void killInstance();

	juce_UseDebuggingNewOperator
  private:
	///	Constructor.
	PropertiesSingleton();
	///	Destructor.
	~PropertiesSingleton();

	///	The global PropertiesSingleton instance.
	static PropertiesSingleton singletonInstance;
	///	The global ApplicationProperties instance.
	ApplicationProperties *instance;
};

#endif
