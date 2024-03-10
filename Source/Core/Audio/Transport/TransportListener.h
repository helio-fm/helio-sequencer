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

class TransportListener
{
public:
    
    virtual ~TransportListener() = default;

    // these methods will be called with message manager locked
    virtual void onPlay() = 0;
    virtual void onStop() = 0;
    virtual void onRecord() = 0;
    virtual void onRecordFailed(const Array<MidiDeviceInfo> &devices) {}

    virtual void onTotalTimeChanged(double timeMs) = 0;
    virtual void onLoopModeChanged(bool hasLoop, float startBeat, float endBeat) = 0;

    // these 2 methods could be called from a separate thread
    virtual void onSeek(float beatPosition) = 0;
    virtual void onCurrentTempoChanged(double msPerQuarterNote) = 0;

};
