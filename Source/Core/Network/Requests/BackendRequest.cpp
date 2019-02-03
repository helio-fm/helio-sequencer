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
#include "BackendRequest.h"
#include "Workspace.h"
#include "UserProfile.h"
#include "Network.h"

// Let OS set the default timeout:
#define CONNECTION_TIMEOUT_MS (0)
#define NUM_CONNECT_ATTEMPTS (3)

static bool progressCallbackInternal(void *const context, int bytesSent, int totalBytes)
{
    const auto connection = static_cast<const BackendRequest *const>(context);
    if (connection->progressCallback != nullptr)
    {
        connection->progressCallback(bytesSent, totalBytes);
    }

    return true; // always continue
}

BackendRequest::Response::Response() :
    statusCode(0),
    receipt(Result::fail({})) {}

bool BackendRequest::Response::isValid() const noexcept
{
    return this->receipt.wasOk() && this->statusCode != 500;
}

bool BackendRequest::Response::is2xx() const noexcept
{
    return (this->statusCode / 100) == 2;
}

bool BackendRequest::Response::is200() const noexcept
{
    return this->statusCode == 200;
}

bool BackendRequest::Response::is(int code) const noexcept
{
    return this->statusCode == code;
}

bool BackendRequest::Response::hasProperty(const Identifier &name) const noexcept
{
    return this->body.hasProperty(name);
}

var BackendRequest::Response::getProperty(const Identifier &name) const noexcept
{
    return this->body.getProperty(name);
}

ValueTree BackendRequest::Response::getChild(const Identifier &name) const noexcept
{
    return this->body.getChildWithName(name);
}

const Array<String> &BackendRequest::Response::getErrors() const noexcept
{
    return this->errors;
}

const ValueTree BackendRequest::Response::getBody() const noexcept
{
    return this->body;
}

const String BackendRequest::Response::getRedirect() const noexcept
{
    //if (this->statusCode == 301 || this->statusCode == 302)
    return this->headers.getValue("location", {});
}

BackendRequest::BackendRequest(const String &apiEndpoint, ProgressCallback progressCallback) :
    apiEndpoint(apiEndpoint),
    progressCallback(progressCallback),
    serializer(true) {}

static String getHeaders()
{
    static const String apiVersion1 = "application/helio.fm.v1+json";
    static const String userAgent = "Helio " + App::getAppReadableVersion() +
        (SystemStats::isOperatingSystem64Bit() ? " 64-bit on " : " 32-bit on ") +
        SystemStats::getOperatingSystemName();

    String extraHeaders;
    extraHeaders
        << "Accept: " << apiVersion1
        << "\r\n"
        << "Content-Type: " << apiVersion1
        << "\r\n"
        << "User-Agent: " << userAgent
        << "\r\n";

    if (App::Workspace().getUserProfile().isLoggedIn())
    {
        extraHeaders
            << "Authorization: Bearer " << App::Workspace().getUserProfile().getApiToken()
            << "\r\n";
    }

    return extraHeaders;
}

void BackendRequest::processResponse(BackendRequest::Response &response, InputStream *const stream) const
{
    if (stream == nullptr)
    {
        return;
    }

    // Try to parse response as JSON object wrapping all properties
    const String responseBody = stream->readEntireStreamAsString();
    if (responseBody.isNotEmpty())
    {
        DBG("<< Received " << response.statusCode << " " // << responseBody);
            << responseBody.substring(0, 128) << (responseBody.length() > 128 ? ".." : ""));

        ValueTree parsedResponse;
        response.receipt = this->serializer.loadFromString(responseBody, parsedResponse);
        if (response.receipt.failed() || !parsedResponse.isValid())
        {
            response.errors.add(TRANS("network error"));
            return;
        }

        response.body = parsedResponse;

        // Try to parse errors
        if (response.statusCode < 200 || response.statusCode >= 400)
        {
            using namespace Serialization;
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

BackendRequest::Response BackendRequest::put(const ValueTree &payload) const
{
    return this->doRequest(payload, "PUT");
}

BackendRequest::Response BackendRequest::post(const ValueTree &payload) const
{
    return this->doRequest(payload, "POST");
}

BackendRequest::Response BackendRequest::get() const
{
    return this->doRequest("GET");
}

BackendRequest::Response BackendRequest::del() const
{
    return this->doRequest("DELETE");
}

BackendRequest::Response BackendRequest::doRequest(const String &verb) const
{
    Response response;
    ScopedPointer<InputStream> stream;
    const auto url = URL(Routes::Api::baseURL + this->apiEndpoint);

    int i = 0;
    do
    {
        DBG(">> " << verb << " " << this->apiEndpoint);
        stream = url.createInputStream(false,
            progressCallbackInternal, (void *)(this),
            getHeaders(), CONNECTION_TIMEOUT_MS,
            &response.headers, &response.statusCode,
            5, verb);
    } while (stream == nullptr && i++ < NUM_CONNECT_ATTEMPTS);

    processResponse(response, stream);
    return response;
}

BackendRequest::Response BackendRequest::doRequest(const ValueTree &payload, const String &verb) const
{
    Response response;
    ScopedPointer<InputStream> stream;

    String jsonPayload;
    if (this->serializer.saveToString(jsonPayload, payload).failed())
    {
        return response;
    }

    const auto url = URL(Routes::Api::baseURL + this->apiEndpoint)
        .withPOSTData(MemoryBlock(jsonPayload.toRawUTF8(), jsonPayload.getNumBytesAsUTF8()));

    int i = 0;
    do
    {
        DBG(">> " << verb << " " << this->apiEndpoint << " " 
            << jsonPayload.substring(0, 128) + (jsonPayload.length() > 128 ? ".." : ""));

        stream = url.createInputStream(true,
            progressCallbackInternal, (void *)(this),
            getHeaders(), CONNECTION_TIMEOUT_MS,
            &response.headers, &response.statusCode,
            5, verb);
    } while (stream == nullptr && i++ < NUM_CONNECT_ATTEMPTS);

    processResponse(response, stream);
    return response;
}
