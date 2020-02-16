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

#pragma once

#include "Transport.h"

class PlayerThread final : public Thread
{
public:

    explicit PlayerThread(Transport &transport);
    ~PlayerThread() override;

    void startPlayback(double start, double end, bool shouldLoop,
        bool shouldBroadcastTransportEvents = true);

    void setRecordMidi(bool shouldRecord)
    {
        // todo something useful
        this->recordMidiMode = shouldRecord;
    }

    bool isRecordingMidi() const
    {
        return this->recordMidiMode;
    }

private:

    void run() override;

    Transport &transport;

    bool loopedMode = false;
    bool broadcastMode = false;
    bool recordMidiMode = false;

    double absStartPosition = 0.0;
    double absEndPosition = 1.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerThread)
};
