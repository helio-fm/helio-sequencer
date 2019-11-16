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
#include "NoteNameGuidesBar.h"
#include "NoteNameGuide.h"
#include "NoteComponent.h"
#include "PianoRoll.h"

NoteNameGuidesBar::NoteNameGuidesBar(PianoRoll &roll) :
    roll(roll)
{
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);

    this->setSize(34, 32);

    for (int i = 0; i < 128; ++i)
    {
        auto *guide = new NoteNameGuide(i);
        this->guides.add(guide);
        this->addChildComponent(guide);
        guide->setVisible(guide->isRootKey());
    }

    this->roll.getLassoSelection().addChangeListener(this);
}

NoteNameGuidesBar::~NoteNameGuidesBar()
{
    this->roll.getLassoSelection().removeChangeListener(this);

    this->guides.clear(true);
}

void NoteNameGuidesBar::updateBounds()
{
    this->setBounds(this->roll.getViewport().getViewPositionX(),
        0, this->getWidth(), this->roll.getHeight());


    for (auto *c : this->guides)
    {
        c->setBounds(0, this->roll.getYPositionByKey(c->getNoteNumber()),
            c->getWidth(), this->roll.getRowHeight());
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

void NoteNameGuidesBar::syncWithSelection(const Lasso *selection)
{
    const bool wasVisible = this->isVisible();
    this->setVisible(false);

    const bool showsRootKeys = (selection->getNumSelected() <= 1);

    for (auto *c : this->guides)
    {
        c->setVisible(showsRootKeys && c->isRootKey());
    }

    for (const auto *e : *selection)
    {
        // assuming we've subscribed only on a piano roll's lasso changes
        const auto *nc = static_cast<const NoteComponent *>(e);
        const auto key = jlimit(0, 128, nc->getNote().getKey() + nc->getClip().getKey());
        this->guides.getUnchecked(key)->setVisible(true);
    }

    this->setVisible(wasVisible);
}

void NoteNameGuidesBar::changeListenerCallback(ChangeBroadcaster *source)
{
    jassert(dynamic_cast<Lasso *>(source));
    const auto *selection = static_cast<const Lasso *>(source);
    this->syncWithSelection(selection);
}
