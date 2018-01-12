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
#include "HttpConnection.h"
#include "App.h"
#include "Config.h"

#define NUM_CONNECT_ATTEMPTS (3)

// Let OS set the default timeout:
#define CONNECTION_TIMEOUT_MS (0)

// This is a testing back-end, which is supposed to be moved to https://helio.fm, once it is ready:
#define BASE_URL "https://musehackers.com/api/v1/"

HttpConnection::HttpConnection(URL url, ProgressCallback progressCallback) :
    url(url),
    progressCallback(progressCallback) {}

static bool progressCallbackInternal(void *const context, int bytesSent, int totalBytes)
{
    const auto connection = static_cast<const HttpConnection *const>(context);
    if (connection->progressCallback != nullptr)
    {
        connection->progressCallback(bytesSent, totalBytes);
    }

    return true; // always continue
}

HttpConnection::Response::Response() :
    statusCode(0),
    result(Result::fail({})) {}

HttpConnection::Response HttpConnection::request(ResponseType type) const
{
    Response response;

    const String deviceId(Config::getMachineId());
    const String authToken = "TODO";
    String extraHeaders;
    extraHeaders
        << "User-Agent: Helio " << App::getAppReadableVersion()
        << "\n"
        << "Device-Id: " << deviceId
        << "\n"
        << "Authorization: " << authToken
        << "\n"
        << "Platform: " << SystemStats::getOperatingSystemName();
    
    ScopedPointer<InputStream> stream;
    for (int i = 0; stream == nullptr && i < NUM_CONNECT_ATTEMPTS; ++i)
    {
        Logger::writeToLog("Connecting to " + this->url.toString(true));
        stream = this->url.createInputStream(true,
            progressCallbackInternal, (void *)(this),
            extraHeaders, CONNECTION_TIMEOUT_MS,
            &response.headers, &response.statusCode);
    }

    if (stream == nullptr)
    {
        return response;
    }

    // TODO process unauthorized requests etc

    if (response.statusCode >= 200 && response.statusCode < 300)
    {
        response.result = JSON::parse(stream->readEntireStreamAsString(), response.jsonBody);
    }

    return response;
}
