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
//[/Headers]

%%include_files_h%%
%%class_declaration%%
{
public:

    %%class_name%%(%%constructor_params%%);
    ~%%class_name%%();

    //[UserMethods]
    //[/UserMethods]

    %%public_member_declarations%%
private:

    //[UserVariables]
    //[/UserVariables]

    %%private_member_declarations%%
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (%%class_name%%)
};

