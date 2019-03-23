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

#include "Common.h"
#include "LassoListeners.h"
#include "MainLayout.h"
#include "Icons.h"
#include "Lasso.h"
#include "PianoRollSelectionMenu.h"
#include "PatternRollSelectionMenu.h"
#include "ProjectNode.h"
#include "ProjectTimeline.h"

//===----------------------------------------------------------------------===//
// Base class
//===----------------------------------------------------------------------===//

SelectionMenuManager::SelectionMenuManager(WeakReference<Lasso> lasso, int minSelection) :
    lasso(lasso),
    minNumberOfSelectedEvents(minSelection)
{
    jassert(this->lasso != nullptr);

    if (this->lasso != nullptr)
    {
        this->lasso->addChangeListener(this);
    }
}

SelectionMenuManager::~SelectionMenuManager()
{
    jassert(this->lasso != nullptr);

    if (this->lasso != nullptr)
    {
        this->lasso->removeChangeListener(this);
    }
}

void SelectionMenuManager::changeListenerCallback(ChangeBroadcaster *source)
{
    Lasso *lasso = static_cast<Lasso *>(source);

    if (lasso->getNumSelected() >= this->minNumberOfSelectedEvents)
    {
        App::Layout().showSelectionMenu(this->menu.get());
    }
    else
    {
        App::Layout().hideSelectionMenu();
    }
}

//===----------------------------------------------------------------------===//
// PianoRoll selection menu
//===----------------------------------------------------------------------===//

class PianoRollMenuSource final : public HeadlineItemDataSource
{
public:

    PianoRollMenuSource(WeakReference<Lasso> lasso, const ProjectNode &project) :
        lasso(lasso), project(project) {}

    bool hasMenu() const noexcept override { return true; }

    ScopedPointer<Component> createMenu() override
    {
        return { new PianoRollSelectionMenu(this->lasso,
            this->project.getTimeline()->getKeySignatures()) };
    }

    Image getIcon() const override
    {
        return Icons::findByName(Icons::selection, HEADLINE_ICON_SIZE);
    }

    String getName() const override
    {
        return TRANS("menu::selection::notes");
    }

    bool canBeSelectedAsMenuItem() const override { return false; }
    void onSelectedAsMenuItem() override {}

private:

    const ProjectNode &project;
    WeakReference<Lasso> lasso;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRollMenuSource);
};

// Piano roll needs at least 2 events to be selected to show selection menu
// I just don't want it to appear too frequently
PianoRollSelectionMenuManager::PianoRollSelectionMenuManager(WeakReference<Lasso> lasso, const ProjectNode &project) :
    SelectionMenuManager(lasso, 2)
{
    this->menu = new PianoRollMenuSource(lasso, project);
}

//===----------------------------------------------------------------------===//
// PatternRoll selection menu
//===----------------------------------------------------------------------===//

class PatternRollMenuSource final : public HeadlineItemDataSource
{
public:

    PatternRollMenuSource(WeakReference<Lasso> lasso) : lasso(lasso) {}

    bool hasMenu() const noexcept override { return true; }

    ScopedPointer<Component> createMenu() override
    {
        return { new PatternRollSelectionMenu(this->lasso) };
    }

    Image getIcon() const override
    {
        return Icons::findByName(Icons::selection, HEADLINE_ICON_SIZE);
    }

    String getName() const override
    {
        return TRANS("menu::selection::notes");
    }

    bool canBeSelectedAsMenuItem() const override { return false; }
    void onSelectedAsMenuItem() override {}

private:

    WeakReference<Lasso> lasso;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternRollMenuSource);
};

PatternRollSelectionMenuManager::PatternRollSelectionMenuManager(WeakReference<Lasso> lasso) :
    SelectionMenuManager(lasso, 1)
{
    this->menu = new PatternRollMenuSource(lasso);
}
