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

#include "PianoTrackDiffLogic.h"
#include "MidiTrackNode.h"

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
    ValueTree getDeltaData(int deltaIndex) const override;
    VCS::DiffLogic *getDiffLogic() const override;
    void resetStateTo(const VCS::TrackedItem &newState) override;
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;

    //===------------------------------------------------------------------===//
    // Deltas
    //===------------------------------------------------------------------===//

    ValueTree serializePathDelta() const;
    ValueTree serializeMuteDelta() const;
    ValueTree serializeColourDelta() const;
    ValueTree serializeInstrumentDelta() const;
    ValueTree serializeEventsDelta() const;

    void resetPathDelta(const ValueTree &state);
    void resetMuteDelta(const ValueTree &state);
    void resetColourDelta(const ValueTree &state);
    void resetInstrumentDelta(const ValueTree &state);
    void resetEventsDelta(const ValueTree &state);

private:

    ScopedPointer<VCS::PianoTrackDiffLogic> vcsDiffLogic;
    OwnedArray<VCS::Delta> deltas;

};
