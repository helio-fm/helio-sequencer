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

class Pattern;
class MidiTrack;
class MidiSequence;

class MidiTrackSource
{
public:

    virtual ~MidiTrackSource() {}

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

class EmptyMidiTrackSource : public MidiTrackSource
{
protected:

    MidiTrack *getTrackById(const String &trackId) override { return nullptr; };
    Pattern *getPatternByTrackId(const String &trackId) override { return nullptr; };
    MidiSequence *getSequenceByTrackId(const String &trackId) override { return nullptr; };

};
