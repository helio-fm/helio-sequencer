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
#include "NoteResizerLeft.h"

#include "HybridRoll.h"
#include "PianoRoll.h"
#include "PianoSequence.h"
#include "NoteComponent.h"
#include "SequencerOperations.h"

NoteResizerLeft::NoteResizerLeft(HybridRoll &parentRoll) : roll(parentRoll)
{
    this->resizeIcon = make<IconComponent>(Icons::stretchLeft);
    this->resizeIcon->setIconAlphaMultiplier(NoteResizerLeft::lineAlpha);
    this->addAndMakeVisible(this->resizeIcon.get());

    this->setAlpha(0.f);
    this->setMouseCursor(MouseCursor::LeftRightResizeCursor);
    this->setInterceptsMouseClicks(false, false);

    this->setSize(NoteResizerLeft::draggerSize, NoteResizerLeft::draggerSize);

    constexpr auto iconOffset = 8;
    constexpr auto iconSize = NoteResizerLeft::draggerSize / 2;
    this->resizeIcon->setBounds(this->getWidth() - iconSize - iconOffset,
        iconOffset, iconSize, iconSize);

    this->draggerShape.clear();
    this->draggerShape.startNewSubPath(0.f, 0.f);
    this->draggerShape.lineTo(float(this->getWidth()) * 0.25f, float(this->getHeight()) * 0.75f);
    this->draggerShape.lineTo(float(this->getWidth() - 0.25f), float(this->getHeight()));
    this->draggerShape.lineTo(float(this->getWidth() - 0.25f), 0.f);
    this->draggerShape.closeSubPath();
}

NoteResizerLeft::~NoteResizerLeft()
{
    Desktop::getInstance().getAnimator().animateComponent(this,
        this->getBounds(), 0.f, Globals::UI::fadeOutShort, true, 0.0, 0.0);
}

void NoteResizerLeft::paint(Graphics &g)
{
    g.setColour(this->fillColour);
    g.fillPath(this->draggerShape);

    g.setColour(Colours::black.withAlpha(NoteResizerLeft::lineAlpha));
    g.strokePath(this->draggerShape, PathStrokeType(2.f));

    g.setColour(this->lineColour);
    g.strokePath(this->draggerShape, PathStrokeType(1.f));

    static constexpr int dashLength = 8;
    for (int i = NoteResizerLeft::draggerSize;
        i < this->getHeight() - 1; i += (dashLength * 2))
    {
        g.fillRect(this->getWidth() - 1, i, 1, dashLength);
    }
}

bool NoteResizerLeft::hitTest(int x, int y)
{
    constexpr auto r = NoteResizerLeft::draggerSize;
    const int dx = x - this->getWidth();
    return (dx * dx + y * y) < (r * r);
}

void NoteResizerLeft::mouseEnter(const MouseEvent &e)
{
    this->resizeIcon->setIconAlphaMultiplier(1.f);
}

void NoteResizerLeft::mouseExit(const MouseEvent &e)
{
    this->resizeIcon->setIconAlphaMultiplier(NoteResizerLeft::lineAlpha);
}

void NoteResizerLeft::mouseDown(const MouseEvent &e)
{
    this->dragger.startDraggingComponent(this, e);

    const auto &selection = this->roll.getLassoSelection();
    const float groupEndBeat = SequencerOperations::findEndBeat(selection);

    this->groupResizerNote = this->findLeftMostEvent(selection);

    for (int i = 0; i < selection.getNumSelected(); i++)
    {
        if (auto *note = dynamic_cast<NoteComponent *>(selection.getSelectedItem(i)))
        {
            if (selection.shouldDisplayGhostNotes())
            { note->getRoll().showGhostNoteFor(note); }

            note->startGroupScalingLeft(groupEndBeat);
        }
    }
}

void NoteResizerLeft::mouseDrag(const MouseEvent &e)
{
    this->dragger.dragComponent(this, e, nullptr);

    const auto &selection = this->roll.getLassoSelection();

    float groupScaleFactor = 1.f;
    const bool scaleFactorChanged = this->groupResizerNote->getGroupScaleLeftFactor(
        e.withNewPosition(Point<int>(this->getWidth(), 0))
            .getEventRelativeTo(this->groupResizerNote), groupScaleFactor);

    if (scaleFactorChanged)
    {
        this->groupResizerNote->checkpointIfNeeded();

        Array<Note> groupDragBefore, groupDragAfter;
        for (int i = 0; i < selection.getNumSelected(); ++i)
        {
            auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            groupDragBefore.add(nc->getNote());
            groupDragAfter.add(nc->continueGroupScalingLeft(groupScaleFactor));
        }

        const auto &event = selection.getFirstAs<NoteComponent>()->getNote();
        auto *sequence = static_cast<PianoSequence *>(event.getSequence());
        sequence->changeGroup(groupDragBefore, groupDragAfter, true);
    }

    this->updateBounds();
}

void NoteResizerLeft::mouseUp(const MouseEvent &e)
{
    const auto &selection = this->roll.getLassoSelection();

    for (int i = 0; i < selection.getNumSelected(); i++)
    {
        auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        nc->getRoll().hideAllGhostNotes();
        nc->endGroupScalingLeft();
    }
}

NoteComponent *NoteResizerLeft::findLeftMostEvent(const Lasso &selection)
{
    MidiEventComponent *mc = nullptr;
    float leftMostBeat = FLT_MAX;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *const e = selection.getItemAs<MidiEventComponent>(i);

        if (leftMostBeat > e->getBeat())
        {
            leftMostBeat = e->getBeat();
            mc = e;
        }
    }

    return static_cast<NoteComponent *>(mc);
}

void NoteResizerLeft::updateBounds()
{
    const auto &selection = this->roll.getLassoSelection();
    const float groupStartBeat = SequencerOperations::findStartBeat(selection);
    const float clipBeat = selection.getFirstAs<NoteComponent>()->getClip().getBeat();
    
    const int xAnchor = this->roll.getXPositionByBeat(groupStartBeat + clipBeat);
    const int yAnchor = this->roll.getViewport().getViewPositionY() + Globals::UI::rollHeaderHeight;
    const int h = this->roll.getViewport().getViewHeight();
    this->setBounds(xAnchor - this->getWidth(), yAnchor, this->getWidth(), h);

    if (this->getAlpha() == 0.f && !this->fader.isAnimating())
    {
        this->fader.fadeIn(this, Globals::UI::fadeInShort);
    }
}

void NoteResizerLeft::updateTopPosition()
{
    const int yAnchor = this->roll.getViewport().getViewPositionY() + Globals::UI::rollHeaderHeight;
    this->setTopLeftPosition(this->getX(), yAnchor);
}
