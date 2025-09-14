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
#include "NoteNameGuidesBar.h"
#include "MidiTrack.h"
#include "SequencerOperations.h"
#include "NoteNameGuide.h"
#include "NoteComponent.h"
#include "PianoRoll.h"
#include "Config.h"

NoteNameGuidesBar::NoteNameGuidesBar(PianoRoll &roll, WeakReference<MidiTrack> keySignatures) :
    roll(roll),
    keySignatures(keySignatures)
{
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
    this->setAccessible(false);

    this->useFixedDoNotation = App::Config().getUiFlags()->isUsingFixedDoNotation();

    this->roll.addRollListener(this);
    this->roll.getLassoSelection().addChangeListener(this);
    App::Config().getUiFlags()->addListener(this);
}

NoteNameGuidesBar::~NoteNameGuidesBar()
{
    App::Config().getUiFlags()->removeListener(this);
    this->roll.getLassoSelection().removeChangeListener(this);
    this->roll.removeRollListener(this);

    this->removeAllChildren();
    this->guides.clear(true);
}

void NoteNameGuidesBar::updateBounds()
{
    if (this->noteShapeWidth != this->getWidth() ||
        this->noteShapeHeight != this->roll.getRowHeight())
    {
        this->noteShapeWidth = this->getWidth();
        this->noteShapeHeight = this->roll.getRowHeight();

        const auto w = float(this->noteShapeWidth);
        const auto h = float(this->noteShapeHeight);

        this->noteShapeOutlinePath.clear();
        this->noteShapeOutlinePath.preallocateSpace(16);
        this->noteShapeOutlinePath.startNewSubPath(NoteNameGuidesBar::borderWidth + 1, 1.f);
        this->noteShapeOutlinePath.lineTo(w - NoteNameGuidesBar::arrowWidth, 1.f);
        this->noteShapeOutlinePath.lineTo(w, (h / 2.f) + 0.5f);
        this->noteShapeOutlinePath.lineTo(w - NoteNameGuidesBar::arrowWidth, h);
        this->noteShapeOutlinePath.lineTo(NoteNameGuidesBar::borderWidth + 1.f, h);
        this->noteShapeOutlinePath.closeSubPath();

        this->noteShapeFillPath.clear();
        this->noteShapeFillPath.preallocateSpace(16);
        this->noteShapeFillPath.startNewSubPath(0.f, 1.f);
        this->noteShapeFillPath.lineTo(w - NoteNameGuidesBar::arrowWidth - 1.f, 1.f);
        this->noteShapeFillPath.lineTo(w - 1.f, (h / 2.f) + 0.5f);
        this->noteShapeFillPath.lineTo(w - NoteNameGuidesBar::arrowWidth - 1.f, h);
        this->noteShapeFillPath.lineTo(0.f, h);
        this->noteShapeFillPath.closeSubPath();
    }

    this->setBounds(this->roll.getViewport().getViewPositionX(),
        0, this->getWidth(), this->roll.getHeight());

    for (auto *c : this->guides)
    {
        c->setBounds(0, this->roll.getYPositionByKey(c->getNoteNumber()),
            this->getWidth(), this->roll.getRowHeight());
    }
}

void NoteNameGuidesBar::updatePosition()
{
    this->setTopLeftPosition(this->roll.getViewport().getViewPositionX(), 0);

    for (auto *c : this->guides)
    {
        c->setTopLeftPosition(0, this->roll.getYPositionByKey(c->getNoteNumber()));
    }
}

void NoteNameGuidesBar::updateContent()
{
    if (!this->isVisible() ||
        this->temperament == nullptr)
    {
        return;
    }

    const auto absBeat = this->selectionStartBeat.orFallback(
        this->roll.getBeatByXPosition(float(this->roll.getViewport().getViewPositionX())));

    if (!SequencerOperations::findHarmonicContext(absBeat, absBeat, this->keySignatures,
        this->scale, this->scaleRootKey, this->scaleRootKeyName))
    {
        this->scale = this->defaultScale;
        this->scaleRootKey = 0;
        this->scaleRootKeyName = {};
    }

    // show all selected notes and root notes, but also try not to clutter the view
    // and hide root notes if the selection is larger than a four-note chord:
    const bool shouldShowsRootKeys = this->selectedKeys.size() <= 4;

    int guidesWidth = 0;
    int periodNumber = 0;
    for (auto *c : this->guides)
    {
        const auto shouldBeVisible = this->selectedKeys.contains(c->getNoteNumber()) ||
            (shouldShowsRootKeys && c->isRootKey(this->scaleRootKey, this->roll.getPeriodSize()));

        if (shouldBeVisible)
        {
            const auto noteName = this->temperament->getMidiNoteName(c->getNoteNumber(),
                this->scaleRootKey, this->scaleRootKeyName, periodNumber);
            const auto width = c->setNoteName(noteName, periodNumber, this->useFixedDoNotation);
            guidesWidth = jmax(guidesWidth, width);
        }

        c->setVisible(shouldBeVisible);
    }

    guidesWidth += int(NoteNameGuidesBar::borderWidth + NoteNameGuidesBar::arrowWidth +
        NoteNameGuidesBar::nameMarginLeft + NoteNameGuidesBar::nameMarginRight);

    this->setSize(guidesWidth, this->getHeight());
    this->updateBounds();
}

void NoteNameGuidesBar::syncWithSelection(const Lasso *selection)
{
    if (selection->getNumSelected() == 0)
    {
        this->selectedKeys.clear();
        this->selectionStartBeat.reset();
    }
    else
    {
        this->selectedKeys.clear();
        this->selectionStartBeat = FLT_MAX;
        for (const auto *e : *selection)
        {
            // assuming we've subscribed on a piano roll's lasso changes
            jassert(dynamic_cast<const NoteComponent *>(e));
            const auto *nc = static_cast<const NoteComponent *>(e);

            this->selectionStartBeat = jmin(*this->selectionStartBeat,
                nc->getNote().getBeat() + nc->getClip().getBeat());

            this->selectedKeys.insert(jlimit(0, this->roll.getNumKeys(),
                nc->getNote().getKey() + nc->getClip().getKey()));
        }
    }

    if (this->isVisible())
    {
        this->toFront(false);
    }

    this->updateContent();
}

void NoteNameGuidesBar::syncWithTemperament(Temperament::Ptr newTemperament)
{
    if (this->temperament == newTemperament)
    {
        return;
    }

    this->removeAllChildren();
    this->temperament = newTemperament;

    this->guides.clearQuick(true);
    for (int i = 0; i < temperament->getNumKeys(); ++i)
    {
        auto *guide = this->guides.add(new NoteNameGuide(*this, i));
        this->addChildComponent(guide);
    }

    this->updateBounds();
    this->triggerAsyncUpdate();
}

//===----------------------------------------------------------------------===//
// UserInterfaceFlags::Listener
//===----------------------------------------------------------------------===//

void NoteNameGuidesBar::onUseFixedDoFlagChanged(bool shouldUseFixedDo)
{
    if (this->useFixedDoNotation == shouldUseFixedDo)
    {
        return;
    }

    this->useFixedDoNotation = shouldUseFixedDo;
    this->updateContent();
}

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void NoteNameGuidesBar::changeListenerCallback(ChangeBroadcaster *source)
{
    jassert(dynamic_cast<Lasso *>(source));
    const auto *selection = static_cast<const Lasso *>(source);
    this->syncWithSelection(selection);
}

//===----------------------------------------------------------------------===//
// RollListener
//===----------------------------------------------------------------------===//

void NoteNameGuidesBar::onMidiRollMoved(RollBase *targetRoll)
{
    if (!this->isVisible())
    {
        return;
    }

    // reposition immediately, but update content asynchronously
    this->updatePosition();
    this->triggerAsyncUpdate();
}

void NoteNameGuidesBar::onMidiRollResized(RollBase *targetRoll)
{
    if (!this->isVisible())
    {
        return;
    }

    // reposition immediately, but update content asynchronously
    this->updateBounds();
    this->triggerAsyncUpdate();
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void NoteNameGuidesBar::visibilityChanged()
{
    if (!this->isVisible() ||
        this->getParentComponent() == nullptr)
    {
        return;
    }

    this->updateContent();
    this->toFront(false);
}

//===----------------------------------------------------------------------===//
// AsyncUpdater
//===----------------------------------------------------------------------===//

void NoteNameGuidesBar::handleAsyncUpdate()
{
    if (this->isVisible())
    {
        this->updateContent();
    }
}
