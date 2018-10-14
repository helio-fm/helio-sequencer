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
#include "UserProfile.h"
#include "SessionService.h"
#include "SerializationKeys.h"
#include "Config.h"
#include "App.h"

void UserProfile::onProjectStateChanged(const String &title, const String &path, const String &id, bool isLoaded)
{
    const int index = id.isEmpty() ? this->findIndexByPath(path) : this->findIndexById(id);

    if (!File(path).existsAsFile())
    {
        this->list.remove(index);
        this->sendChangeMessage();
        return;
    }

    if (index >= 0)
    {
        RecentProjectInfo::Ptr fd = this->list[index];
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
        RecentProjectInfo::Ptr fd = new RecentProjectInfo();
        fd->lastModifiedTime = Time::getCurrentTime().toMilliseconds();
        fd->title = title;
        fd->path = path;
        fd->isLoaded = isLoaded;
        fd->projectId = id;
        this->list.add(fd);
    }

    this->sendChangeMessage();
}

void UserProfile::onProjectDeleted(const String &id)
{
    const int index = this->findIndexById(id);

    if (index >= 0)
    {
        this->list.remove(index);
        this->sendChangeMessage();
    }
}

const ReferenceCountedArray<RecentProjectInfo> &UserProfile::getList() const noexcept
{
    return this->list;
}


int UserProfile::findIndexByPath(const String &path) const
{
    for (int i = 0; i < this->list.size(); ++i)
    {
        if (this->list[i]->path == path)
        {
            return i;
        }
    }

    return -1;
}

int UserProfile::findIndexById(const String &id) const
{
    for (int i = 0; i < this->list.size(); ++i)
    {
        if (this->list[i]->projectId == id)
        {
            return i;
        }
    }

    return -1;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree UserProfile::serialize() const
{
    using namespace Serialization;
    const ScopedReadLock lock(this->projectsListLock);
    ValueTree tree(Core::recentFiles);

    for (const auto localFile : this->projects)
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

void UserProfile::deserialize(const ValueTree &tree)
{
    // TODO cleanup invalid projects on deserialize

    this->reset();
    using namespace Serialization;

    const ScopedWriteLock lock(this->projectsListLock);

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
            RecentProjectInfo::Ptr fd = new RecentProjectInfo();
            fd->title = title;
            fd->path = path;
            fd->lastModifiedTime = time;
            fd->projectId = id;
            fd->isLoaded = false;
            this->list.addSorted(*fd, fd);
        }
    }

    this->sendChangeMessage();
}

void UserProfile::reset()
{
    const ScopedWriteLock lock(this->projectsListLock);
    this->projects.clear();
    this->sendChangeMessage();
}
