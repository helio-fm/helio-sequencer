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

// A shorthand to call request thread's listener on a main thread
#define callRequestListener(threadType, function, ...) \
    MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void* \
        { \
            const auto self = static_cast<threadType *>(ptr); \
            self->listener->function(__VA_ARGS__); \
            return nullptr; \
        }, this)

class HelioApiRequest final
{
public:

    using ProgressCallback = Function<void(int, int)>;

    HelioApiRequest(String apiEndpoint, ProgressCallback progressCallback = nullptr);

    struct Response final
    {
        Response();

        // Was able to connect and parse correct response
        bool isValid() const noexcept;

        // Status code check
        bool is2xx() const noexcept;
        bool is200() const noexcept;

        // ValueTree wrappers
        bool hasProperty(const Identifier &name) const noexcept;
        var getProperty(const Identifier &name) const noexcept;
        ValueTree getChild(const Identifier &name) const noexcept;

        const Array<String> &getErrors() const noexcept;
        const ValueTree getBody() const noexcept;

    private:

        ValueTree body;

        // optional detailed errors descriptions
        Array<String> errors;

        int statusCode;
        Result receipt;
        StringPairArray headers;

        friend class HelioApiRequest;
    };

    Response post(const ValueTree &payload) const;
    Response get() const;

    ProgressCallback progressCallback;

private:

    String apiEndpoint;
    JsonSerializer serializer;

    void processResponse(Response &response, InputStream *const stream) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HelioApiRequest)
};
