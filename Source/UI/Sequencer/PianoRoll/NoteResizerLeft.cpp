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
#include "NoteResizerLeft.h"

#include "RollBase.h"
#include "PianoRoll.h"
#include "PianoSequence.h"
#include "NoteComponent.h"
#include "SequencerOperations.h"
#include "HelioTheme.h"

NoteResizerLeft::NoteResizerLeft(RollBase &parentRoll) : roll(parentRoll)
{
    constexpr auto iconSize = NoteResizerLeft::draggerSize / 2;
    this->resizeIcon = make<IconComponent>(Icons::stretchLeft);
    this->resizeIcon->setSize(iconSize, iconSize);
    this->addAndMakeVisible(this->resizeIcon.get());

    this->setAlpha(0.f);
    this->setMouseCursor(MouseCursor::LeftRightResizeCursor);
    this->setInterceptsMouseClicks(false, false);
    this->setAlwaysOnTop(true);

    this->setSize(NoteResizerLeft::draggerSize, NoteResizerLeft::draggerSize);
}

NoteResizerLeft::~NoteResizerLeft()
{
    App::fadeOutComponent(this, Globals::UI::fadeOutShort);
}

void NoteResizerLeft::paint(Graphics &g)
{
    g.setColour(this->fillColour);
    g.fillPath(this->draggerShape);

    g.setColour(this->shadowColour);
    g.drawVerticalLine(this->getWidth() - 1, 1,
        float(this->getHeight() - NoteResizerLeft::draggerSize));

    g.setColour(this->lineColour);
    g.strokePath(this->draggerShape, PathStrokeType(1.25f));

    HelioTheme::drawDashedVerticalLine(g,
        float(this->getWidth() - 1),
        1.f,
        float(this->getHeight() - NoteResizerLeft::draggerSize - 1),
        8.f);
}

bool NoteResizerLeft::hitTest(int x, int y)
{
    return y > (this->getHeight() - NoteResizerLeft::draggerSize);
}

void NoteResizerLeft::resized()
{
    constexpr auto iconOffset = 14;
    this->resizeIcon->setCentrePosition(this->getWidth() - iconOffset, this->getHeight() - iconOffset);

    const auto w = float(this->getWidth());
    const auto h = float(this->getHeight());
    const auto r = w - 0.25f;
    const auto shapeSize = float(NoteResizerLeft::draggerSize);

    this->draggerShape.clear();
    this->draggerShape.startNewSubPath(r, h - shapeSize);
    this->draggerShape.lineTo(w * 0.25f, h - (shapeSize * 0.75f));
    this->draggerShape.lineTo(0.f, h);
    this->draggerShape.lineTo(r, h);
    this->draggerShape.closeSubPath();
}

void NoteResizerLeft::mouseDown(const MouseEvent &e)
{
    this->dragger.startDraggingComponent(this, e);

    const auto &selection = this->roll.getLassoSelection();
    const float groupEndBeat = SequencerOperations::findEndBeat(selection);

    this->groupResizerNote = this->findLeftmostTopmostEvent(selection);

    for (int i = 0; i < selection.getNumSelected(); i++)
    {
        auto *note = selection.getItemAs<NoteComponent>(i);
        if (selection.shouldDisplayGhostNotes())
        {
            note->getRoll().showGhostNoteFor(note);
        }

        note->startGroupScalingLeft(groupEndBeat);
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
            auto *nc = selection.getItemAs<NoteComponent>(i);
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
        auto *nc = selection.getItemAs<NoteComponent>(i);
        nc->getRoll().setDefaultNoteLength(nc->getLength());
        nc->getRoll().hideAllGhostNotes();
        nc->endGroupScalingLeft();
    }
}

NoteComponent *NoteResizerLeft::findLeftmostTopmostEvent(const Lasso &selection)
{
    jassert(selection.size() > 0);

    NoteComponent *resultNoteComponent = nullptr;
    auto startBeat = std::numeric_limits<float>::max();
    auto startKey = std::numeric_limits<Note::Key>::lowest();

    for (int i = 0; i < selection.size(); ++i)
    {
        auto *noteComponent = selection.getItemAs<NoteComponent>(i);
        if (startBeat > noteComponent->getBeat() ||
            (startBeat == noteComponent->getBeat() && startKey < noteComponent->getKey()))
        {
            startBeat = noteComponent->getBeat();
            startKey = noteComponent->getKey();
            resultNoteComponent = noteComponent;
        }
    }

    return resultNoteComponent;
}

void NoteResizerLeft::updateBounds()
{
    const auto &selection = this->roll.getLassoSelection();
    jassert(selection.getNumSelected() > 0);

    auto *groupStartNoteComponent = this->findLeftmostTopmostEvent(selection);
    const auto anchor = this->roll.getEventBounds(groupStartNoteComponent);

    const auto &viewport = this->roll.getViewport();
    const int minY = viewport.getViewPositionY() + Globals::UI::rollHeaderHeight;
    const int maxY = viewport.getViewPositionY() + viewport.getViewHeight() - NoteResizerLeft::draggerSize;

    const auto y = jmax(minY, jmin(maxY, roundToIntAccurate(anchor.getBottom() - 1)));
    const auto h = maxY - y + NoteResizerLeft::draggerSize + 1;

    this->setBounds(int(floorf(anchor.getX())) - this->getWidth() + 1, y, this->getWidth(), h);

    if (this->getAlpha() < 1.f)
    {
        App::fadeInComponent(this, Globals::UI::fadeInShort);
    }
}
