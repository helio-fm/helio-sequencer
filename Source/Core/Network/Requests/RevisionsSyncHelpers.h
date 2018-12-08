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

#include "Revision.h"
#include "RevisionDto.h"

using RevisionsMap = FlatHashMap<String, VCS::Revision::Ptr, StringHash>;

struct RevisionsSyncHelpers final
{
    static void buildLocalRevisionsIndex(RevisionsMap &map, VCS::Revision::Ptr root);

    static bool findParentIn(const String &id, const ReferenceCountedArray<VCS::Revision> &list);

    static ReferenceCountedArray<VCS::Revision> constructNewLocalTrees(const ReferenceCountedArray<VCS::Revision> &list);

    // returns a map of <id of a parent to mount to : root revision containing all the children>
    static RevisionsMap constructRemoteBranches(const Array<RevisionDto> &list);

    // only used when cloning projects, assuming all revisions will fit in one subtree
    static VCS::Revision::Ptr constructRemoteTree(const Array<RevisionDto> &list);
};
