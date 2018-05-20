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
#include "Pack.h"

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

        RevisionItem(Pack::Ptr packPtr, Type type, TrackedItem *targetToCopy);

        void flushData(); // move deltas data to pack
        Pack::Ptr getPackPtr() const;
        RevisionItem::Type getType() const;
        String getTypeAsString() const;
        void importDataForDelta(const ValueTree &deltaDataToCopy, const String &deltaUuid);

        //===--------------------------------------------------------------===//
        // TrackedItem
        //===--------------------------------------------------------------===//

        int getNumDeltas() const override;
        Delta *getDelta(int index) const override;
        ValueTree serializeDeltaData(int deltaIndex) const override;
        String getVCSName() const override;
        DiffLogic *getDiffLogic() const override;
        void resetStateTo(const TrackedItem &newState) override { } // never reset
    
        //===--------------------------------------------------------------===//
        // Serializable
        //===--------------------------------------------------------------===//

        ValueTree serialize() const override;
        void deserialize(const ValueTree &tree) override;
        void reset() override;

        using Ptr = ReferenceCountedObjectPtr<RevisionItem>;

    private:

        Pack::Ptr pack;

        OwnedArray<Delta> deltas;
        Array<ValueTree> deltasData;
        ScopedPointer<DiffLogic> logic;

        Type vcsItemType;
        String description;
        String diffLogicType;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RevisionItem);

    };
}  // namespace VCS
