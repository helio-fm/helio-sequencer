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

#include "MidiEvent.h"

#define KEY_C5 60

class Note : public MidiEvent
{
public:

    typedef int Key;

    Note();

    // конструктор копирвания и оператор копирования имеют разное поведение
    // первый копирует указатель на слой, чтоб можно было создать точную копию ноты (см массивы)
    // оператор копирования только заполняет параметры, не трогая указатель - это нужно для undo/redo операций
    Note(const Note &other);
    explicit Note(MidiSequence *owner,
         int keyVal = 0, float beatVal = 0.f,
         float lengthVal = 1.f, float velocityVal = 1.f);

    Note(MidiSequence *newOwner, const Note &parametersToCopy);
    ~Note() override {}

    Array<MidiMessage> toMidiMessages() const override;
    
    Note copyWithNewId(MidiSequence *newOwner = nullptr) const;
    Note withBeat(float newBeat) const;
    Note withKeyBeat(int newKey, float newBeat) const;
    Note withDeltaBeat(float deltaPosition) const;
    Note withDeltaKey(int deltaKey) const;
    Note withLength(float newLength) const;
    Note withDeltaLength(float deltaLength) const;
    Note withVelocity(float newVelocity) const;
    Note withParameters(const XmlElement &xml) const;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    int getKey() const noexcept;
    float getLength() const noexcept;
    float getVelocity() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Helpers
    //===------------------------------------------------------------------===//
    
    Note &operator=(const Note &right);

    friend inline bool operator==(const Note &lhs, const Note &rhs)
    {
        return (&lhs == &rhs || lhs.id == rhs.id);
    }

    int hashCode() const noexcept;

    static int compareElements(const MidiEvent *const first, const MidiEvent *const second);
    static int compareElements(Note *const first, Note *const second);
    static int compareElements(const Note &first, const Note &second);

protected:

    Key key;

    float length, velocity;

private:

    JUCE_LEAK_DETECTOR(Note);

};


class NoteHashFunction
{
public:
    
    static int generateHash(const Note &key, const int upperLimit) noexcept
    {
        return static_cast<int>((static_cast<uint32>( key.hashCode())) % static_cast<uint32>( upperLimit));
    }
};
