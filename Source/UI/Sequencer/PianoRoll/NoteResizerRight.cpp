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
#include "NoteResizerRight.h"

#include "RollBase.h"
#include "PianoRoll.h"
#include "PianoSequence.h"
#include "SequencerOperations.h"
#include "NoteComponent.h"
#include "HelioTheme.h"

NoteResizerRight::NoteResizerRight(RollBase &parentRoll) : roll(parentRoll)
{
    this->resizeIcon = make<IconComponent>(Icons::stretchRight);
    this->resizeIcon->setIconAlphaMultiplier(NoteResizerRight::lineAlpha);
    this->addAndMakeVisible(this->resizeIcon.get());

    this->setAlpha(0.f);
    this->setMouseCursor(MouseCursor::LeftRightResizeCursor);
    this->setInterceptsMouseClicks(false, false);

    this->setSize(NoteResizerRight::draggerSize, NoteResizerRight::draggerSize);

    constexpr auto iconOffset = 8;
    constexpr auto iconSize = NoteResizerRight::draggerSize / 2;
    this->resizeIcon->setBounds(iconOffset, iconOffset, iconSize, iconSize);

    this->draggerShape.clear();
    this->draggerShape.startNewSubPath(0.25f, 0.f);
    this->draggerShape.lineTo(float(this->getWidth()), 0.f);
    this->draggerShape.lineTo(float(this->getWidth()) * 0.75f, float(this->getHeight()) * 0.75f);
    this->draggerShape.lineTo(0.25f, float(this->getHeight()));
    this->draggerShape.closeSubPath();
}

NoteResizerRight::~NoteResizerRight()
{
    Desktop::getInstance().getAnimator().animateComponent(this,
        this->getBounds(), 0.f, Globals::UI::fadeOutShort, true, 0.0, 0.0);
}

void NoteResizerRight::paint(Graphics &g)
{
    g.setColour(this->fillColour);
    g.fillPath(this->draggerShape);

    g.setColour(Colours::black.withAlpha(NoteResizerRight::lineAlpha));
    g.strokePath(this->draggerShape, PathStrokeType(2.f));

    g.setColour(this->lineColour);
    g.strokePath(this->draggerShape, PathStrokeType(1.f));

    HelioTheme::drawDashedVerticalLine(g,
        0.f,
        NoteResizerRight::draggerSize + 1.f,
        float(this->getHeight() - 1),
        8.f);
}

bool NoteResizerRight::hitTest(int x, int y)
{
    constexpr auto r = NoteResizerRight::draggerSize;
    return (x * x + y * y) < (r * r);
}

void NoteResizerRight::mouseEnter(const MouseEvent &e)
{
    this->resizeIcon->setIconAlphaMultiplier(1.f);
}

void NoteResizerRight::mouseExit(const MouseEvent &e)
{
    this->resizeIcon->setIconAlphaMultiplier(NoteResizerRight::lineAlpha);
}

void NoteResizerRight::mouseDown(const MouseEvent &e)
{
    this->dragger.startDraggingComponent(this, e);

    const auto &selection = this->roll.getLassoSelection();
    const float groupStartBeat = SequencerOperations::findStartBeat(selection);

    this->groupResizerNote = this->findRightMostEvent(selection);

    for (int i = 0; i < selection.getNumSelected(); i++)
    {
        if (auto *note = dynamic_cast<NoteComponent *>(selection.getSelectedItem(i)))
        {
            if (selection.shouldDisplayGhostNotes())
            { note->getRoll().showGhostNoteFor(note); }

            note->startGroupScalingRight(groupStartBeat);
        }
    }
}

void NoteResizerRight::mouseDrag(const MouseEvent &e)
{
    this->dragger.dragComponent(this, e, nullptr);

    const auto &selection = this->roll.getLassoSelection();

    float groupScaleFactor = 1.f;
    const bool scaleFactorChanged =
        this->groupResizerNote->getGroupScaleRightFactor(
            e.withNewPosition(Point<int>(0, 0))
            .getEventRelativeTo(this->groupResizerNote), groupScaleFactor);

    if (scaleFactorChanged)
    {
        this->groupResizerNote->checkpointIfNeeded();

        Array<Note> groupDragBefore, groupDragAfter;
        for (int i = 0; i < selection.getNumSelected(); ++i)
        {
            auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
            groupDragBefore.add(nc->getNote());
            groupDragAfter.add(nc->continueGroupScalingRight(groupScaleFactor));
        }

        const auto &event = selection.getFirstAs<NoteComponent>()->getNote();
        auto *sequence = static_cast<PianoSequence *>(event.getSequence());
        sequence->changeGroup(groupDragBefore, groupDragAfter, true);
    }

    this->updateBounds();
}

void NoteResizerRight::mouseUp(const MouseEvent &e)
{
    const auto &selection = this->roll.getLassoSelection();

    for (int i = 0; i < selection.getNumSelected(); i++)
    {
        auto *nc = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        nc->getRoll().hideAllGhostNotes();
        nc->endGroupScalingLeft();
    }
}

NoteComponent *NoteResizerRight::findRightMostEvent(const Lasso &selection)
{
    NoteComponent *mc = nullptr;
    float rightMostBeat = -FLT_MAX;

    for (int i = 0; i < selection.getNumSelected(); ++i)
    {
        auto *currentEvent = static_cast<NoteComponent *>(selection.getSelectedItem(i));
        const float beatPlusLength = currentEvent->getBeat() + currentEvent->getLength();

        if (rightMostBeat < beatPlusLength)
        {
            rightMostBeat = beatPlusLength;
            mc = currentEvent;
        }
    }

    return mc;
}

void NoteResizerRight::updateBounds()
{
    const auto &selection = this->roll.getLassoSelection();
    const float groupEndBeat = SequencerOperations::findEndBeat(selection);
    const float clipBeat = selection.getFirstAs<NoteComponent>()->getClip().getBeat();

    const int xAnchor = this->roll.getXPositionByBeat(groupEndBeat + clipBeat);
    const int yAnchor = this->roll.getViewport().getViewPositionY() + Globals::UI::rollHeaderHeight;
    const int h = this->roll.getViewport().getViewHeight();
    this->setBounds(xAnchor, yAnchor, this->getWidth(), h);

    if (this->getAlpha() == 0.f && !this->fader.isAnimating())
    {
        this->fader.fadeIn(this, Globals::UI::fadeInShort);
    }
}

void NoteResizerRight::updateTopPosition()
{
    const int yAnchor = this->roll.getViewport().getViewPositionY() + Globals::UI::rollHeaderHeight;
    this->setTopLeftPosition(this->getX(), yAnchor);
}
