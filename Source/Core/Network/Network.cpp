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
#include "Network.h"
#include "Workspace.h"
#include "SessionService.h"
#include "ProjectSyncService.h"
#include "ResourceSyncService.h"

Network::Network(Workspace &workspace)
{
    // Prepare back-end APIs communication services
    this->sessionService.reset(new SessionService(workspace.getUserProfile()));
    this->projectSyncService.reset(new ProjectSyncService());
    this->resourceSyncService.reset(new ResourceSyncService());
}

Network::~Network()
{
    this->resourceSyncService = nullptr;
    this->projectSyncService = nullptr;
    this->sessionService = nullptr;
}

SessionService *Network::getSessionService() const noexcept
{
    return this->sessionService.get();
}

ProjectSyncService *Network::getProjectSyncService() const noexcept
{
    return this->projectSyncService.get();
}

ResourceSyncService *Network::getResourceSyncService() const noexcept
{
    return this->resourceSyncService.get();
}
