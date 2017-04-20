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
    class DiffLogic;

    // Diff - это просто будущий RevisionItem.
    // в дифф можно добавлять данные (это делает DiffLogic), а в RevisionItem - нельзя.

    class Diff : public TrackedItem
    {
    public:

        explicit Diff(TrackedItem &diffTarget);

        ~Diff() override;

        bool hasAnyChanges() const;

        // manages new Delta and XmlElement
        void addOwnedDelta(Delta *newDelta, XmlElement *newDeltaData);

        void clear();


        //===------------------------------------------------------------------===//
        // TrackedItem
        //

        int getNumDeltas() const override;

        Delta *getDelta(int index) const override;

        XmlElement *createDeltaDataFor(int index) const override;

        String getVCSName() const override;

        DiffLogic *getDiffLogic() const override;
        
        void resetStateTo(const TrackedItem &newState) override { }


    protected:

        OwnedArray<Delta> deltas;

        OwnedArray<XmlElement> deltasData;

        String description;

    private:

        ScopedPointer<DiffLogic> logic;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Diff)

    };
} // namespace VCS
