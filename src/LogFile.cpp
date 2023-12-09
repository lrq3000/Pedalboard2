//	LogFile.cpp - Singleton used for logging events.
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

#include "LogFile.h"

//------------------------------------------------------------------------------
void LogFile::start()
{
	if(!isLogging)
	{
		const ScopedLock lock(critSec);

		String tempstr;
		Time timmy(Time::getCurrentTime());

		tempstr << "Pedalboard2LogFile-" << String(timmy.getDayOfMonth()).paddedLeft('0', 2) << ".";
		tempstr << String(timmy.getMonth()).paddedLeft('0', 2) << ".";
		tempstr << timmy.getYear() << ".";
		tempstr << String(timmy.getHours()).paddedLeft('0', 2) << ".";
		tempstr << String(timmy.getMinutes()).paddedLeft('0', 2) << ".";
		tempstr << String(timmy.getSeconds()).paddedLeft('0', 2) << ".txt";
		logFile = new FileOutputStream(File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile("Pedalboard2").getChildFile(tempstr));

		if(logFile->failedToOpen())
		{
			AlertWindow::showMessageBox(AlertWindow::WarningIcon,
										"LogFileError",
										"Could not open log file for writing");
		}
		else
			isLogging = true;
	}
}

//------------------------------------------------------------------------------
void LogFile::stop()
{
	if(isLogging)
	{
		const ScopedLock lock(critSec);

		FileOutputStream *tempStream;

		isLogging = false;
		logFile->flush();
		tempStream = logFile.release();

		events.clear();

		delete tempStream;
	}
}

//------------------------------------------------------------------------------
void LogFile::logEvent(const String& eventType, const String& message)
{
	if(isLogging)
	{
		Time timmy(Time::getCurrentTime());
		LogEvent tempEv;

		tempEv.eventType = eventType;

		tempEv.typeHash = DefaultHashFunctions::generateHash(eventType, 4096);

		tempEv.message << timmy.getHours() << ":";
		tempEv.message << String(timmy.getMinutes()).paddedLeft('0', 2) << ":";
		tempEv.message << String(timmy.getSeconds()).paddedLeft('0', 2) << ":";
		tempEv.message << String(timmy.getMilliseconds()).paddedLeft('0', 3) << " ";
		tempEv.message << eventType << ": ";
		tempEv.message << message;

		{
			const ScopedLock lock(critSec);

			events.insert(std::make_pair(timmy.toMilliseconds(), tempEv));

			(*logFile) << tempEv.message << newLine;
		}
		sendChangeMessage();
	}
}

//------------------------------------------------------------------------------
const String& LogFile::getLogContents(const StringArray& eventTypes,
									  Time& eventsSince)
{
	int i;
	std::map<int64, LogEvent>::iterator it;
	Array<int> typesHash;

	tempContents = String::empty;

	for(i=0;i<eventTypes.size();++i)
		typesHash.add(DefaultHashFunctions::generateHash(eventTypes[i], 4096));

	{
		const ScopedLock lock(critSec);

		for(it=events.lower_bound(eventsSince.toMilliseconds());
			it!=events.end();
			++it)
		{
			eventsSince = Time(it->first);

			for(i=0;i<typesHash.size();++i)
			{
				if(it->second.typeHash == typesHash[i])
				{
					tempContents << it->second.message << newLine;
					break;
				}
			}
		}
	}

	return tempContents;
}

//------------------------------------------------------------------------------
LogFile& LogFile::getInstance()
{
	static LogFile instance;

	return instance;
}

//------------------------------------------------------------------------------
LogFile::LogFile():
isLogging(false)
{

}

//------------------------------------------------------------------------------
LogFile::~LogFile()
{
	if(isLogging)
		logFile->flush();
}
