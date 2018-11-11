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
#include "ProjectSyncHelpers.h"

void ProjectSyncHelpers::buildLocalRevisionsIndex(RevisionsMap &map, VCS::Revision::Ptr root)
{
    map[root->getUuid()] = root;
    for (auto *child : root->getChildren())
    {
        buildLocalRevisionsIndex(map, child);
    }
}

bool ProjectSyncHelpers::findParentIn(const String &id, const ReferenceCountedArray<VCS::Revision> &list)
{
    for (const auto *child : list)
    {
        if (child->getUuid() == id)
        {
            return true;
        }
    }

    return false;
}

ReferenceCountedArray<VCS::Revision> ProjectSyncHelpers::constructNewLocalTrees(const ReferenceCountedArray<VCS::Revision> &list)
{
    ReferenceCountedArray<VCS::Revision> trees;
    for (const auto child : list)
    {
        // Since local revisions already have their children in places,
        // we only need to figure out which ones are the roots:
        if (child->getParent() == nullptr || !findParentIn(child->getParent()->getUuid(), list))
        {
            trees.add(child);
        }
    }

    return trees;
}

RevisionsMap ProjectSyncHelpers::constructNewRemoteTrees(const Array<RevisionDto> &list)
{
    struct ShallowRevision final
    {
        VCS::Revision::Ptr revision;
        RevisionDto associatedDto;
    };

    using ShallowRevisionsMap = FlatHashMap<String, ShallowRevision, StringHash>;

    RevisionsMap trees;
    ShallowRevisionsMap lookup;
    for (const auto &dto : list)
    {
        VCS::Revision::Ptr newRevision(new VCS::Revision(dto));
        lookup[dto.getId()] = { newRevision, dto };
    }

    for (const auto &child : lookup)
    {
        if (lookup.contains(child.second.associatedDto.getParentId()))
        {
            const auto proposedParent = lookup[child.second.associatedDto.getParentId()];
            proposedParent.revision->addChild(child.second.revision);
        }
    }

    for (const auto &child : lookup)
    {
        if (child.second.revision->getParent() == nullptr)
        {
            trees[child.second.associatedDto.getParentId()] = child.second.revision;
        }
    }

    return trees;
}
