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
#include "ResourceManager.h"

ResourceManager::ResourceManager(const Identifier &resourceName) :
    resourceName(resourceName) {}

void ResourceManager::initialise()
{
    this->reloadResources();
}

void ResourceManager::shutdown()
{
    this->reset();
}

void ResourceManager::updateBaseResource(const ValueTree &resource)
{
    Logger::writeToLog("Updating downloaded resource file for " + this->resourceName.toString());

    //XmlSerializer serializer; // debug
    //BinarySerializer serializer;
    JsonSerializer serializer(false); // debug
    serializer.saveToFile(this->getDownloadedResourceFile(), resource);

    this->reloadResources();

    // Do not send update message here, since resource update should go silently
    //this->sendChangeMessage();
}

void ResourceManager::updateUserResource(const BaseResource::Ptr resource)
{
    this->resources.set(resource->getResourceId(), resource);

    const ValueTree existingChild =
        this->userResources.getChildWithProperty(resource->getResourceIdProperty(),
            resource->getResourceId());

    if (existingChild.isValid())
    {
        this->userResources.removeChild(existingChild, nullptr);
    }

    this->userResources.appendChild(resource->serialize(), nullptr);

    // TODO sync with server?
    Logger::writeToLog("Updating user's resource file for " + this->resourceName.toString());

    //XmlSerializer serializer; // debug
    //BinarySerializer serializer;
    JsonSerializer serializer(false); // debug
    serializer.setHeaderComments({ "User-defined " + this->resourceName.toString(), "Feel free to edit manually" });
    serializer.saveToFile(this->getUsersResourceFile(), this->userResources);

    // Should we really send update message here?
    this->sendChangeMessage();
}

File ResourceManager::getDownloadedResourceFile() const
{
    const String assumedFileName = this->resourceName + ".helio";
    return DocumentHelpers::getConfigSlot(assumedFileName);
}

File ResourceManager::getUsersResourceFile() const
{
    const String assumedFileName = this->resourceName + ".json";
    return DocumentHelpers::getDocumentSlot(assumedFileName);
}

String ResourceManager::getBuiltInResourceString() const
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

const BaseResource &ResourceManager::getResourceComparator() const
{
    return this->comparator;
}

void ResourceManager::reloadResources()
{
    bool shouldBroadcastChange = false;

    // Reset and store an empty tree to append user objects to
    this->reset();
    this->userResources = this->serialize();

    // First, get base config (downloaded, or built-in)
    const File downloadedResource(this->getDownloadedResourceFile());
    if (downloadedResource.existsAsFile())
    {
        const auto tree(DocumentHelpers::load(downloadedResource));
        if (tree.isValid())
        {
            Logger::writeToLog("Found downloaded resource file for " + this->resourceName.toString());
            this->deserialize(tree);
            shouldBroadcastChange = true;
        }
    }
    else
    {
        const String builtInResource(this->getBuiltInResourceString());
        const auto tree(DocumentHelpers::load(builtInResource));
        if (tree.isValid())
        {
            Logger::writeToLog("Loading built-in resource for " + this->resourceName.toString());
            this->deserialize(tree);
            shouldBroadcastChange = true;
        }
    }

    // Try to override base config with user's settings
    const File usersResource(this->getUsersResourceFile());

    if (usersResource.existsAsFile())
    {
        const auto tree(DocumentHelpers::load(usersResource));
        if (tree.isValid())
        {
            Logger::writeToLog("Found users resource file for " + this->resourceName.toString());
            this->deserialize(tree);
            this->userResources = tree;
            shouldBroadcastChange = true;
        }
    }

    if (shouldBroadcastChange)
    {
        this->sendChangeMessage();
    }
}

void ResourceManager::reset()
{
    this->resources.clear();
}
