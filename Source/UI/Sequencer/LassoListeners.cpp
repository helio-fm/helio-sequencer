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
#include "PianoClipComponent.h"
#include "Pattern.h"
#include "PatternRoll.h"

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

    PianoRollMenuSource(WeakReference<Lasso> lasso, ProjectNode &project) :
        lasso(lasso), project(project) {}

    bool hasMenu() const noexcept override { return true; }

    UniquePointer<Component> createMenu() override
    {
        return make<PianoRollSelectionMenu>(this->project, this->lasso);
    }

    Image getIcon() const override
    {
        return Icons::findByName(Icons::selection, Globals::UI::headlineIconSize);
    }

    String getName() const override
    {
        return TRANS(I18n::Menu::Selection::notes);
    }

    bool canBeSelectedAsMenuItem() const override { return false; }
    void onSelectedAsMenuItem() override {}

private:

    ProjectNode &project;
    WeakReference<Lasso> lasso;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRollMenuSource);
};

// Piano roll needs at least 2 events to be selected to show selection menu
// I just don't want it to appear too frequently
PianoRollSelectionMenuManager::PianoRollSelectionMenuManager(WeakReference<Lasso> lasso, ProjectNode &project) :
    SelectionMenuManager(lasso, 2)
{
    this->menu = make<PianoRollMenuSource>(lasso, project);
}

//===----------------------------------------------------------------------===//
// PatternRoll selection menu
//===----------------------------------------------------------------------===//

class PatternRollMenuSource final : public HeadlineItemDataSource
{
public:

    PatternRollMenuSource(WeakReference<Lasso> lasso) : lasso(lasso) {}

    bool hasMenu() const noexcept override { return true; }

    UniquePointer<Component> createMenu() override
    {
        return make<PatternRollSelectionMenu>(this->lasso);
    }

    Image getIcon() const override
    {
        return Icons::findByName(Icons::selection, Globals::UI::headlineIconSize);
    }

    String getName() const override
    {
        return TRANS(I18n::Menu::Selection::notes);
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
    this->menu = make<PatternRollMenuSource>(lasso);
}

//===----------------------------------------------------------------------===//
// PatternRoll recording target logic
//===----------------------------------------------------------------------===//

PatternRollRecordingTargetController::PatternRollRecordingTargetController(
    WeakReference<Lasso> lasso, ProjectNode &project) :
    lasso(lasso),
    project(project)
{
    jassert(this->lasso != nullptr);

    if (this->lasso != nullptr)
    {
        this->lasso->addChangeListener(this);
    }
}

PatternRollRecordingTargetController::~PatternRollRecordingTargetController()
{
    jassert(this->lasso != nullptr);

    if (this->lasso != nullptr)
    {
        this->lasso->removeChangeListener(this);
    }
}

void PatternRollRecordingTargetController::changeListenerCallback(ChangeBroadcaster *source)
{
    //jassert(dynamic_cast<Lasso *>(source));
    const auto *selection = static_cast<Lasso *>(source);

    // bail out as early as possible
    if (selection->getNumSelected() != 1)
    {
        this->project.setMidiRecordingTarget(nullptr, nullptr);
        return;
    }

    // if the single piano clip is selected,
    // set it as the recording target, and mark it with red color
    auto *cc = selection->getFirstAs<ClipComponent>();
    if (auto *pc = dynamic_cast<PianoClipComponent *>(cc))
    {
        if (this->project.getTransport().isRecording())
        {
            pc->setShowRecordingMode(true);
        }

        auto *track = cc->getClip().getPattern()->getTrack();
        this->project.setMidiRecordingTarget(track, &cc->getClip());
    }
}
