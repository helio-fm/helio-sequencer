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

#include "Scale.h"
#include "ResourceManager.h"

class ScalesManager : public ResourceManager
{
public:

    static ScalesManager &getInstance()
    {
        static ScalesManager Instance;
        return Instance;
    }

    inline const Array<Scale::Ptr> getScales() const
    {
        return this->getResources<Scale::Ptr>();
    }

private:

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;

private:

    ScalesManager();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScalesManager)

};
