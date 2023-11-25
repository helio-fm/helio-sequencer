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

#include "AutomationEvent.h"
#include "AutomationEditorBase.h"
#include "ProjectListener.h"
#include "MultiTouchListener.h"
#include "ComponentFader.h"
#include "RollListener.h"
#include "EditorPanelBase.h"
#include "Lasso.h"

class RollBase;
class ProjectNode;
class MultiTouchController;

class AutomationEditor final :
    public EditorPanelBase,
    public AutomationEditorBase,
    public MultiTouchListener,
    public ProjectListener
{
public:

    AutomationEditor(ProjectNode &project, SafePointer<RollBase> roll);
    ~AutomationEditor() override;

    //===------------------------------------------------------------------===//
    // AutomationEditorBase
    //===------------------------------------------------------------------===//

    Colour getColour(const AutomationEvent &event) const override;
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
    // EditorPanelBase
    //===------------------------------------------------------------------===//

    void switchToRoll(SafePointer<RollBase> roll) override;
    void setEditableClip(Optional<Clip> clip) override;
    void setEditableClip(const Clip &selectedClip, const EventFilter &filter) override;
    void setEditableSelection(WeakReference<Lasso> selection) override;
    bool canEditSequence(WeakReference<MidiSequence> sequence) const override;
    Array<EventFilter> getAllEventFilters() const override;

    //===------------------------------------------------------------------===//
    // MultiTouchListener
    //===------------------------------------------------------------------===//

    void multiTouchStartZooming() override;
    void multiTouchContinueZooming(
            const Rectangle<float> &relativePosition,
            const Rectangle<float> &relativePositionAnchor,
            const Rectangle<float> &absolutePositionAnchor) override;
    void multiTouchEndZooming(const MouseEvent &anchorEvent) override;

    Point<float> getMultiTouchRelativeAnchor(const MouseEvent &e) override;
    Point<float> getMultiTouchAbsoluteAnchor(const MouseEvent &e) override;

    bool hasMultiTouch(const MouseEvent &e) const;

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
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

private:

    ProjectNode &project;

    SafePointer<RollBase> roll;

    Optional<Clip> activeClip;

    float projectFirstBeat = 0.f;
    float projectLastBeat = Globals::Defaults::projectLength;

    float rollFirstBeat = 0.f;
    float rollLastBeat = Globals::Defaults::projectLength;

    struct SequenceMap final
    {
        // owned components, sorted by beat
        OwnedArray<EventComponentBase> sortedComponents;
        // component map for faster access
        FlatHashMap<AutomationEvent, EventComponentBase *, MidiEventHash> eventsMap;
    };

    using PatternMap = FlatHashMap<Clip, UniquePointer<SequenceMap>, ClipHash>;
    PatternMap patternMap;

private:

    EventComponentBase *createCurveEventComponent(const AutomationEvent &event, const Clip &clip);
    EventComponentBase *createOnOffEventComponent(const AutomationEvent &event, const Clip &clip);

    friend class EventComponentBase;

#if PLATFORM_DESKTOP
    static constexpr auto curveEventComponentDiameter = 20.f;
#elif PLATFORM_MOBILE
    static constexpr auto curveEventComponentDiameter = 24.f;
#endif

    Rectangle<float> getCurveEventBounds(float beat,
        float sequenceLength, double controllerValue) const;

    Rectangle<float> getOnOffEventBounds(float beat,
        float sequenceLength, bool isPedalDown) const;

private:

    UniquePointer<MultiTouchController> multiTouchController;

    Point<int> dragStartPoint;

    ComponentFader fader;

    void applyEventBounds(EventComponentBase *c);
    void applyEventsBounds(SequenceMap *map);

    void reloadTrackMap();
    void loadTrack(const MidiTrack *const track);

    JUCE_LEAK_DETECTOR(AutomationEditor)
};
