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
#include "NoteComponent.h"
#include "Pattern.h"
#include "PatternRoll.h"
#include "PianoRoll.h"
#include "RollHeader.h"

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
    if (this->lasso->getNumSelected() >= this->minNumberOfSelectedEvents)
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRollMenuSource)
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

    PatternRollMenuSource(WeakReference<Lasso> lasso) :
        lasso(lasso) {}

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternRollMenuSource)
};

PatternRollSelectionMenuManager::PatternRollSelectionMenuManager(WeakReference<Lasso> lasso) :
    SelectionMenuManager(lasso, 1)
{
    this->menu = make<PatternRollMenuSource>(lasso);
}

//===----------------------------------------------------------------------===//
// Roll selection range dashed indicators
//===----------------------------------------------------------------------===//

// unlike the clip range indicator, this one is much simpler to manage separately
PianoRollSelectionRangeIndicatorController::PianoRollSelectionRangeIndicatorController(
    WeakReference<Lasso> lasso, PianoRoll &roll) :
    lasso(lasso),
    roll(roll)
{
    jassert(this->lasso != nullptr);

    if (this->lasso != nullptr)
    {
        this->lasso->addChangeListener(this);
    }
}

PianoRollSelectionRangeIndicatorController::~PianoRollSelectionRangeIndicatorController()
{
    jassert(this->lasso != nullptr);

    if (this->lasso != nullptr)
    {
        this->lasso->removeChangeListener(this);
    }
}

void PianoRollSelectionRangeIndicatorController::changeListenerCallback(ChangeBroadcaster *source)
{
    const auto numSelected = this->lasso->getNumSelected();
    const auto hasSelection = numSelected > 0;

    jassert(!hasSelection || this->roll.getActiveClip().isValid());

    auto lastBeat = hasSelection ? std::numeric_limits<float>::lowest() : 0.f;
    auto firstBeat = hasSelection ? std::numeric_limits<float>::max() : 0.f;
    const auto clipBeat = hasSelection ? this->roll.getActiveClip().getBeat() : 0.f;
    for (int i = 0; i < numSelected; ++i)
    {
        auto *nc = this->lasso->getItemAs<NoteComponent>(i);
        firstBeat = jmin(firstBeat, nc->getBeat() + clipBeat);
        lastBeat = jmax(lastBeat, nc->getBeat() + nc->getLength() + clipBeat);
    }

    const auto colour = hasSelection ?
        this->roll.getActiveClip().getTrackColour() : Colours::transparentBlack;

    this->roll.getHeaderComponent()->updateSelectionRangeIndicator(colour, firstBeat, lastBeat);
}

PatternRollSelectionRangeIndicatorController::PatternRollSelectionRangeIndicatorController(
    WeakReference<Lasso> lasso, PatternRoll &roll) :
    lasso(lasso),
    roll(roll)
{
    jassert(this->lasso != nullptr);

    if (this->lasso != nullptr)
    {
        this->lasso->addChangeListener(this);
    }
}

PatternRollSelectionRangeIndicatorController::~PatternRollSelectionRangeIndicatorController()
{
    jassert(this->lasso != nullptr);

    if (this->lasso != nullptr)
    {
        this->lasso->removeChangeListener(this);
    }
}

void PatternRollSelectionRangeIndicatorController::changeListenerCallback(ChangeBroadcaster *source)
{
    const auto numSelected = this->lasso->getNumSelected();

    auto lastBeat = numSelected > 0 ? std::numeric_limits<float>::lowest() : 0.f;
    auto firstBeat = numSelected > 0 ? std::numeric_limits<float>::max() : 0.f;

    Colour colour;
    for (int i = 0; i < numSelected; ++i)
    {
        const auto *cc = this->lasso->getItemAs<ClipComponent>(i);
        const auto *sequence = cc->getClip().getPattern()->getTrack()->getSequence();
        firstBeat = jmin(firstBeat, cc->getBeat() + sequence->getFirstBeat());
        lastBeat = jmax(lastBeat, cc->getBeat() + sequence->getLengthInBeats() + sequence->getFirstBeat());
        colour = colour.interpolatedWith(cc->getClip().getTrackColour(), 1.f / float(i + 1));
    }

    this->roll.getHeaderComponent()->updateSelectionRangeIndicator(colour, firstBeat, lastBeat);
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
    // bail out as early as possible
    if (this->lasso->getNumSelected() != 1)
    {
        this->project.setMidiRecordingTarget(nullptr);
        return;
    }

    // if the single piano clip is selected,
    // set it as the recording target, and mark it with red color
    auto *cc = this->lasso->getFirstAs<ClipComponent>();
    if (auto *pc = dynamic_cast<PianoClipComponent *>(cc))
    {
        if (this->project.getTransport().isRecording())
        {
            pc->setShowRecordingMode(true);
        }

        this->project.setMidiRecordingTarget(&cc->getClip());
    }
}

//===----------------------------------------------------------------------===//
// Time signature picker
//===----------------------------------------------------------------------===//

PatternRollTimeSignaturePicker::PatternRollTimeSignaturePicker(WeakReference<Lasso> lasso,
    ProjectNode &project) :
    lasso(lasso),
    project(project)
{
    jassert(this->lasso != nullptr);

    if (this->lasso != nullptr)
    {
        this->lasso->addChangeListener(this);
    }
}

PatternRollTimeSignaturePicker::~PatternRollTimeSignaturePicker()
{
    jassert(this->lasso != nullptr);

    if (this->lasso != nullptr)
    {
        this->lasso->removeChangeListener(this);
    }
}

void PatternRollTimeSignaturePicker::changeListenerCallback(ChangeBroadcaster *source)
{
    // if all selected piano clips which have time signature
    // are of the same track, use this time signature override

    WeakReference<MidiTrack> targetTrack = nullptr;

    for (int i = 0; i < this->lasso->getNumSelected(); ++i)
    {
        auto *cc = this->lasso->getItemAs<ClipComponent>(i);
        auto *clipTrack = cc->getClip().getPattern()->getTrack();

        if (!clipTrack->hasTimeSignatureOverride() ||
            dynamic_cast<PianoClipComponent *>(cc) == nullptr)
        {
            continue;
        }

        if (targetTrack == nullptr)
        {
            targetTrack = clipTrack;
        }
        else if (targetTrack != clipTrack)
        {
            this->project.getTimeline()->getTimeSignaturesAggregator()->setActiveScope(nullptr);
            return;
        }
    }

    this->project.getTimeline()->getTimeSignaturesAggregator()->setActiveScope(targetTrack);
}
