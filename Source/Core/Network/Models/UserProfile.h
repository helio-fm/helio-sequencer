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

class UserProfile final : public ApiModel
{
public:

    UserProfile(const ValueTree &tree) : ApiModel(tree) {}

    //struct ProjectInfo final
    //{
    //    String getProjectName() const noexcept { return this->data.getProperty(Serialization::Api::V1::projectName); }
    //    const ValueTree data;
    //};

    String getEmail() const noexcept
    { return this->data.getProperty(Serialization::Api::V1::email); }

    String getLogin() const noexcept
    { return this->data.getProperty(Serialization::Api::V1::login); }

    String getName() const noexcept
    { return this->data.getProperty(Serialization::Api::V1::name); }

    JUCE_LEAK_DETECTOR(UserProfile)
};
