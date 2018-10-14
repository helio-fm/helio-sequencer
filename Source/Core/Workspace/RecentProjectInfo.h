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

#include "Serializable.h"

class RecentProjectInfo final : public Serializable,
                                public ReferenceCountedObject
{
public:

    RecentProjectInfo() = default;

    // bool isLoaded() const; // fuck that. will ask for workspace

    String getProjectId() const noexcept
    {
        return this->projectId;
    }

    File getLocalFile() const
    {
        if (this->local != nullptr)
        {
            // file may be missed here, because we store absolute path,
            // but some platforms, like iOS, change documents folder path on every app run
            // so we need to check in a document folder as well:
            return this->local->path.existsAsFile() ?
                this->local->path :
                File(DocumentHelpers::getDocumentSlot(this->local->path.getFileName()));
        }

        return {};
    }

    using Ptr = ReferenceCountedObjectPtr<RecentProjectInfo>;
    static int compareElements(RecentProjectInfo::Ptr first, RecentProjectInfo::Ptr second)
    {
        // todo order, loaded first, etc
        //if (first->local == nullptr && second->local != nullptr) { return -1; }
        //if (second->local == nullptr && first->local != nullptr) { return 1; }

        return int(second->local->lastModifiedMs - first->local->lastModifiedMs);
    }

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    virtual ValueTree serialize() const override
    {

    }

    virtual void deserialize(const ValueTree &tree) override
    {

    }

    virtual void reset() override
    {

    }

private:

    struct LocalInfo final
    {
        File path;
        String title;
        int64 lastModifiedMs;
    };

    struct RemoteInfo final
    {
        String alias;
        String title;
        int64 lastModifiedMs;
    };

    String projectId;

    UniquePointer<LocalInfo> local;
    UniquePointer<RemoteInfo> remote;

};