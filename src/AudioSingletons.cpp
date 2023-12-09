//	AudioSingletons.cpp - A couple of singleton wrappers to some of JUCE' audio
//						  classes.
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

#include "AudioSingletons.h"

#include <cassert>

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AudioPluginFormatManagerSingleton AudioPluginFormatManagerSingleton::singletonInstance;

//------------------------------------------------------------------------------
AudioPluginFormatManager& AudioPluginFormatManagerSingleton::getInstance()
{
	return *(singletonInstance.instance);
}

//------------------------------------------------------------------------------
void AudioPluginFormatManagerSingleton::killInstance()
{
	assert(singletonInstance.instance != 0);

	if(singletonInstance.instance)
	{
		delete singletonInstance.instance;
		singletonInstance.instance = 0;
	}
}

//------------------------------------------------------------------------------
AudioPluginFormatManagerSingleton::AudioPluginFormatManagerSingleton():
instance(0)
{
	instance = new AudioPluginFormatManager();
	instance->addDefaultFormats();
}

//------------------------------------------------------------------------------
AudioPluginFormatManagerSingleton::~AudioPluginFormatManagerSingleton()
{
	if(instance)
	{
		assert(1);
		delete instance;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AudioFormatManagerSingleton AudioFormatManagerSingleton::singletonInstance;

//------------------------------------------------------------------------------
AudioFormatManager& AudioFormatManagerSingleton::getInstance()
{
	return *(singletonInstance.instance);
}

//------------------------------------------------------------------------------
void AudioFormatManagerSingleton::killInstance()
{
	assert(singletonInstance.instance != 0);

	if(singletonInstance.instance)
	{
		delete singletonInstance.instance;
		singletonInstance.instance = 0;
	}
}

//------------------------------------------------------------------------------
AudioFormatManagerSingleton::AudioFormatManagerSingleton():
instance(0)
{
	instance = new AudioFormatManager();
	instance->registerBasicFormats();
}

//------------------------------------------------------------------------------
AudioFormatManagerSingleton::~AudioFormatManagerSingleton()
{
	if(instance)
	{
		assert(1);
		delete instance;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
AudioThumbnailCacheSingleton AudioThumbnailCacheSingleton::singletonInstance;

//------------------------------------------------------------------------------
AudioThumbnailCache& AudioThumbnailCacheSingleton::getInstance()
{
	return *(singletonInstance.instance);
}

//------------------------------------------------------------------------------
void AudioThumbnailCacheSingleton::killInstance()
{
	assert(singletonInstance.instance != 0);

	if(singletonInstance.instance)
	{
		delete singletonInstance.instance;
		singletonInstance.instance = 0;
	}
}

//------------------------------------------------------------------------------
AudioThumbnailCacheSingleton::AudioThumbnailCacheSingleton():
instance(0)
{
	instance = new AudioThumbnailCache(32);
}

//------------------------------------------------------------------------------
AudioThumbnailCacheSingleton::~AudioThumbnailCacheSingleton()
{
	if(instance)
	{
		assert(1);
		delete instance;
	}
}
