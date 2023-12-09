//	LogFile.h - Singleton used for logging events.
//	----------------------------------------------------------------------------
//	This file is part of Pedalboard2, an audio plugin host.
//	Copyright (c) 2013 Niall Moody.
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

#ifndef LOGFILE_H_
#define LOGFILE_H_

#include "../JuceLibraryCode/JuceHeader.h"

#include <map>

///	Singleton used for logging events.
class LogFile : public ChangeBroadcaster
{
  public:
	///	Called to start logging (creates a date-stamped file).
	void start();
	///	Called to stop logging (closes the opened log file).
	void stop();
	///	Returns true if we are currently logging events.
	bool getIsLogging() const {return isLogging;};

	///	Call this to log an event.
	/*!
		\param eventType Which type of event this is (MIDI, OSC, Pedalboard).
		\param message The message to write.

		Events will appear in the log like:
		\code
		12:00:00:000 MIDI: Note on (v:127 n:64).
		\endcode
		i.e. <time (hrs:mins:secs:millisecs)> <eventType>: <message>
	 */
	void logEvent(const String& eventType, const String& message);

	///	Returns the contents of the log file.
	/*!
		\param eventTypes This should hold the names of all the event types
		you're interested in, and can be used to filter out event types
		you're not interested in.
		\param eventsSince This can be used to only return events which have
		occurred since this time. Just pass in a Time initialised with its
		default constructor if you want to see the entire log. On return this
		will be set to the time of the latest event in the log.
	 */
	const String& getLogContents(const StringArray& eventTypes,
								 Time& eventsSince);

	///	Returns the sole instance of the LogFile.
	static LogFile& getInstance();
  private:
	///	Constructor.
	LogFile();
	///	Destructor.
	~LogFile();

	///	Struct representing a single event in the log.
	struct LogEvent
	{
		///	The event's type.
		String eventType;
		///	The event's type, hashed.
		/*!
			So we're not doing tons of String comparisons every time we call
			getLogContents().
		 */
		int typeHash;
		///	The event's message.
		/*!
			This is the full text, i.e.:
			<time> <type> <message>
		 */
		String message;
	};

	///	The log file output stream.
	ScopedPointer<FileOutputStream> logFile;
	///	True if we're currently logging events.
	bool isLogging;

	///	All the events in the log file.
	std::map<int64, LogEvent> events;

	///	Used to protect events from threading issues.
	CriticalSection critSec;

	///	Temporary variable used by getLogContents().
	String tempContents;
};

#endif
