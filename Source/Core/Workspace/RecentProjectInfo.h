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
#include "ProjectDto.h"

class RecentProjectInfo final : public Serializable,
                                public ReferenceCountedObject
{
public:

    RecentProjectInfo() = default;
    RecentProjectInfo(const ProjectDto &remoteInfo);
    RecentProjectInfo(const String &localId,
        const String &localTitle, const String &localPath);

    Time getUpdatedAt() const noexcept;
    String getProjectId() const noexcept;
    String getTitle() const;
    File getLocalFile() const;

    bool hasLocalCopy() const noexcept;
    bool hasRemoteCopy() const noexcept;
    bool isValid() const;

    void updateRemoteInfo(const ProjectDto &remoteInfo);
    void updateLocalInfo(const String &localId, const String &localTitle, const String &localPath);
    void updateLocalTimestampAsNow();

    void resetLocalInfo();
    void resetRemoteInfo();

    using Ptr = ReferenceCountedObjectPtr<RecentProjectInfo>;
    static int compareElements(RecentProjectInfo *first, RecentProjectInfo *second);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    virtual ValueTree serialize() const override;
    virtual void deserialize(const ValueTree &tree) override;
    virtual void reset() override;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecentProjectInfo)
};
