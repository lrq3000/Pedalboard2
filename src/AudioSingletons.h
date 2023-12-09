//	AudioSingletons.h - A couple of singleton wrappers to some of JUCE' audio
//						classes.
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

#ifndef AUDIOSINGLETONS_H_
#define AUDIOSINGLETONS_H_

#include "../JuceLibraryCode/JuceHeader.h"

//------------------------------------------------------------------------------
///	Singleton wrapper for AudioFormatManager.
class AudioPluginFormatManagerSingleton
{
  public:
	///	Returns the sole instance of ApplicationProperties.
	static AudioPluginFormatManager& getInstance();
	///	Kills the ApplicationProperties instance.
	/*!
		****ONLY CALL THIS ONCE, WHEN YOU'RE CLOSING THE PROGRAM.****
	 */
	static void killInstance();

	juce_UseDebuggingNewOperator
  private:
	///	Constructor.
	AudioPluginFormatManagerSingleton();
	///	Destructor.
	~AudioPluginFormatManagerSingleton();

	///	The global AudioFormatManagerSingleton instance.
	static AudioPluginFormatManagerSingleton singletonInstance;
	///	The global AudioFormatManager instance.
	AudioPluginFormatManager *instance;
};

//------------------------------------------------------------------------------
///	Singleton wrapper for AudioFormatManager.
class AudioFormatManagerSingleton
{
  public:
	///	Returns the sole instance of ApplicationProperties.
	static AudioFormatManager& getInstance();
	///	Kills the ApplicationProperties instance.
	/*!
		****ONLY CALL THIS ONCE, WHEN YOU'RE CLOSING THE PROGRAM.****
	 */
	static void killInstance();

	juce_UseDebuggingNewOperator
  private:
	///	Constructor.
	AudioFormatManagerSingleton();
	///	Destructor.
	~AudioFormatManagerSingleton();

	///	The global AudioFormatManagerSingleton instance.
	static AudioFormatManagerSingleton singletonInstance;
	///	The global AudioFormatManager instance.
	AudioFormatManager *instance;
};

//------------------------------------------------------------------------------
///	Singleton wrapper for AudioThumbnailCache.
class AudioThumbnailCacheSingleton
{
  public:
	///	Returns the sole instance of ApplicationProperties.
	static AudioThumbnailCache& getInstance();
	///	Kills the ApplicationProperties instance.
	/*!
		****ONLY CALL THIS ONCE, WHEN YOU'RE CLOSING THE PROGRAM.****
	 */
	static void killInstance();

	juce_UseDebuggingNewOperator
  private:
	///	Constructor.
	AudioThumbnailCacheSingleton();
	///	Destructor.
	~AudioThumbnailCacheSingleton();

	///	The global AudioThumbnailCacheSingleton instance.
	static AudioThumbnailCacheSingleton singletonInstance;
	///	The global AudioThumbnailCacheManager instance.
	AudioThumbnailCache *instance;
};

#endif
