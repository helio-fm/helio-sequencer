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

using namespace Serialization;

SerializablePluginDescription::SerializablePluginDescription() {}
SerializablePluginDescription::SerializablePluginDescription(const PluginDescription *other) :
    PluginDescription(*other) {}

ValueTree SerializablePluginDescription::serialize() const
{
    ValueTree tree(Audio::plugin);
    tree.setProperty(Audio::pluginName, this->name, nullptr);

    if (this->descriptiveName != this->name)
    {
        tree.setProperty(Audio::pluginDescription, this->descriptiveName, nullptr);
    }

    tree.setProperty(Audio::pluginFormat, this->pluginFormatName, nullptr);
    tree.setProperty(Audio::pluginCategory, this->category, nullptr);
    tree.setProperty(Audio::pluginManufacturer, this->manufacturerName, nullptr);
    tree.setProperty(Audio::pluginVersion, this->version, nullptr);
    tree.setProperty(Audio::pluginFile, this->fileOrIdentifier, nullptr);
    tree.setProperty(Audio::pluginFileModTime, String::toHexString(this->lastFileModTime.toMilliseconds()), nullptr);
    tree.setProperty(Audio::pluginId, String::toHexString(this->uid), nullptr);
    tree.setProperty(Audio::pluginIsInstrument, this->isInstrument, nullptr);
    tree.setProperty(Audio::pluginNumInputs, this->numInputChannels, nullptr);
    tree.setProperty(Audio::pluginNumOutputs, this->numOutputChannels, nullptr);

    return tree;
}

void SerializablePluginDescription::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root = tree.hasType(Audio::plugin) ?
        tree : tree.getChildWithName(Audio::plugin);

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
        this->uid = root.getProperty(Audio::pluginId).toString().getHexValue32();
        this->isInstrument = root.getProperty(Audio::pluginIsInstrument, false);
        this->numInputChannels = root.getProperty(Audio::pluginNumInputs);
        this->numOutputChannels = root.getProperty(Audio::pluginNumOutputs);
    }
}

void SerializablePluginDescription::reset()
{
    this->name = {};
    this->uid = {};
}

bool SerializablePluginDescription::isValid() const
{
    return this->name.isNotEmpty() && this->pluginFormatName.isNotEmpty();
}
