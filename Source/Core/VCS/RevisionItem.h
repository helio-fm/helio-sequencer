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

#include "TrackedItem.h"

namespace VCS
{
    class RevisionItem final :
        public TrackedItem,
        public Serializable,
        public ReferenceCountedObject
    {
    public:

        enum class Type : int8
        {
            Undefined = 0,
            Added = 1,
            Removed = 2,
            Changed = 3
        };

        RevisionItem(Type type, TrackedItem *targetToCopy);

        RevisionItem::Type getType() const noexcept;
        String getTypeAsString() const;
        Colour getDisplayColour() const noexcept;

        //===--------------------------------------------------------------===//
        // TrackedItem
        //===--------------------------------------------------------------===//

        int getNumDeltas() const noexcept override;
        Delta *getDelta(int index) const noexcept override;
        SerializedData getDeltaData(int deltaIndex) const noexcept override;

        String getVCSName() const noexcept override;
        DiffLogic *getDiffLogic() const noexcept override;
        void resetStateTo(const TrackedItem &newState) noexcept override {} // never reset

        //===--------------------------------------------------------------===//
        // Serializable
        //===--------------------------------------------------------------===//

        SerializedData serialize() const override;
        void deserialize(const SerializedData &data) override;
        void reset() override;

        using Ptr = ReferenceCountedObjectPtr<RevisionItem>;

    private:

        OwnedArray<Delta> deltas;
        Array<SerializedData> deltasData;
        UniquePointer<DiffLogic> logic;

        Type vcsItemType;
        String description;

        // optional, not serialized
        Colour displayColour;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RevisionItem);
    };
}  // namespace VCS
