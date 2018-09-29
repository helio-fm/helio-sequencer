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

#include "Revision.h"
#include "Pack.h"

namespace VCS
{
    class StashesRepository final : public ReferenceCountedObject, public Serializable
    {
    public:

        explicit StashesRepository(Pack::Ptr pack);

        int getNumUserStashes() const;
        String getUserStashDescription(int index) const;
        Revision::Ptr getUserStash(int index) const;
        Revision::Ptr getUserStashWithName(const String &stashName) const;
        
        Revision::Ptr getQuickStash() const noexcept;
        bool hasQuickStash() const noexcept;
        void storeQuickStash(Revision::Ptr newStash);
        void resetQuickStash();
        
        void addStash(Revision::Ptr newStash);
        void removeStash(Revision::Ptr stashToRemove);

        //===--------------------------------------------------------------===//
        // Serializable
        //===--------------------------------------------------------------===//

        ValueTree serialize() const override;
        void deserialize(const ValueTree &tree) override;
        void reset() override;

        using Ptr = ReferenceCountedObjectPtr<StashesRepository>;

    private:

        Pack::Ptr pack;

        // root node for the stashes
        Revision::Ptr userStashes;

        // root node for quick-toggled changes
        Revision::Ptr quickStash;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StashesRepository);
    };
} // namespace VCS
