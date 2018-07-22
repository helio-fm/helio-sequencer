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

#include "ApiModel.h"

class AuthSession final : public ApiModel
{
public:

    AuthSession(const ValueTree &tree) : ApiModel(tree) {}

    String getSessionId() const noexcept
    { return this->data.getProperty(Serialization::Api::V1::authSessionId); }

    String getKey() const noexcept
    { return this->data.getProperty(Serialization::Api::V1::authSessionKey); }

    URL getAuthURI() const noexcept
    { return { this->data.getProperty(Serialization::Api::V1::authSessionUri) }; }

    String getToken() const noexcept
    { return this->data.getProperty(Serialization::Api::V1::token); }

    JUCE_LEAK_DETECTOR(AuthSession)
};
