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

#include "Serializable.h"
#include "SerializationKeys.h"

struct ApiModel : Serializable
{
    ApiModel(const ValueTree &tree) : data(tree) {}

    template<typename T>
    Array<T> getChildren(const Identifier &id) const
    {
        Array<T> result;
        forEachValueTreeChildWithType(this->data, child, id)
        {
            result.add({ child });
        }
        return result;
    }

    bool isValid() const noexcept
    {
        return this->data.isValid();
    }

    ValueTree serialize() const override
    {
        return this->data;
    }

    void deserialize(const ValueTree &tree) override
    {
        this->data = tree;
    }

    void reset() override
    {
        this->data = {};
    }

protected:

    ValueTree data;
};

#define DTO_PROPERTY(x) this->data.getProperty(Serialization::Api::V1::x)
#define DTO_CHILDREN(c, x) this->getChildren<c>(Serialization::Api::V1::x);
#define DTO_CHILD(x) this->data.getChildWithName(Serialization::Api::V1::x);
