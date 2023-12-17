/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class RollBase;
class ProjectNode;
class TimeSignatureComponent;

#include "ProjectListener.h"
#include "TimeSignatureEvent.h"
#include "TimeSignaturesAggregator.h"
#include "ProjectMapsScroller.h"

class TimeSignaturesProjectMap final :
    public ProjectMapsScroller::ScrolledComponent,
    public TimeSignaturesAggregator::Listener,
    public ProjectListener
{
public:

    enum class Type : int8 { Large, Small };

    TimeSignaturesProjectMap(ProjectNode &parentProject,
        SafePointer<RollBase> roll, Type type);

    ~TimeSignaturesProjectMap() override;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override;

    //===------------------------------------------------------------------===//
    // TimeSignaturesAggregator::Listener
    //===------------------------------------------------------------------===//

    void onTimeSignaturesUpdated() override;

    //===------------------------------------------------------------------===//
    // Stuff for children
    //===------------------------------------------------------------------===//

    void onTimeSignatureTapped(TimeSignatureComponent *nc);
    void showDialogFor(TimeSignatureComponent *nc);
    void alternateActionFor(TimeSignatureComponent *nc);

    float getBeatByXPosition(int x) const;
    void applyTimeSignatureBounds(TimeSignatureComponent *c,
        TimeSignatureComponent *nextOne = nullptr);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;

private:
    
    void reloadTrackMap();
    
    float projectFirstBeat = 0.f;
    float projectLastBeat = Globals::Defaults::projectLength;

    float rollFirstBeat = 0.f;
    float rollLastBeat = Globals::Defaults::projectLength;

    ProjectNode &project;
    
    ComponentAnimator animator;

    const Type type;
    TimeSignatureComponent *createComponent();

    OwnedArray<TimeSignatureComponent> timeSignatureComponents;
    FlatHashMap<TimeSignatureEvent, TimeSignatureComponent *, MidiEventHash> timeSignaturesMap;
};
