/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "DefaultSynthAudioPlugin.h"
#include "BuiltInSynthsPluginFormat.h"

const String DefaultSynthAudioPlugin::instrumentId = "<default>";
const String DefaultSynthAudioPlugin::instrumentName = "Helio Default";

DefaultSynthAudioPlugin::DefaultSynthAudioPlugin()
{
    this->setPlayConfigDetails(0, 2, this->getSampleRate(), this->getBlockSize());
}

void DefaultSynthAudioPlugin::fillInPluginDescription(PluginDescription &description) const
{
    description.name = this->getName();
    description.descriptiveName = description.name;
    description.uniqueId = description.name.hashCode();
    description.category = "Synth";
    description.pluginFormatName = BuiltInSynthsPluginFormat::formatName;
    description.fileOrIdentifier = BuiltInSynthsPluginFormat::formatIdentifier;
    description.manufacturerName = "Built-in";
    description.version = "1.0";
    description.isInstrument = true;
    description.numInputChannels = this->getTotalNumInputChannels();
    description.numOutputChannels = this->getTotalNumOutputChannels();
}

const String DefaultSynthAudioPlugin::getName() const
{
    return DefaultSynthAudioPlugin::instrumentName;
}

void DefaultSynthAudioPlugin::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
    buffer.clear(0, buffer.getNumSamples());
    this->synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void DefaultSynthAudioPlugin::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    this->synth.setCurrentPlaybackSampleRate(sampleRate);
}

void DefaultSynthAudioPlugin::reset()
{
    this->synth.allNotesOff(0, true);
}

void DefaultSynthAudioPlugin::releaseResources() {}
double DefaultSynthAudioPlugin::getTailLengthSeconds() const { return 3.0; }

bool DefaultSynthAudioPlugin::acceptsMidi() const { return true; }
bool DefaultSynthAudioPlugin::producesMidi() const { return false; }

AudioProcessorEditor *DefaultSynthAudioPlugin::createEditor() { return nullptr; }
bool DefaultSynthAudioPlugin::hasEditor() const { return false; }

int DefaultSynthAudioPlugin::getNumPrograms() { return 0; }
int DefaultSynthAudioPlugin::getCurrentProgram() { return 0; }
void DefaultSynthAudioPlugin::setCurrentProgram(int index) {}
const String DefaultSynthAudioPlugin::getProgramName(int index) { return ""; }
void DefaultSynthAudioPlugin::changeProgramName(int index, const String &newName) {}

void DefaultSynthAudioPlugin::getStateInformation(MemoryBlock &destData) {}
void DefaultSynthAudioPlugin::setStateInformation(const void *data, int sizeInBytes) {}
