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
        ValueTree getUserStash(int index) const;
        ValueTree getUserStashWithName(const String &stashName) const;
        
        ValueTree getQuickStash() const;
        bool hasQuickStash() const;
        void storeQuickStash(ValueTree newStash);
        void resetQuickStash();
        
        void addStash(ValueTree newStash);
        void removeStash(ValueTree stashToRemove);

        //===--------------------------------------------------------------===//
        // Serializable
        //===--------------------------------------------------------------===//

        ValueTree serialize() const override;
        void deserialize(const ValueTree &tree) override;
        void reset() override;

        typedef ReferenceCountedObjectPtr<StashesRepository> Ptr;

    private:

        Pack::Ptr pack;

        // root node for the stashes
        ValueTree userStashes;

        // root node for quick-toggled changes
        ValueTree quickStash;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StashesRepository);
    };
} // namespace VCS
