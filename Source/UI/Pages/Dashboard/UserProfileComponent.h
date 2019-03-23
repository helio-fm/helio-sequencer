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

//[Headers]
#include "IconComponent.h"
#include "OverlayButton.h"
//[/Headers]

#include "../../Themes/SeparatorVertical.h"

class UserProfileComponent final : public Component
{
public:

    UserProfileComponent();
    ~UserProfileComponent();

    //[UserMethods]
    void updateProfileInfo();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]
    //[/UserVariables]

    UniquePointer<Label> nameLabel;
    UniquePointer<IconComponent> avatar;
    UniquePointer<SeparatorVertical> separator;
    UniquePointer<OverlayButton> clickHandler;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UserProfileComponent)
};
