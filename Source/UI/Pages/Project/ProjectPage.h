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

class ProjectNode;

#include "TransportListener.h"
#include "PanelBackgroundA.h"
#include "PanelBackgroundB.h"
#include "SeparatorVerticalSkew.h"

class ProjectPage final : public Component,
    protected TransportListener,
    protected ChangeListener
{
public:

    explicit ProjectPage(ProjectNode &parentProject);
    ~ProjectPage();

    void updateContent();

    //===----------------------------------------------------------------------===//
    // Component
    //===----------------------------------------------------------------------===//

    void resized() override;
    void visibilityChanged() override;

private:

    ProjectNode &project;
    Atomic<double> totalTimeMs = 0.0;

    void changeListenerCallback(ChangeBroadcaster *source) override;

    //===----------------------------------------------------------------------===//
    // TransportListener
    //===----------------------------------------------------------------------===//

    void onSeek(float beatPosition, double currentTimeMs, double totalTimeMs) override;
    void onTempoChanged(double msPerQuarter) noexcept override {}
    void onTotalTimeChanged(double timeMs) noexcept override;
    void onLoopModeChanged(bool hasLoop, float start, float end) override {}

    void onPlay() noexcept override {}
    void onRecord() noexcept override {}
    void onStop() noexcept override {}

    //===----------------------------------------------------------------------===//
    // Some helpers for layout
    //===----------------------------------------------------------------------===//

    Array<Label *> metadataCaptions;
    Array<Label *> metadataEditors;
    Array<Label *> statisticsCaptions;
    Array<Label *> statisticsLabels;

    //===----------------------------------------------------------------------===//
    // Children
    //===----------------------------------------------------------------------===//

    UniquePointer<PanelBackgroundB> backgroundB;
    UniquePointer<SeparatorVerticalSkew> skew;
    UniquePointer<PanelBackgroundA> backgroundA;
    UniquePointer<Label> projectTitleEditor;
    UniquePointer<Label> projectTitleLabel;
    UniquePointer<Label> authorEditor;
    UniquePointer<Label> authorLabel;
    UniquePointer<Label> descriptionEditor;
    UniquePointer<Label> descriptionLabel;
    UniquePointer<Label> locationLabel;
    UniquePointer<Label> locationText;
    UniquePointer<Label> contentStatsLabel;
    UniquePointer<Label> contentStatsText;
    UniquePointer<Label> vcsStatsLabel;
    UniquePointer<Label> vcsStatsText;
    UniquePointer<Label> startTimeLabel;
    UniquePointer<Label> startTimeText;
    UniquePointer<Label> lengthLabel;
    UniquePointer<Label> lengthText;
    UniquePointer<Label> licenseLabel;
    UniquePointer<Label> licenseEditor;
    UniquePointer<ImageButton> revealLocationButton;
    UniquePointer<Label> temperamentLabel;
    UniquePointer<Label> temperamentText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectPage)
};
