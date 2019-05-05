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
#include "TransportListener.h"

class PanelBackgroundC;
class MenuItemComponent;
class MenuPanel;
class PopupButton;
class PlayButton;
class ProjectNode;
class PianoRoll;
//[/Headers]

#include "../Themes/PanelBackgroundC.h"
#include "../Common/PlayButton.h"

class ArpTuningPanel final : public Component,
                             public ChangeBroadcaster,
                             private TransportListener,
                             public Slider::Listener
{
public:

    ArpTuningPanel(ProjectNode &parentProject, PianoRoll &targetRoll);
    ~ArpTuningPanel();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void handleCommandMessage (int commandId) override;


private:

    //[UserVariables]

    PianoRoll &roll;
    ProjectNode &project;

    bool hasMadeChanges;
    void undoIfNeeded();

    void onTempoChanged(double) override;
    void onTotalTimeChanged(double) override;
    void onSeek(double, double, double) override;
    void onPlay() override;
    void onStop() override;

    //[/UserVariables]

    UniquePointer<PanelBackgroundC> bg;
    UniquePointer<PopupButton> sliderLinearButton;
    UniquePointer<PopupButton> sliderSineButton;
    UniquePointer<Slider> volumeMultiplier;
    UniquePointer<Slider> randomMultiplier;
    UniquePointer<PopupButton> sliderMultiplyButton;
    UniquePointer<Slider> speedMultiplier;
    UniquePointer<MenuPanel> arpsList;
    UniquePointer<MenuItemComponent> resetButton;
    UniquePointer<PlayButton> playButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArpTuningPanel)
};


