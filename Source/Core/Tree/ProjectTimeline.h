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
#include "ProjectEventDispatcher.h"
#include "Serializable.h"

class ProjectTimeline :
    public ProjectEventDispatcher,
    public VCS::TrackedItem,
    public Serializable
{
public:

    ProjectTimeline(ProjectTreeItem &parentProject, String trackName);

    ~ProjectTimeline() override;

    inline MidiLayer *getAnnotations() const noexcept
    { return this->annotations; }

    inline MidiLayer *getTimeSignatures() const noexcept
    { return this->timeSignatures; }


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
    // ProjectEventDispatcher
    //===------------------------------------------------------------------===//
    
    void dispatchChangeEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
    void dispatchAddEvent(const MidiEvent &event) override;
    void dispatchRemoveEvent(const MidiEvent &event) override;
	void dispatchPostRemoveEvent(MidiLayer *const layer) override;

    void dispatchReloadLayer(MidiLayer *const layer) override;
    void dispatchChangeLayerBeatRange() override;

	void dispatchAddClip(const Clip &clip) override;
	void dispatchChangeClip(const Clip &oldClip, const Clip &newClip) override;
	void dispatchRemoveClip(const Clip &clip) override;
	void dispatchPostRemoveClip(Pattern *const pattern) override;

	void dispatchReloadPattern(Pattern *const pattern) override;
	void dispatchChangePatternBeatRange() override;

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

    XmlElement *serializeTimeSignaturesDelta() const;

    void resetTimeSignaturesDelta(const XmlElement *state);
    

private:

    ScopedPointer<VCS::DiffLogic> vcsDiffLogic;

    OwnedArray<VCS::Delta> deltas;
    
    ProjectTreeItem &project;
    
    String name;
    
    ScopedPointer<MidiLayer> annotations;

    ScopedPointer<MidiLayer> timeSignatures;

};
