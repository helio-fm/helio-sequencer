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

#include "AutomationTrackDiffLogic.h"
#include "MidiTrackTreeItem.h"

class AutomationTrackTreeItem : public MidiTrackTreeItem
{
public:

    explicit AutomationTrackTreeItem(const String &name);

    Image getIcon() const override;
    void paintItem(Graphics &g, int width, int height) override;

    //===------------------------------------------------------------------===//
    // VCS::TrackedItem
    //===------------------------------------------------------------------===//

    int getNumDeltas() const override;
    VCS::Delta *getDelta(int index) const override;
    XmlElement *createDeltaDataFor(int index) const override;
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

    XmlElement *serializePathDelta() const;
    XmlElement *serializeMuteDelta() const;
    XmlElement *serializeColourDelta() const;
    XmlElement *serializeInstrumentDelta() const;
    XmlElement *serializeControllerDelta() const;
    XmlElement *serializeEventsDelta() const;

    void resetPathDelta(const XmlElement *state);
    void resetMuteDelta(const XmlElement *state);
    void resetColourDelta(const XmlElement *state);
    void resetInstrumentDelta(const XmlElement *state);
    void resetControllerDelta(const XmlElement *state);
    void resetEventsDelta(const XmlElement *state);

private:

    ScopedPointer<VCS::AutomationTrackDiffLogic> vcsDiffLogic;
    OwnedArray<VCS::Delta> deltas;

};
