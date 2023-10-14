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

class Pattern;
class MidiTrack;
class MidiSequence;

class MidiTrackSource
{
public:

    virtual ~MidiTrackSource() = default;

    inline Pattern *findPatternByTrackId(const String &trackId)
    {
        return this->getPatternByTrackId(trackId);
    }

    template<typename T>
    T *findSequenceByTrackId(const String &trackId)
    {
        return dynamic_cast<T *>(this->getSequenceByTrackId(trackId));
    }

    template<typename T>
    T *findTrackById(const String &trackId)
    {
        return dynamic_cast<T *>(this->getTrackById(trackId));
    }

protected:

    virtual MidiTrack *getTrackById(const String &trackId) = 0;
    virtual Pattern *getPatternByTrackId(const String &trackId) = 0;
    virtual MidiSequence *getSequenceByTrackId(const String &trackId) = 0;

};
