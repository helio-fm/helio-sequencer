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

// Prevents 406 errors when juce sends its headers:
#define HELIO_USERAGENT ("User-Agent: Mozilla")
// For a simple client checks:
#define HELIO_SALT ("Vx_C+[VX2w xA 6WLhnLNRT{1#P<etaFG/zo5ckazm{4!a{JV7y+=}-q+^t|p`c")
#define HELIO_ARPS_URL ("https://helioworkstation.com/api/v2/arps/")
#define HELIO_UPDATES_CHECK_URL ("https://helioworkstation.com/api/v2/updates/")
#define HELIO_TRANSLATIONS_URL ("https://helioworkstation.com/api/v2/translations/")
#define HELIO_VCS_REMOTE_URL ("https://helioworkstation.com/api/v2/vcs/")
#define HELIO_LOGIN_URL ("https://helioworkstation.com/api/v2/login/")
#define HELIO_LOGOUT_URL ("https://helioworkstation.com/api/v2/logout/")
#define HELIO_PROJECTLIST_URL ("https://helioworkstation.com/api/v2/projects/")
#define HELIO_SUPERVISOR_URL ("https://helioworkstation.com/api/v2/statistics/")
#define HELIO_COLOUR_SCHEMES_URL ("https://helioworkstation.com/api/v2/colours/")
#define HELIO_TRANSLATIONS_HELP_URL ("https://helioworkstation.com/translations/")
