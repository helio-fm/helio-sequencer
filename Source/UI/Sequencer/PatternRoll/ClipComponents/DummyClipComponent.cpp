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
#include "DummyClipComponent.h"
#include "ProjectNode.h"
#include "MidiSequence.h"
#include "MidiEvent.h"
#include "HybridRoll.h"
#include "Pattern.h"
#include "MidiSequence.h"

DummyClipComponent::DummyClipComponent(HybridRoll &editor, Clip clip) :
    ClipComponent(editor, clip)
{
    //
}

void DummyClipComponent::paint(Graphics &g)
{
    const Colour myColour(Colours::white
        //.interpolatedWith(this->layer->getColour(), 0.5f)
        .withAlpha(this->ghostMode ? 0.2f : 0.95f)
        .darker(this->selectedState ? 0.5f : 0.f));

    g.setColour(myColour);
    g.fillRoundedRectangle(this->floatLocalBounds, 2.f);
}
