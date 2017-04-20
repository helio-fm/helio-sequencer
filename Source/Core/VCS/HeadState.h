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

    class HeadState : public TrackedItemsSource
    {
    public:

        HeadState();
        
        HeadState(const HeadState &other);
        
        explicit HeadState(const HeadState *other);

        ~HeadState() override;


        void addItem(RevisionItem::Ptr item);

        void removeItem(RevisionItem::Ptr item);

        void mergeItem(RevisionItem::Ptr item);


        //===------------------------------------------------------------------===//
        // TrackedItemsSource
        //

        String getVCSName() const override
        {
            return "<head state>";
        }
        
        int getNumTrackedItems() override;

        TrackedItem *getTrackedItem(int index) override;


        RevisionItem::Ptr getItemWithSameUuid(RevisionItem::Ptr item) const;

        RevisionItem::Ptr getItemWithUuid(const Uuid &uuid) const;

    private:

        Array<RevisionItem::Ptr> items;

        JUCE_LEAK_DETECTOR(HeadState);

    };
} // namespace VCS
