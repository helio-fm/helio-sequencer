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

#include "JsonSerializer.h"

class BackendRequest final
{
public:

    BackendRequest(const String &apiEndpoint);

    struct Response final
    {
        Response();

        // Was able to connect and parse correct response
        bool isValid() const noexcept;

        // Status code check
        bool is2xx() const noexcept;
        bool is200() const noexcept;
        bool is(int code) const noexcept;

        // SerializedData wrappers
        bool hasProperty(const Identifier &name) const noexcept;
        var getProperty(const Identifier &name) const noexcept;
        SerializedData getChild(const Identifier &name) const noexcept;

        const Array<String> &getErrors() const noexcept;
        const SerializedData getBody() const noexcept;
        const String getRedirect() const noexcept;

    private:

        SerializedData body;

        // optional detailed errors descriptions
        Array<String> errors;

        int statusCode;
        Result receipt;
        StringPairArray headers;

        friend class BackendRequest;
    };

    Response get() const;
    Response post(const SerializedData &payload) const;
    Response put(const SerializedData &payload) const;
    Response del() const;

private:

    String apiEndpoint;
    JsonSerializer serializer;

    Response doRequest(const String &verb) const;
    Response doRequest(const SerializedData &payload, const String &verb) const;
    void processResponse(Response &response, InputStream *const stream) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BackendRequest)
};
