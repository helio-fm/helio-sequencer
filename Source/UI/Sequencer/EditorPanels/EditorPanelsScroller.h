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

#include "RollListener.h"
#include "ProjectListener.h"
#include "EditorPanelBase.h"

class EditorPanelsScroller final :
    public Component,
    public ProjectListener,
    public RollListener,
    public EditorPanelBase::Listener,
    public ChangeListener, // subscribes on the parent roll's lasso changes
    private AsyncUpdater, // triggers batch move/resize events for children
    private Timer // optionally animates transitions between rolls
{
public:

    EditorPanelsScroller(ProjectNode &project,
        SafePointer<RollBase> roll, SafePointer<EditorPanelsSwitcher> panelsSwitcher);

    ~EditorPanelsScroller() override;

    template <typename T, typename... Args> inline
    void addOwnedEditorPanel(Args &&... args)
    {
        auto *newTrackMap = this->editorPanels.add(new T(std::forward<Args>(args)...));
        this->addAndMakeVisible(newTrackMap);
        newTrackMap->addListener(this);
        newTrackMap->toFront(false);
        this->updateSwitcher();
    }

    void switchToRoll(SafePointer<RollBase> roll);

    void setAnimationsEnabled(bool shouldBeEnabled)
    {
        this->animationsEnabled = shouldBeEnabled;
    }

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;
    void paint(Graphics &g) override;
    void mouseWheelMove(const MouseEvent &event, const MouseWheelDetails &wheel) override;

    //===------------------------------------------------------------------===//
    // RollListener
    //===------------------------------------------------------------------===//

    void onMidiRollMoved(RollBase *targetRoll) override;
    void onMidiRollResized(RollBase *targetRoll) override;

    //===------------------------------------------------------------------===//
    // EditorPanelBase::Listener
    //===------------------------------------------------------------------===//

    void onUpdateEventFilters() override;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeClip(const Clip &clip, const Clip &newClip) override;
    void onChangeViewEditableScope(MidiTrack *const, const Clip &, bool) override;

private:

    ProjectNode &project;

    SafePointer<RollBase> roll;

    Clip activeClip;

    void changeListenerCallback(ChangeBroadcaster *source) override;
    
    void handleAsyncUpdate() override;

    void timerCallback() override;
    Rectangle<float> panelsBoundsAnimationAnchor;
    bool animationsEnabled = true;

    Rectangle<int> getEditorPanelBounds() const noexcept;

    OwnedArray<EditorPanelBase> editorPanels;

    const Colour borderColourLight =
        findDefaultColour(ColourIDs::TrackScroller::borderLineLight);

    const Colour borderColourDark =
        findDefaultColour(ColourIDs::TrackScroller::borderLineDark);

private:

    enum class EditorPanelSelectionMode
    {
        Manual, // let user decide what to edit, e.g. velocity, tempo, pedals
        Automatic // based on the which clip is currently selected
    };

    EditorPanelSelectionMode editorPanelSelectionMode =
        EditorPanelSelectionMode::Manual;

    int selectedEditorPanelIndex = 0;
    EditorPanelBase::EventFilter selectedEventFilter;

    // owned and positioned by SequencerLayout:
    SafePointer<EditorPanelsSwitcher> editorPanelsSwitcher;

    void updateSwitcher();

    EditorPanelBase *showEditorPanel(int panelIndex);
    EditorPanelBase *showEditorPanelForClip(const Clip &clip);

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditorPanelsScroller)
};
