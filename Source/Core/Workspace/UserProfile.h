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

    void onProjectUnloaded(const String &id);
    void onProjectLocalInfoUpdated(const String &id, const String &title, const String &path);
    void onProjectLocalInfoReset(const String &id);

    void onProjectRemoteInfoReset(const String &id);
    void onConfigurationInfoReset(const Identifier &type, const String &name);

    void deleteProjectLocally(const String &id);

#if !NO_NETWORK

    void clearProfileAndSession();

    //===------------------------------------------------------------------===//
    // Network
    //===------------------------------------------------------------------===//

    void updateProfile(const UserProfileDto &dto);

    void onProjectRemoteInfoUpdated(const ProjectDto &info);
    void onConfigurationInfoUpdated(const UserResourceDto &resource);

    void deleteProjectRemotely(const String &id);

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    String getApiToken() const;
    void setApiToken(const String &token);

    bool needsAvatarImage() const noexcept;
    Image getAvatar() const noexcept;
    String getLogin() const noexcept;
    String getProfileUrl() const noexcept;

    bool hasSyncedConfiguration(const Identifier &type, const String &name) const;

#endif

    bool isLoggedIn() const;

#if !NO_NETWORK
    using SessionsList = ReferenceCountedArray<UserSessionInfo, CriticalSection>;
    const SessionsList &getSessions() const noexcept;
#endif

    using ProjectsList = ReferenceCountedArray<RecentProjectInfo, CriticalSection>;
    const ProjectsList &getProjects() const noexcept;

    using ResourcesList = ReferenceCountedArray<SyncedConfigurationInfo, CriticalSection>;
    const ResourcesList &getResources() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    virtual SerializedData serialize() const override;
    virtual void deserialize(const SerializedData &data) override;
    virtual void reset() override;

private:

    RecentProjectInfo *findProject(const String &id) const;

#if !NO_NETWORK
    UserSessionInfo *findSession(const String &deviceId) const;
#endif

    Image avatar;
    String avatarThumbnail;
    PNGImageFormat imageFormat;

    String name;
    String login;
    String profileUrl;

#if !NO_NETWORK
    SessionsList sessions;
#endif
    ProjectsList projects;
    ResourcesList resources;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UserProfile)
    JUCE_DECLARE_WEAK_REFERENCEABLE(UserProfile)
};
