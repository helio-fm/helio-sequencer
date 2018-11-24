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
#include "RevisionsSyncHelpers.h"

void RevisionsSyncHelpers::buildLocalRevisionsIndex(RevisionsMap &map, VCS::Revision::Ptr root)
{
    map[root->getUuid()] = root;
    for (auto *child : root->getChildren())
    {
        buildLocalRevisionsIndex(map, child);
    }
}

bool RevisionsSyncHelpers::findParentIn(const String &id, const ReferenceCountedArray<VCS::Revision> &list)
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

ReferenceCountedArray<VCS::Revision> RevisionsSyncHelpers::constructNewLocalTrees(const ReferenceCountedArray<VCS::Revision> &list)
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

struct ShallowRevision final
{
    String parentId;
    VCS::Revision::Ptr revision;
};

using ShallowRevisionsMap = FlatHashMap<String, ShallowRevision, StringHash>;

RevisionsMap RevisionsSyncHelpers::constructRemoteBranches(const Array<RevisionDto> &list)
{
    RevisionsMap trees;
    ShallowRevisionsMap lookup;
    for (const auto &dto : list)
    {
        VCS::Revision::Ptr newRevision(new VCS::Revision(dto));
        lookup[dto.getId()] = { dto.getParentId(), newRevision };
    }

    for (const auto &child : lookup)
    {
        if (lookup.contains(child.second.parentId))
        {
            const auto proposedParent = lookup[child.second.parentId];
            proposedParent.revision->addChild(child.second.revision);
        }
    }

    for (const auto &child : lookup)
    {
        if (child.second.revision->getParent() == nullptr)
        {
            // despite the parent revision is nullptr (i.e. just missing in the given list),
            // it's stored parent id should not be empty, otherwise we have different root revisions
            // locally and remotely, which hopefully should never happen:
            jassert(child.second.parentId.isNotEmpty());
            trees[child.second.parentId] = child.second.revision;
        }
    }

    return trees;
}

VCS::Revision::Ptr RevisionsSyncHelpers::constructRemoteTree(const Array<RevisionDto> &list)
{
    VCS::Revision::Ptr root;
    ShallowRevisionsMap lookup;
    for (const auto &dto : list)
    {
        VCS::Revision::Ptr newRevision(new VCS::Revision(dto));
        lookup[dto.getId()] = { dto.getParentId(), newRevision };
    }

    for (const auto &child : lookup)
    {
        if (lookup.contains(child.second.parentId))
        {
            const auto proposedParent = lookup[child.second.parentId];
            proposedParent.revision->addChild(child.second.revision);
        }
    }

    for (const auto &child : lookup)
    {
        if (child.second.revision->getParent() == nullptr)
        {
            // hitting this line means that given list cannot be composed into a single tree,
            // which should never happen assuming this method is used then cloning a history
            jassert(root == nullptr);
            root = child.second.revision;
        }
    }

    jassert(root != nullptr);
    return root;
}
