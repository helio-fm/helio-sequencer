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

namespace Routes
{
    namespace HelioFM
    {
        namespace Web
        {
            static const String baseURL = "https://helio.fm";
            static const String translationsURL = "/translations";
        }

        namespace Api
        {
            static const String baseURL = "https://api.helio.fm";
            static const String tokenCheck = "/session/status";
            static const String tokenUpdate = "/session/update";
            static const String initWebAuth = "/clients/helio/auth";
            static const String finaliseWebAuth = "/clients/helio/auth/check";
            static const String requestResource = "/clients/helio";
            static const String requestUpdatesInfo = "/clients/helio/info";
            static const String requestUserProfile = "/my/profile";
            static const String requestUserResource = "/my/resources";
            static const String vcs = "/vcs";
        } // namespace Api
    } // namespace HelioFM
} // namespace Routes
