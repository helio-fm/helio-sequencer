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

#include "MidiSequence.h"
#include "AnnotationEvent.h"

class AnnotationsSequence final : public MidiSequence
{
public:

    explicit AnnotationsSequence(MidiTrack &track,
        ProjectEventDispatcher &dispatcher) noexcept;

    //===------------------------------------------------------------------===//
    // Import/export
    //===------------------------------------------------------------------===//

    void importMidi(const MidiMessageSequence &sequence, short timeFormat) override;

    //===------------------------------------------------------------------===//
    // Undoable track editing
    //===------------------------------------------------------------------===//
    
    MidiEvent *insert(const AnnotationEvent &annotationToCopy, bool undoable);
    bool remove(const AnnotationEvent &annotation, bool undoable);
    bool change(const AnnotationEvent &annotation,
        const AnnotationEvent &newAnnotation,
        bool undoable);

    bool insertGroup(Array<AnnotationEvent> &annotations, bool undoable);
    bool removeGroup(Array<AnnotationEvent> &annotations, bool undoable);
    bool changeGroup(Array<AnnotationEvent> &annotationsBefore,
        Array<AnnotationEvent> &annotationsAfter,
        bool undoable);

    //===------------------------------------------------------------------===//
    // Callbacks
    //===------------------------------------------------------------------===//

    Function<void(const String &text)> getEventRenameCallback(const AnnotationEvent &event);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnnotationsSequence);
};
