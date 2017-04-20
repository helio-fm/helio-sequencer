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

class MidiRoll;
class ProjectTreeItem;
class TrackStartIndicator;
class TrackEndIndicator;

template< typename T >
class AnnotationsTrackMap :
    public Component,
    public ProjectListener
{
public:

    AnnotationsTrackMap(ProjectTreeItem &parentProject, MidiRoll &parentRoll);

    ~AnnotationsTrackMap() override;

    void alignAnnotationComponent(T *nc);


    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;


    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;

    void onEventAdded(const MidiEvent &event) override;

    void onEventRemoved(const MidiEvent &event) override;

    void onLayerChanged(const MidiLayer *layer) override;

    void onLayerAdded(const MidiLayer *layer) override;

    void onLayerRemoved(const MidiLayer *layer) override;

    void onProjectBeatRangeChanged(float firstBeat, float lastBeat) override;

    
    //===------------------------------------------------------------------===//
    // Stuff for children
    //===------------------------------------------------------------------===//

    void onAnnotationMoved(T *nc);
    
    void onAnnotationTapped(T *nc);
    
    void showContextMenuFor(T *nc);

    void alternateActionFor(T *nc);

    float getBeatByXPosition(int x) const;
    
private:
    
    void reloadTrackMap();
    void applyAnnotationBounds(T *nc, T *nextOne = nullptr);
    
    T *getPreviousEventComponent(int indexOfSorted) const;
    T *getNextEventComponent(int indexOfSorted) const;
    
    void updateTrackRangeIndicatorsAnchors();
    
private:
    
    float projectFirstBeat;
    float projectLastBeat;

    float rollFirstBeat;
    float rollLastBeat;
    
    MidiRoll &roll;
    ProjectTreeItem &project;
    
    ScopedPointer<TrackStartIndicator> trackStartIndicator;
    ScopedPointer<TrackEndIndicator> trackEndIndicator;
    
    ComponentAnimator animator;

    OwnedArray<T> annotationComponents;
    HashMap<AnnotationEvent, T *, AnnotationEventHashFunction> annotationsHash;
    
};

