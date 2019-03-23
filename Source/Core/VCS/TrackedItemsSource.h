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

namespace VCS
{
    class TrackedItem;

    class TrackedItemsSource
    {
    public:

        TrackedItemsSource() {}
        virtual ~TrackedItemsSource() {};

        virtual String getVCSId() const = 0;
        virtual String getVCSName() const = 0;

        virtual int getNumTrackedItems() = 0;
        virtual TrackedItem *getTrackedItem(int index) = 0;

        virtual TrackedItem *initTrackedItem(const Identifier &type,
            const Uuid &id, const VCS::TrackedItem &newState) { return nullptr; }

        virtual bool deleteTrackedItem(TrackedItem *item) { return false; }
        virtual void clearAllTrackedItems()
        {
            Array<TrackedItem *> itemsToClear;

            for (int i = 0; i < this->getNumTrackedItems(); ++i)
            {
                itemsToClear.add(this->getTrackedItem(i));
            }

            for (auto i : itemsToClear)
            {
                this->deleteTrackedItem(i);
            }
        }

        // Called after checkout / reset to / etc
        virtual void onResetState() = 0;

    private:

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackedItemsSource)
    };
} // namespace VCS
