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
#include "RecentProjectInfo.h"
#include "DocumentHelpers.h"
#include "SerializationKeys.h"

#if !NO_NETWORK

RecentProjectInfo::RecentProjectInfo(const ProjectDto &remoteInfo) :
    projectId(remoteInfo.getId()),
    local(nullptr),
    remote(new RemoteInfo())
{
    this->updateRemoteInfo(remoteInfo);
}


void RecentProjectInfo::updateRemoteInfo(const ProjectDto &remoteInfo)
{
    jassert(this->projectId == remoteInfo.getId());
    if (this->remote == nullptr)
    {
        this->remote = make<RemoteInfo>();
    }

    this->remote->title = remoteInfo.getTitle();
    this->remote->alias = remoteInfo.getAlias();
    this->remote->lastModifiedMs = remoteInfo.getUpdateTime();
}

#endif

RecentProjectInfo::RecentProjectInfo(const String &localId,
    const String &localTitle, const String &localPath) :
    projectId(localId),
    local(new LocalInfo()),
    remote(nullptr)
{
    this->updateLocalInfo(localId, localTitle, localPath);
}

void RecentProjectInfo::updateLocalInfo(const String &localId,
    const String &localTitle, const String &localPath)
{
    jassert(this->projectId == localId);
    if (this->local == nullptr)
    {
        this->local = make<LocalInfo>();
    }

    jassert(localPath.isNotEmpty());
    this->local->title = localTitle;
    this->local->path = localPath;
    this->local->lastModifiedMs = Time::currentTimeMillis();
}

void RecentProjectInfo::updateLocalTimestampAsNow()
{
    jassert(this->local != nullptr);
    if (this->local != nullptr)
    {
        this->local->lastModifiedMs = Time::currentTimeMillis();
    }
}

void RecentProjectInfo::resetLocalInfo()
{
    this->local = nullptr;
}

void RecentProjectInfo::resetRemoteInfo()
{
    this->remote = nullptr;
}

String RecentProjectInfo::getProjectId() const noexcept
{
    return this->projectId;
}

File RecentProjectInfo::getLocalFile() const
{
    if (this->local != nullptr)
    {
        // the file may be missing here, because we store absolute path,
        // but some platforms, like iOS, change documents folder path on every app run
        // so we need to check in a the document folder as well:
        return this->local->path.existsAsFile() ? this->local->path :
            File(DocumentHelpers::getDocumentSlot(this->local->path.getFileName()));
    }

    return {};
}

String RecentProjectInfo::getTitle() const
{
    if (this->local != nullptr)
    {
        if (this->remote != nullptr &&
            this->remote->lastModifiedMs > this->local->lastModifiedMs)
        {
            return this->remote->title;
        }

        return this->local->title;
    }
    else if (this->remote != nullptr)
    {
        return this->remote->title;
    }

    return {};
}

Time RecentProjectInfo::getUpdatedAt() const noexcept
{
    if (this->local != nullptr && this->remote != nullptr)
    {
        return Time(jmax(this->remote->lastModifiedMs, this->local->lastModifiedMs));
    }
    else if (this->local != nullptr)
    {
        return Time(this->local->lastModifiedMs);
    }
    else if (this->remote != nullptr)
    {
        return Time(this->remote->lastModifiedMs);
    }

    return {};
}

bool RecentProjectInfo::hasLocalCopy() const noexcept
{
    return this->local != nullptr;
}

bool RecentProjectInfo::hasRemoteCopy() const noexcept
{
    return this->remote != nullptr;
}

bool RecentProjectInfo::isValid() const
{
    const bool isFoundSomewhere = this->hasRemoteCopy() ||
        (this->hasLocalCopy() && this->getLocalFile().existsAsFile());

    return this->projectId.isNotEmpty() && isFoundSomewhere;
}

int RecentProjectInfo::compareElements(RecentProjectInfo *first, RecentProjectInfo *second)
{
    jassert(first != nullptr && second != nullptr);
    if (first == second || first->projectId == second->projectId)
    {
        return 0;
    }

    const auto firstLocalTime = first->getUpdatedAt();
    const auto secondLocalTime = second->getUpdatedAt();
    return (firstLocalTime < secondLocalTime) - (firstLocalTime > secondLocalTime);
}

SerializedData RecentProjectInfo::serialize() const
{
    using namespace Serialization::User;
    SerializedData root(RecentProjects::recentProject);

    root.setProperty(RecentProjects::projectId, this->projectId);

    if (this->local != nullptr)
    {
        SerializedData localRoot(RecentProjects::localProjectInfo);
        localRoot.setProperty(RecentProjects::path, this->local->path.getFullPathName());
        localRoot.setProperty(RecentProjects::title, this->local->title);
        localRoot.setProperty(RecentProjects::updatedAt, this->local->lastModifiedMs);
        root.appendChild(localRoot);
    }

    if (this->remote != nullptr)
    {
        SerializedData remoteRoot(RecentProjects::remoteProjectInfo);
        remoteRoot.setProperty(RecentProjects::path, this->remote->alias);
        remoteRoot.setProperty(RecentProjects::title, this->remote->title);
        remoteRoot.setProperty(RecentProjects::updatedAt, this->remote->lastModifiedMs);
        root.appendChild(remoteRoot);
    }

    return root;
}

void RecentProjectInfo::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization::User;

    const auto root = data.hasType(RecentProjects::recentProject) ?
        data : data.getChildWithName(RecentProjects::recentProject);

    if (!root.isValid()) { return; }

    this->projectId = root.getProperty(RecentProjects::projectId);

    const auto localRoot(root.getChildWithName(RecentProjects::localProjectInfo));
    if (localRoot.isValid())
    {
        this->local = make<LocalInfo>();
        this->local->path = localRoot.getProperty(RecentProjects::path);
        this->local->title = localRoot.getProperty(RecentProjects::title);
        this->local->lastModifiedMs = localRoot.getProperty(RecentProjects::updatedAt);
    }

    const auto remoteRoot(root.getChildWithName(RecentProjects::remoteProjectInfo));
    if (remoteRoot.isValid())
    {
        this->remote = make<RemoteInfo>();
        this->remote->alias = remoteRoot.getProperty(RecentProjects::path);
        this->remote->title = remoteRoot.getProperty(RecentProjects::title);
        this->remote->lastModifiedMs = remoteRoot.getProperty(RecentProjects::updatedAt);
    }
}

void RecentProjectInfo::reset()
{
    this->local.reset();
    this->remote.reset();
}
