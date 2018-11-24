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

#include "TrackedItemsSource.h"
#include "RevisionItem.h"

namespace VCS
{
    class RevisionItem;

    class Snapshot final : public TrackedItemsSource
    {
    public:

        Snapshot() = default;
        explicit Snapshot(const Snapshot &other);
        explicit Snapshot(const Snapshot *other);

        void addItem(RevisionItem::Ptr item);
        void removeItem(RevisionItem::Ptr item);
        void mergeItem(RevisionItem::Ptr item);

        //===--------------------------------------------------------------===//
        // TrackedItemsSource
        //===--------------------------------------------------------------===//

        String getVCSId() const noexcept override { return "<snapshot>"; }
        String getVCSName() const noexcept override { return "<snapshot>"; }
        
        int getNumTrackedItems() noexcept override;
        TrackedItem *getTrackedItem(int index) noexcept override;

        RevisionItem::Ptr getItemWithUuid(const Uuid &uuid) const;

        void onResetState() override {}

    private:

        RevisionItem::Ptr getItemWithSameUuid(RevisionItem::Ptr item) const;

        Array<RevisionItem::Ptr> items;

        JUCE_LEAK_DETECTOR(Snapshot);

    };
} // namespace VCS
