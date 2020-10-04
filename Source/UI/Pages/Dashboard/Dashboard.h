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
class IconComponent;
class MainLayout;
class SpectralLogo;
class UserProfileComponent;
class LoginButton;
//[/Headers]

#include "../../Themes/PanelBackgroundA.h"
#include "../../Themes/PanelBackgroundB.h"
#include "Menu/OpenProjectButton.h"
#include "../../Themes/SeparatorVerticalSkew.h"
#include "Menu/CreateProjectButton.h"
#include "../../Themes/SeparatorHorizontalFadingReversed.h"
#include "UpdatesInfoComponent.h"

class Dashboard final : public Component,
                        public ChangeListener
{
public:

    Dashboard(MainLayout &workspaceRef);
    ~Dashboard();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;


private:

    //[UserVariables]

    MainLayout &workspace;

    void changeListenerCallback(ChangeBroadcaster *source) override;
    void updateProfileViews();

    //[/UserVariables]

    UniquePointer<PanelBackgroundA> backgroundA;
    UniquePointer<Label> patreonLabel;
    UniquePointer<UserProfileComponent> userProfile;
    UniquePointer<LoginButton> loginButton;
    UniquePointer<PanelBackgroundB> backgroundB;
    UniquePointer<OpenProjectButton> openProjectButton;
    UniquePointer<MobileComboBox::Primer> createProjectComboSource;
    UniquePointer<SeparatorVerticalSkew> skew;
    UniquePointer<SpectralLogo> logo;
    UniquePointer<DashboardMenu> projectsList;
    UniquePointer<CreateProjectButton> createProjectButton;
    UniquePointer<SeparatorHorizontalFadingReversed> separator2;
    UniquePointer<UpdatesInfoComponent> updatesInfo;
    UniquePointer<OverlayButton> patreonButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dashboard)
};


