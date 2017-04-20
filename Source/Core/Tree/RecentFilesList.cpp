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
#include "RecentFilesList.h"

#include "Config.h"
#include "SerializationKeys.h"

#include "App.h"
#include "AuthorizationManager.h"

RecentFilesList::RecentFilesList()
{
    App::Helio()->getAuthManager()->addChangeListener(this);
    //Config::load(Serialization::Core::recentFiles, this);
    // todo update list
}

RecentFilesList::~RecentFilesList()
{
    App::Helio()->getAuthManager()->removeChangeListener(this);
    //Config::save(Serialization::Core::recentFiles, this);
    this->masterReference.clear();
}

void RecentFilesList::onProjectStateChanged(const String &title, const String &path,
                                            const String &id, bool isLoaded)
{
//    Logger::writeToLog("RecentFilesList::onProjectStateChanged: " + title);
    
    const int index = id.isEmpty() ? this->findIndexByPath(path) : this->findIndexById(id);

    if (! File(path).existsAsFile())
    {
        this->localFiles.remove(index);
        this->sendChangeMessage();
        return;
    }
    
    if (index >= 0)
    {
        Logger::writeToLog("onProjectStateChanged (found recent file) " + title + " :: " + path);

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
        Logger::writeToLog("onProjectStateChanged (new recent file) " + title + " :: " + path);

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

void RecentFilesList::removeByPath(const String &path)
{
    const int index = this->findIndexByPath(path);
    
    if (index >= 0)
    {
        this->localFiles.remove(index);
        this->sendChangeMessage();
    }
}

void RecentFilesList::removeById(const String &id)
{
    const int index = this->findIndexById(id);

    if (index >= 0)
    {
        this->localFiles.remove(index);
        this->sendChangeMessage();
    }
}

void RecentFilesList::cleanup()
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

RecentFileDescription::Ptr RecentFilesList::getItem(int index) const
{
    return this->createCoalescedList()[index];
}

int RecentFilesList::getNumItems() const
{
    return this->createCoalescedList().size();
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *RecentFilesList::serialize() const
{
    const ScopedReadLock lock(this->listLock);
    auto xml = new XmlElement(Serialization::Core::recentFiles);

    for (auto && localFile : this->localFiles)
    {
        auto item = new XmlElement(Serialization::Core::recentFileItem);
        item->setAttribute("title", localFile->title);
        item->setAttribute("path", localFile->path);
        item->setAttribute("id", localFile->projectId);
        item->setAttribute("time", String(localFile->lastModifiedTime));
        xml->addChildElement(item);
    }

    return xml;
}

void RecentFilesList::deserialize(const XmlElement &xml)
{
    this->reset();

    const ScopedWriteLock lock(this->listLock);

    const XmlElement *root = xml.hasTagName(Serialization::Core::recentFiles) ?
                             &xml : xml.getChildByName(Serialization::Core::recentFiles);

    if (root == nullptr) { return; }

    forEachXmlChildElementWithTagName(*root, child, Serialization::Core::recentFileItem)
    {
        const String title = child->getStringAttribute("title", "");
        const String path = child->getStringAttribute("path", "");
        const int64 time = child->getStringAttribute("time", "").getLargeIntValue();
        const String id = child->getStringAttribute("id", "");

        if (path != "")
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

void RecentFilesList::reset()
{
    const ScopedWriteLock lock(this->listLock);
    this->localFiles.clear();
    this->sendChangeMessage();
}


//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void RecentFilesList::forceUpdate()
{
    this->remoteFiles = App::Helio()->getAuthManager()->getListOfRemoteProjects();
    
//    for (int i = 0; i < this->remoteFiles.size(); ++i)
//    {
//        Logger::writeToLog("Received remote file: " + this->remoteFiles[i].projectTitle);
//    }
    
    this->sendChangeMessage();
}

void RecentFilesList::changeListenerCallback(ChangeBroadcaster *source)
{
    this->forceUpdate();
}

ReferenceCountedArray<RecentFileDescription> RecentFilesList::createCoalescedList() const
{
    ReferenceCountedArray<RecentFileDescription> resultList;
    
    // adds remote files
    for (auto && remoteFile : this->remoteFiles)
    {
        RecentFileDescription::Ptr description = new RecentFileDescription();
        description->projectId = remoteFile.projectId;
        description->projectKey = remoteFile.projectKey;
        description->title = remoteFile.projectTitle;
        description->lastModifiedTime = remoteFile.lastModifiedTime;
        description->path = "";
        description->isLoaded = false;
        
        bool hasAlsoLocalVersion = false;
        RecentFileDescription::Ptr localVersion = nullptr;
        
        for (auto && localFile : this->localFiles)
        {
            if (localFile->projectId == remoteFile.projectId)
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
    for (auto && localFile : this->localFiles)
    {
        bool alreadyAdded = false;
        
        for (auto && remoteFile : this->remoteFiles)
        {
            if (localFile->projectId == remoteFile.projectId)
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

int RecentFilesList::findIndexByPath(const String &path) const
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

int RecentFilesList::findIndexById(const String &id) const
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
