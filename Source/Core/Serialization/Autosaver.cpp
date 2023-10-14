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
#include "Autosaver.h"
#include "DocumentOwner.h"

Autosaver::Autosaver(DocumentOwner &targetDocumentOwner, int waitDelayMs) :
    documentOwner(targetDocumentOwner),
    delay(waitDelayMs)
{
    this->documentOwner.addChangeListener(this);
}

Autosaver::~Autosaver()
{
    this->documentOwner.removeChangeListener(this);
}

void Autosaver::changeListenerCallback(ChangeBroadcaster *source)
{
    // add some randomness to the delay, so that 
    // several open projects are not saved at once:
    static Random r;
    this->startTimer(this->delay + r.nextInt(1000));
}

void Autosaver::timerCallback()
{
    this->stopTimer();
    this->documentOwner.getDocument()->save();
}
