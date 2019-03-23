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

#define PATTERN_ROLL_CLIP_HEIGHT 48
#define PATTERN_ROLL_TRACK_HEADER_HEIGHT 3

class CutPointMark;
class ClipComponent;
class PatternRollSelectionMenuManager;

#include "HelioTheme.h"
#include "HybridRoll.h"
#include "MidiTrack.h"
#include "Pattern.h"
#include "Clip.h"

class PatternRoll final : public HybridRoll
{
public:

    PatternRoll(ProjectNode &parentProject,
        Viewport &viewportRef,
        WeakReference<AudioMonitor> clippingDetector);

    void selectAll() override;
    int getNumRows() const noexcept;

    //===------------------------------------------------------------------===//
    // Ghost clips
    //===------------------------------------------------------------------===//
    
    void showGhostClipFor(ClipComponent *targetClipComponent);
    void hideAllGhostClips();

    //===------------------------------------------------------------------===//
    // Note management
    //===------------------------------------------------------------------===//

    void addClip(Pattern *pattern, float beat);
    Rectangle<float> getEventBounds(FloatBoundsComponent *mc) const override;
    Rectangle<float> getEventBounds(const Clip &clip, float beat) const;
    float getBeatForClipByXPosition(const Clip &clip, float x) const;
    float getBeatByMousePosition(const Pattern *pattern, int x) const;
    Pattern *getPatternByMousePosition(int y) const;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onAddMidiEvent(const MidiEvent &event) override {}
    void onChangeMidiEvent(const MidiEvent &e1, const MidiEvent &e2) override {}
    void onRemoveMidiEvent(const MidiEvent &event) override {}
    void onPostRemoveMidiEvent(MidiSequence *const layer) override {}

    void onAddClip(const Clip &clip) override;
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override;
    void onRemoveClip(const Clip &clip) override;
    void onPostRemoveClip(Pattern *const pattern) override;

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks) override;

    //===------------------------------------------------------------------===//
    // LassoSource
    //===------------------------------------------------------------------===//

    void selectEventsInRange(float startBeat,
        float endBeat, bool shouldClearAllOthers) override;

    void findLassoItemsInArea(Array<SelectableComponent *> &itemsFound,
        const Rectangle<int> &rectangle) override;

    //===------------------------------------------------------------------===//
    // SmoothZoomListener
    //===------------------------------------------------------------------===//

    void zoomRelative(const Point<float> &origin, const Point<float> &factor) override;
    void zoomAbsolute(const Point<float> &zoom) override;
    float getZoomFactorY() const noexcept override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void handleCommandMessage(int commandId) override;
    void resized() override;
    void paint(Graphics &g) override;
    void parentSizeChanged() override;
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
protected:

    //===------------------------------------------------------------------===//
    // HybridRoll
    //===------------------------------------------------------------------===//

    void setChildrenInteraction(bool interceptsMouse, MouseCursor c) override;
    void updateRollSize();

public:

    Image rowPattern;
    static Image renderRowsPattern(const HelioTheme &theme, int height);
    void repaintBackgroundsCache();

    void reloadRollContent();
    void insertNewClipAt(const MouseEvent &e);

    void focusToRegionAnimated(int startKey, int endKey, float startBeat, float endBeat);
    class FocusToRegionAnimator;
    ScopedPointer<Timer> focusToRegionAnimator;
    
private:

    ClipComponent *newClipDragging;
    bool addNewClipMode;

    ScopedPointer<CutPointMark> knifeToolHelper;
    void startCuttingClips(const MouseEvent &e);
    void continueCuttingClips(const MouseEvent &e);
    void endCuttingClipsIfNeeded();

private:
    
    Array<MidiTrack *> tracks;

    OwnedArray<ClipComponent> ghostClips;

    ScopedPointer<PatternRollSelectionMenuManager> selectedClipsMenuManager;

    using ClipComponentsMap = FlatHashMap<Clip, UniquePointer<ClipComponent>, ClipHash>;
    ClipComponentsMap clipComponents;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternRoll)
};
