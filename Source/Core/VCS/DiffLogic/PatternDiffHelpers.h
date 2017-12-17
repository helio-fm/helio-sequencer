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

#include "PatternDeltas.h"
#include "Pattern.h"
#include "Delta.h"

namespace VCS
{
    class PatternDiffHelpers
    {
    public:

        static NewSerializedDelta serializePatternChanges(Array<Clip> changes,
            const String &description, int64 numChanges,
            const String &deltaType);

        static bool checkIfDeltaIsPatternType(const Delta *delta);

        static XmlElement *mergeClipsAdded(const XmlElement *state, const XmlElement *changes);
        static XmlElement *mergeClipsRemoved(const XmlElement *state, const XmlElement *changes);
        static XmlElement *mergeClipsChanged(const XmlElement *state, const XmlElement *changes);

        static Array<NewSerializedDelta> createClipsDiffs(const XmlElement *state, const XmlElement *changes);
    };
} // namespace VCS
