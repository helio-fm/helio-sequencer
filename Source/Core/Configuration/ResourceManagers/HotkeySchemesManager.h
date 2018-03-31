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

#include "HotkeyScheme.h"
#include "ResourceManager.h"

class HotkeySchemesManager : public ResourceManager
{
public:

    static HotkeySchemesManager &getInstance()
    {
        static HotkeySchemesManager Instance;
        return Instance;
    }

    inline const Array<HotkeyScheme::Ptr> getSchemes() const noexcept
    {
        return this->getResources<HotkeyScheme::Ptr>();
    }

    const HotkeyScheme::Ptr getCurrentScheme() const noexcept;
    void setCurrentScheme(const HotkeyScheme::Ptr scheme);

private:

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    HotkeyScheme::Ptr activeScheme;
    HotkeyScheme::Ptr findActiveScheme() const;
    
    HotkeySchemesManager();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HotkeySchemesManager)

};
