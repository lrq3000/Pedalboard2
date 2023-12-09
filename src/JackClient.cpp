//	JackClient.cpp - Class representing a Jack client.
//	----------------------------------------------------------------------------
//	This file is part of FourTrack, a simple Jack-based 4 track looper.
//	Copyright (c) 2007 Niall Moody.
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

#include "JackClient.h"

#include <iostream>
#include <sstream>

using namespace std;

//------------------------------------------------------------------------------
JackClient::JackClient():
callback(0),
name("JackClient"),
numInputs(1),
numOutputs(1),
running(false),
jackClient(0),
bufferSize(0)
{
	
}

//------------------------------------------------------------------------------
JackClient::~JackClient()
{
	if(running)
		stop();
}

//------------------------------------------------------------------------------
void JackClient::setCallback(JackCallback *call)
{
	callback = call;
}

//------------------------------------------------------------------------------
void JackClient::setName(const string& val)
{
	name = val;
}

//------------------------------------------------------------------------------
void JackClient::setNumInputs(int num)
{
	numInputs = num;
}

//------------------------------------------------------------------------------
void JackClient::setNumOutputs(int num)
{
	numOutputs = num;
}

//------------------------------------------------------------------------------
bool JackClient::start()
{
	int i;
	bool retval = false;

	if(!running && callback)
	{
		//Connect to server.
		jackClient = jack_client_new(name.c_str());
		if(!jackClient)
			cout << "JackClient: Could not connect to Jack Server." << endl;
		else
		{
			//Setup the various callbacks.
			jack_set_process_callback(jackClient,
									  jackCallback,
									  callback);
			jack_on_shutdown(jackClient,
							 jackShutdown,
							 callback);
			jack_set_buffer_size_callback(jackClient,
										  jackBufferSize,
										  callback);
			jack_set_sample_rate_callback(jackClient,
										  jackSamplerate,
										  callback);

			//Setup input ports.
			for(i=0;i<numInputs;++i)
			{
				stringstream tempstr;
				jack_port_t *input;

				tempstr << "Track " << (i+1) << " In";
				input = jack_port_register(jackClient,
										   tempstr.str().c_str(),
										   JACK_DEFAULT_AUDIO_TYPE,
										   JackPortIsInput,
										   0);
				inputs.push_back(input);
			}

			//Setup output ports.
			//for(i=0;i<numOutputs-1;++i)
			for(i=0;i<numOutputs;++i)
			{
				stringstream tempstr;
				jack_port_t *output;

				tempstr << "Track " << (i+1) << " Out";
				output = jack_port_register(jackClient,
										    tempstr.str().c_str(),
										    JACK_DEFAULT_AUDIO_TYPE,
										    JackPortIsOutput,
										    0);
				outputs.push_back(output);
			}
			//Metronome output.
			/*{
				jack_port_t *output;

				output = jack_port_register(jackClient,
										    "Metronome Out",
										    JACK_DEFAULT_AUDIO_TYPE,
										    JackPortIsOutput,
										    0);
				outputs.push_back(output);
			}*/

			bufferSize = jack_get_buffer_size(jackClient);

			//Start running.
			if(jack_activate(jackClient))
			{
				cout << "JackClient: Could not activate." << endl;
				for(i=0;i<numInputs;++i)
					jack_port_unregister(jackClient, inputs[i]);
				for(i=0;i<numOutputs;++i)
					jack_port_unregister(jackClient, outputs[i]);
				inputs.clear();
				outputs.clear();
			}
			else
			{
				//Connect our ports to hardware ports if possible(?).
				

				running = true;
				retval = true;
			}
		}
	}
	else if(running)
		cout << "JackClient: Already running." << endl;
	else if(!callback)
		cout << "JackClient: No callback." << endl;

	return retval;
}

//------------------------------------------------------------------------------
void JackClient::stop()
{
	int i;

	if(running)
	{
		for(i=0;i<numInputs;++i)
			jack_port_unregister(jackClient, inputs[i]);
		for(i=0;i<numOutputs;++i)
			jack_port_unregister(jackClient, outputs[i]);
		inputs.clear();
		outputs.clear();
		jack_client_close(jackClient);
		running = false;
	}
}

//------------------------------------------------------------------------------
float *JackClient::getInputBuffer(int index, int numSamples)
{
	float *retval = 0;

	if(inputs.size() > 0)
		retval = (float *)jack_port_get_buffer(inputs[index], numSamples);

	return retval;
}

//------------------------------------------------------------------------------
float *JackClient::getOutputBuffer(int index, int numSamples)
{
	float *retval = 0;

	if(outputs.size() > 0)
		retval = (float *)jack_port_get_buffer(outputs[index], numSamples);

	return retval;
}

//------------------------------------------------------------------------------
int JackClient::jackCallback(jack_nframes_t nframes, void *arg)
{
	int retval = 1;

	if(arg)
	{
		JackCallback *callback = (JackCallback *)arg;

		callback->processAudio(nframes);

		retval = 0;
	}

	return retval;
}

//------------------------------------------------------------------------------
void JackClient::jackShutdown(void *arg)
{
	if(arg)
	{
		JackCallback *callback = (JackCallback *)arg;

		callback->serverShutdown();
	}
}

//------------------------------------------------------------------------------
int JackClient::jackBufferSize(jack_nframes_t nframes, void *arg)
{
	int retval = 1;

	if(arg)
	{
		JackCallback *callback = (JackCallback *)arg;

		callback->bufferSizeChanged(nframes);

		retval = 0;
	}

	return retval;
}

//------------------------------------------------------------------------------
int JackClient::jackSamplerate(jack_nframes_t nframes, void *arg)
{
	int retval = 1;

	if(arg)
	{
		JackCallback *callback = (JackCallback *)arg;

		callback->samplerateChanged(nframes);

		retval = 0;
	}

	return retval;
}

