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
    this->setInterceptsMouseClicks(true, true);

    this->editorPanelsSwitcher->onChangeSelection = [this](int panelId,
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
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void EditorPanelsScroller::resized()
{
    for (auto *editor : this->editorPanels)
    {
        editor->setBounds(this->getEditorPanelBounds());
    }
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

void EditorPanelsScroller::mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel)
{
    if (this->roll != nullptr)
    {
        this->roll->mouseWheelMove(event.getEventRelativeTo(this->roll), wheel);
    }
}

//===----------------------------------------------------------------------===//
// MidiRollListener
//===----------------------------------------------------------------------===//

void EditorPanelsScroller::onMidiRollMoved(RollBase *targetRoll)
{
    if (this->isVisible() && this->roll == targetRoll)
    {
        this->triggerAsyncUpdate();
    }
}

void EditorPanelsScroller::onMidiRollResized(RollBase *targetRoll)
{
    if (this->isVisible() && this->roll == targetRoll)
    {
        this->triggerAsyncUpdate();
    }
}

//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void EditorPanelsScroller::handleAsyncUpdate()
{
    for (auto *editor : this->editorPanels)
    {
        editor->setBounds(this->getEditorPanelBounds());
    }
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
// ProjectListener & editable scope selection
//===----------------------------------------------------------------------===//

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
    if (this->activeClip == clip)
    {
        return;
    }

    this->activeClip = clip;

    if (this->editorPanelSelectionMode == EditorPanelSelectionMode::Automatic)
    {
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
                auto *editor = this->showEditorPanel(this->selectedEditorPanelIndex);
                editor->setEditableClip(cc->getClip(), this->selectedEventFilter);
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
