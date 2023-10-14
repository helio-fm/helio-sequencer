/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class Workspace;
class SpectralLogo;
class OverlayButton;
class DashboardMenu;
class PageBackgroundA;
class PageBackgroundB;
class OpenProjectButton;
class CreateProjectButton;
class SeparatorVerticalSkew;

#include "MobileComboBox.h"

class Dashboard final : public Component,
                        public ChangeListener
{
public:

    explicit Dashboard(Workspace &workspace);
    ~Dashboard();

    void resized() override;

private:

    Workspace &workspace;

    void changeListenerCallback(ChangeBroadcaster *source) override;

    UniquePointer<PageBackgroundA> backgroundA;
    UniquePointer<PageBackgroundB> backgroundB;
    UniquePointer<OpenProjectButton> openProjectButton;
    UniquePointer<MobileComboBox::Container> createProjectCombo;
    UniquePointer<SeparatorVerticalSkew> skew;
    UniquePointer<SpectralLogo> logo;
    UniquePointer<DashboardMenu> projectsList;
    UniquePointer<CreateProjectButton> createProjectButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Dashboard)
};
