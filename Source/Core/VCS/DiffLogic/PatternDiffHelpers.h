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

#include "Pattern.h"
#include "Delta.h"

namespace VCS
{
    class PatternDiffHelpers
    {
    public:

        static DeltaDiff serializePatternChanges(Array<Clip> changes,
            const String &description, int64 numChanges,
            const Identifier &deltaType);

        static bool checkIfDeltaIsPatternType(const Delta *delta);

        static ValueTree mergeClipsAdded(const ValueTree &state, const ValueTree &changes);
        static ValueTree mergeClipsRemoved(const ValueTree &state, const ValueTree &changes);
        static ValueTree mergeClipsChanged(const ValueTree &state, const ValueTree &changes);

        static Array<DeltaDiff> createClipsDiffs(const ValueTree &state, const ValueTree &changes);
    };
} // namespace VCS
