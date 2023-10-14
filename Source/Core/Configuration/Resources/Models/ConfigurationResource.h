/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class ConfigurationResource : public Serializable, public ReferenceCountedObject
{
public:

    virtual String getResourceId() const = 0;

    virtual Identifier getResourceType() const = 0;

    using Ptr = ReferenceCountedObjectPtr<ConfigurationResource>;

    virtual int compareElements(const ConfigurationResource::Ptr first,
        const ConfigurationResource::Ptr second) const
    {
        return first->getResourceId().compare(second->getResourceId());
    }
};

class DummyConfigurationResource : public ConfigurationResource
{
public:
    String getResourceId() const override { return {}; }
    Identifier getResourceType() const override { return {}; }
    SerializedData serialize() const override { return {}; }
    void deserialize(const SerializedData &data) override {}
    void reset() override {}
};
