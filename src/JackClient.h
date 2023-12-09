//	JackClient.h - Class representing a Jack client.
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

#ifndef JACKCLIENT_H_
#define JACKCLIENT_H_

#include <jack/jack.h>
#include <vector>
#include <string>

//------------------------------------------------------------------------------
///	Abstract base class for anything wanting to receive Jack callbacks.
class JackCallback
{
  public:
	///	Constructor.
	JackCallback() {};
	///	Destructor.
	virtual ~JackCallback() {};

	///	Audio processing callback.
	virtual int processAudio(int numSamples) = 0;
	///	Called when the Jack server shuts down.
	virtual void serverShutdown() {};
	///	Called when the audio buffer size is about to change.
	virtual void bufferSizeChanged(int numSamples) {};
	///	Called when the samplerate changes.
	virtual void samplerateChanged(int newRate) {};
};

//------------------------------------------------------------------------------
///	Class representing a Jack client.
class JackClient
{
  public:
	///	Constructor.
	JackClient();
	///	Destructor.
	~JackClient();

	///	Sets the callback to use.
	void setCallback(JackCallback *call);

	///	Sets the name of our client.
	void setName(const std::string& val);
	///	Sets the number of input ports to use.
	/*!
		Set to 1 by default.
	 */
	void setNumInputs(int num);
	///	Sets the number of output ports to use.
	/*!
		Set to 1 by default.
	 */
	void setNumOutputs(int num);

	///	Starts audio processing.
	/*!
		\return True if successful, false otherwise.
	 */
	bool start();
	///	Stops audio processing.
	void stop();

	///	Returns the input ports we're using.
	std::vector<jack_port_t *>& getInputs() {return inputs;};
	///	Returns the output ports we're using.
	std::vector<jack_port_t *>& getOutputs() {return outputs;};
	///	Returns the callback.
	JackCallback *getCallback() {return callback;};

	///	Returns the current samplerate.
	/*!
		Will only work if the client's currently running.
	 */
	int getSamplerate() const {return jack_get_sample_rate(jackClient);};
	/// Returns the current (max) buffer size.
	int getBufferSize() const {return bufferSize;};
	///	Returns the indexed input buffer.
	/*!
		\param index Which input port to get the buffer for.
		\param numSamples The size of the buffer to get.

		This should only be called from JackCallback::processAudio(), and will return
		a pointer to an array of samples.
	 */
	float *getInputBuffer(int index, int numSamples);
	///	Returns the indexed output buffer.
	/*!
		\param index Which output port to get the buffer for.
		\param numSamples The size of the buffer to get.

		This should only be called from JackCallback::processAudio(), and will return
		a pointer to an array of samples.
	 */
	float *getOutputBuffer(int index, int numSamples);

	///	Audio callback function.
	static int jackCallback(jack_nframes_t nframes, void *arg);
	///	Shutdown callback function.
	static void jackShutdown(void *arg);
	///	Buffer size changed function.
	static int jackBufferSize(jack_nframes_t nframes, void *arg);
	///	Samplerate changed function.
	static int jackSamplerate(jack_nframes_t nframes, void *arg);
  private:
	///	The callback to use.
	JackCallback *callback;

	///	The name of our client.
	std::string name;
	///	The number of input ports.
	int numInputs;
	///	The number of output ports.
	int numOutputs;

	///	Whether or not we're currently running.
	bool running;

	///	Our Jack reference.
	jack_client_t *jackClient;
	///	Our Jack input ports.
	std::vector<jack_port_t *> inputs;
	///	Out Jack output ports.
	std::vector<jack_port_t *> outputs;

	/// The buffer size before activation.
	int bufferSize;
};

#endif

