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

#include "Common.h"
#include "EditorPanelsScroller.h"
#include "EditorPanelsSwitcher.h"
#include "RollBase.h"
#include "ProjectNode.h"
#include "PianoClipComponent.h"
#include "HeadlineItemArrow.h"
#include "TrackStartIndicator.h"
#include "TrackEndIndicator.h"
#include "ColourIDs.h"
#include "HelioTheme.h"

EditorPanelsScroller::EditorPanelsScroller(ProjectNode &project,
    SafePointer<RollBase> roll, SafePointer<EditorPanelsSwitcher> panelsSwitcher) :
    project(project),
    roll(roll),
    editorPanelsSwitcher(panelsSwitcher)
{
    this->setOpaque(true);
    this->setPaintingIsUnclipped(false);
    this->setInterceptsMouseClicks(false, true);

    this->editorPanelsSwitcher->onClick = [this](int panelId,
        const EditorPanelBase::EventFilter &filter)
    {
        if (this->selectedEditorPanelIndex == panelId &&
            this->selectedEventFilter == filter)
        {
            return;
        }

        this->selectedEditorPanelIndex = panelId;
        this->selectedEventFilter = filter;

        auto *editor = this->showEditorPanel(this->selectedEditorPanelIndex);
        if (this->activeClip.isValid())
        {
            editor->setEditableClip(this->activeClip, this->selectedEventFilter);
        }

        this->editorPanelsSwitcher->updateSelection(panelId, filter);
    };

    this->editorPanelsSwitcher->onWheelMove = [this](const MouseEvent &e,
        const MouseWheelDetails &wheel)
    {
        if (this->roll != nullptr)
        {
            this->roll->mouseWheelMove(e.getEventRelativeTo(this->roll), wheel);
        }
    };

    this->projectStartIndicator = make<TrackStartIndicator>();
    this->addAndMakeVisible(this->projectStartIndicator.get());

    this->projectEndIndicator = make<TrackEndIndicator>();
    this->addAndMakeVisible(this->projectEndIndicator.get());

    this->project.addListener(this);
    this->roll->getLassoSelection().addChangeListener(this);
}

EditorPanelsScroller::~EditorPanelsScroller()
{
    for (auto *editor : this->editorPanels)
    {
        editor->removeListener(this);
    }

    this->roll->getLassoSelection().removeChangeListener(this);
    this->project.removeListener(this);
}

void EditorPanelsScroller::switchToRoll(SafePointer<RollBase> roll)
{
    this->panelsBoundsAnimationAnchor = this->getEditorPanelBounds().toFloat();

    this->roll->getLassoSelection().removeChangeListener(this);
    this->roll = roll;
    this->roll->getLassoSelection().addChangeListener(this);

    for (auto *editor : this->editorPanels)
    {
        editor->switchToRoll(roll);
    }

    // in piano roll, allow manually switching between velocity and various automation tracks
    this->editorPanelSelectionMode = dynamic_cast<PianoRoll *>(roll.getComponent()) != nullptr ?
        EditorPanelSelectionMode::Manual : EditorPanelSelectionMode::Automatic;

    auto *editor = this->showEditorPanel(this->selectedEditorPanelIndex);
    if (this->activeClip.isValid())
    {
        editor->setEditableClip(this->activeClip, this->selectedEventFilter);
    }

    if (this->animationsEnabled)
    {
        this->startTimerHz(60);
    }
    else
    {
        this->updateAllChildrenBounds();
    }
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void EditorPanelsScroller::resized()
{
    this->updateAllChildrenBounds();
}

void EditorPanelsScroller::paint(Graphics &g)
{
    const auto &theme = HelioTheme::getCurrentTheme();
    g.setFillType({ theme.getSidebarBackground(), {} });
    g.fillRect(this->getLocalBounds());

    g.setColour(this->borderColourDark);
    g.fillRect(0, 0, this->getWidth(), 1);

    g.setColour(this->borderColourLight);
    g.fillRect(0, 1, this->getWidth(), 1);
}

//===----------------------------------------------------------------------===//
// RollListener
//===----------------------------------------------------------------------===//

void EditorPanelsScroller::onMidiRollMoved(RollBase *targetRoll)
{
    if (this->isVisible() && this->roll == targetRoll && !this->isTimerRunning())
    {
        this->triggerAsyncUpdate();
    }
}

void EditorPanelsScroller::onMidiRollResized(RollBase *targetRoll)
{
    if (this->isVisible() && this->roll == targetRoll && !this->isTimerRunning())
    {
        this->triggerAsyncUpdate();
    }
}

//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void EditorPanelsScroller::handleAsyncUpdate()
{
    this->updateAllChildrenBounds();
}

void EditorPanelsScroller::updateAllChildrenBounds()
{
    const auto panelBounds = this->getEditorPanelBounds();

    for (auto *editor : this->editorPanels)
    {
        editor->setBounds(panelBounds);
    }

    this->projectStartIndicator->updateBounds(panelBounds);
    this->projectEndIndicator->updateBounds(panelBounds);
}

//===----------------------------------------------------------------------===//
// Timer animating transitions between rolls
//===----------------------------------------------------------------------===//

static Rectangle<float> lerpEditorPanelBounds(const Rectangle<float> &r1,
    const Rectangle<float> &r2, float factor)
{
    const float x1 = r1.getX();
    const float x2 = r1.getBottomRight().getX();

    jassert(r1.getY() == r2.getY());
    jassert(r1.getBottomRight().getY() == r2.getBottomRight().getY());
    const float y1 = r1.getY();
    const float y2 = r1.getBottomRight().getY();

    const float dx1 = r2.getX() - x1;
    const float dx2 = r2.getBottomRight().getX() - x2;

    const float lx1 = x1 + dx1 * factor;
    const float lx2 = x2 + dx2 * factor;

    return { lx1, y1, lx2 - lx1, y2 };
}

static float getEditorPanelBoundsDistance(const Rectangle<float> &r1,
    const Rectangle<float> &r2)
{
    return fabs(r1.getX() - r2.getX()) + fabs(r1.getWidth() - r2.getWidth());
}

void EditorPanelsScroller::timerCallback()
{
    const auto newBounds = this->getEditorPanelBounds().toFloat();
    const auto interpolatedBounds = lerpEditorPanelBounds(this->panelsBoundsAnimationAnchor, newBounds, 0.6f);
    const auto distance = getEditorPanelBoundsDistance(this->panelsBoundsAnimationAnchor, newBounds);
    const bool shouldStop = distance < 1.f;
    this->panelsBoundsAnimationAnchor = shouldStop ? newBounds : interpolatedBounds;

    if (shouldStop)
    {
        this->stopTimer();
    }

    const auto finalBounds = this->panelsBoundsAnimationAnchor.toNearestInt();
    for (auto *editor : this->editorPanels)
    {
        editor->setBounds(finalBounds);
    }

    this->projectStartIndicator->updateBounds(finalBounds);
    this->projectEndIndicator->updateBounds(finalBounds);
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

Rectangle<int> EditorPanelsScroller::getEditorPanelBounds() const noexcept
{
    if (this->roll == nullptr)
    {
        return {};
    }

    const auto viewX = this->roll->getViewport().getViewPositionX();
    return { -viewX, 0, this->roll->getWidth(), this->getHeight() };
}

//===----------------------------------------------------------------------===//
// EditorPanelBase::Listener
//===----------------------------------------------------------------------===//

void EditorPanelsScroller::onUpdateEventFilters()
{
    this->updateSwitcher();
}

//===----------------------------------------------------------------------===//
// ProjectListener
//===----------------------------------------------------------------------===//

void EditorPanelsScroller::onChangeProjectBeatRange(float firstBeat, float lastBeat)
{
    this->projectStartIndicator->updatePosition(firstBeat);
    this->projectEndIndicator->updatePosition(lastBeat);

    if (this->isVisible())
    {
        this->triggerAsyncUpdate();
    }
}

void EditorPanelsScroller::onChangeViewBeatRange(float firstBeat, float lastBeat)
{
    this->projectStartIndicator->updateViewRange(firstBeat, lastBeat);
    this->projectEndIndicator->updateViewRange(firstBeat, lastBeat);

    if (this->isVisible())
    {
        this->triggerAsyncUpdate();
    }
}

// editable scope selection:

void EditorPanelsScroller::onChangeClip(const Clip &clip, const Clip &newClip)
{
    if (this->activeClip == clip) // same id
    {
        this->activeClip = newClip; // new parameters
    }
}

// Only called when the piano roll is switched to another clip;
void EditorPanelsScroller::onChangeViewEditableScope(MidiTrack *const, const Clip &clip, bool)
{
    this->activeClip = clip;

    for (auto *editor : this->editorPanels)
    {
        if (editor->canEditSequence(clip.getPattern()->getTrack()->getSequence()))
        {
            editor->setEditableClip(clip);
        }
    }

    if (this->editorPanelSelectionMode == EditorPanelSelectionMode::Automatic)
    {
        jassertfalse; // piano roll is intended to use manual switching mode
        auto *editor = this->showEditorPanelForClip(clip);
        editor->setEditableClip(clip);
    }
    else
    {
        auto *editor = this->showEditorPanel(this->selectedEditorPanelIndex);
        editor->setEditableClip(clip, this->selectedEventFilter);
    }
}

// Can be called by both the piano roll and the pattern roll
void EditorPanelsScroller::changeListenerCallback(ChangeBroadcaster *source)
{
    jassert(dynamic_cast<Lasso *>(source));
    auto *selection = static_cast<Lasso *>(source);

    if (dynamic_cast<PianoRoll *>(this->roll.getComponent()) != nullptr)
    {
        for (auto *editor : this->editorPanels)
        {
            editor->setEditableSelection(selection);
        }
    }
    else if (dynamic_cast<PatternRoll *>(this->roll.getComponent()) != nullptr)
    {
        if (selection->getNumSelected() == 1)
        {
            auto *cc = selection->getFirstAs<ClipComponent>();

            if (this->editorPanelSelectionMode == EditorPanelSelectionMode::Automatic)
            {
                auto *editor = this->showEditorPanelForClip(cc->getClip());
                editor->setEditableClip(cc->getClip());
            }
            else
            {
                jassertfalse; // pattern roll is intended to use automatic switching mode
                auto *editor = this->showEditorPanel(this->selectedEditorPanelIndex);
                editor->setEditableClip(cc->getClip(), this->selectedEventFilter);
            }

            // reset event selection, if any
            for (auto *editor : this->editorPanels)
            {
                editor->setEditableSelection({});
            }
        }
        else
        {
            // simply disallow editing multiple clips at once,
            // because some of them may be instances of the same track
            for (auto *editor : this->editorPanels)
            {
                editor->setEditableClip(Optional<Clip>());
            }
        }
    }
}

EditorPanelBase *EditorPanelsScroller::showEditorPanelForClip(const Clip &clip)
{
    EditorPanelBase *result = nullptr;

    for (auto *editor : this->editorPanels)
    {
        const auto isEditable = editor->canEditSequence(clip.getPattern()->getTrack()->getSequence());
        editor->setVisible(isEditable);
        editor->setEnabled(isEditable);
        if (isEditable)
        {
            result = editor;
        }
    }

    jassert(result != nullptr);
    return result;
}

EditorPanelBase *EditorPanelsScroller::showEditorPanel(int panelIndex)
{
    EditorPanelBase *result = nullptr;

    for (int i = 0; i < this->editorPanels.size(); ++i)
    {
        auto *editor = this->editorPanels.getUnchecked(i);
        const auto isEditable = i == panelIndex;
        editor->setVisible(isEditable);
        editor->setEnabled(isEditable);
        if (isEditable)
        {
            result = editor;
        }
    }

    jassert(result != nullptr);
    return result;
}

void EditorPanelsScroller::updateSwitcher()
{
    Array<EditorPanelsSwitcher::Filters> filters;
    for (int i = 0; i < this->editorPanels.size(); ++i)
    {
        auto subFilters = this->editorPanels.getUnchecked(i)->getAllEventFilters();
        if (!subFilters.isEmpty())
        {
            filters.add({i, move(subFilters)});
        }
    }

    if (this->selectedEditorPanelIndex >= filters.size())
    {
        this->selectedEditorPanelIndex = 0;
        jassert(!filters.isEmpty());
        jassert(!filters.getFirst().eventFilters.isEmpty());
        this->selectedEventFilter = filters.getFirst().eventFilters.getFirst();
    }
 
    this->editorPanelsSwitcher->reload(filters);
    this->editorPanelsSwitcher->updateSelection(this->selectedEditorPanelIndex, this->selectedEventFilter);
}
