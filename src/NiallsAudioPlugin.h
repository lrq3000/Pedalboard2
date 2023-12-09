//	NiallsAudioPlugin.h - Base class for NAPs.
//	----------------------------------------------------------------------------
//	This file is part of Pedalboard2, an audio plugin host.
//	Copyright (c) 2009 Niall Moody.
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

#ifndef NIALLSAUDIOPLUGIN_H_
#define NIALLSAUDIOPLUGIN_H_

#include <string>

namespace juce
{
class AudioProcessorEditor;
};

///	Base class for NAPs.
/*!
	The only real difference between my NAP format and other plugin formats is
	that with NAPs, parameters get modulated at audio rate, and are just treated
	as extra audio inputs.

	In addition to a NiallsAudioPlugin subclass, a NAP library should have a 
	single creator function of the form:
\code
NiallsAudioPlugin *createPlugin()
{
	return new ExamplePlugin();
}
\endcode
	This will be used to load the plugin by the host.
 */
class NiallsAudioPlugin
{
  public:
	///	Constructor.
	NiallsAudioPlugin(int numAudioInputs,
					  int numAudioOutputs,
					  int numParameters,
					  const std::string& pluginName);
	///	Destructor.
	virtual ~NiallsAudioPlugin();

	///	Where all the audio processing takes place.
	/*!
		\param input This array will contain the audio input data, followed by
		the parameter input data.
	 */
	virtual void processAudio(float **input, float **output, int numSamples) = 0;

	///	Used to create an editor for the plugin.
	/*!
		\return A JUCE Component.  This limits NAP editors to being JUCE-based,
		but I don't have any need to support other toolkits.
	 */
	virtual juce::AudioProcessorEditor *createEditor() = 0;
	///	Returns true if the plugin has an editor.
	virtual bool hasEditor() const {return false;};

	///	Returns the plugin's name.
	const std::string& getName() const {return name;};
	///	Returns the number of audio inputs.
	int getNumAudioInputs() const {return numInputs;};
	///	Returns the number of audio outputs.
	int getNumAudioOutputs() const {return numOutputs;};
	///	Returns the number of parameters.
	int getNumParameters() const {return numParameters;};

	///	Returns the name of the indexed audio Input.
	const std::string& getInputName(int index) const {return inputNames[index];};
	///	Returns the name of the indexed audio Output.
	const std::string& getOutputName(int index) const {return outputNames[index];};
	///	Returns the name of the indexed parameter.
	const std::string& getParameterName(int index) const {return parameterNames[index];};
  protected:
	///	Sets the indexed inputName.
	void setInputName(int index, const std::string& inputName);
	///	Sets the indexed outputName.
	void setOutputName(int index, const std::string& outputName);
	///	Sets the indexed parameterName.
	void setParameterName(int index, const std::string& parameterName);
  private:
	///	The plugin's name.
	std::string name;
	///	The number of audio inputs.
	int numInputs;
	///	The number of audio outputs.
	int numOutputs;
	///	The number of parameters.
	int numParameters;

	///	The names of the audio inputs.
	std::string *inputNames;
	///	The names of the audio outputs.
	std::string *outputNames;
	///	The names of the parameters.
	std::string *parameterNames;
};

#endif

