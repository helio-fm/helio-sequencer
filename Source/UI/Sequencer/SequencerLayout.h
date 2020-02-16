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
class ProjectNode;
class MidiTrack;
class ProjectMapScroller;
class LevelsMapScroller;
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

    static constexpr int getPianoMapHeight() { return 80; }
    static constexpr int getLevelsMapHeight() { return 128; }

    void showPatternEditor();
    void showLinearEditor(WeakReference<MidiTrack> activeTrack);
    void switchMiniMaps();

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

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    void proceedToRenderDialog(const String &extension);

private:

    ProjectNode &project;
    
    UniquePointer<Viewport> pianoViewport;
    UniquePointer<Viewport> patternViewport;

    UniquePointer<ProjectMapScroller> mapScroller;
    UniquePointer<LevelsMapScroller> levelsScroller;
    UniquePointer<Component> scrollerShadow;

    UniquePointer<PianoRoll> pianoRoll;
    UniquePointer<PatternRoll> patternRoll;
    UniquePointer<RollsSwitchingProxy> rollContainer;

    UniquePointer<SequencerSidebarLeft> rollNavSidebar;
    UniquePointer<SequencerSidebarRight> rollToolsSidebar;

    UniquePointer<Origami> sequencerLayout; // all editors combined with sidebars

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SequencerLayout);
};
