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

#include "Delta.h"
#include "SerializationKeys.h"

namespace VCS
{
    class DiffLogic;

    class TrackedItem
    {
    public:

        TrackedItem() {}

        virtual ~TrackedItem() {}

        const Uuid &getUuid() const { return this->vcsUuid; }

        void setVCSUuid(Uuid value) { this->vcsUuid = value; }

        
        virtual int getNumDeltas() const = 0;

        virtual Delta *getDelta(int index) const = 0;

        virtual XmlElement *createDeltaDataFor(int index) const = 0;

        virtual String getVCSName() const = 0;

        virtual DiffLogic *getDiffLogic() const = 0;

        virtual void resetStateTo(const TrackedItem &newState) = 0;


        void serializeVCSUuid(XmlElement &xml) const
        {
            xml.setAttribute(Serialization::VCS::vcsItemId, this->getUuid().toString());
        }

        void deserializeVCSUuid(const XmlElement &xml)
        {
            this->vcsUuid = xml.getStringAttribute(Serialization::VCS::vcsItemId, this->vcsUuid.toString());
        }

    protected:

        Uuid vcsUuid; // needs to be serialized by subclasses

    };
} // namespace VCS
