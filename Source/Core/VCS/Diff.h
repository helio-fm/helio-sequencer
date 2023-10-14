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
    class DiffLogic;

    // You can think of a Diff as of mutable RevisionItem.
    // DiffLogic fills diff with data and after that diff is used to create RevisionItem.

    class Diff : public TrackedItem
    {
    public:

        explicit Diff(TrackedItem &diffTarget);

        bool hasAnyChanges() const;
        void applyDeltas(Array<DeltaDiff> &&deltaDiffs);
        void applyDelta(DeltaDiff &&deltaDiff);
        void applyDelta(Delta *newDelta, SerializedData data);
        void clear();

        //===--------------------------------------------------------------===//
        // TrackedItem
        //===--------------------------------------------------------------===//

        int getNumDeltas() const override;
        Delta *getDelta(int index) const override;
        SerializedData getDeltaData(int deltaIndex) const override;
        String getVCSName() const override;
        DiffLogic *getDiffLogic() const override;
        void resetStateTo(const TrackedItem &newState) override {}
        Colour getRevisionDisplayColour() const override;

    protected:

        OwnedArray<Delta> deltas;
        Array<SerializedData> deltasData;
        String description;
        Colour displayColur;

    private:

        UniquePointer<DiffLogic> logic;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Diff)

    };
} // namespace VCS
