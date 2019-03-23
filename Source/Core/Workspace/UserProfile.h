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
#include "SyncedConfigurationInfo.h"
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

    void onConfigurationInfoUpdated(const UserResourceDto &resource);
    void onConfigurationInfoReset(const Identifier &type, const String &name);


    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    bool isLoggedIn() const;
    String getApiToken() const;
    void setApiToken(const String &token);

    bool needsAvatarImage() const noexcept;
    Image getAvatar() const noexcept;
    String getLogin() const noexcept;
    String getProfileUrl() const noexcept;

    bool hasSyncedConfiguration(const Identifier &type, const String &name) const;

    using ProjectsList = ReferenceCountedArray<RecentProjectInfo, CriticalSection>;
    using SessionsList = ReferenceCountedArray<UserSessionInfo, CriticalSection>;
    using ResourcesList = ReferenceCountedArray<SyncedConfigurationInfo, CriticalSection>;

    const SessionsList &getSessions() const noexcept;
    const ProjectsList &getProjects() const noexcept;
    const ResourcesList &getResources() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    virtual ValueTree serialize() const override;
    virtual void deserialize(const ValueTree &tree) override;
    virtual void reset() override;

private:

    RecentProjectInfo *findProject(const String &id) const;
    UserSessionInfo *findSession(const String &deviceId) const;
    SyncedConfigurationInfo *findResource(const Identifier &type, const String &name) const;

    Image avatar;
    String avatarThumbnail;
    PNGImageFormat imageFormat;

    String name;
    String login;
    String profileUrl;

    SessionsList sessions;
    ProjectsList projects;
    ResourcesList resources;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UserProfile)
    JUCE_DECLARE_WEAK_REFERENCEABLE(UserProfile)
};
