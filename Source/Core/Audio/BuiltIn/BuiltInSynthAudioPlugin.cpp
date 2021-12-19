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

const String BuiltInSynthAudioPlugin::instrumentId = "<default>";
const String BuiltInSynthAudioPlugin::instrumentName = "Helio Default";
const String BuiltInSynthAudioPlugin::instrumentNameOld = "Helio Piano";

BuiltInSynthAudioPlugin::BuiltInSynthAudioPlugin()
{
    this->setPlayConfigDetails(0, 2,
        this->getSampleRate(), this->getBlockSize());
}

void BuiltInSynthAudioPlugin::fillInPluginDescription(PluginDescription &description) const
{
    description.name = this->getName();
    description.descriptiveName = description.name;
    description.uniqueId = description.name.hashCode();
    description.category = "Synth";
    description.pluginFormatName = BuiltInSynthFormat::formatName;
    description.fileOrIdentifier = BuiltInSynthFormat::formatIdentifier;
    description.manufacturerName = "Helio Workstation";
    description.version = "1.0";
    description.isInstrument = true;
    description.numInputChannels = this->getTotalNumInputChannels();
    description.numOutputChannels = this->getTotalNumOutputChannels();
}

const String BuiltInSynthAudioPlugin::getName() const
{
    return BuiltInSynthAudioPlugin::instrumentName;
}

void BuiltInSynthAudioPlugin::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
    buffer.clear(0, buffer.getNumSamples());
    this->synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void BuiltInSynthAudioPlugin::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    this->synth.setCurrentPlaybackSampleRate(sampleRate);
}

void BuiltInSynthAudioPlugin::reset()
{
    this->synth.allNotesOff(0, true);
}

void BuiltInSynthAudioPlugin::releaseResources() {}

double BuiltInSynthAudioPlugin::getTailLengthSeconds() const
{
    return 3.0;
}

bool BuiltInSynthAudioPlugin::acceptsMidi() const
{
    return true;
}

bool BuiltInSynthAudioPlugin::producesMidi() const
{
    return false;
}

AudioProcessorEditor *BuiltInSynthAudioPlugin::createEditor()
{
    return nullptr;
}

bool BuiltInSynthAudioPlugin::hasEditor() const
{
    return false;
}

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

void BuiltInSynthAudioPlugin::getStateInformation(MemoryBlock &destData) {}
void BuiltInSynthAudioPlugin::setStateInformation(const void *data, int sizeInBytes) {}
