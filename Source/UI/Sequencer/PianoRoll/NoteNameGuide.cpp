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
#include "NoteNameGuide.h"
#include "CachedLabelImage.h"
#include "ColourIDs.h"

NoteNameGuide::NoteNameGuide(const String &noteName, int noteNumber) :
    noteNumber(noteNumber),
    fillColour(findDefaultColour(ColourIDs::Roll::noteNameFill)),
    borderColour(findDefaultColour(ColourIDs::Roll::noteNameBorder)),
    shadowColour(findDefaultColour(ColourIDs::Roll::noteNameShadow))
{
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);

    this->noteNameLabel = make<Label>();
    this->addAndMakeVisible(this->noteNameLabel.get());
    this->noteNameLabel->setFont({ 16.f });
    this->noteNameLabel->setJustificationType(Justification::centredLeft);
    
    this->noteNameLabel->setBufferedToImage(true);
    this->noteNameLabel->setCachedComponentImage(new CachedLabelImage(*this->noteNameLabel));
    this->noteNameLabel->setText(noteName, dontSendNotification);
}

void NoteNameGuide::paint(Graphics &g)
{
    g.setColour(this->shadowColour);
    g.fillPath(this->internalPath1);

    g.setColour(this->fillColour);
    g.fillPath(this->internalPath2);

    g.setColour(this->borderColour);
    g.fillRect(0, 1, 3, this->getHeight() - 1);
}

void NoteNameGuide::resized()
{
    this->noteNameLabel->setBounds(1, (this->getHeight() / 2) - 10, 45, 21);
    
    this->internalPath1.clear();
    this->internalPath1.startNewSubPath (3.f, 1.f);
    this->internalPath1.lineTo(30.f, 1.f);
    this->internalPath1.lineTo(34.f, static_cast<float> (this->getHeight()));
    this->internalPath1.lineTo(3.f, static_cast<float> (this->getHeight()));
    this->internalPath1.closeSubPath();

    this->internalPath2.clear();
    this->internalPath2.startNewSubPath(0.f, 1.f);
    this->internalPath2.lineTo(29.f, 1.f);
    this->internalPath2.lineTo(33.f, static_cast<float>(this->getHeight()));
    this->internalPath2.lineTo(0.f, static_cast<float>(this->getHeight()));
    this->internalPath2.closeSubPath();
}
