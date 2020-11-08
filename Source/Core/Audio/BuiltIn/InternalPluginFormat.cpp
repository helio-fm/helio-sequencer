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
#include "InternalPluginFormat.h"
#include "Instrument.h"

const String InternalPluginFormat::formatName = "Internal";
const String InternalPluginFormat::manufacturer = "Helio Workstation";

String InternalPluginFormat::getName() const
{
    return InternalPluginFormat::formatName;
}

InternalPluginFormat::InternalPluginFormat()
{
    {
        AudioProcessorGraph::AudioGraphIOProcessor p(AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);
        p.fillInPluginDescription(this->audioInDesc);
        this->audioInDesc.manufacturerName = InternalPluginFormat::manufacturer;
        this->audioInDesc.fileOrIdentifier = InternalPluginFormat::formatName;
    }

    {
        AudioProcessorGraph::AudioGraphIOProcessor p(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
        p.fillInPluginDescription(this->audioOutDesc);
        this->audioOutDesc.manufacturerName = InternalPluginFormat::manufacturer;
        this->audioOutDesc.fileOrIdentifier = InternalPluginFormat::formatName;
    }

    {
        AudioProcessorGraph::AudioGraphIOProcessor p(AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode);
        p.fillInPluginDescription(this->midiInDesc);
        this->midiInDesc.manufacturerName = InternalPluginFormat::manufacturer;
        this->midiInDesc.fileOrIdentifier = InternalPluginFormat::formatName;
    }

    {
        AudioProcessorGraph::AudioGraphIOProcessor p(AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode);
        p.fillInPluginDescription(this->midiOutDesc);
        this->midiOutDesc.manufacturerName = InternalPluginFormat::manufacturer;
        this->midiOutDesc.fileOrIdentifier = InternalPluginFormat::formatName;
    }
}

bool InternalPluginFormat::fileMightContainThisPluginType(const String &fileOrIdentifier)
{
    return (fileOrIdentifier.isEmpty() ||
            fileOrIdentifier == InternalPluginFormat::formatName);
}

void InternalPluginFormat::createPluginInstance(const PluginDescription &desc,
    double initialSampleRate, int initialBufferSize, PluginCreationCallback callback)
{
    if (desc.uid == this->audioOutDesc.uid || desc.name.equalsIgnoreCase(this->audioOutDesc.name))
    {
        callback(make<AudioProcessorGraph::AudioGraphIOProcessor>(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode), {});
        return;
    }
    if (desc.uid == this->audioInDesc.uid || desc.name.equalsIgnoreCase(this->audioInDesc.name))
    {
        callback(make<AudioProcessorGraph::AudioGraphIOProcessor>(AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode), {});
        return;
    }
    if (desc.uid == this->midiInDesc.uid || desc.name.equalsIgnoreCase(this->midiInDesc.name))
    {
        callback(make<AudioProcessorGraph::AudioGraphIOProcessor>(AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode), {});
        return;
    }
    else if (desc.uid == this->midiOutDesc.uid || desc.name.equalsIgnoreCase(this->midiOutDesc.name))
    {
        callback(make<AudioProcessorGraph::AudioGraphIOProcessor>(AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode), {});
        return;
    }
    
    callback(nullptr, {});
}

const PluginDescription *InternalPluginFormat::getDescriptionFor(const InternalFilterType type)
{
    switch (type)
    {
    case InternalFilterType::audioInputFilter:
        return &this->audioInDesc;
    case InternalFilterType::audioOutputFilter:
        return &this->audioOutDesc;
    case InternalFilterType::midiInputFilter:
        return &this->midiInDesc;
    case InternalFilterType::midiOutputFilter:
        return &this->midiOutDesc;
    default:
        break;
    }

    return nullptr;
}

void InternalPluginFormat::getAllTypes(OwnedArray <PluginDescription> &results)
{
    for (int i = 0; i < static_cast<int>(InternalFilterType::endOfFilterTypes); ++i)
    {
        results.add(new PluginDescription(*getDescriptionFor(static_cast<InternalFilterType>( i))));
    }
}
