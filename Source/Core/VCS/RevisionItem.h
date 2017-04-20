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

        ~RevisionItem() override;


        void flushData(); // move deltas data to pack

        Pack::Ptr getPackPtr() const;

        RevisionItem::Type getType() const;

        String getTypeAsString() const;
        
        void importDataForDelta(const XmlElement *deltaDataToCopy, const String &deltaUuid);


        //===------------------------------------------------------------------===//
        // TrackedItem
        //

        int getNumDeltas() const override;

        Delta *getDelta(int index) const override;

        XmlElement *createDeltaDataFor(int index) const override;

        String getVCSName() const override;

        DiffLogic *getDiffLogic() const override;
        
        void resetStateTo(const TrackedItem &newState) override { } // never resetted

    

        //===------------------------------------------------------------------===//
        // Serializable
        //

        XmlElement *serialize() const override;

        void deserialize(const XmlElement &xml) override;

        void reset() override;


        typedef ReferenceCountedObjectPtr<RevisionItem> Ptr;

    private:

        Pack::Ptr pack;

        OwnedArray<Delta> deltas;

        OwnedArray<XmlElement> deltasData;

        ScopedPointer<DiffLogic> logic;

        Type vcsItemType;

        String description;

        String diffLogicType;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RevisionItem);

    };
}  // namespace VCS
