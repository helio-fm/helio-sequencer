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
    result(Result::fail({})) {}

HelioApiRequest::HelioApiRequest(String apiEndpoint, ProgressCallback progressCallback) :
    apiEndpoint(std::move(apiEndpoint)),
    progressCallback(progressCallback) {}

static String getPostHeaders()
{
    String extraHeaders;
    extraHeaders
        << "Content-Type: application/json"
        << "\n"
        << "User-Agent: Helio " << App::getAppReadableVersion()
        << "\n"
        << "Authorization: Bearer " << SessionManager::getApiToken()
        << "\n"
        << "Platform-Id: " << SystemStats::getOperatingSystemName()
        << "\n"
        << "Device-Id: " << Config::getMachineId();

    return extraHeaders;
}

static Array<String> parseVarAsArray(const var &json, const String &prefix = {})
{
    Array<String> result;
    if (json.isArray())
    {
        for (int i = 0; i < json.size(); ++i)
        {
            const var &message(json[i]);
            if (message.isString())
            {
                result.add(prefix + message);
            }
        }
    }
    return result;
}

HelioApiRequest::Response HelioApiRequest::post(const var payload) const
{
    const String jsonPayload = JSON::toString(payload);
    const URL url = URL(HelioFM::baseURL + this->apiEndpoint)
        .withPOSTData(MemoryBlock(jsonPayload.toRawUTF8(), jsonPayload.getNumBytesAsUTF8() + 1));

    Response response;
    ScopedPointer<InputStream> stream;
    int i = 0;

    do
    {
        Logger::writeToLog("Connecting to " + this->apiEndpoint);
        stream = url.createInputStream(true,
            progressCallbackInternal, (void *)(this),
            getPostHeaders(), CONNECTION_TIMEOUT_MS,
            &response.headers, &response.statusCode);
    }
    while (stream == nullptr && i++ < NUM_CONNECT_ATTEMPTS);

    if (stream == nullptr)
    {
        return response;
    }

    // Try to parse response as JSON object wrapping all properties
    const String responseBody = stream->readEntireStreamAsString();
    if (responseBody.isNotEmpty())
    {
        var parsedJson;
        response.result = JSON::parse(responseBody, parsedJson);
        if (DynamicObject *responseObject = parsedJson.getDynamicObject())
        {
            response.jsonBody = responseObject->getProperties();

            // Try to parse errors
            if (response.statusCode < 200 && response.statusCode >= 300)
            {
                // Response may contain `errors` object or array with detailed descriptions
                const var errorsJson = responseObject->getProperty(Serialization::Api::V1::errors);
                if (DynamicObject *errorsObject = errorsJson.getDynamicObject())
                {
                    for (const auto &error : errorsObject->getProperties())
                    {
                        response.errors.addArray(parseVarAsArray(errorsJson, error.name + " "));
                    }
                }
                else
                {
                    response.errors.addArray(parseVarAsArray(errorsJson));
                }
            }
        }
    }

    return response;
}
