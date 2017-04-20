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

#include "Serializable.h"
#include "Revision.h"
#include "Pack.h"

namespace VCS
{
    class StashesRepository : public ReferenceCountedObject, public Serializable
    {
    public:

        explicit StashesRepository(Pack::Ptr pack);


        int getNumUserStashes() const;

        String getUserStashDescription(int index) const;

        Revision getUserStash(int index) const;

        Revision getUserStashWithName(const String &stashName) const;

        
        Revision getQuickStash() const;
        
        bool hasQuickStash() const;
        
        void storeQuickStash(Revision newStash);

        void resetQuickStash();

        
        void addStash(Revision newStash);
        
        void removeStash(Revision stashToRemove);


        //===------------------------------------------------------------------===//
        // Serializable
        //

        XmlElement *serialize() const override;

        void deserialize(const XmlElement &xml) override;

        void reset() override;


        typedef ReferenceCountedObjectPtr<StashesRepository> Ptr;

    private:

        Pack::Ptr pack;

        // корневая нода для стэшей
        Revision userStashes;

        // ревизия для quick toggle changes on/off
        Revision quickStash;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StashesRepository);
    };
} // namespace VCS
