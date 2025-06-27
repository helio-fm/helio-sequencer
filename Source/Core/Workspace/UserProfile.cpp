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
#include "UserProfile.h"
#include "App.h"
#include "Config.h"
#include "SerializationKeys.h"

static RecentProjectInfo kProjectsSort;

void UserProfile::onProjectLocalInfoUpdated(const String &id, const String &title, const String &path)
{
    if (auto *project = this->findProject(id))
    {
        project->updateLocalInfo(title, path);
        this->projects.sort(kProjectsSort);
    }
    else
    {
        this->projects.addSorted(kProjectsSort, new RecentProjectInfo(id, title, path));
    }

    this->sendChangeMessage();
}

void UserProfile::onProjectUnloaded(const String &id)
{
    if (auto *project = this->findProject(id))
    {
        project->updateLocalTimestampAsNow();
        this->projects.sort(kProjectsSort);
    }

    this->sendChangeMessage();
}

void UserProfile::deleteProjectLocally(const String &id)
{
    if (auto *project = this->findProject(id))
    {
        if (project->getLocalFile().existsAsFile())
        {
            project->getLocalFile().deleteFile();
            this->projects.removeObject(project);
            this->sendChangeMessage();
        }
    }
}

const UserProfile::ProjectsList &UserProfile::getProjects() const noexcept
{
    return this->projects;
}

// there won't be too much projects, so linear search should be ok
// (might have to replace this with binary search someday though)
RecentProjectInfo *UserProfile::findProject(const String &id) const
{
    for (auto *project : this->projects)
    {
        if (project->getProjectId() == id)
        {
            return project;
        }
    }

    return nullptr;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData UserProfile::serialize() const
{
    using namespace Serialization;

    SerializedData tree(User::profile);

    for (const auto *project : this->projects)
    {
        tree.appendChild(project->serialize());
    }

    return tree;
}

void UserProfile::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization;
    const auto root = data.hasType(User::profile) ?
        data : data.getChildWithName(User::profile);

    if (!root.isValid()) { return; }

    forEachChildWithType(root, child, User::RecentProjects::recentProject)
    {
        RecentProjectInfo::Ptr p(new RecentProjectInfo());
        p->deserialize(child);
        if (p->isValid())
        {
            this->projects.addSorted(kProjectsSort, p.get());
        }
    }

    // TODO scan documents folder for existing projects not present in the list?
    // or only do it when the list is empty?

    // and get all workspace's projects

    this->sendChangeMessage();
}

void UserProfile::reset()
{
    this->projects.clearQuick();
    this->sendChangeMessage();
}
