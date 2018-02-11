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

PluginSmartDescription::PluginSmartDescription() {}
PluginSmartDescription::PluginSmartDescription(const PluginDescription *other) : PluginDescription(*other) {}

ValueTree PluginSmartDescription::serialize() const
{
    ValueTree tree(Serialization::Core::plugin);
    tree.setProperty("name", this->name);

    if (this->descriptiveName != this->name)
    {
        tree.setProperty("descriptiveName", this->descriptiveName);
    }

    tree.setProperty("format", this->pluginFormatName);
    tree.setProperty("category", this->category);
    tree.setProperty("manufacturer", this->manufacturerName);
    tree.setProperty("version", this->version);
    tree.setProperty("file", this->fileOrIdentifier);
    tree.setProperty("uid", String::toHexString(this->uid));
    tree.setProperty("isInstrument", this->isInstrument);
    tree.setProperty("fileTime", String::toHexString(this->lastFileModTime.toMilliseconds()));
    tree.setProperty("numInputs", this->numInputChannels);
    tree.setProperty("numOutputs", this->numOutputChannels);

    return tree;
}

void PluginSmartDescription::deserialize(const ValueTree &tree)
{
    this->reset();

    if (tree.hasType(Serialization::Core::plugin))
    {
        this->name = tree.getProperty("name");
        this->descriptiveName = tree.getProperty("descriptiveName", name);
        this->pluginFormatName = tree.getProperty("format");
        this->category = tree.getProperty("category");
        this->manufacturerName = tree.getProperty("manufacturer");
        this->version = tree.getProperty("version");
        this->fileOrIdentifier = tree.getProperty("file");
        this->uid = tree.getProperty("uid").toString().getHexValue32();
        this->lastFileModTime = Time(tree.getProperty("fileTime").toString().getHexValue64());
        this->isInstrument = tree.getProperty("isInstrument", false);
        this->numInputChannels = tree.getProperty("numInputs");
        this->numOutputChannels = tree.getProperty("numOutputs");

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
    return this->name.isNotEmpty();
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
