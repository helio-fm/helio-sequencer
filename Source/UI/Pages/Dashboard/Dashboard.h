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
class RecentFilesComponent;
class RootNode;
class MainLayout;
class ComponentsList;
class LogoFader;
class AudioSettings;
class ThemeSettings;
class OpenGLSettings;
class Log;
//[/Headers]

#include "../../Themes/SeparatorVerticalSkew.h"
#include "../../Themes/PanelBackgroundB.h"
#include "../../Themes/PanelBackgroundA.h"
#include "Menu/DashboardMenu.h"
#include "Menu/OpenProjectButton.h"
#include "Menu/CreateProjectButton.h"
#include "../../Themes/SeparatorHorizontalFadingReversed.h"
#include "Menu/LoginButton.h"
#include "UserProfileComponent.h"
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
    void visibilityChanged() override;


private:

    //[UserVariables]

    MainLayout &workspace;

    void changeListenerCallback(ChangeBroadcaster *source) override;
    void updateProfileViews();

    //[/UserVariables]

    UniquePointer<SeparatorVerticalSkew> skew;
    UniquePointer<PanelBackgroundB> backgroundB;
    UniquePointer<PanelBackgroundA> backgroundA;
    UniquePointer<LogoFader> logoImage;
    UniquePointer<DashboardMenu> projectsList;
    UniquePointer<OpenProjectButton> openProjectButton;
    UniquePointer<CreateProjectButton> createProjectButton;
    UniquePointer<SeparatorHorizontalFadingReversed> separator2;
    UniquePointer<LoginButton> loginButton;
    UniquePointer<UserProfileComponent> userProfile;
    UniquePointer<UpdatesInfoComponent> updatesInfo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dashboard)
};
