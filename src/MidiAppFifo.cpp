//	MidiAppFifo.cpp - A lock-free FIFO used to pass messages from the audio
//					  thread to the message thread.
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

#include "MidiAppFifo.h"

//------------------------------------------------------------------------------
MidiAppFifo::MidiAppFifo():
idFifo(BufferSize),
tempoFifo(BufferSize),
patchChangeFifo(BufferSize)
{
	int i;

	for(i=0;i<BufferSize;++i)
	{
		idBuffer[i] = 0;
		tempoBuffer[i] = 0;
		patchChangeBuffer[i] = 0;
	}
}

//------------------------------------------------------------------------------
MidiAppFifo::~MidiAppFifo()
{

}

//------------------------------------------------------------------------------
void MidiAppFifo::writeID(CommandID id)
{
	int start1, size1, start2, size2;

    idFifo.prepareToWrite(1, start1, size1, start2, size2);

    if(size1 > 0)
		idBuffer[start1] = id;
    else if(size2 > 0)
		idBuffer[start2] = id;

    idFifo.finishedWrite(size1 + size2);
}

//------------------------------------------------------------------------------
CommandID MidiAppFifo::readID()
{
	int start1, size1, start2, size2;
	CommandID retval = -1;

    idFifo.prepareToRead(1, start1, size1, start2, size2);

    if(size1 > 0)
		retval = idBuffer[start1];
    else if(size2 > 0)
        retval = idBuffer[start2];

    idFifo.finishedRead(size1 + size2);

	return retval;
}

//------------------------------------------------------------------------------
void MidiAppFifo::writeTempo(double tempo)
{
	int start1, size1, start2, size2;

    tempoFifo.prepareToWrite(1, start1, size1, start2, size2);

    if(size1 > 0)
		tempoBuffer[start1] = tempo;
    else if(size2 > 0)
		tempoBuffer[start2] = tempo;

    tempoFifo.finishedWrite(size1 + size2);
}

//------------------------------------------------------------------------------
double MidiAppFifo::readTempo()
{
	int start1, size1, start2, size2;
	double retval = 120.0;

    tempoFifo.prepareToRead(1, start1, size1, start2, size2);

    if(size1 > 0)
		retval = tempoBuffer[start1];
    else if(size2 > 0)
        retval = tempoBuffer[start2];

    tempoFifo.finishedRead(size1 + size2);

	return retval;
}

//------------------------------------------------------------------------------
void MidiAppFifo::writePatchChange(int tempo)
{
	int start1, size1, start2, size2;

    patchChangeFifo.prepareToWrite(1, start1, size1, start2, size2);

    if(size1 > 0)
		patchChangeBuffer[start1] = tempo;
    else if(size2 > 0)
		patchChangeBuffer[start2] = tempo;

    patchChangeFifo.finishedWrite(size1 + size2);
}

//------------------------------------------------------------------------------
int MidiAppFifo::readPatchChange()
{
	int start1, size1, start2, size2;
	int retval = 0;

	patchChangeFifo.prepareToRead(1, start1, size1, start2, size2);

    if(size1 > 0)
		retval = patchChangeBuffer[start1];
    else if(size2 > 0)
        retval = patchChangeBuffer[start2];

    patchChangeFifo.finishedRead(size1 + size2);

	return retval;
}
