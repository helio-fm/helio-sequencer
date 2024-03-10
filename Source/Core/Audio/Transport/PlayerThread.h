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

#include "Transport.h"

class PlayerThread final : public Thread
{
public:

    explicit PlayerThread(Transport &transport);
    ~PlayerThread() override;

    void startPlayback(Transport::PlaybackContext::Ptr context);

    void setSpeedMultiplier(float multiplier);

private:

    void run() override;

    Transport &transport;
    TransportPlaybackCache sequences;

    Transport::PlaybackContext::Ptr context;

    Atomic<float> speedMultiplier = 1.f;
    Atomic<bool> speedMultiplierChanged = false;

    Atomic<double> currentTempo = 0.f;

    // check if the thread needs to stop at least every x ms:
    static constexpr auto minStopCheckTimeMs = 200;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerThread)
};
