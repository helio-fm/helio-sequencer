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

#include "TrackedItem.h"

namespace VCS
{
    class RevisionItem :
        public TrackedItem,
        public Serializable,
        public ReferenceCountedObject
    {
    public:

        enum Type
        {
            Undefined = 0,
            Added = 1,
            Removed = 2,
            Changed = 3
        };

        RevisionItem(Type type, TrackedItem *targetToCopy);

        RevisionItem::Type getType() const noexcept;
        String getTypeAsString() const;

        //===--------------------------------------------------------------===//
        // TrackedItem
        //===--------------------------------------------------------------===//

        int getNumDeltas() const noexcept override;
        Delta *getDelta(int index) const noexcept override;
        ValueTree getDeltaData(int deltaIndex) const noexcept override;

        String getVCSName() const noexcept override;
        DiffLogic *getDiffLogic() const noexcept override;
        void resetStateTo(const TrackedItem &newState) noexcept override {} // never reset
    
        //===--------------------------------------------------------------===//
        // Serializable
        //===--------------------------------------------------------------===//

        ValueTree serialize() const override;
        void deserialize(const ValueTree &tree) override;
        void deserialize(const ValueTree &tree, const DeltaDataLookup &dataLookup);
        void reset() override;

        using Ptr = ReferenceCountedObjectPtr<RevisionItem>;

    private:

        OwnedArray<Delta> deltas;
        Array<ValueTree> deltasData;
        ScopedPointer<DiffLogic> logic;

        Type vcsItemType;
        String description;
        String diffLogicType;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RevisionItem);

    };
}  // namespace VCS
