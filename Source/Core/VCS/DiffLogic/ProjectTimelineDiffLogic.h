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

class MidiEvent;
class MidiLayer;
class ProjectTimeline;

#include "Diff.h"
#include "DiffLogic.h"

namespace VCS
{
    class ProjectTimelineDiffLogic : public DiffLogic
    {
    public:

        explicit ProjectTimelineDiffLogic(TrackedItem &targetItem);

        ~ProjectTimelineDiffLogic() override;

        //===------------------------------------------------------------------===//
        // DiffLogic
        //

        const String getType() const override;

        void resetStateTo(const TrackedItem &newState) override;

        Diff *createDiff(const TrackedItem &initialState) const override;

        Diff *createMergedItem(const TrackedItem &initialState) const override;

    private:

        XmlElement *mergeAnnotationsAdded(const XmlElement *state, const XmlElement *changes) const;

        XmlElement *mergeAnnotationsRemoved(const XmlElement *state, const XmlElement *changes) const;

        XmlElement *mergeAnnotationsChanged(const XmlElement *state, const XmlElement *changes) const;

        XmlElement *mergeTimeSignaturesAdded(const XmlElement *state, const XmlElement *changes) const;
        
        XmlElement *mergeTimeSignaturesRemoved(const XmlElement *state, const XmlElement *changes) const;
        
        XmlElement *mergeTimeSignaturesChanged(const XmlElement *state, const XmlElement *changes) const;

    private:

        Array<NewSerializedDelta> createAnnotationsDiffs(const XmlElement *state, const XmlElement *changes) const;

        Array<NewSerializedDelta> createTimeSignaturesDiffs(const XmlElement *state, const XmlElement *changes) const;

    private:

        void deserializeChanges(MidiLayer &layer,
                                const XmlElement *state,
                                const XmlElement *changes,
                                OwnedArray<MidiEvent> &stateNotes,
                                OwnedArray<MidiEvent> &changesNotes) const;

        NewSerializedDelta serializeChanges(Array<const MidiEvent *> changes,
                                            const String &description,
                                            int64 numChanges,
                                            const String &deltaType) const;

        XmlElement *serializeLayer(Array<const MidiEvent *> changes,
                                   const String &tag) const;

        bool checkIfDeltaIsAnnotationType(const Delta *delta) const;

        bool checkIfDeltaIsTimeSignatureType(const Delta *delta) const;

    };
}  // namespace VCS
