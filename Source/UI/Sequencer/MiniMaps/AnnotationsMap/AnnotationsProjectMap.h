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

#include "AnnotationEvent.h"
#include "ProjectListener.h"

class HybridRoll;
class ProjectNode;
class AnnotationComponent;

class AnnotationsProjectMap final :
    public Component,
    public ProjectListener
{
public:

    enum class Type : int8 { Large, Small };

    AnnotationsProjectMap(ProjectNode &parentProject, HybridRoll &parentRoll, Type type);
    ~AnnotationsProjectMap() override;

    void alignAnnotationComponent(AnnotationComponent *nc);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;

    // Assuming the timeline has no patterns/clips:
    void onAddClip(const Clip &clip) override {}
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override {}
    void onRemoveClip(const Clip &clip) override {}

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks) override;


    //===------------------------------------------------------------------===//
    // Stuff for children
    //===------------------------------------------------------------------===//

    void onAnnotationMoved(AnnotationComponent *nc);
    void onAnnotationTapped(AnnotationComponent *nc);
    void showContextMenuFor(AnnotationComponent *nc);
    void alternateActionFor(AnnotationComponent *nc);

    float getBeatByXPosition(int x) const;
    
private:
    
    void reloadTrackMap();
    void applyAnnotationBounds(AnnotationComponent *nc, AnnotationComponent *nextOne = nullptr);
    
    AnnotationComponent *getPreviousEventComponent(int indexOfSorted) const;
    AnnotationComponent *getNextEventComponent(int indexOfSorted) const;
    
private:
    
    float projectFirstBeat = 0.f;
    float projectLastBeat = Globals::Defaults::projectLength;

    float rollFirstBeat = 0.f;
    float rollLastBeat = Globals::Defaults::projectLength;
    
    HybridRoll &roll;
    ProjectNode &project;
    
    ComponentAnimator animator;

    const Type type;
    AnnotationComponent *createComponent(const AnnotationEvent &event);

    OwnedArray<AnnotationComponent> annotationComponents;
    FlatHashMap<AnnotationEvent, AnnotationComponent *, MidiEventHash> annotationsHash;
    
};
