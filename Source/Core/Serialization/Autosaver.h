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

#pragma once

#include "DocumentOwner.h"

class Autosaver final : private ChangeListener, private Timer
{
public:

    explicit Autosaver(DocumentOwner &documentOwner, int waitDelayMs = 30000) :
        documentOwner(documentOwner),
        delay(waitDelayMs)
    {
        this->documentOwner.addChangeListener(this);
    }

    ~Autosaver() override
    {
        this->documentOwner.removeChangeListener(this);
    }

private:

    void changeListenerCallback(ChangeBroadcaster *source) override
    {
        // add some randomness to the delay, so that 
        // several open projects are not saved at once:
        static Random r;
        this->startTimer(this->delay + r.nextInt(1000));
    }

    void timerCallback() override
    {
        this->stopTimer();
        this->documentOwner.getDocument()->save();
    }


    DocumentOwner &documentOwner;

    const int delay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Autosaver)

};
