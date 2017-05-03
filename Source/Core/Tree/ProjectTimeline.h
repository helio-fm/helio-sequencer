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

class ProjectTreeItem;

#include "ProjectTimelineDiffLogic.h"
#include "MidiLayerOwner.h"
#include "Serializable.h"

class ProjectTimeline :
    public MidiLayerOwner,
    public VCS::TrackedItem,
    public Serializable
{
public:

    ProjectTimeline(ProjectTreeItem &parentProject, String trackName);

    ~ProjectTimeline() override;
    
    inline MidiLayer *getLayer() const { return this->layer; }
    

    //===------------------------------------------------------------------===//
    // VCS::TrackedItem
    //===------------------------------------------------------------------===//

    String getVCSName() const override;

    int getNumDeltas() const override;

    VCS::Delta *getDelta(int index) const override;

    XmlElement *createDeltaDataFor(int index) const override;

    VCS::DiffLogic *getDiffLogic() const override;

    void resetStateTo(const VCS::TrackedItem &newState) override;
    
    
    //===------------------------------------------------------------------===//
    // MidiLayerOwner
    //===------------------------------------------------------------------===//
    
    Transport *getTransport() const override;
    
    String getXPath() const override;
    
    void setXPath(const String &path) override;
    
    void onEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
    
    void onEventAdded(const MidiEvent &event) override;
    
    void onEventRemoved(const MidiEvent &event) override;
    
    void onLayerChanged(const MidiLayer *layer) override;
    
    void onBeatRangeChanged() override;

    ProjectTreeItem *getProject() const override;
    
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    void reset() override;
    
    XmlElement *serialize() const override;

    void deserialize(const XmlElement &xml) override;


    //===------------------------------------------------------------------===//
    // Deltas
    //===------------------------------------------------------------------===//

    XmlElement *serializeAnnotationsDelta() const;

    void resetAnnotationsDelta(const XmlElement *state);

private:

    ScopedPointer<VCS::DiffLogic> vcsDiffLogic;

    OwnedArray<VCS::Delta> deltas;
    
    ProjectTreeItem &project;
    
    String name;
    
    ScopedPointer<MidiLayer> layer;

};
