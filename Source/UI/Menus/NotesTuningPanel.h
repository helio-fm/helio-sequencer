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

class PianoRoll;
class ProjectNode;
class NotesTuningDiagram;
class MenuItemComponent;
//[/Headers]

#include "../Themes/PanelBackgroundC.h"
#include "../Popups/PopupButton.h"
#include "../Popups/PopupButton.h"
#include "../Common/PlayButton.h"
#include "../Popups/PopupButton.h"

class NotesTuningPanel final : public Component,
                               public ChangeBroadcaster,
                               private TransportListener,
                               public Slider::Listener
{
public:

    NotesTuningPanel(ProjectNode &parentProject, PianoRoll &targetRoll);
    ~NotesTuningPanel();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void handleCommandMessage (int commandId) override;
    bool keyPressed (const KeyPress& key) override;


private:

    //[UserVariables]

    PianoRoll &roll;
    ProjectNode &project;

    bool hasMadeChanges;

    float volumeAnchorSine;
    float volumeAnchorMulti;
    float volumeAnchorLinear;

    void sliderDragStarted(Slider *slider) override;
    void sliderDragEnded(Slider *slider) override;

    void syncVolumeLinear(Slider *volumeSlider);
    void syncVolumeMultiplied(Slider *volumeSlider);
    void syncVolumeSine(Slider *volumeSlider);

    void syncSliders();
    float calcSliderValue();
    float flattenValue(float value) const;
    void startTuning();
    void endTuning();


    //===----------------------------------------------------------------------===//
    // TransportListener
    //

    void onSeek(double absolutePosition, double currentTimeMs, double totalTimeMs) override;
    void onTempoChanged(double msPerQuarter) override;
    void onTotalTimeChanged(double timeMs) override;
    void onPlay() override;
    void onStop() override;

    //[/UserVariables]

    UniquePointer<PanelBackgroundC> bg;
    UniquePointer<PopupButton> sliderLinearButton;
    UniquePointer<PopupButton> sliderMultiplyButton;
    UniquePointer<Slider> volumeSliderMulti;
    UniquePointer<Slider> volumeSliderLinear;
    UniquePointer<NotesTuningDiagram> tuningDiagram;
    UniquePointer<MenuItemComponent> resetButton;
    UniquePointer<PlayButton> playButton;
    UniquePointer<PopupButton> sliderSineButton;
    UniquePointer<Slider> sineSlider;
    UniquePointer<Label> linearLabel;
    UniquePointer<Label> multiLabel;
    UniquePointer<Label> sineLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NotesTuningPanel)
};
