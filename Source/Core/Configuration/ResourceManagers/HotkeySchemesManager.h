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

class HotkeySchemesManager final : public ResourceManager
{
public:

    HotkeySchemesManager();

    BaseResource::Ptr createResource() const override
    {
        return { new HotkeyScheme() };
    }

    inline const Array<HotkeyScheme::Ptr> getAll() const noexcept
    {
        return this->getAllResources<HotkeyScheme>();
    }

    const HotkeyScheme::Ptr getCurrent() const noexcept;
    void setCurrent(const HotkeyScheme::Ptr scheme);

private:

    void deserializeResources(const ValueTree &tree, Resources &outResources) override;
    void reset() override;

    HotkeyScheme::Ptr activeScheme;
    HotkeyScheme::Ptr findActiveScheme() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HotkeySchemesManager)

};
