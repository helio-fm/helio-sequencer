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

class PianoRoll;
class PatternRoll;
class HybridRoll;
class RollsSwitchingProxy;
class MidiTrack;
class TrackScroller;
class ProjectNode;
class SequencerSidebarRight;
class SequencerSidebarLeft;
class Origami;
class Headline;
class Clip;

#define SEQUENCER_SIDEBAR_WIDTH (44)
#define SEQUENCER_SIDEBAR_ROW_HEIGHT (38)

class SequencerLayout final :
    public Component,
    public Serializable,
    public FileDragAndDropTarget
{
public:

    explicit SequencerLayout(ProjectNode &parentProject);
    ~SequencerLayout() override;

    void showPatternEditor();
    void showLinearEditor(WeakReference<MidiTrack> activeTrack);
    void setEditableScope(WeakReference<MidiTrack> activeTrack,
        const Clip &clip, bool zoomToArea);
    
    HybridRoll *getRoll() const;

    //===------------------------------------------------------------------===//
    // FileDragAndDropTarget
    //===------------------------------------------------------------------===//

    void filesDropped(const StringArray &filenames, int mouseX, int mouseY) override;
    bool isInterestedInFileDrag(const StringArray &files) override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override {}
    void resized() override;
    void handleCommandMessage(int commandId) override;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    void proceedToRenderDialog(const String &extension);

private:

    ProjectNode &project;
    
    ScopedPointer<Viewport> pianoViewport;
    ScopedPointer<Viewport> patternViewport;
    ScopedPointer<TrackScroller> scroller;

    ScopedPointer<PianoRoll> pianoRoll;
    ScopedPointer<PatternRoll> patternRoll;
    ScopedPointer<RollsSwitchingProxy> rollContainer;

    ScopedPointer<SequencerSidebarLeft> rollNavSidebar;
    ScopedPointer<SequencerSidebarRight> rollToolsSidebar;

    ScopedPointer<Origami> sequencerLayout; // all editors combined with sidebars

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequencerLayout);
};
