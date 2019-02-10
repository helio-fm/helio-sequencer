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

#include "Icons.h"
#include "HeadlineItemDataSource.h"

class SelectionMenuManager : public ChangeListener
{
public:

    SelectionMenuManager(WeakReference<Lasso> lasso, int minSelection);
    ~SelectionMenuManager() override;

    void changeListenerCallback(ChangeBroadcaster *source) override;

protected:

    ScopedPointer<HeadlineItemDataSource> menu;

private:

    WeakReference<Lasso> lasso;
    int minNumberOfSelectedEvents;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SelectionMenuManager);
};

class PianoRollSelectionMenuManager final : public SelectionMenuManager
{
public:
    PianoRollSelectionMenuManager(WeakReference<Lasso> lasso, const ProjectNode &project);
};

class PatternRollSelectionMenuManager final : public SelectionMenuManager
{
public:
    PatternRollSelectionMenuManager(WeakReference<Lasso> lasso);
};
