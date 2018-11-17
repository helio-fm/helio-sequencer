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
#include "UserProfile.h"
#include "SessionService.h"
#include "SerializationKeys.h"
#include "Config.h"
#include "App.h"

static UserSessionInfo kSessionsSort;
static RecentProjectInfo kProjectsSort;

void UserProfile::updateProfile(const UserProfileDto &dto)
{
    // Resize avatar thumbnail and cache it as png-encoded base64 string:
    const int size = 16;
    this->avatar = { Image::RGB, size, size, true };
    MemoryInputStream inputStream(dto.getAvatarData(), false);
    const Image remoteAvatar = ImageFileFormat::loadFrom(inputStream)
        .rescaled(size, size, Graphics::highResamplingQuality);

    Graphics g(this->avatar);
    g.setTiledImageFill(remoteAvatar, 0, 0, 1.f);
    g.fillAll();

    if (this->avatar.isValid() && this->avatarThumbnail.isEmpty())
    {
        MemoryBlock block;
        {
            MemoryOutputStream outStream(block, false);
            this->imageFormat.writeImageToStream(this->avatar, outStream);
        }

        this->avatarThumbnail = Base64::toBase64(block.getData(), block.getSize());
    }

    for (const auto p : dto.getProjects())
    {
        this->onProjectRemoteInfoUpdated(p);
    }

    for (const auto s : dto.getSessions())
    {
        if (auto *session = this->findSession(s.getDeviceId()))
        {
            const ScopedWriteLock lock(this->sessionsListLock);
            session->updateRemoteInfo(s);
        }
        else
        {
            const ScopedWriteLock lock(this->sessionsListLock);
            this->sessions.add(new UserSessionInfo(s));
        }
    }

    this->name = dto.getName();
    this->login = dto.getLogin();
    this->profileUrl = dto.getProfileUrl();

    this->sendChangeMessage();
}

void UserProfile::onProjectLocalInfoUpdated(const String &id, const String &title, const String &path)
{
    if (auto *project = this->findProject(id))
    {
        const ScopedWriteLock lock(this->projectsListLock);
        project->updateLocalInfo(id, title, path);
    }
    else
    {
        const ScopedWriteLock lock(this->projectsListLock);
        this->projects.add(new RecentProjectInfo(id, title, path));
    }

    this->sendChangeMessage();
}

void UserProfile::onProjectRemoteInfoUpdated(const ProjectDto &info)
{
    if (auto *project = this->findProject(info.getId()))
    {
        const ScopedWriteLock lock(this->projectsListLock);
        project->updateRemoteInfo(info);
    }
    else
    {
        const ScopedWriteLock lock(this->projectsListLock);
        this->projects.add(new RecentProjectInfo(info));
    }

    this->sendChangeMessage();
}

void UserProfile::onProjectUnloaded(const String &id)
{
    if (auto *project = this->findProject(id))
    {
        const ScopedWriteLock lock(this->projectsListLock);
        project->updateLocalTimestampAsNow();
    }

    this->sendChangeMessage();
}

void UserProfile::deleteProjectLocally(const String &id)
{
    for (auto *project : this->projects)
    {
        if (project->getProjectId() == id && project->hasLocalCopy())
        {
            project->getLocalFile().deleteFile();
            project->resetLocalInfo();
            if (!project->isValid()) // i.e. doesn't have remote copy
            {
                const ScopedWriteLock lock(this->projectsListLock);
                this->projects.removeObject(project);
            }
            break;
        }
    }

    this->sendChangeMessage();
}

void UserProfile::deleteProjectRemotely(const String &id)
{
    for (auto *project : this->projects)
    {
        if (project->getProjectId() == id && project->hasRemoteCopy())
        {
            //App::Helio().getResourceSyncService()->deleteProject(projectId);
            //// and then:
            //project->resetRemoteInfo();

            //if (!project->isValid()) // i.e. doesn't have local copy
            //{
            //    const ScopedWriteLock lock(this->projectsListLock);
            //    this->projects.removeObject(project);
            //}
            break;
        }
    }

    this->sendChangeMessage();

}

void UserProfile::clearProfileAndSession()
{
    this->setApiToken({});
    this->avatar = {};
    this->avatarThumbnail.clear();
    this->name.clear();
    this->login.clear();
    this->profileUrl.clear();
}

bool UserProfile::needsAvatarImage() const noexcept
{
    return this->avatarThumbnail.isEmpty();
}

Image UserProfile::getAvatar() const noexcept
{
    return this->avatar;
}

String UserProfile::getLogin() const noexcept
{
    return this->login;
}

String UserProfile::getProfileUrl() const noexcept
{
    return this->profileUrl;
}

String UserProfile::getApiToken() const
{
    // Kinda legacy, it should have been stored in profile
    // but let's keep it as is for now
    return Config::get(Serialization::Api::sessionToken, {});
}

void UserProfile::setApiToken(const String &token)
{
    Config::set(Serialization::Api::sessionToken, token);
    this->sendChangeMessage();
}

bool UserProfile::isLoggedIn() const
{
    return this->getApiToken().isNotEmpty();
}

const ReferenceCountedArray<UserSessionInfo> &UserProfile::getSessions() const noexcept
{
    return this->sessions;
}

const ReferenceCountedArray<RecentProjectInfo> &UserProfile::getProjects() const noexcept
{
    return this->projects;
}

// There won't be too much projects and sessions, so linear search should be ok:
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

UserSessionInfo *UserProfile::findSession(const String &deviceId) const
{
    for (auto *session : this->sessions)
    {
        if (session->getDeviceId() == deviceId)
        {
            return session;
        }
    }

    return nullptr;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree UserProfile::serialize() const
{
    using namespace Serialization::User;

    const ScopedReadLock lock(this->projectsListLock);
    ValueTree tree(Profile::userProfile);

    if (this->profileUrl.isNotEmpty())
    {
        tree.setProperty(Profile::url, this->profileUrl, nullptr);
    }

    if (this->name.isNotEmpty())
    {
        tree.setProperty(Profile::name, this->name, nullptr);
    }

    if (this->login.isNotEmpty())
    {
        tree.setProperty(Profile::login, this->login, nullptr);
    }

    if (this->avatarThumbnail.isNotEmpty())
    {
        tree.setProperty(Profile::thumbnail, this->avatarThumbnail, nullptr);
    }

    for (const auto *project : this->projects)
    {
        tree.appendChild(project->serialize(), nullptr);
    }

    for (const auto *session : this->sessions)
    {
        tree.appendChild(session->serialize(), nullptr);
    }

    return tree;
}

void UserProfile::deserialize(const ValueTree &tree)
{
    this->reset();
    using namespace Serialization::User;
    const auto root = tree.hasType(Profile::userProfile) ?
        tree : tree.getChildWithName(Profile::userProfile);

    if (!root.isValid()) { return; }

    this->name = root.getProperty(Profile::name);
    this->login = root.getProperty(Profile::login);
    this->profileUrl = root.getProperty(Profile::url);
    this->avatarThumbnail = root.getProperty(Profile::thumbnail);

    if (this->avatarThumbnail.isNotEmpty())
    {
        MemoryBlock block;
        {
            MemoryOutputStream outStream(block, false);
            Base64::convertFromBase64(outStream, this->avatarThumbnail);
        }

        this->avatar = ImageFileFormat::loadFrom(block.getData(), block.getSize());
    }

    {
        const ScopedWriteLock lock(this->projectsListLock);
        forEachValueTreeChildWithType(root, child, RecentProjects::recentProject)
        {
            RecentProjectInfo::Ptr p(new RecentProjectInfo());
            p->deserialize(child);
            if (p->isValid())
            {
                this->projects.addSorted(kProjectsSort, p.get());
            }
        }
    }
    
    {
        const ScopedWriteLock lock(this->sessionsListLock);
        forEachValueTreeChildWithType(root, child, Sessions::session)
        {
            UserSessionInfo::Ptr s(new UserSessionInfo());
            s->deserialize(child);
            // TODO store JWT as well? remove current session if stale?
            this->sessions.addSorted(kSessionsSort, s.get());
        }
    }

    // TODO scan documents folder for existing projects not present in the list?
    // or only do it when the list is empty?

    // and get all workspace's projects

    this->sendChangeMessage();
}

void UserProfile::reset()
{
    const ScopedWriteLock lock(this->projectsListLock);
    this->projects.clearQuick();
    this->sessions.clearQuick();
    this->sendChangeMessage();
}
