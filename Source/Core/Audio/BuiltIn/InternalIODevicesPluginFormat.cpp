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
#include "InternalIODevicesPluginFormat.h"

const String InternalIODevicesPluginFormat::formatName = "Internal";
const String InternalIODevicesPluginFormat::manufacturer = "Built-in";

String InternalIODevicesPluginFormat::getName() const
{
    return InternalIODevicesPluginFormat::formatName;
}

InternalIODevicesPluginFormat::InternalIODevicesPluginFormat()
{
    {
        AudioProcessorGraph::AudioGraphIOProcessor p(AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);
        p.fillInPluginDescription(this->audioInDesc);
        this->audioInDesc.manufacturerName = InternalIODevicesPluginFormat::manufacturer;
        this->audioInDesc.fileOrIdentifier = InternalIODevicesPluginFormat::formatName;
    }

    {
        AudioProcessorGraph::AudioGraphIOProcessor p(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
        p.fillInPluginDescription(this->audioOutDesc);
        this->audioOutDesc.manufacturerName = InternalIODevicesPluginFormat::manufacturer;
        this->audioOutDesc.fileOrIdentifier = InternalIODevicesPluginFormat::formatName;
    }

    {
        AudioProcessorGraph::AudioGraphIOProcessor p(AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode);
        p.fillInPluginDescription(this->midiInDesc);
        this->midiInDesc.manufacturerName = InternalIODevicesPluginFormat::manufacturer;
        this->midiInDesc.fileOrIdentifier = InternalIODevicesPluginFormat::formatName;
    }

    {
        AudioProcessorGraph::AudioGraphIOProcessor p(AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode);
        p.fillInPluginDescription(this->midiOutDesc);
        this->midiOutDesc.manufacturerName = InternalIODevicesPluginFormat::manufacturer;
        this->midiOutDesc.fileOrIdentifier = InternalIODevicesPluginFormat::formatName;
    }
}

bool InternalIODevicesPluginFormat::fileMightContainThisPluginType(const String &fileOrIdentifier)
{
    return (fileOrIdentifier.isEmpty() ||
            fileOrIdentifier == InternalIODevicesPluginFormat::formatName);
}

void InternalIODevicesPluginFormat::createPluginInstance(const PluginDescription &desc,
    double initialSampleRate, int initialBufferSize, PluginCreationCallback callback)
{
    if (desc.uniqueId == this->audioOutDesc.uniqueId || desc.name.equalsIgnoreCase(this->audioOutDesc.name))
    {
        callback(make<AudioProcessorGraph::AudioGraphIOProcessor>(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode), {});
        return;
    }
    if (desc.uniqueId == this->audioInDesc.uniqueId || desc.name.equalsIgnoreCase(this->audioInDesc.name))
    {
        callback(make<AudioProcessorGraph::AudioGraphIOProcessor>(AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode), {});
        return;
    }
    if (desc.uniqueId == this->midiInDesc.uniqueId || desc.name.equalsIgnoreCase(this->midiInDesc.name))
    {
        callback(make<AudioProcessorGraph::AudioGraphIOProcessor>(AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode), {});
        return;
    }
    else if (desc.uniqueId == this->midiOutDesc.uniqueId || desc.name.equalsIgnoreCase(this->midiOutDesc.name))
    {
        callback(make<AudioProcessorGraph::AudioGraphIOProcessor>(AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode), {});
        return;
    }
    
    callback(nullptr, {});
}

const PluginDescription *InternalIODevicesPluginFormat::getDescriptionFor(const Type type)
{
    switch (type)
    {
    case Type::audioInput:
        return &this->audioInDesc;
    case Type::audioOutput:
        return &this->audioOutDesc;
    case Type::midiInput:
        return &this->midiInDesc;
    case Type::midiOutput:
        return &this->midiOutDesc;
    default:
        break;
    }

    return nullptr;
}

void InternalIODevicesPluginFormat::getAllTypes(OwnedArray<PluginDescription> &results)
{
    for (int i = 0; i < static_cast<int>(Type::endOfDeviceTypes); ++i)
    {
        results.add(new PluginDescription(*this->getDescriptionFor(static_cast<Type>(i))));
    }
}
