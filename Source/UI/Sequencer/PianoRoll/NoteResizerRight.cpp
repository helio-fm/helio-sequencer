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
#include "NoteResizerRight.h"

#include "RollBase.h"
#include "PianoRoll.h"
#include "PianoSequence.h"
#include "SequencerOperations.h"
#include "NoteComponent.h"
#include "HelioTheme.h"

NoteResizerRight::NoteResizerRight(RollBase &parentRoll) : roll(parentRoll)
{
    constexpr auto iconSize = NoteResizerRight::draggerSize / 2;
    this->resizeIcon = make<IconComponent>(Icons::stretchRight);
    this->resizeIcon->setSize(iconSize, iconSize);
    this->addAndMakeVisible(this->resizeIcon.get());

    this->setAlpha(0.f);
    this->setMouseCursor(MouseCursor::LeftRightResizeCursor);
    this->setInterceptsMouseClicks(false, false);
    this->setAlwaysOnTop(true);

    this->setSize(NoteResizerRight::draggerSize, NoteResizerRight::draggerSize);
}

NoteResizerRight::~NoteResizerRight()
{
    App::fadeOutComponent(this, Globals::UI::fadeOutShort);
}

void NoteResizerRight::paint(Graphics &g)
{
    g.setColour(this->fillColour);
    g.fillPath(this->draggerShape);

    g.setColour(this->shadowColour);
    g.drawVerticalLine(0, 1,
        float(this->getHeight() - NoteResizerRight::draggerSize));

    g.setColour(this->lineColour);
    g.strokePath(this->draggerShape, PathStrokeType(1.f));

    HelioTheme::drawDashedVerticalLine(g,
        0.f,
        1.f,
        float(this->getHeight() - NoteResizerRight::draggerSize - 1),
        8.f);
}

bool NoteResizerRight::hitTest(int x, int y)
{
    return y > (this->getHeight() - NoteResizerRight::draggerSize);
}

void NoteResizerRight::resized()
{
    constexpr auto iconOffset = 14;
    this->resizeIcon->setCentrePosition(iconOffset, this->getHeight() - iconOffset);

    const auto w = float(this->getWidth());
    const auto h = float(this->getHeight());
    const auto l = 0.25f;
    const auto shapeSize = float(NoteResizerRight::draggerSize);

    this->draggerShape.clear();
    this->draggerShape.startNewSubPath(l, h - shapeSize);
    this->draggerShape.lineTo(w * 0.75f, h - (shapeSize * 0.75f));
    this->draggerShape.lineTo(w, h);
    this->draggerShape.lineTo(l, h);
    this->draggerShape.closeSubPath();
}

void NoteResizerRight::mouseDown(const MouseEvent &e)
{
    if (this->roll.isMultiTouchEvent(e))
    {
        return;
    }

    this->dragger.startDraggingComponent(this, e);

    const auto &selection = this->roll.getLassoSelection();
    if (selection.getNumSelected() == 0)
    {
        jassertfalse;
        return;
    }

    const float groupStartBeat = SequencerOperations::findStartBeat(selection);

    this->groupResizerNote = this->findRightmostTopmostEvent(selection);

    for (int i = 0; i < selection.getNumSelected(); i++)
    {
        auto *note = selection.getItemAs<NoteComponent>(i);
        if (selection.shouldDisplayGhostNotes())
        {
            note->getRoll().showGhostNoteFor(note);
        }

        note->startGroupScalingRight(groupStartBeat);
    }
}

void NoteResizerRight::mouseDrag(const MouseEvent &e)
{
    if (this->roll.isMultiTouchEvent(e) || this->groupResizerNote == nullptr)
    {
        return;
    }
    
    this->dragger.dragComponent(this, e, nullptr);

    const auto &selection = this->roll.getLassoSelection();
    if (selection.getNumSelected() == 0)
    {
        jassertfalse;
        return;
    }

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
            auto *nc = selection.getItemAs<NoteComponent>(i);
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
    if (this->roll.isMultiTouchEvent(e))
    {
        return;
    }

    const auto &selection = this->roll.getLassoSelection();
    if (selection.getNumSelected() == 0)
    {
        jassertfalse;
        return;
    }

    for (int i = 0; i < selection.getNumSelected(); i++)
    {
        auto *nc = selection.getItemAs<NoteComponent>(i);
        nc->getRoll().setDefaultNoteLength(nc->getLength());
        nc->getRoll().hideAllGhostNotes();
        nc->endGroupScalingLeft();
    }
}

NoteComponent *NoteResizerRight::findRightmostTopmostEvent(const Lasso &selection)
{
    jassert(selection.size() > 0);

    NoteComponent *resultNoteComponent = nullptr;
    auto endBeat = std::numeric_limits<float>::lowest();
    auto startKey = std::numeric_limits<Note::Key>::lowest();

    for (int i = 0; i < selection.size(); ++i)
    {
        auto *noteComponent = selection.getItemAs<NoteComponent>(i);
        const auto beatPlusLength = noteComponent->getBeat() + noteComponent->getLength();
        if (endBeat < beatPlusLength ||
            (endBeat == beatPlusLength && startKey < noteComponent->getKey()))
        {
            endBeat = beatPlusLength;
            startKey = noteComponent->getKey();
            resultNoteComponent = noteComponent;
        }
    }

    return resultNoteComponent;
}

void NoteResizerRight::updateBounds()
{
    const auto &selection = this->roll.getLassoSelection();
    if (selection.getNumSelected() == 0)
    {
        this->setAlpha(0.f);
        return; // the roll somehow called this method before hiding the resizer
    }

    auto *groupStartNoteComponent = this->findRightmostTopmostEvent(selection);
    const auto anchor = this->roll.getEventBounds(groupStartNoteComponent);

    const auto &viewport = this->roll.getViewport();
    const int minY = viewport.getViewPositionY() + Globals::UI::rollHeaderHeight;
    const int maxY = viewport.getViewPositionY() + viewport.getViewHeight() - NoteResizerRight::draggerSize;

    const auto y = jmax(minY, jmin(maxY, roundToIntAccurate(anchor.getBottom() - 1)));
    const auto h = maxY - y + NoteResizerRight::draggerSize + 1;

    this->setBounds(int(floorf(anchor.getRight())), y, this->getWidth(), h);

    if (this->getAlpha() < 1.f)
    {
        App::fadeInComponent(this, Globals::UI::fadeInShort);
    }
}
