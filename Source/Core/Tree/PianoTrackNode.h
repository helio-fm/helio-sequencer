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

#include "MidiTrackNode.h"
#include "PianoTrackDiffLogic.h"

class PianoTrackNode final : public MidiTrackNode
{
public:

    explicit PianoTrackNode(const String &name);

    Image getIcon() const noexcept override;

    //===------------------------------------------------------------------===//
    // VCS::TrackedItem
    //===------------------------------------------------------------------===//

    int getNumDeltas() const override;
    VCS::Delta *getDelta(int index) const override;
    SerializedData getDeltaData(int deltaIndex) const override;
    VCS::DiffLogic *getDiffLogic() const override;
    void resetStateTo(const VCS::TrackedItem &newState) override;
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;

    //===------------------------------------------------------------------===//
    // Deltas
    //===------------------------------------------------------------------===//

    SerializedData serializePathDelta() const;
    SerializedData serializeColourDelta() const;
    SerializedData serializeInstrumentDelta() const;
    SerializedData serializeEventsDelta() const;

    void resetPathDelta(const SerializedData &state);
    void resetColourDelta(const SerializedData &state);
    void resetInstrumentDelta(const SerializedData &state);
    void resetEventsDelta(const SerializedData &state);

private:

    //TimeSignatureEvent timeSignatureOverride;

    UniquePointer<VCS::PianoTrackDiffLogic> vcsDiffLogic;
    OwnedArray<VCS::Delta> deltas;

};
