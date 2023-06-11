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

#include "AutomationEditorBase.h"
#include "ProjectListener.h"
#include "ComponentFader.h"
#include "HelperRectangle.h"
#include "RollListener.h"

class RollBase;
class TrackMap;
class ProjectNode;

class AutomationEditor final :
    public Component,
    public AutomationEditorBase,
    public ProjectListener,
    public AsyncUpdater, // triggers batch repaints for children
    public ChangeListener // subscribes on parent roll's lasso changes
{
public:

    AutomationEditor(ProjectNode &parentProject, RollBase &parentRoll);
    ~AutomationEditor() override;

    //===------------------------------------------------------------------===//
    // AutomationEditorBase
    //===------------------------------------------------------------------===//

    const Colour &getColour(const AutomationEvent &event) const override;
    Rectangle<float> getEventBounds(const AutomationEvent &event, const Clip &clip) const override;
    void getBeatValueByPosition(int x, int y, const Clip &clip, float &value, float &beat) const override;
    float getBeatByPosition(int x, const Clip &clip) const override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &w) override;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onAddMidiEvent(const MidiEvent &event) override;
    void onChangeMidiEvent(const MidiEvent &e1, const MidiEvent &e2) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;

    void onAddClip(const Clip &clip) override;
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override;
    void onRemoveClip(const Clip &clip) override;

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewEditableScope(MidiTrack *const, const Clip &clip, bool) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

private:

    EventComponentBase *getPreviousEventComponent(int indexOfSorted) const;
    EventComponentBase *getNextEventComponent(int indexOfSorted) const;

    friend class EventComponentBase;

private:

    Rectangle<float> getEventBounds(float beat, float sequenceLength, double controllerValue) const;

    void changeListenerCallback(ChangeBroadcaster *source) override;

    void applyEventBounds(const AutomationEvent &event);
    void reloadTrackMap();
    void loadTrack(const MidiTrack *const track);

    float projectFirstBeat = 0.f;
    float projectLastBeat = Globals::Defaults::projectLength;

    float rollFirstBeat = 0.f;
    float rollLastBeat = Globals::Defaults::projectLength;

    RollBase &roll;
    ProjectNode &project;

    Clip activeClip;

    using SequenceMap = FlatHashMap<Note, UniquePointer<EventComponentBase>, MidiEventHash>;
    using PatternMap = FlatHashMap<Clip, UniquePointer<SequenceMap>, ClipHash>;
    PatternMap patternMap;

    ComponentFader fader;

    void triggerBatchRepaintFor(Component *target);
    void handleAsyncUpdate() override;
    Array<WeakReference<Component>> batchRepaintList;

    JUCE_LEAK_DETECTOR(AutomationEditor)
};
