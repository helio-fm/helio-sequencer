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

#include "Serializable.h"
#include "RevisionItem.h"

namespace VCS
{
    class Revision final : public Serializable, public ReferenceCountedObject
    {
    public:

        using Ptr = ReferenceCountedObjectPtr<Revision>;

        Revision(const String &name = {});

        const ReferenceCountedArray<RevisionItem> &getItems() const noexcept;
        const ReferenceCountedArray<Revision> &getChildren() const noexcept;

        void addItem(RevisionItem *item);
        void addItem(RevisionItem::Ptr item);

        void addChild(Revision *revision);
        void addChild(Revision::Ptr revision);

        void removeChild(Revision *revision);
        void removeChild(Revision::Ptr revision);

        void copyDeltasFrom(Revision::Ptr other);

        WeakReference<Revision> getParent() const noexcept;
        String getMessage() const noexcept;
        String getUuid() const noexcept;
        int64 getTimeStamp() const noexcept;
        bool isEmpty() const noexcept;

        //===--------------------------------------------------------------===//
        // Serializable
        //===--------------------------------------------------------------===//

        // with no properties and no children, but with full deltas data
        // (to be used in synchronization threads):
        SerializedData serializeDeltas() const;
        void deserializeDeltas(SerializedData data);

        SerializedData serialize() const;
        void deserialize(const SerializedData &data);
        void reset();

    private:

        WeakReference<Revision> parent;

        String id;
        String message;
        int64 timestamp;

        ReferenceCountedArray<Revision> children;
        ReferenceCountedArray<RevisionItem> deltas;

        JUCE_DECLARE_WEAK_REFERENCEABLE(Revision)
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Revision)
    };
}  // namespace VCS
