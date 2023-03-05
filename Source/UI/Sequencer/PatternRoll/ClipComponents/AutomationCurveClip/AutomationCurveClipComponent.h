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

class RollBase;
class MidiSequence;
class ProjectNode;
class AutomationCurveEventComponent;

#include "ClipComponent.h"
#include "AutomationEvent.h"
#include "ProjectListener.h"

class AutomationCurveClipComponent final : public ClipComponent, public ProjectListener
{
public:

    AutomationCurveClipComponent(ProjectNode &project, MidiSequence *sequence,
        RollBase &roll, const Clip &clip);

    ~AutomationCurveClipComponent() override;

    void insertNewEventAt(const MouseEvent &e);

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDoubleClick(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void resized() override;
    void mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel) override;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeMidiEvent(const MidiEvent &oldEvent,
        const MidiEvent &newEvent) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;

    // TODO! As a part of `automation editors` story
    void onAddClip(const Clip &clip) override {}
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override {}
    void onRemoveClip(const Clip &clip) override {}

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override {}
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override {}
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

protected:

    void removeEventIfPossible(const AutomationEvent &e);

    Rectangle<int> getEventBounds(AutomationCurveEventComponent *event) const;
    Rectangle<int> getEventBounds(float beat, float sequenceLength, double controllerValue) const;

    void getRowsColsByMousePosition(int x, int y, float &targetValue, float &targetBeat) const;
    int getAvailableHeight() const;

#if PLATFORM_DESKTOP
    static constexpr auto eventComponentDiameter = 16.f;
    static constexpr auto helperComponentDiameter = 8.f;
#elif PLATFORM_MOBILE
    static constexpr auto eventComponentDiameter = 24.f;
    static constexpr auto helperComponentDiameter = 20.f;
#endif

    AutomationCurveEventComponent *getPreviousEventComponent(int indexOfSorted) const;
    AutomationCurveEventComponent *getNextEventComponent(int indexOfSorted) const;

    friend class AutomationCurveEventComponent;

private:

    void updateCurveComponent(AutomationCurveEventComponent *);
    void reloadTrack();

    ProjectNode &project;
    WeakReference<MidiSequence> sequence;

    OwnedArray<AutomationCurveEventComponent> eventComponents;
    FlatHashMap<AutomationEvent, AutomationCurveEventComponent *, MidiEventHash> eventsHash;

    AutomationCurveEventComponent *draggingEvent = nullptr;
    bool addNewEventMode = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutomationCurveClipComponent)
};
