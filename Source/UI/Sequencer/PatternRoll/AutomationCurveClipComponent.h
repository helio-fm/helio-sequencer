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
class MidiSequence;
class ProjectNode;
class AutomationCurveEventComponent;

#include "ClipComponent.h"
#include "AutomationEvent.h"
#include "AutomationEditorBase.h"
#include "ProjectListener.h"

class AutomationCurveClipComponent final :
    public AutomationEditorBase,
    public ClipComponent,
    public ProjectListener
{
public:

    AutomationCurveClipComponent(ProjectNode &project,
        MidiSequence *sequence, RollBase &roll, const Clip &clip);

    ~AutomationCurveClipComponent() override;

    void insertNewEventAt(const MouseEvent &e);

    //===------------------------------------------------------------------===//
    // AutomationEditorBase
    //===------------------------------------------------------------------===//

    Colour getColour(const AutomationEvent &event) const override;
    Rectangle<float> getEventBounds(const AutomationEvent &event, const Clip &clip) const override;
    void getBeatValueByPosition(int x, int y, const Clip &clip, float &value, float &beat) const override;
    float getBeatByPosition(int x, const Clip &clip) const override;
    bool hasEditMode(RollEditMode::Mode mode) const noexcept override;

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

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;

    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

protected:

    Rectangle<float> getEventBounds(float beat, float sequenceLength, double controllerValue) const;

    static constexpr auto eventComponentDiameter = 16.f;

    friend class EventComponentBase;

private:

    void reloadTrack();

    ProjectNode &project;
    WeakReference<MidiSequence> sequence;

    OwnedArray<EventComponentBase> eventComponents;
    FlatHashMap<AutomationEvent, EventComponentBase *, MidiEventHash> eventsMap;

    AutomationCurveEventComponent *draggingEvent = nullptr;
    bool dragNewEventMode = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutomationCurveClipComponent)
};
