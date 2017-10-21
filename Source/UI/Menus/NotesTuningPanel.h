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
class ProjectTreeItem;
class NotesTuningDiagram;
class CommandItemComponent;
//[/Headers]

#include "../Themes/PanelBackgroundC.h"
#include "../Popups/PopupButton.h"
#include "../Popups/PopupButton.h"
#include "../Themes/PanelA.h"
#include "../Common/PlayButton.h"
#include "../Themes/ShadowDownwards.h"
#include "../Popups/PopupButton.h"

class NotesTuningPanel  : public Component,
                          public ChangeBroadcaster,
                          private TransportListener,
                          public Slider::Listener
{
public:

    NotesTuningPanel (ProjectTreeItem &parentProject, PianoRoll &targetRoll);

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
    ProjectTreeItem &project;

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

    void onSeek(const double newPosition, const double currentTimeMs, const double totalTimeMs) override;
    void onTempoChanged(const double newTempo) override;
    void onTotalTimeChanged(const double timeMs) override;
    void onPlay() override;
    void onStop() override;

    //[/UserVariables]

    ScopedPointer<PanelBackgroundC> bg;
    ScopedPointer<PopupButton> sliderLinearButton;
    ScopedPointer<PopupButton> sliderMultiplyButton;
    ScopedPointer<Slider> volumeSliderMulti;
    ScopedPointer<Slider> volumeSliderLinear;
    ScopedPointer<NotesTuningDiagram> tuningDiagram;
    ScopedPointer<PanelA> panel;
    ScopedPointer<CommandItemComponent> resetButton;
    ScopedPointer<PlayButton> playButton;
    ScopedPointer<ShadowDownwards> shadowDown;
    ScopedPointer<PopupButton> sliderSineButton;
    ScopedPointer<Slider> sineSlider;
    ScopedPointer<Label> linearLabel;
    ScopedPointer<Label> multiLabel;
    ScopedPointer<Label> sineLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NotesTuningPanel)
};
