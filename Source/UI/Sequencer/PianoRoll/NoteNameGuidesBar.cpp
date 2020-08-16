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

    this->setSize(NoteNameGuidesBar::defaultWidth, 32);

    this->roll.getLassoSelection().addChangeListener(this);
}

NoteNameGuidesBar::~NoteNameGuidesBar()
{
    this->roll.getLassoSelection().removeChangeListener(this);

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
    const auto periodSize = this->roll.getPeriodSize();

    for (auto *c : this->guides)
    {
        c->setVisible(showsRootKeys && c->isRootKey(periodSize));
    }

    for (const auto *e : *selection)
    {
        // assuming we've subscribed only on a piano roll's lasso changes
        const auto *nc = static_cast<const NoteComponent *>(e);
        const auto key = jlimit(0, this->roll.getNumKeys(),
            nc->getNote().getKey() + nc->getClip().getKey());

        if (key <= this->guides.size() - 1)
        {
            this->guides.getUnchecked(key)->setVisible(true);
        }
    }

    this->setVisible(wasVisible);
}

void NoteNameGuidesBar::syncWithTemperament(Temperament::Ptr temperament)
{
    this->removeAllChildren();
    this->guides.clearQuick(true);
    const auto periodSize = temperament->getPeriodSize();

    if (periodSize > Globals::twelveTonePeriodSize)
    {
        // add more width for longer note names:
        this->setSize(NoteNameGuidesBar::extendedWidth, this->getHeight());
    }
    else
    {
        this->setSize(NoteNameGuidesBar::defaultWidth, this->getHeight());
    }

    for (int i = 0; i < temperament->getNumKeys(); ++i)
    {
        const auto noteName = temperament->getMidiNoteName(i, true);
        auto *guide = this->guides.add(new NoteNameGuide(noteName, i));
        this->addChildComponent(guide);
        guide->setVisible(guide->isRootKey(periodSize));
    }

    this->updateBounds();
}

void NoteNameGuidesBar::changeListenerCallback(ChangeBroadcaster *source)
{
    jassert(dynamic_cast<Lasso *>(source));
    const auto *selection = static_cast<const Lasso *>(source);
    this->syncWithSelection(selection);
}
