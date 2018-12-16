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

    void onProjectLocalInfoUpdated(const String &id, const String &title, const String &path);
    void onProjectLocalInfoReset(const String &id);

    void onProjectRemoteInfoUpdated(const ProjectDto &info);
    void onProjectRemoteInfoReset(const String &id);

    void onProjectUnloaded(const String &id);

    void deleteProjectLocally(const String &id);
    void deleteProjectRemotely(const String &id);

    void clearProfileAndSession();

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    bool isLoggedIn() const;
    String getApiToken() const;
    void setApiToken(const String &token);

    const ReferenceCountedArray<UserSessionInfo> &getSessions() const noexcept;
    const ReferenceCountedArray<RecentProjectInfo> &getProjects() const noexcept;

    bool needsAvatarImage() const noexcept;
    Image getAvatar() const noexcept;
    String getLogin() const noexcept;
    String getProfileUrl() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    virtual ValueTree serialize() const override;
    virtual void deserialize(const ValueTree &tree) override;
    virtual void reset() override;

private:

    RecentProjectInfo *findProject(const String &id) const;
    UserSessionInfo *findSession(const String &deviceId) const;

    Image avatar;
    String avatarThumbnail;
    PNGImageFormat imageFormat;

    String name;
    String login;
    String profileUrl;

    ReadWriteLock projectsListLock;
    ReferenceCountedArray<RecentProjectInfo> projects;

    ReadWriteLock sessionsListLock;
    ReferenceCountedArray<UserSessionInfo> sessions;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UserProfile)
    JUCE_DECLARE_WEAK_REFERENCEABLE(UserProfile)
};
