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
#include "RemoteCache.h"

using namespace VCS;

bool RemoteCache::hasRevisionTracked(const Revision::Ptr revision) const
{
    ScopedReadLock lock(this->cacheLock);
    return this->fetchCache.contains(revision->getUuid());
}

void RemoteCache::updateForRemoteRevisions(const Array<RevisionDto> &revisions)
{
    ScopedWriteLock lock(this->cacheLock);

    this->fetchCache.clear();
    for (const auto &child : revisions)
    {
        this->fetchCache[child.getId()] = child.getTimestamp();
    }

    this->lastSyncTime = Time::getCurrentTime();
}

void RemoteCache::updateForLocalRevision(const Revision::Ptr revision)
{
    ScopedWriteLock lock(this->cacheLock);
    this->fetchCache[revision->getUuid()] = revision->getTimeStamp();
    this->lastSyncTime = Time::getCurrentTime();
}

bool RemoteCache::isOutdated() const
{
    // if history has not been synced for at least a couple of days,
    // version control will fetch remote revisions in a background
    ScopedReadLock lock(this->cacheLock);
    return this->fetchCache.size() > 0 &&
        (Time::getCurrentTime() - this->lastSyncTime).inDays() > 1;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree RemoteCache::serialize() const
{
    ValueTree tree(Serialization::VCS::remoteCache);

    tree.setProperty(Serialization::VCS::remoteCacheSyncTime, this->lastSyncTime.toMilliseconds(), nullptr);

    for (const auto &child : this->fetchCache)
    {
        ValueTree revNode(Serialization::VCS::remoteRevision);
        revNode.setProperty(Serialization::VCS::remoteRevisionId, child.first, nullptr);
        revNode.setProperty(Serialization::VCS::remoteRevisionTimeStamp, child.second, nullptr);
        tree.appendChild(revNode, nullptr);
    }

    return tree;
}

void RemoteCache::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root = tree.hasType(Serialization::VCS::remoteCache) ?
        tree : tree.getChildWithName(Serialization::VCS::remoteCache);

    if (!root.isValid()) { return; }

    this->lastSyncTime = Time(root.getProperty(Serialization::VCS::remoteCacheSyncTime));

    forEachValueTreeChildWithType(root, e, Serialization::VCS::remoteRevision)
    {
        const String revisionId = e.getProperty(Serialization::VCS::remoteRevisionId);
        const int64 revisionTimestamp = e.getProperty(Serialization::VCS::remoteRevisionTimeStamp);
        this->fetchCache[revisionId] = revisionTimestamp;
    }
}

void RemoteCache::reset()
{
    this->fetchCache.clear();
}
