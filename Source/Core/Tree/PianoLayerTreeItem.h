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

#include "PianoLayerDiffLogic.h"
#include "LayerTreeItem.h"

class PianoLayerTreeItem : public LayerTreeItem
{
public:

    explicit PianoLayerTreeItem(const String &name);

    Image getIcon() const override;

    static void selectAllPianoSiblings(PianoLayerTreeItem *layerItem);

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

    XmlElement *serialize() const override;

    void deserialize(const XmlElement &xml) override;


    //===------------------------------------------------------------------===//
    // Deltas
    //===------------------------------------------------------------------===//

    XmlElement *serializePathDelta() const;

    XmlElement *serializeMuteDelta() const;

    XmlElement *serializeColourDelta() const;

    XmlElement *serializeInstrumentDelta() const;

    XmlElement *serializeEventsDelta() const;

    void resetPathDelta(const XmlElement *state);

    void resetMuteDelta(const XmlElement *state);

    void resetColourDelta(const XmlElement *state);

    void resetInstrumentDelta(const XmlElement *state);

    void resetEventsDelta(const XmlElement *state);


private:

    ScopedPointer<VCS::PianoLayerDiffLogic> vcsDiffLogic;

    OwnedArray<VCS::Delta> deltas;

};
