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

class Clip;
class Pattern;

#include "Diff.h"
#include "DiffLogic.h"

namespace VCS
{
    class PatternDiffLogic : public DiffLogic
    {
    public:

        explicit PatternDiffLogic(TrackedItem &targetItem);

        ~PatternDiffLogic() override;

        //===------------------------------------------------------------------===//
        // DiffLogic
        //

        const String getType() const override;

        void resetStateTo(const TrackedItem &newState) override;

        Diff *createDiff(const TrackedItem &initialState) const override;

        Diff *createMergedItem(const TrackedItem &initialState) const override;

    private:

        XmlElement *mergeClipsAdded(const XmlElement *state, const XmlElement *changes) const;

        XmlElement *mergeClipsRemoved(const XmlElement *state, const XmlElement *changes) const;

        XmlElement *mergeClipsChanged(const XmlElement *state, const XmlElement *changes) const;

    private:

        Array<NewSerializedDelta> createEventsDiffs(const XmlElement *state, const XmlElement *changes) const;

    private:

        void deserializeChanges(Pattern &pattern,
            const XmlElement *state,
            const XmlElement *changes,
            Array<Clip> &stateClips,
            Array<Clip> &changesClips) const;

        NewSerializedDelta serializeChanges(Array<Clip> changes,
            const String &description,
            int64 numChanges,
            const String &deltaType) const;

        XmlElement *serializePattern(Array<Clip> changes,
            const String &tag) const;

        bool checkIfDeltaIsPatternType(const Delta *delta) const;


    };
} // namespace VCS
