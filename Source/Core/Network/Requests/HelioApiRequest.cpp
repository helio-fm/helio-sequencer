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
#include "SessionService.h"
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
    receipt(Result::fail({})) {}

bool HelioApiRequest::Response::isValid() const noexcept
{
    return this->receipt.wasOk() && this->statusCode != 500;
}

bool HelioApiRequest::Response::is2xx() const noexcept
{
    return (this->statusCode / 100) == 2;
}

bool HelioApiRequest::Response::is200() const noexcept
{
    return this->statusCode == 200;
}

bool HelioApiRequest::Response::hasProperty(const Identifier &name) const noexcept
{
    return this->body.hasProperty(name);
}

var HelioApiRequest::Response::getProperty(const Identifier &name) const noexcept
{
    return this->body.getProperty(name);
}

ValueTree HelioApiRequest::Response::getChild(const Identifier &name) const noexcept
{
    return this->body.getChildWithName(name);
}

const Array<String> &HelioApiRequest::Response::getErrors() const noexcept
{
    return this->errors;
}

const ValueTree HelioApiRequest::Response::getBody() const noexcept
{
    return this->body;
}

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
        << "Authorization: Bearer " << SessionService::getApiToken()
        << "\n"
        << "Platform-Id: " << SystemStats::getOperatingSystemName()
        << "\n"
        << "Device-Id: " << Config::getDeviceId();

    return extraHeaders;
}

void HelioApiRequest::processResponse(HelioApiRequest::Response &response, InputStream *const stream) const
{
    if (stream == nullptr)
    {
        return;
    }

    // Try to parse response as JSON object wrapping all properties
    const String responseBody = stream->readEntireStreamAsString();
    if (responseBody.isNotEmpty())
    {
        Logger::writeToLog("Response: " + String(response.statusCode));
        Logger::writeToLog(responseBody);

        ValueTree parsedResponse;
        static JsonSerializer serializer;

        response.receipt = serializer.loadFromString(responseBody, parsedResponse);
        if (response.receipt.failed() || !parsedResponse.isValid())
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
