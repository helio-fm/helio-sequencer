/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "BuiltInSynthAudioPlugin.h"
#include "BuiltInSynthFormat.h"

//===----------------------------------------------------------------------===//
// AudioPluginInstance
//===----------------------------------------------------------------------===//

void BuiltInSynthAudioPlugin::fillInPluginDescription(PluginDescription &description) const
{
    description.name = this->getName();
    description.descriptiveName = description.name;
    description.uid = description.name.hashCode();
    description.category = "Synth";
    description.pluginFormatName = HELIO_BUILT_IN_PLUGIN_FORMAT_NAME;
    description.fileOrIdentifier = HELIO_BUILT_IN_PLUGIN_IDENTIFIER;
    description.manufacturerName = "Helio Workstation";
    description.version = "1.0";
    description.isInstrument = true;
    description.numInputChannels = this->getTotalNumInputChannels();
    description.numOutputChannels = this->getTotalNumOutputChannels();
}

//===----------------------------------------------------------------------===//
// AudioProcessor
//===----------------------------------------------------------------------===//

void BuiltInSynthAudioPlugin::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
    buffer.clear(0, buffer.getNumSamples());
    this->synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    //// This is the place where you'd normally do the guts of your plugin's
    //// audio processing...
    //for (int channel = 0; channel < getNumInputChannels(); ++channel)
    //{
    //    float *channelData = buffer.getSampleData(channel);
    //    // ..do something to the data...
    //}

    //// In case we have more outputs than inputs, we'll clear any output
    //// channels that didn't contain input data, (because these aren't
    //// guaranteed to be empty - they may contain garbage).
    //for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    //{
    //    buffer.clear(i, 0, buffer.getNumSamples());
    //}
}

void BuiltInSynthAudioPlugin::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    this->synth.setCurrentPlaybackSampleRate(sampleRate);
}

void BuiltInSynthAudioPlugin::releaseResources()
{
    // when playback stops
}

double BuiltInSynthAudioPlugin::getTailLengthSeconds() const
{
    return 1.0;
}

bool BuiltInSynthAudioPlugin::acceptsMidi() const
{
    return true;
}

bool BuiltInSynthAudioPlugin::producesMidi() const
{
    return false;
}


//===----------------------------------------------------------------------===//
// Editor
//===----------------------------------------------------------------------===//

AudioProcessorEditor *BuiltInSynthAudioPlugin::createEditor()
{
    return nullptr;
}

bool BuiltInSynthAudioPlugin::hasEditor() const
{
    return false;
}


//===----------------------------------------------------------------------===//
// Programs
//===----------------------------------------------------------------------===//

int BuiltInSynthAudioPlugin::getNumPrograms()
{
    return 0;
}

int BuiltInSynthAudioPlugin::getCurrentProgram()
{
    return 0;
}

void BuiltInSynthAudioPlugin::setCurrentProgram(int index) {}

const String BuiltInSynthAudioPlugin::getProgramName(int index)
{
    return "";
}

void BuiltInSynthAudioPlugin::changeProgramName(int index, const String &newName) {}


//===----------------------------------------------------------------------===//
// State
//===----------------------------------------------------------------------===//

void BuiltInSynthAudioPlugin::getStateInformation(MemoryBlock &destData) {}

void BuiltInSynthAudioPlugin::setStateInformation(const void *data, int sizeInBytes) {}
