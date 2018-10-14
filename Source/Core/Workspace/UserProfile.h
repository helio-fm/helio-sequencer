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
#include "RecentProjectInfo.h"
#include "UserSessionInfo.h"
#include "UserProfileDto.h"
#include "ProjectDto.h"

class UserProfile final : public Serializable,
                          public ChangeBroadcaster
{
public:

    UserProfile() = default;

    void updateProfile(const UserProfileDto &dto);

    void updateLocalProjectInfo(const String &id, const String &title, const String &path);
    void updateRemoteProjectInfo(const ProjectDto &info);

    void onProjectLoaded(const String &id);
    void onProjectUnloaded(const String &id);
    void onProjectDeleted(const String &id);

    //void onSessionRemoved(const String &deviceId);

    const ReferenceCountedArray<UserSessionInfo> &getSessions() const noexcept;
    const ReferenceCountedArray<RecentProjectInfo> &getProjects() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    virtual ValueTree serialize() const override;
    virtual void deserialize(const ValueTree &tree) override;
    virtual void reset() override;

private:

    // returns found item's index - or -1, if not found
    int findProjectIndexByPath(const String &path) const;
    int findProjectIndexById(const String &id) const;

    ReadWriteLock projectsListLock;
    ReferenceCountedArray<RecentProjectInfo> projects;

    ReadWriteLock sessionsListLock;
    ReferenceCountedArray<UserSessionInfo> sessions;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UserProfile)
    JUCE_DECLARE_WEAK_REFERENCEABLE(UserProfile)
};