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

#include "Common.h"
#include "NetworkServices.h"
#include "Workspace.h"
#include "SessionService.h"
#include "ProjectSyncService.h"
#include "ResourceSyncService.h"

#if !NO_NETWORK

Network::Network(Workspace &workspace)
{
    // Prepare back-end APIs communication services
    this->sessionService = make<SessionService>(workspace.getUserProfile());
    this->projectSyncService = make<ProjectSyncService>();
    this->resourceSyncService = make<ResourceSyncService>();
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

#endif
