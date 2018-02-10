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

#include "RevisionItem.h"
#include "Pack.h"

namespace VCS
{
    // First, this class was a wrapped around ValueTree
    // But once JUCE developers have decided to make ValueTree final,
    // I had to turn this into a set of static helper functions

    class Revision final
    {
    public:

        static ValueTree create(Pack::Ptr pack, const String &name = String::empty);
        static void copyProperties(ValueTree one, ValueTree another);
        static void copyDeltas(ValueTree one, ValueTree another);

        static MD5 calculateHash(ValueTree revision);
        static void incrementVersion(ValueTree revision);
        static void flush(ValueTree revision);

        static String getMessage(ValueTree revision);
        static String getUuid(ValueTree revision);
        static int64 getTimeStamp(ValueTree revision);
        static bool isEmpty(ValueTree revision);

        //===--------------------------------------------------------------===//
        // Serializable
        //===--------------------------------------------------------------===//

        static XmlElement *serialize(ValueTree revision);
        static void deserialize(ValueTree revision, const XmlElement &xml);
        static void reset(ValueTree revision);

    };
}  // namespace VCS
