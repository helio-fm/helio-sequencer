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
#include "RevisionDto.h"

namespace VCS
{
    class Revision final : public Serializable, public ReferenceCountedObject
    {
    public:

        enum SyncState
        {
            NoSync,         // revision is present locally, but it is unknown if remote copy exists
            ShallowCopy,    // remote revision info has been added to the tree, but data is yet to be fetched
            FullSync,       // either local revision that was pushed, or a remote that was fully pulled
        };

        using Ptr = ReferenceCountedObjectPtr<Revision>;

        Revision(const String &name = {});

#if !NO_NETWORK
        Revision(const RevisionDto &remoteDescription); // creates shallow copy
#endif

        const ReferenceCountedArray<RevisionItem> &getItems() const noexcept;
        const ReferenceCountedArray<Revision> &getChildren() const noexcept;

        void addItem(RevisionItem *item);
        void addItem(RevisionItem::Ptr item);

        void addChild(Revision *revision);
        void addChild(Revision::Ptr revision);

        void removeChild(Revision *revision);
        void removeChild(Revision::Ptr revision);

        void copyDeltasFrom(Revision::Ptr other);

        bool isShallowCopy() const noexcept;

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
