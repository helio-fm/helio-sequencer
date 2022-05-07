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
#include "MetronomeSynthAudioPlugin.h"
#include "BuiltInSynthsPluginFormat.h"

const String MetronomeSynthAudioPlugin::instrumentId = "<metronome>";
const String MetronomeSynthAudioPlugin::instrumentName = "Metronome";

MetronomeSynthAudioPlugin::MetronomeSynthAudioPlugin()
{
    this->setPlayConfigDetails(0, 2, this->getSampleRate(), this->getBlockSize());
}

void MetronomeSynthAudioPlugin::fillInPluginDescription(PluginDescription &description) const
{
    description.name = this->getName();
    description.descriptiveName = description.name;
    description.uniqueId = description.name.hashCode();
    description.category = "Synth";
    description.pluginFormatName = BuiltInSynthsPluginFormat::formatName;
    description.fileOrIdentifier = BuiltInSynthsPluginFormat::formatIdentifier;
    description.manufacturerName = "Helio Workstation";
    description.version = "1.0";
    description.isInstrument = true;
    description.numInputChannels = this->getTotalNumInputChannels();
    description.numOutputChannels = this->getTotalNumOutputChannels();
}

const String MetronomeSynthAudioPlugin::getName() const
{
    return MetronomeSynthAudioPlugin::instrumentName;
}

void MetronomeSynthAudioPlugin::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
    if (this->synth.getNumSounds() == 0 && midiMessages.getNumEvents() > 0)
    {
        // Initialization takes time, i.e. slows app loading way down,
        // and consumes RAM, although user might never use the built-in metronome,
        // so let's do lazy initialization on the first use, here in processBlock:
        this->synth.initVoices();
        this->synth.initSampler();
    }

    buffer.clear(0, buffer.getNumSamples());
    this->synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void MetronomeSynthAudioPlugin::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    this->synth.setCurrentPlaybackSampleRate(sampleRate);
}

void MetronomeSynthAudioPlugin::reset()
{
    this->synth.allNotesOff(0, true);
}

void MetronomeSynthAudioPlugin::releaseResources() {}
double MetronomeSynthAudioPlugin::getTailLengthSeconds() const { return 3.0; }

bool MetronomeSynthAudioPlugin::acceptsMidi() const { return true; }
bool MetronomeSynthAudioPlugin::producesMidi() const { return false; }

AudioProcessorEditor *MetronomeSynthAudioPlugin::createEditor() { return nullptr; }
bool MetronomeSynthAudioPlugin::hasEditor() const { return false; }

int MetronomeSynthAudioPlugin::getNumPrograms() { return 0; }
int MetronomeSynthAudioPlugin::getCurrentProgram() { return 0; }
void MetronomeSynthAudioPlugin::setCurrentProgram(int index) {}
const String MetronomeSynthAudioPlugin::getProgramName(int index) { return ""; }
void MetronomeSynthAudioPlugin::changeProgramName(int index, const String &newName) {}

void MetronomeSynthAudioPlugin::getStateInformation(MemoryBlock &destData) {}
void MetronomeSynthAudioPlugin::setStateInformation(const void *data, int sizeInBytes) {}
