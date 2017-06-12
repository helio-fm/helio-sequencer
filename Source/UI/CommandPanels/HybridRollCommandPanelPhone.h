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
#include "HybridRollCommandPanel.h"
//[/Headers]

#include "../Themes/PanelBackgroundC.h"
#include "../Themes/PanelBackgroundC.h"
#include "../Themes/SeparatorHorizontalReversed.h"
#include "../Themes/LighterShadowUpwards.h"
#include "../Themes/GradientVertical.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Common/PlayButton.h"
#include "../Themes/LighterShadowDownwards.h"
#include "../Themes/GradientVerticalReversed.h"

class HybridRollCommandPanelPhone  : public HybridRollCommandPanel
{
public:

    HybridRollCommandPanelPhone (ProjectTreeItem &parent);

    ~HybridRollCommandPanelPhone();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void childrenChanged() override;
    void mouseMove (const MouseEvent& e) override;

    // Binary resources:
    static const char* gray1x1_png;
    static const int gray1x1_pngSize;

private:

    //[UserVariables]

    //===----------------------------------------------------------------------===//
    // HybridRollCommandPanel
    //

    void updateModeButtons() override;
    void emitAnnotationsCallout(Component *newAnnotationsMenu) override;

    //===----------------------------------------------------------------------===//
    // ListBoxModel
    //

    Component *refreshComponentForRow(int rowNumber, bool isRowSelected,
                                      Component *existingComponentToUpdate) override;

    //===----------------------------------------------------------------------===//
    // AsyncUpdater
    //

    void handleAsyncUpdate() override;

    //===----------------------------------------------------------------------===//
    // TransportListener
    //

    void onTotalTimeChanged(const double timeMs) override;
    void onPlay() override;
    void onStop() override;

    //[/UserVariables]

    ScopedPointer<PanelBackgroundC> headBg;
    ScopedPointer<PanelBackgroundC> bodyBg;
    ScopedPointer<ListBox> listBox;
    ScopedPointer<SeparatorHorizontalReversed> headLine;
    ScopedPointer<LighterShadowUpwards> shadow;
    ScopedPointer<GradientVertical> gradient2;
    ScopedPointer<SeparatorHorizontal> separator;
    ScopedPointer<Label> totalTime;
    ScopedPointer<Label> currentTime;
    ScopedPointer<PlayButton> playButton;
    ScopedPointer<LighterShadowDownwards> headShadow;
    ScopedPointer<GradientVerticalReversed> gradient;
    ScopedPointer<CommandItemComponent> annotationsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HybridRollCommandPanelPhone)
};
