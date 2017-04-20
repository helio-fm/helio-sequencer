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

class DocumentOwner;
class RecentFilesList;

#include "Serializable.h"
#include "RequestProjectsListThread.h"

class RecentFileDescription : public ReferenceCountedObject
{
public:
    
    // filled for both remote and local projects
    String title;
    String projectId;
    bool hasRemoteCopy;
    bool hasLocalCopy;
    int64 lastModifiedTime; // in ms

    // filled for local projects
    String path;
    bool isLoaded;

    // filled for remote projects
    String projectKey;
    //int numCommitsToPush; // todo in next version?
    //int numCommitsToPull; // how do i do that, yo?

    typedef ReferenceCountedObjectPtr<RecentFileDescription> Ptr;

    static int compareElements(RecentFileDescription::Ptr first, RecentFileDescription::Ptr second)
    {
        // time diff in seconds
        return int(second->lastModifiedTime / 1000) - int(first->lastModifiedTime / 1000);
    }
    
};

class RecentFilesList :
    public Serializable,
    public ChangeBroadcaster,
    private ChangeListener // listens to AuthorizationManager
{
public:
    
    RecentFilesList();
    
    ~RecentFilesList() override;
    
    void forceUpdate();
    
    
    void onProjectStateChanged(const String &title, const String &path, const String &id, bool isLoaded);
    
    void removeByPath(const String &path);
    
    void removeById(const String &id);
    
    void cleanup();
    
    
    RecentFileDescription::Ptr getItem(int index) const;
    
    int getNumItems() const;
    
    class Owner
    {
    public:
        
        virtual ~Owner() {}
        
        virtual RecentFilesList &getRecentFilesList() const = 0;
        
        virtual bool onClickedLoadRecentFile(RecentFileDescription::Ptr fileDescription) = 0;
        
        virtual void onClickedUnloadRecentFile(RecentFileDescription::Ptr fileDescription) = 0;
        
    };
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;

    void deserialize(const XmlElement &xml) override;

    void reset() override;

private:

    //===------------------------------------------------------------------===//
    // ChangeListener
    //===------------------------------------------------------------------===//
    
    void changeListenerCallback(ChangeBroadcaster *source) override;
    
    
    // returns found item's index - or -1, if not found
    int findIndexByPath(const String &path) const;

    int findIndexById(const String &id) const;

    ReadWriteLock listLock;

    ReferenceCountedArray<RecentFileDescription> localFiles;

    ReferenceCountedArray<RecentFileDescription> createCoalescedList() const;
    
    Array<RemoteProjectDescription> remoteFiles;
    
    WeakReference<RecentFilesList>::Master masterReference;
    friend class WeakReference<RecentFilesList>;

};
