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
#include "SerializablePluginDescription.h"
#include "SerializationKeys.h"
#include "BuiltInSynthAudioPlugin.h"

SerializablePluginDescription::SerializablePluginDescription() {}
SerializablePluginDescription::SerializablePluginDescription(const PluginDescription &other) :
    PluginDescription(other) {}

SerializedData SerializablePluginDescription::serialize() const
{
    using namespace Serialization;

    SerializedData tree(Audio::plugin);
    tree.setProperty(Audio::pluginName, this->name);

    if (this->descriptiveName != this->name)
    {
        tree.setProperty(Audio::pluginDescription, this->descriptiveName);
    }

    tree.setProperty(Audio::pluginFormat, this->pluginFormatName);
    tree.setProperty(Audio::pluginCategory, this->category);
    tree.setProperty(Audio::pluginManufacturer, this->manufacturerName);
    tree.setProperty(Audio::pluginVersion, this->version);
    tree.setProperty(Audio::pluginFile, this->fileOrIdentifier);
    tree.setProperty(Audio::pluginFileModTime, String::toHexString(this->lastFileModTime.toMilliseconds()));
    tree.setProperty(Audio::pluginId, String::toHexString(this->uniqueId));
    tree.setProperty(Audio::pluginIsInstrument, this->isInstrument);
    tree.setProperty(Audio::pluginNumInputs, this->numInputChannels);
    tree.setProperty(Audio::pluginNumOutputs, this->numOutputChannels);

    return tree;
}

void SerializablePluginDescription::deserialize(const SerializedData &data)
{
    using namespace Serialization;

    this->reset();

    const auto root = data.hasType(Audio::plugin) ?
        data : data.getChildWithName(Audio::plugin);

    if (root.isValid())
    {
        this->name = root.getProperty(Audio::pluginName);
        this->descriptiveName = root.getProperty(Audio::pluginDescription, name);
        this->pluginFormatName = root.getProperty(Audio::pluginFormat);
        this->category = root.getProperty(Audio::pluginCategory);
        this->manufacturerName = root.getProperty(Audio::pluginManufacturer);
        this->version = root.getProperty(Audio::pluginVersion);
        this->fileOrIdentifier = root.getProperty(Audio::pluginFile);
        this->lastFileModTime = Time(root.getProperty(Audio::pluginFileModTime).toString().getHexValue64());
        this->uniqueId = root.getProperty(Audio::pluginId).toString().getHexValue32();
        this->isInstrument = root.getProperty(Audio::pluginIsInstrument, false);
        this->numInputChannels = root.getProperty(Audio::pluginNumInputs);
        this->numOutputChannels = root.getProperty(Audio::pluginNumOutputs);

        // legacy naming workaround for the built-in instrument
        // todo remove in future versions - here and in Instrument.cpp
        if (this->name == BuiltInSynthAudioPlugin::instrumentNameOld)
        {
            this->name = BuiltInSynthAudioPlugin::instrumentName;
            this->descriptiveName = BuiltInSynthAudioPlugin::instrumentName;
        }
    }
}

void SerializablePluginDescription::reset()
{
    this->name = {};
    this->uniqueId = {};
}

bool SerializablePluginDescription::isValid() const
{
    return this->name.isNotEmpty() && this->pluginFormatName.isNotEmpty();
}
