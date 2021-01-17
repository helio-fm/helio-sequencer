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

class CutPointMark;
class ClipComponent;

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
    void selectClip(const Clip &clip);
    int getNumRows() const noexcept;

    //===------------------------------------------------------------------===//
    // Note management
    //===------------------------------------------------------------------===//

    void addClip(Pattern *pattern, float beat);
    Rectangle<float> getEventBounds(FloatBoundsComponent *mc) const override;
    Rectangle<float> getEventBounds(const Clip &clip, float beat) const;
    float getBeatForClipByXPosition(const Clip &clip, float x) const;
    float getBeatByMousePosition(const Pattern *pattern, int x) const;

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
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

    //===------------------------------------------------------------------===//
    // LassoSource
    //===------------------------------------------------------------------===//

    void selectEventsInRange(float startBeat,
        float endBeat, bool shouldClearAllOthers) override;

    void findLassoItemsInArea(Array<SelectableComponent *> &itemsFound,
        const Rectangle<int> &rectangle) override;

    void updateHighlightedInstances();

    //===------------------------------------------------------------------===//
    // SmoothZoomListener
    //===------------------------------------------------------------------===//

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

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

protected:

    //===------------------------------------------------------------------===//
    // TransportListener
    //===------------------------------------------------------------------===//

    void onRecord() override;
    void onStop() override;

    //===------------------------------------------------------------------===//
    // HybridRoll
    //===------------------------------------------------------------------===//

    void setChildrenInteraction(bool interceptsMouse, MouseCursor c) override;
    void updateRollSize();

    float findNextAnchorBeat(float beat) const override;
    float findPreviousAnchorBeat(float beat) const override;

private:

    static constexpr auto clipHeight = 48;
    static constexpr auto trackHeaderHeight = 3;
    static constexpr auto rowHeight = clipHeight + trackHeaderHeight;

private:

    Image rowPattern;
    static Image renderRowsPattern(const HelioTheme &theme, int height);
    void repaintBackgroundsCache();

    void reloadRollContent();
    void insertNewClipAt(const MouseEvent &e);

private:

    ClipComponent *newClipDragging = nullptr;
    bool addNewClipMode = false;

    UniquePointer<CutPointMark> knifeToolHelper;
    void startCuttingClips(const MouseEvent &e);
    void continueCuttingClips(const MouseEvent &e);
    void endCuttingClipsIfNeeded(const MouseEvent &e);

private:
    
    // needed for grouping:
    Array<String> rows;
    void reloadRowsGrouping();

    // for grouping and highlighting:
    Array<const MidiTrack *> tracks;

    OwnedArray<ChangeListener> selectionListeners;

    using ClipComponentsMap = FlatHashMap<Clip, UniquePointer<ClipComponent>, ClipHash>;
    ClipComponentsMap clipComponents;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternRoll)
};
