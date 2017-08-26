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

#include "MidiTrack.h"
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

    MidiTrack *getAnnotations() const noexcept;
    MidiTrack *getTimeSignatures() const noexcept;


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
    void dispatchPostRemoveEvent(MidiSequence *const layer) override;

    void dispatchAddClip(const Clip &clip) override;
    void dispatchChangeClip(const Clip &oldClip, const Clip &newClip) override;
    void dispatchRemoveClip(const Clip &clip) override;
    void dispatchPostRemoveClip(Pattern *const pattern) override;

    void dispatchChangeTrackProperties(MidiTrack *const track) override;
    void dispatchChangeTrackBeatRange(MidiTrack *const track) override;
    void dispatchChangeTrackContent(MidiTrack *const track) override;

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
    
    Uuid annotationsId;
    Uuid timeSignaturesId;

    ScopedPointer<MidiTrack> annotationsTrack;
    ScopedPointer<MidiTrack> timeSignaturesTrack;

    ScopedPointer<MidiSequence> annotationsSequence;
    ScopedPointer<MidiSequence> timeSignaturesSequence;

    friend class AnnotationsTrack;
    friend class TimeSignaturesTrack;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectTimeline)
};
