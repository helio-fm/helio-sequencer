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

#define INTERNAL_PLUGIN_MANUFACTURER_HACK "Helio Workstation"
#define INTERNAL_PLUGIN_IDENTIFIER_HACK "Internal"

String InternalPluginFormat::getName() const
{
    return "Internal";
}

InternalPluginFormat::InternalPluginFormat()
{
    {
        AudioProcessorGraph::AudioGraphIOProcessor p(AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);
        p.fillInPluginDescription(this->audioInDesc);
        this->audioInDesc.manufacturerName = INTERNAL_PLUGIN_MANUFACTURER_HACK;
        this->audioInDesc.fileOrIdentifier = INTERNAL_PLUGIN_IDENTIFIER_HACK;
    }

    {
        AudioProcessorGraph::AudioGraphIOProcessor p(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
        p.fillInPluginDescription(this->audioOutDesc);
        this->audioOutDesc.manufacturerName = INTERNAL_PLUGIN_MANUFACTURER_HACK;
        this->audioOutDesc.fileOrIdentifier = INTERNAL_PLUGIN_IDENTIFIER_HACK;
    }

    {
        AudioProcessorGraph::AudioGraphIOProcessor p(AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode);
        p.fillInPluginDescription(this->midiInDesc);
        this->midiInDesc.manufacturerName = INTERNAL_PLUGIN_MANUFACTURER_HACK;
        this->midiInDesc.fileOrIdentifier = INTERNAL_PLUGIN_IDENTIFIER_HACK;
    }

    {
        AudioProcessorGraph::AudioGraphIOProcessor p(AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode);
        p.fillInPluginDescription(this->midiOutDesc);
        this->midiOutDesc.manufacturerName = INTERNAL_PLUGIN_MANUFACTURER_HACK;
        this->midiOutDesc.fileOrIdentifier = INTERNAL_PLUGIN_IDENTIFIER_HACK;
    }
}

bool InternalPluginFormat::fileMightContainThisPluginType(const String &fileOrIdentifier)
{
    return (fileOrIdentifier.isEmpty() ||
            fileOrIdentifier == INTERNAL_PLUGIN_IDENTIFIER_HACK);
}

void InternalPluginFormat::createPluginInstance(const PluginDescription &desc, double initialSampleRate,
                                                int initialBufferSize, void *userData,
                                                void (*callback) (void*, AudioPluginInstance*, const String&))
{
    if (desc.uid == this->audioOutDesc.uid ||
        desc.name == this->audioOutDesc.name)
    {
        callback(userData,
                 new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode), {});
        return;
    }
    if (desc.uid == this->audioInDesc.uid ||
             desc.name == this->audioInDesc.name)
    {
        callback(userData,
                 new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode), {});
        return;
    }
    if (desc.uid == this->midiInDesc.uid ||
             desc.name == this->midiInDesc.name)
    {
        callback(userData,
                 new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode), {});
        return;
    }
    else if (desc.uid == this->midiOutDesc.uid ||
             desc.name == this->midiOutDesc.name)
    {
        callback(userData,
                 new AudioProcessorGraph::AudioGraphIOProcessor(AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode), {});
        return;
    }
    
    callback(userData, nullptr, {});
}

const PluginDescription *InternalPluginFormat::getDescriptionFor(const InternalFilterType type)
{
    switch (type)
    {
    case audioInputFilter:
        return &this->audioInDesc;

    case audioOutputFilter:
        return &this->audioOutDesc;

    case midiInputFilter:
        return &this->midiInDesc;

    case midiOutputFilter:
        return &this->midiOutDesc;

    default:
        break;
    }

    return nullptr;
}

void InternalPluginFormat::getAllTypes(OwnedArray <PluginDescription> &results)
{
    for (int i = 0; i < static_cast<int>(endOfFilterTypes); ++i)
    {
        results.add(new PluginDescription(*getDescriptionFor(static_cast<InternalFilterType>( i))));
    }
}
