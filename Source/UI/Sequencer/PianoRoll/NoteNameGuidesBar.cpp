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

NoteNameGuidesBar::NoteNameGuidesBar(PianoRoll &roll, WeakReference<MidiTrack> keySignatures) :
    roll(roll),
    keySignatures(keySignatures)
{
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);

    this->roll.addRollListener(this);
    this->roll.getLassoSelection().addChangeListener(this);
}

NoteNameGuidesBar::~NoteNameGuidesBar()
{
    this->roll.getLassoSelection().removeChangeListener(this);
    this->roll.removeRollListener(this);

    this->removeAllChildren();
    this->guides.clear(true);
}

void NoteNameGuidesBar::updateBounds()
{
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

    const bool shouldShowsRootKeys = this->selectedKeys.size() <= 1;

    for (auto *c : this->guides)
    {
        const auto shouldBeVisible = shouldShowsRootKeys &&
            c->isRootKey(this->scaleRootKey, this->roll.getPeriodSize());

        if (shouldBeVisible)
        {
            c->setNoteName(this->temperament->getMidiNoteName(c->getNoteNumber(),
                this->scaleRootKey, this->scaleRootKeyName, true));
        }

        c->setVisible(shouldBeVisible);
    }

    for (const auto &key : this->selectedKeys)
    {
        const auto noteName = this->temperament->getMidiNoteName(key,
            this->scaleRootKey, this->scaleRootKeyName, true);

        if (key <= this->guides.size() - 1)
        {
            this->guides.getUnchecked(key)->setNoteName(noteName);
            this->guides.getUnchecked(key)->setVisible(true);
        }
    }
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

    if (temperament->getPeriodSize() > Globals::twelveTonePeriodSize)
    {
        // add more width for longer note names:
        this->setSize(NoteNameGuidesBar::extendedWidth, this->getHeight());
    }
    else
    {
        this->setSize(NoteNameGuidesBar::defaultWidth, this->getHeight());
    }

    this->guides.clearQuick(true);
    for (int i = 0; i < temperament->getNumKeys(); ++i)
    {
        auto *guide = this->guides.add(new NoteNameGuide(i));
        this->addChildComponent(guide);
    }

    this->updateBounds();
    this->triggerAsyncUpdate();
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

    this->updateBounds();
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
