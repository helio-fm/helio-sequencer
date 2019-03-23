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

class Workspace;
class SessionService;
class ResourceSyncService;
class ProjectSyncService;

namespace Routes
{
    namespace Web
    {
        // TODO: make base urls configurable
        static const String baseURL = "https://helio.fm";
        static const String translationsURL = "/translations";
    }

    namespace Api
    {
        static const String baseURL = "https://api.helio.fm";
        static const String initWebAuth = "/clients/helio/auth";
        static const String finaliseWebAuth = "/clients/helio/auth/check";
        static const String baseResource = "/clients/helio/:resourceType";
        static const String customResource = "/my/resources/:resourceType/:resourceId";
        static const String updatesInfo = "/clients/helio/info";
        static const String userProfile = "/my/profile";
        static const String tokenCheck = "/my/sessions/current/status";
        static const String tokenUpdate = "/my/sessions/current/update";
        static const String session = "/my/sessions/:deviceId";
        static const String projects = "/my/projects";
        static const String project = "/my/projects/:projectId";
        static const String projectRevision = "/my/projects/:projectId/revisions/:revisionId";
    }
}

class Network final
{
public:

    explicit Network(Workspace &workspace);
    ~Network();
    
    SessionService *getSessionService() const noexcept;
    ProjectSyncService *getProjectSyncService() const noexcept;
    ResourceSyncService *getResourceSyncService() const noexcept;

private:

    UniquePointer<SessionService> sessionService;
    UniquePointer<ProjectSyncService> projectSyncService;
    UniquePointer<ResourceSyncService> resourceSyncService;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Network)
};
