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
class HelioTheme;

//#if HELIO_MOBILE
#   define PANEL_C_HAS_PRERENDERED_BACKGROUND 1
//#endif
//[/Headers]


class PanelBackgroundC  : public Component
{
public:

    PanelBackgroundC ();

    ~PanelBackgroundC();

    //[UserMethods]

    enum ColourIds
    {
        panelFillStartId       = 0x99103000,
        panelFillEndId         = 0x99103010,

        panelShadeStartId      = 0x99103020,
        panelShadeEndId        = 0x99103030,
    };

    static void updateRender(HelioTheme &theme);

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]
    //[/UserVariables]


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanelBackgroundC)
};
