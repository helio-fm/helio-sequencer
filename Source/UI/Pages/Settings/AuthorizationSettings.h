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
class ShapeComponent;
//[/Headers]


class AuthorizationSettings  : public Component,
                               public Label::Listener
{
public:

    AuthorizationSettings ();

    ~AuthorizationSettings();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void labelTextChanged (Label* labelThatHasChanged) override;
    void visibilityChanged() override;


private:

    //[UserVariables]
    //[/UserVariables]

    ScopedPointer<Label> emailEditor;
    ScopedPointer<Label> authorLabel;
    ScopedPointer<Label> passwordEditor;
    ScopedPointer<Label> passwordLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AuthorizationSettings)
};
