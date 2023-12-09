//	PropertiesSingleton.cpp - Because Jules changed ApplicationProperties to no
//							  longer be a singleton, and that causes big problems.
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

#include "PropertiesSingleton.h"

#include <cassert>

//------------------------------------------------------------------------------
PropertiesSingleton PropertiesSingleton::singletonInstance;

//------------------------------------------------------------------------------
ApplicationProperties& PropertiesSingleton::getInstance()
{
	return *(singletonInstance.instance);
}

//------------------------------------------------------------------------------
void PropertiesSingleton::killInstance()
{
	assert(singletonInstance.instance != 0);

	if(singletonInstance.instance)
	{
		delete singletonInstance.instance;
		singletonInstance.instance = 0;
	}
}

//------------------------------------------------------------------------------
PropertiesSingleton::PropertiesSingleton():
instance(0)
{
	instance = new ApplicationProperties();
}

//------------------------------------------------------------------------------
PropertiesSingleton::~PropertiesSingleton()
{
	if(instance)
	{
		assert(1);
		delete instance;
	}
}
