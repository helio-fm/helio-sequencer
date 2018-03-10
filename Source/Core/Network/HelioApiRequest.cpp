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
#include "HelioApiRequest.h"
#include "SessionManager.h"
#include "App.h"

// Let OS set the default timeout:
#define CONNECTION_TIMEOUT_MS (0)
#define NUM_CONNECT_ATTEMPTS (3)

static bool progressCallbackInternal(void *const context, int bytesSent, int totalBytes)
{
    const auto connection = static_cast<const HelioApiRequest *const>(context);
    if (connection->progressCallback != nullptr)
    {
        connection->progressCallback(bytesSent, totalBytes);
    }

    return true; // always continue
}

HelioApiRequest::Response::Response() :
    statusCode(0),
    parseResult(Result::fail({})) {}

HelioApiRequest::HelioApiRequest(String apiEndpoint, ProgressCallback progressCallback) :
    apiEndpoint(std::move(apiEndpoint)),
    progressCallback(progressCallback) {}

static String getHeaders()
{
    String extraHeaders;
    extraHeaders
        << "Content-Type: application/json"
        << "\n"
        << "Client: Helio " << App::getAppReadableVersion()
        << "\n"
        << "Authorization: Bearer " << SessionManager::getApiToken()
        << "\n"
        << "Platform-Id: " << SystemStats::getOperatingSystemName()
        << "\n"
        << "Device-Id: " << Config::getDeviceId();

    return extraHeaders;
}

static void processResponse(HelioApiRequest::Response &response, InputStream *const stream)
{
    if (stream == nullptr)
    {
        return;
    }

    // Try to parse response as JSON object wrapping all properties
    const String responseBody = stream->readEntireStreamAsString();
    if (responseBody.isNotEmpty())
    {
        ValueTree parsedResponse;
        static JsonSerializer serializer;

        response.parseResult = serializer.loadFromString(responseBody, parsedResponse);
        if (response.parseResult.failed() || !parsedResponse.isValid())
        {
            response.errors.add(TRANS("network error"));
            return;
        }

        using namespace Serialization;

        if (parsedResponse.hasType(Api::V1::rootElementSuccess) ||
            parsedResponse.hasType(Api::V1::rootElementErrors))
        {
            response.body = parsedResponse;
        }

        // Try to parse errors
        if (response.statusCode < 200 && response.statusCode >= 300)
        {
            for (int i = 0; i < parsedResponse.getNumProperties(); ++i)
            {
                const auto key = parsedResponse.getPropertyName(i);
                const auto value = parsedResponse.getProperty(key).toString();
                if (key == Api::V1::status || key == Api::V1::message)
                {
                    response.errors.add(value);
                }
                else
                {
                    response.errors.add(key.toString() + ": " + value);
                }
            }
        }
    }
}

HelioApiRequest::Response HelioApiRequest::post(const var payload) const
{
    Response response;
    ScopedPointer<InputStream> stream;
    const auto jsonPayload = JSON::toString(payload);
    const auto url = URL(HelioFM::baseURL + this->apiEndpoint)
        .withPOSTData(MemoryBlock(jsonPayload.toRawUTF8(), jsonPayload.getNumBytesAsUTF8() + 1));

    int i = 0;
    do
    {
        Logger::writeToLog("Connecting to " + this->apiEndpoint);
        stream = url.createInputStream(true,
            progressCallbackInternal, (void *)(this),
            getHeaders(), CONNECTION_TIMEOUT_MS,
            &response.headers, &response.statusCode);
    }
    while (stream == nullptr && i++ < NUM_CONNECT_ATTEMPTS);

    processResponse(response, stream);
    return response;
}

HelioApiRequest::Response HelioApiRequest::get() const
{
    Response response;
    ScopedPointer<InputStream> stream;
    const auto url = URL(HelioFM::baseURL + this->apiEndpoint);

    int i = 0;
    do
    {
        Logger::writeToLog("Connecting to " + this->apiEndpoint);
        stream = url.createInputStream(false,
            progressCallbackInternal, (void *)(this),
            getHeaders(), CONNECTION_TIMEOUT_MS,
            &response.headers, &response.statusCode);
    } while (stream == nullptr && i++ < NUM_CONNECT_ATTEMPTS);

    processResponse(response, stream);
    return response;
}
