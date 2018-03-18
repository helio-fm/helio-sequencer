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
#include "PluginSmartDescription.h"
#include "SerializationKeys.h"

using namespace Serialization;

PluginSmartDescription::PluginSmartDescription() {}
PluginSmartDescription::PluginSmartDescription(const PluginDescription *other) :
    PluginDescription(*other) {}

ValueTree PluginSmartDescription::serialize() const
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

void PluginSmartDescription::deserialize(const ValueTree &tree)
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

        this->verify();
    }
}

void PluginSmartDescription::reset()
{
    this->name = {};
    this->uid = {};
}

bool PluginSmartDescription::isValid() const
{
    return this->name.isNotEmpty() && this->pluginFormatName.isNotEmpty();
}

void PluginSmartDescription::verify()
{
    //if (this->fileOrIdentifier == "")
    //{
    //    return;
    //}

    //File my_path(this->fileOrIdentifier);

    //if (!my_path.exists()) {

    //    boost::filesystem::path filename = my_path.filename();

    //    foreach (const boost::filesystem::path &path,
    //        this->owner_.getPluginManager()->getPaths())
    //    {
    //        const boost::filesystem::path path_and_filename = path / filename;
    //        if (boost::filesystem::exists(path_and_filename)) {
    //            DBG("Updating plugin location for: " + path_and_filename.string());
    //            this->fileOrIdentifier = path_and_filename.string().c_str();
    //            break;
    //        }
    //    }
    //}
}
