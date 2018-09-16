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
#include "ProjectsList.h"
#include "SessionService.h"
#include "SerializationKeys.h"
#include "Config.h"
#include "App.h"

ProjectsList::ProjectsList()
{
    App::Helio().getSessionService()->addChangeListener(this);
}

ProjectsList::~ProjectsList()
{
    App::Helio().getSessionService()->removeChangeListener(this);
}

void ProjectsList::onProjectStateChanged(const String &title, const String &path,
                                            const String &id, bool isLoaded)
{
    const int index = id.isEmpty() ? this->findIndexByPath(path) : this->findIndexById(id);

    if (! File(path).existsAsFile())
    {
        this->localFiles.remove(index);
        this->sendChangeMessage();
        return;
    }
    
    if (index >= 0)
    {
        RecentFileDescription::Ptr fd = this->localFiles[index];
        fd->title = title;
        fd->isLoaded = isLoaded;
        fd->projectId = id;
        
        if (isLoaded)
        {
            fd->lastModifiedTime = Time::getCurrentTime().toMilliseconds();
        }
    }
    else
    {
        RecentFileDescription::Ptr fd = new RecentFileDescription();
        fd->lastModifiedTime = Time::getCurrentTime().toMilliseconds();
        fd->title = title;
        fd->path = path;
        fd->isLoaded = isLoaded;
        fd->projectId = id;
        this->localFiles.add(fd);
    }
    
    this->sendChangeMessage();
}

void ProjectsList::removeByPath(const String &path)
{
    const int index = this->findIndexByPath(path);
    
    if (index >= 0)
    {
        this->localFiles.remove(index);
        this->sendChangeMessage();
    }
}

void ProjectsList::removeById(const String &id)
{
    const int index = this->findIndexById(id);

    if (index >= 0)
    {
        this->localFiles.remove(index);
        this->sendChangeMessage();
    }
}

void ProjectsList::cleanup()
{
    for (int i = 0; i < this->localFiles.size(); ++i)
    {
        File localFile(this->localFiles[i]->path);
        
        if (! localFile.existsAsFile())
        {
            this->localFiles.remove(i);
            this->sendChangeMessage();
            return;
        }
    }
}

RecentFileDescription::Ptr ProjectsList::getItem(int index) const
{
    return this->createCoalescedList()[index];
}

int ProjectsList::getNumItems() const
{
    return this->createCoalescedList().size();
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree ProjectsList::serialize() const
{
    using namespace Serialization;
    const ScopedReadLock lock(this->listLock);
    ValueTree tree(Core::recentFiles);

    for (const auto localFile : this->localFiles)
    {
        ValueTree item(Core::recentFileItem);
        item.setProperty(Core::recentFileTitle, localFile->title, nullptr);
        item.setProperty(Core::recentFilePath, localFile->path, nullptr);
        item.setProperty(Core::recentFileProjectId, localFile->projectId, nullptr);
        item.setProperty(Core::recentFileTime, String(localFile->lastModifiedTime), nullptr);
        tree.appendChild(item, nullptr);
    }

    return tree;
}

void ProjectsList::deserialize(const ValueTree &tree)
{
    this->reset();
    using namespace Serialization;

    const ScopedWriteLock lock(this->listLock);

    const auto root = tree.hasType(Core::recentFiles) ?
        tree : tree.getChildWithName(Core::recentFiles);

    if (!root.isValid()) { return; }

    forEachValueTreeChildWithType(root, child, Core::recentFileItem)
    {
        const String title = child.getProperty(Core::recentFileTitle);
        const String path = child.getProperty(Core::recentFilePath);
        const String id = child.getProperty(Core::recentFileProjectId);
        const int64 time = child.getProperty(Core::recentFileTime);

        if (path.isNotEmpty())
        {
            RecentFileDescription::Ptr fd = new RecentFileDescription();
            fd->title = title;
            fd->path = path;
            fd->lastModifiedTime = time;
            fd->projectId = id;
            fd->isLoaded = false;
            this->localFiles.addSorted(*fd, fd);
        }
    }

    this->sendChangeMessage();
}

void ProjectsList::reset()
{
    const ScopedWriteLock lock(this->listLock);
    this->localFiles.clear();
    this->sendChangeMessage();
}


//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void ProjectsList::forceUpdate()
{
    this->sendChangeMessage();
}

void ProjectsList::changeListenerCallback(ChangeBroadcaster *source)
{
    this->forceUpdate();
}

ReferenceCountedArray<RecentFileDescription> ProjectsList::createCoalescedList() const
{
    ReferenceCountedArray<RecentFileDescription> resultList;
    ReferenceCountedArray<RecentFileDescription> remoteFiles; // TODO

    // adds remote files
    for (auto &remoteFile : remoteFiles)
    {
        RecentFileDescription::Ptr description = new RecentFileDescription();
        description->projectId = remoteFile->projectId;
        description->title = remoteFile->title;
        description->lastModifiedTime = remoteFile->lastModifiedTime;
        description->path = "";
        description->isLoaded = false;
        
        bool hasAlsoLocalVersion = false;
        RecentFileDescription::Ptr localVersion = nullptr;
        
        for (auto && localFile : this->localFiles)
        {
            if (localFile->projectId == remoteFile->projectId)
            {
                localVersion = localFile;
                description->lastModifiedTime = localVersion->lastModifiedTime;
                description->isLoaded = localVersion->isLoaded;
                description->title = localVersion->title;
                description->path = localVersion->path;
                hasAlsoLocalVersion = true;
                break;
            }
        }
        
        description->hasLocalCopy = hasAlsoLocalVersion;
        description->hasRemoteCopy = true;
        
        resultList.add(description);
    }
    
    // adds the rest of the local files
    for (auto &localFile : this->localFiles)
    {
        bool alreadyAdded = false;
        
        for (auto &remoteFile : remoteFiles)
        {
            if (localFile->projectId == remoteFile->projectId)
            {
                alreadyAdded = true;
                break;
            }
        }
        
        if (!alreadyAdded)
        {
            RecentFileDescription::Ptr localDescription = localFile;
            localDescription->hasLocalCopy = true;
            localDescription->hasRemoteCopy = false;
            //Logger::writeToLog("Adding " + localDescription.path);
            resultList.add(localDescription);
        }
    }
    
    RecentFileDescription sorter;
    resultList.sort(sorter);
    return resultList;
}

int ProjectsList::findIndexByPath(const String &path) const
{
    for (int i = 0; i < this->localFiles.size(); ++i)
    {
        if (this->localFiles[i]->path == path)
        {
            return i;
        }
    }
    
    return -1;
}

int ProjectsList::findIndexById(const String &id) const
{
    for (int i = 0; i < this->localFiles.size(); ++i)
    {
        if (this->localFiles[i]->projectId == id)
        {
            return i;
        }
    }

    return -1;
}
