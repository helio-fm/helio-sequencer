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
#include "DiffLogic.h"

namespace VCS
{
    class TrackedItem
    {
    public:

        TrackedItem() = default;
        virtual ~TrackedItem() = default;

        const Uuid &getUuid() const { return this->vcsUuid; }
        void setVCSUuid(Uuid value) { this->vcsUuid = value; }
        
        virtual int getNumDeltas() const = 0;
        virtual Delta *getDelta(int index) const = 0;
        virtual SerializedData getDeltaData(int deltaIndex) const = 0;
        virtual bool deltaHasDefaultData(int deltaIndex) const { return false; }
        
        // optional, not persistent
        virtual Colour getRevisionDisplayColour() const { return {}; }

        virtual String getVCSName() const = 0;
        virtual DiffLogic *getDiffLogic() const = 0;
        virtual void resetStateTo(const TrackedItem &newState) = 0;

        void serializeVCSUuid(SerializedData &tree) const
        {
            tree.setProperty(Serialization::VCS::vcsItemId, this->getUuid().toString());
        }

        void deserializeVCSUuid(const SerializedData &tree)
        {
            this->vcsUuid = tree.getProperty(Serialization::VCS::vcsItemId, this->vcsUuid.toString());
        }

    protected:

        Uuid vcsUuid; // needs to be serialized by subclasses

    };
} // namespace VCS
