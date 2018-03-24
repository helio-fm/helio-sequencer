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

namespace HelioFM
{
    // This is a testing back-end, which is supposed
    // to be moved to https://helio.fm, once it is ready:
    static const String baseURL = "https://musehackers.com";
    static const String translationsURL = baseURL + "/translations";

    namespace Api
    {
        namespace V1
        {
            static const String join = "/api/v1/join";
            static const String login = "/api/v1/login";
            static const String tokenCheck = "/api/v1/session-status";
            static const String tokenUpdate = "/api/v1/relogin";
            static const String requestResource = "/api/v1/client/helio";
            static const String requestUpdatesInfo = "/api/v1/client/helio/info";
            static const String requestUserProfile = "/api/v1/me";
            static const String vcs = "/api/v1/vcs/:project";
        } // namespace V1
    } // namespace API
}  // namespace HelioFM
