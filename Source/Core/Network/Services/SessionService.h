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

#if !NO_NETWORK

#include "BackendService.h"
#include "AuthThread.h"
#include "TokenUpdateThread.h"
#include "BaseConfigSyncThread.h"
#include "RequestUserProfileThread.h"
#include "UpdatesCheckThread.h"

class SessionService final : private BackendService
{
public:
    
    SessionService(UserProfile &userProfile);

    void signIn(const String &provider);
    void cancelSignInProcess();
    void signOut();

private:

    UserProfile &userProfile;

    AuthThread *prepareAuthThread();
    TokenUpdateThread *prepareTokenUpdateThread();
    RequestUserProfileThread *prepareProfileRequestThread();

    JUCE_DECLARE_NON_COPYABLE(SessionService)
};

#endif
