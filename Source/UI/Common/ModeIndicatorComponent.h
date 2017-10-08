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

class ModeIndicatorBar;

#include "ComponentFader.h"

class ModeIndicatorComponent : public Component
{
public:
    
    explicit ModeIndicatorComponent(int numModes);
    ~ModeIndicatorComponent();

    typedef int Mode;
    Mode scrollToNextMode();

    void resized() override;

private:

    Mode activeMode;
    OwnedArray<ModeIndicatorBar> bars;
    ComponentFader fader;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModeIndicatorComponent)
};

class ModeIndicatorOwnerComponent : public Component
{
public:

    void showModeIndicator();
    void hideModeIndicator();
    virtual void handleChangeMode() = 0;

private:

    ComponentFader modeIndicatorFader;

};

class ModeIndicatorTrigger : public Component
{
public:

    ModeIndicatorTrigger();
    void mouseUp(const MouseEvent& event);
    void mouseEnter(const MouseEvent &event);
    void mouseExit(const MouseEvent &event);

};
