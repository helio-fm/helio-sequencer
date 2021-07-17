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

class Lasso;
class PianoRollMenuSource;
class PatternRollMenuSource;
class ProjectNode;
class PianoRoll;
class PatternRoll;

#include "Icons.h"
#include "HeadlineItemDataSource.h"

// Menus

class SelectionMenuManager : public ChangeListener
{
public:

    SelectionMenuManager(WeakReference<Lasso> lasso, int minSelection);
    ~SelectionMenuManager() override;

protected:

    void changeListenerCallback(ChangeBroadcaster *source) override;

    UniquePointer<HeadlineItemDataSource> menu;

private:

    WeakReference<Lasso> lasso;
    int minNumberOfSelectedEvents;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SelectionMenuManager);
};

class PianoRollSelectionMenuManager final : public SelectionMenuManager
{
public:
    PianoRollSelectionMenuManager(WeakReference<Lasso> lasso, ProjectNode &project);
};

class PatternRollSelectionMenuManager final : public SelectionMenuManager
{
public:
    explicit PatternRollSelectionMenuManager(WeakReference<Lasso> lasso);
};

// Else helpers

class PianoRollSelectionRangeIndicatorController final : public ChangeListener
{
public:

    PianoRollSelectionRangeIndicatorController(WeakReference<Lasso> lasso, PianoRoll &roll);
    ~PianoRollSelectionRangeIndicatorController() override;

private:

    void changeListenerCallback(ChangeBroadcaster *source) override;

    WeakReference<Lasso> lasso;
    PianoRoll &roll;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRollSelectionRangeIndicatorController);
};

class PatternRollSelectionRangeIndicatorController final : public ChangeListener
{
public:

    PatternRollSelectionRangeIndicatorController(WeakReference<Lasso> lasso, PatternRoll &roll);
    ~PatternRollSelectionRangeIndicatorController() override;

private:

    void changeListenerCallback(ChangeBroadcaster *source) override;

    WeakReference<Lasso> lasso;
    PatternRoll &roll;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternRollSelectionRangeIndicatorController);
};

class PatternRollRecordingTargetController final : public ChangeListener
{
public:

    PatternRollRecordingTargetController(WeakReference<Lasso> lasso, ProjectNode &project);
    ~PatternRollRecordingTargetController() override;

private:

    void changeListenerCallback(ChangeBroadcaster *source) override;

    WeakReference<Lasso> lasso;
    ProjectNode &project;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternRollRecordingTargetController);
};
