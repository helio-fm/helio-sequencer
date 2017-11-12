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

#include "Serializable.h"

class MidiSequence;

class MidiEvent : public Serializable
{
public:

    // TODO! replace ids with int sequences
    using Id = String;
    //using Id = int64;

    // Non-serialized field to be used instead of expensive dynamic casts:
    enum Type { Note = 1, Auto = 2, Annotation = 3, TimeSignature = 4, KeySignature = 5 };
    inline Type getType() const noexcept { return this->type; }
    inline bool isTypeOf(Type val) const noexcept { return this->type == val; }

    MidiEvent(MidiSequence *owner, Type type, float beat);
    ~MidiEvent() override;

    virtual Array<MidiMessage> toMidiMessages() const = 0;

    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    MidiSequence *getSequence() const noexcept;
    int getControllerNumber() const noexcept;
    int getChannel() const noexcept;
    Colour getColour() const noexcept;
    Id getId() const noexcept;
    float getBeat() const noexcept;

    // эта штука используется для сортировки списка событий:
    // сортировка списка событий нужна в двух целях
    // для бинарного поиска конкретного элемента по отсортированному списку
    // и для быстрого экспорта в MidiMessageSequence для плеера
    // это означает, что он должен возвращать 0 (равенство) в случае равенства указателей
    // и это означает, что она всегда должна сортировать список одинаково
    // то есть, если по позиции и ключу они одинаковы, сравниваем по адйишнику
    static int compareElements(const MidiEvent *const first, const MidiEvent *const second)
    {
        if (first == second) { return 0; }
        
        const float diff = first->getBeat() - second->getBeat();
        const int diffResult = (diff > 0.f) - (diff < 0.f);
        if (diffResult != 0) { return diffResult; }
        
        return first->getId().compare(second->getId());
    }

protected:

    MidiSequence *sequence;

    float beat;

    static Id createId() noexcept;

    Id id;

    Type type;
    
};
