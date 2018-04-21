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
class GenericAudioMonitorComponent;
class WaveformAudioMonitorComponent;
class SpectrogramAudioMonitorComponent;
class ModeIndicatorComponent;
class AudioMonitor;

#include "ModeIndicatorComponent.h"
//[/Headers]

#include "../Themes/PanelBackgroundC.h"
#include "../Themes/LighterShadowUpwards.h"
#include "../Themes/SeparatorHorizontalReversed.h"
#include "../Themes/LighterShadowDownwards.h"
#include "../Themes/SeparatorHorizontal.h"

class SequencerSidebarLeft  : public ModeIndicatorOwnerComponent
{
public:

    SequencerSidebarLeft ();

    ~SequencerSidebarLeft();

    //[UserMethods]
    void setAudioMonitor(AudioMonitor *audioMonitor);
    void handleChangeMode() override;
    void paintOverChildren(Graphics& g) override;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;

    // Binary resources:
    static const char* gray1x1_png;
    static const int gray1x1_pngSize;

private:

    //[UserVariables]
    ComponentAnimator animator;
    void switchMonitorsAnimated(Component *oldOne, Component *newOne);

    ScopedPointer<GenericAudioMonitorComponent> genericMonitor;
    ScopedPointer<WaveformAudioMonitorComponent> waveformMonitor;
    ScopedPointer<SpectrogramAudioMonitorComponent> spectrogramMonitor;
    //[/UserVariables]

    ScopedPointer<PanelBackgroundC> background;
    ScopedPointer<LighterShadowUpwards> shadow;
    ScopedPointer<SeparatorHorizontalReversed> headLine;
    ScopedPointer<LighterShadowDownwards> headShadow;
    ScopedPointer<SeparatorHorizontal> separator;
    ScopedPointer<ModeIndicatorTrigger> modeIndicatorSelector;
    ScopedPointer<ModeIndicatorComponent> modeIndicator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerSidebarLeft)
};
