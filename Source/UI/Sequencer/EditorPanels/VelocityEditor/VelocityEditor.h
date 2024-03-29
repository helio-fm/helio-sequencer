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

#include "Clip.h"
#include "Note.h"
#include "ProjectListener.h"
#include "MultiTouchListener.h"
#include "ComponentFader.h"
#include "RollListener.h"
#include "RollEditMode.h"
#include "EditorPanelBase.h"
#include "FineTuningComponentDragger.h"

class RollBase;
class ProjectNode;
class VelocityEditorNoteComponent;
class VelocityHandDrawingHelper;
class FineTuningValueIndicator;
class MultiTouchController;

class VelocityEditor final :
    public EditorPanelBase,
    public MultiTouchListener,
    public RollEditMode::Listener,
    public ProjectListener,
    public AsyncUpdater // triggers batch repaints for children
{
public:

    VelocityEditor(ProjectNode &project, SafePointer<RollBase> roll);
    ~VelocityEditor() override;

    //===------------------------------------------------------------------===//
    // EditorPanelBase
    //===------------------------------------------------------------------===//

    void switchToRoll(SafePointer<RollBase> roll) override;
    void setEditableClip(Optional<Clip> clip) override;
    void setEditableClip(const Clip &selectedClip, const EventFilter &filter) override;
    void setEditableSelection(WeakReference<Lasso> selection) override;
    bool canEditSequence(WeakReference<MidiSequence> sequence) const override;
    Array<EventFilter> getAllEventFilters() const override;
    float getBeatByXPosition(float x) const noexcept override;

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

    bool isMultiTouchEvent(const MouseEvent &e) const noexcept;

    //===------------------------------------------------------------------===//
    // RollEditMode::Listener
    //===------------------------------------------------------------------===//

    void onChangeEditMode(const RollEditMode &mode) override;

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

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &w) override;

private:

    void applyNoteBounds(VelocityEditorNoteComponent *nc);
    void reloadAllTracks();
    void loadTrack(const MidiTrack *const track);

    float projectFirstBeat = 0.f;
    float projectLastBeat = Globals::Defaults::projectLength;

    float rollFirstBeat = 0.f;
    float rollLastBeat = Globals::Defaults::projectLength;

    ProjectNode &project;

    SafePointer<RollBase> roll;

    Optional<Clip> activeClip;

    using SequenceMap = FlatHashMap<Note, UniquePointer<VelocityEditorNoteComponent>, MidiEventHash>;
    using PatternMap = FlatHashMap<Clip, UniquePointer<SequenceMap>, ClipHash>;
    PatternMap patternMap;

private:

    // for fine-tuning a single note or entire selection, if any:

    float fineTuningAnchor = 0.f;
    FineTuningComponentDragger fineTuningDragger;
    UniquePointer<FineTuningValueIndicator> fineTuningIndicator;

    void startFineTuning(VelocityEditorNoteComponent *target, const MouseEvent &e);
    void continueFineTuning(VelocityEditorNoteComponent *target, const MouseEvent &e);
    void endFineTuning(VelocityEditorNoteComponent *target, const MouseEvent &e);

    friend class VelocityEditorNoteComponent;

    // for adjusting a group of notes with hand-drawn curve:

    UniquePointer<VelocityHandDrawingHelper> handDrawingHelper;
    FlatHashMap<Note, float, MidiEventHash> groupDragIntersections;
    Array<Note> groupDragChangesBefore, groupDragChangesAfter;

    UniquePointer<FineTuningValueIndicator> volumeBlendingIndicator;
    void updateVolumeBlendingIndicator(const Point<int> &pos);
    float volumeBlendingAmount = 0.67f;

    // for checkpoints in both ^ modes

    bool editingHadChanges = false;

private:

    RollEditMode getEditMode() const noexcept;
    RollEditMode getSupportedEditMode(const RollEditMode &rollMode) const noexcept;
    bool isDraggingEvent(const MouseEvent &e) const;
    bool isDrawingEvent(const MouseEvent &e) const;

private:

    WeakReference<Lasso> selection;

    UniquePointer<MultiTouchController> multiTouchController;

    Point<int> panningStart;

    ComponentFader fader;

    void applyGroupVolumeChanges();

    void triggerBatchRepaintFor(VelocityEditorNoteComponent *target);
    void handleAsyncUpdate() override;
    Array<WeakReference<Component>> batchRepaintList;

    JUCE_LEAK_DETECTOR(VelocityEditor)
};
