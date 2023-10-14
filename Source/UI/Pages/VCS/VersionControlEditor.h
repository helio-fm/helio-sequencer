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

class VersionControl;

#include "StageComponent.h"
#include "HistoryComponent.h"
#include "PageBackgroundA.h"
#include "PageBackgroundB.h"
#include "SeparatorVerticalSkew.h"

class VersionControlEditor final :
    public Component,
    public ChangeListener, // listens to VCS and project changes
    public Timer // coalesce multiple updates on these ^ changes after a timeout
{
public:

    explicit VersionControlEditor(VersionControl &versionControl);
    ~VersionControlEditor();

    void updateState();
    void onStageSelectionChanged();
    void onHistorySelectionChanged();

    void resized() override;
    void broughtToFront() override;

private:

    VersionControl &vcs;
    void changeListenerCallback(ChangeBroadcaster *source) override;

    void timerCallback() override;

    UniquePointer<PageBackgroundA> backgroundA;
    UniquePointer<SeparatorVerticalSkew> skew;
    UniquePointer<PageBackgroundB> backgroundB;
    UniquePointer<StageComponent> stageComponent;
    UniquePointer<HistoryComponent> historyComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VersionControlEditor)
};
