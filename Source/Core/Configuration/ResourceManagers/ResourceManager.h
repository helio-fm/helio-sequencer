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

#pragma once

#include "DocumentHelpers.h"
#include "BinarySerializer.h"
#include "XmlSerializer.h"

class ResourceManager : public Serializable, public ChangeBroadcaster
{
public:

    ResourceManager(const Identifier &resourceName) : resourceName(resourceName) {}

    virtual File getDownloadedResourceFile() const
    {
        static String assumedFileName = this->resourceName + ".helio";
        return DocumentHelpers::getConfigSlot(assumedFileName);
    }

    virtual File getUsersResourceFile() const
    {
        static String assumedFileName = this->resourceName + ".json";
        return DocumentHelpers::getDocumentSlot(assumedFileName);
    }

    virtual String getBuiltInResource() const
    {
        int dataSize;
        const String n = this->resourceName.toString();
        const String assumedResourceName = n.substring(0, 1).toUpperCase() + n.substring(1) + "_json";
        if (const auto *data = BinaryData::getNamedResource(assumedResourceName.toUTF8(), dataSize))
        {
            return String::fromUTF8(data, dataSize);
        }

        jassertfalse;
        return {};
    }

    virtual void onDownloadedLatestResource(const ValueTree &resource)
    {
        Logger::writeToLog("Updating downloaded resource file for " + this->resourceName.toString());

        //XmlSerializer serializer; // debug
        BinarySerializer serializer;
        serializer.saveToFile(this->getDownloadedResourceFile(), resource);

        this->deserialize(resource);

        // TODO test if this change message is really needed here:
        this->sendChangeMessage();
    }

protected:

    // Each resource pool (like scales, hotkeys, etc)
    // is loaded in this priority:
    // 1 - user-defined config
    // 2 - if the latter is not found, latest downloaded config
    // 3 - if no downloaded config found, use built-in one
    void reloadResources()
    {
        const File usersResource(this->getUsersResourceFile());
        if (usersResource.existsAsFile())
        {
            const auto tree(DocumentHelpers::load(usersResource));
            if (tree.isValid())
            {
                Logger::writeToLog("Found users resource file for " + this->resourceName.toString());
                this->deserialize(tree);
                this->sendChangeMessage();
                return;
            }
        }

        const File downloadedResource(this->getUsersResourceFile());
        if (downloadedResource.existsAsFile())
        {
            const auto tree(DocumentHelpers::load(downloadedResource));
            if (tree.isValid())
            {
                Logger::writeToLog("Found downloaded resource file for " + this->resourceName.toString());
                this->deserialize(tree);
                this->sendChangeMessage();
                return;
            }
        }

        const String builtInResource(this->getBuiltInResource());
        const auto tree(DocumentHelpers::load(builtInResource));
        if (tree.isValid())
        {
            Logger::writeToLog("Loading built-in resource for " + this->resourceName.toString());
            this->deserialize(tree);
        }
    }

private: 

    const Identifier resourceName;

};
