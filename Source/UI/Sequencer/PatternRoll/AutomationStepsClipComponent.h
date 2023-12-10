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

#include "ClipComponent.h"
#include "AutomationEvent.h"
#include "AutomationEditorBase.h"
#include "ProjectListener.h"

class MidiSequence;
class RollBase;
class ProjectNode;
class AutomationStepEventComponent;
class AutomationStepEventsConnector;

class AutomationStepsClipComponent final :
    public AutomationEditorBase,
    public ClipComponent,
    public ProjectListener
{
public:

    AutomationStepsClipComponent(ProjectNode &project,
        MidiSequence *sequence, RollBase &roll, const Clip &clip);

    ~AutomationStepsClipComponent() override;

    //===------------------------------------------------------------------===//
    // AutomationEditorBase
    //===------------------------------------------------------------------===//

    Colour getColour(const AutomationEvent &event) const override;
    Rectangle<float> getEventBounds(const AutomationEvent &event, const Clip &clip) const override;
    void getBeatValueByPosition(int x, int y, const Clip &clip, float &value, float &beat) const override;
    float getBeatByPosition(int x, const Clip &clip) const override;
    bool hasEditMode(RollEditMode::Mode mode) const noexcept override;
    bool isMultiTouchEvent(const MouseEvent &e) const noexcept override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;
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

    void insertNewEventAt(const MouseEvent &e, bool shouldAddPairedEvents);

    Rectangle<float> getEventBounds(float targetBeat, float sequenceLength, bool isPedalDown) const;

    friend class EventComponentBase;

private:

    void reloadTrack();

    ProjectNode &project;
    WeakReference<MidiSequence> sequence;

    OwnedArray<EventComponentBase> eventComponents;
    FlatHashMap<AutomationEvent, EventComponentBase *, MidiEventHash> eventsMap;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutomationStepsClipComponent)
};
