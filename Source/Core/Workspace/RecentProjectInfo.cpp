/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "RecentProjectInfo.h"
#include "DocumentHelpers.h"
#include "SerializationKeys.h"

RecentProjectInfo::RecentProjectInfo(const String &localId,
    const String &localTitle, const String &localPath) :
    projectId(localId)
{
    this->updateLocalInfo(localTitle, localPath);
}

void RecentProjectInfo::updateLocalInfo(const String &localTitle, const String &localPath)
{
    jassert(localPath.isNotEmpty());
    this->localInfo.title = localTitle;
    this->localInfo.path = localPath;
    this->localInfo.lastModifiedMs = Time::currentTimeMillis();
}

void RecentProjectInfo::updateLocalTimestampAsNow()
{
    this->localInfo.lastModifiedMs = Time::currentTimeMillis();
}

String RecentProjectInfo::getProjectId() const noexcept
{
    return this->projectId;
}

File RecentProjectInfo::getLocalFile() const
{
    // the file may be missing here, because we store the absolute path,
    // but some platforms, like iOS, change documents folder path on every app run
    // so we need to check in a the document folder as well:
    return this->localInfo.path.existsAsFile() ? this->localInfo.path :
        File(DocumentHelpers::getDocumentSlot(this->localInfo.path.getFileName()));
}

String RecentProjectInfo::getTitle() const
{
    return this->localInfo.title;
}

Time RecentProjectInfo::getUpdatedAt() const noexcept
{
    return Time(this->localInfo.lastModifiedMs);
}

bool RecentProjectInfo::isValid() const
{
    return this->projectId.isNotEmpty() && this->getLocalFile().existsAsFile();
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

    SerializedData localRoot(RecentProjects::localProjectInfo);
    localRoot.setProperty(RecentProjects::path, this->localInfo.path.getFullPathName());
    localRoot.setProperty(RecentProjects::title, this->localInfo.title);
    localRoot.setProperty(RecentProjects::updatedAt, this->localInfo.lastModifiedMs);
    root.appendChild(localRoot);

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
        this->localInfo.path = localRoot.getProperty(RecentProjects::path);
        this->localInfo.title = localRoot.getProperty(RecentProjects::title);
        this->localInfo.lastModifiedMs = localRoot.getProperty(RecentProjects::updatedAt);
    }
}

void RecentProjectInfo::reset()
{
    this->localInfo.path = File();
    this->localInfo.title = {};
    this->localInfo.lastModifiedMs = 0;
}
