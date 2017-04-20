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

class AnnotationEvent : public MidiEvent
{
public:
    
    enum Type
    {
        TEXT_EVENT = 0,
        TIME_SIGNATURE_EVENT = 1
    };

    AnnotationEvent();

    AnnotationEvent(const AnnotationEvent &other);

    explicit AnnotationEvent(MidiLayer *owner,
                    float newBeat = 0.f,
                    String newDescription = "",
                    const Colour &newColour = Colours::white);

    ~AnnotationEvent() override;
    

	Array<MidiMessage> getSequence() const override;

    
    AnnotationEvent copyWithNewId() const;
    
    AnnotationEvent withDeltaBeat(float beatOffset) const;
    
    AnnotationEvent withBeat(float newBeat) const;

    AnnotationEvent withDescription(const String &newDescription) const;
    
    AnnotationEvent withColour(const Colour &newColour) const;

    AnnotationEvent withParameters(const XmlElement &xml) const;


    //===------------------------------------------------------------------===//
    // Accessors
    //===------------------------------------------------------------------===//

    String getDescription() const noexcept;

    Colour getColour() const noexcept;


    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;

    void deserialize(const XmlElement &xml) override;

    void reset() override;


    //===------------------------------------------------------------------===//
    // Stuff for hashtables
    //===------------------------------------------------------------------===//

    AnnotationEvent &operator=(const AnnotationEvent &right);

    friend inline bool operator==(const AnnotationEvent &lhs, const AnnotationEvent &rhs)
    {
        return (lhs.getID() == rhs.getID());
    }

    int hashCode() const noexcept;


protected:

    String description;
    
    Colour colour;

private:

    JUCE_LEAK_DETECTOR(AnnotationEvent);

};


class AnnotationEventHashFunction
{
public:
    static int generateHash(const AnnotationEvent key, const int upperLimit) noexcept
    {
        return static_cast<int>((static_cast<uint32>( key.hashCode())) % static_cast<uint32>( upperLimit));
    }
};
