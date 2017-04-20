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

PluginSmartDescription::PluginSmartDescription() noexcept
{
}

PluginSmartDescription::~PluginSmartDescription() noexcept
{
}

XmlElement *PluginSmartDescription::createXml() const
{
    auto const e = new XmlElement(Serialization::Core::plugin);
    e->setAttribute("name", name);

    if (descriptiveName != name)
    {
        e->setAttribute("descriptiveName", descriptiveName);
    }

    e->setAttribute("format", pluginFormatName);
    e->setAttribute("category", category);
    e->setAttribute("manufacturer", manufacturerName);
    e->setAttribute("version", version);
    e->setAttribute("file", fileOrIdentifier);
    e->setAttribute("uid", String::toHexString(uid));
    e->setAttribute("isInstrument", isInstrument);
    e->setAttribute("fileTime", String::toHexString(lastFileModTime.toMilliseconds()));
    e->setAttribute("numInputs", numInputChannels);
    e->setAttribute("numOutputs", numOutputChannels);

    return e;
}

bool PluginSmartDescription::loadFromXml(const XmlElement &xml)
{
    if (xml.hasTagName(Serialization::Core::plugin))
    {
        name                = xml.getStringAttribute("name");
        descriptiveName     = xml.getStringAttribute("descriptiveName", name);
        pluginFormatName    = xml.getStringAttribute("format");
        category            = xml.getStringAttribute("category");
        manufacturerName    = xml.getStringAttribute("manufacturer");
        version             = xml.getStringAttribute("version");
        fileOrIdentifier    = xml.getStringAttribute("file");
        uid                 = xml.getStringAttribute("uid").getHexValue32();
        lastFileModTime     = Time(xml.getStringAttribute("fileTime").getHexValue64());
        isInstrument        = xml.getBoolAttribute("isInstrument", false);
        numInputChannels    = xml.getIntAttribute("numInputs");
        numOutputChannels   = xml.getIntAttribute("numOutputs");

        this->verify();

        return true;
    }

    return false;
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
