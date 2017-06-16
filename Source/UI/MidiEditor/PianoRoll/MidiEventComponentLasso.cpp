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
#include "MidiEventSelection.h"
#include "MidiEventComponentLasso.h"
#include "HelioTheme.h"

MidiEventComponentLasso::MidiEventComponentLasso() :
    source(nullptr)
{
}

void MidiEventComponentLasso::beginLasso(const MouseEvent &e, LassoSource<MidiEventComponent *> *const lassoSource)
{
    jassert(source == nullptr);
    jassert(lassoSource != nullptr);
    jassert(getParentComponent() != nullptr);

    if (lassoSource != nullptr)
    {
        source = lassoSource;
        originalSelection = lassoSource->getLassoSelection().getItemArray();
        this->setSize(0, 0);
        this->toFront(false);
        dragStartPos = e.getMouseDownPosition();
    }
}

void MidiEventComponentLasso::dragLasso(const MouseEvent &e)
{
    if (source != nullptr)
    {
        this->setBounds(Rectangle<int>(dragStartPos, e.getPosition()));
        this->setVisible(true);

        Array<MidiEventComponent *> itemsInLasso;
        source->findLassoItemsInArea(itemsInLasso, getBounds());

        if (e.mods.isShiftDown())
        {
            itemsInLasso.removeValuesIn(originalSelection);
            itemsInLasso.addArray(originalSelection);
        }
        else if (e.mods.isAltDown())
        {
            Array<MidiEventComponent *> originalMinusNew(originalSelection);
            originalMinusNew.removeValuesIn(itemsInLasso);

            itemsInLasso.removeValuesIn(originalSelection);
            itemsInLasso.addArray(originalMinusNew);
        }

        source->getLassoSelection() = MidiEventSelection(itemsInLasso);
    }
}

void MidiEventComponentLasso::endLasso()
{
    if (source != nullptr)
    {
        this->source = nullptr;
        this->originalSelection.clear();
        this->setVisible(false);
    }
}

bool MidiEventComponentLasso::isDragging() const
{
    return (this->source != nullptr);
}

void MidiEventComponentLasso::paint(Graphics &g)
{
    this->getLookAndFeel().drawLasso(g, *this);
    jassert(isMouseButtonDownAnywhere());
}
