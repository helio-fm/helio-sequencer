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

class ClipComponent;
class ClipCutPointMark;
class MergingClipsConnector;
class PatternRollCursor;

#include "HelioTheme.h"
#include "RollBase.h"
#include "MidiTrack.h"
#include "Pattern.h"
#include "Clip.h"

class PatternRoll final : public RollBase
{
public:

    PatternRoll(ProjectNode &parentProject,
        Viewport &viewportRef,
        WeakReference<AudioMonitor> clippingDetector);

    void selectAll() override;
    void selectClip(const Clip &clip);
    int getNumRows() const noexcept;

    //===------------------------------------------------------------------===//
    // Clip management
    //===------------------------------------------------------------------===//

    inline int getRowByYPosition(int y) const noexcept
    {
        const auto rowNumber = (this->getHeight() - y +
            Globals::UI::rollHeaderHeight) / this->rowHeight;
        return jlimit(0, this->getNumRows(), rowNumber);
    }

    void addNewClip(int row, float beat);
    void addNewClip(const MouseEvent &e);

    Rectangle<float> getEventBounds(FloatBoundsComponent *mc) const override;
    Rectangle<float> getEventBounds(const Clip &clip) const;
    Rectangle<float> getEventBounds(int row, float beat, float length) const;
    float getClipBeatByXPosition(const Clip &clip, float x) const;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

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
    // DrawableLassoSource
    //===------------------------------------------------------------------===//

    void selectEventsInRange(float startBeat,
        float endBeat, bool shouldClearAllOthers) override;

    ClipComponent *findClipComponentAt(const Point<int> &point) const;
    void findLassoItemsInArea(Array<SelectableComponent *> &itemsFound,
        const Rectangle<int> &bounds) override;
    void findLassoItemsInPolygon(Array<SelectableComponent *> &itemsFound,
        const Rectangle<int> &bounds, const Array<Point<float>> &polygon) override;

    void updateHighlightedInstances();

    //===------------------------------------------------------------------===//
    // LongTapListener
    //===------------------------------------------------------------------===//

    void onLongTap(const Point<float> &position,
        const WeakReference<Component> &target) override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void handleCommandMessage(int commandId) override;
    void resized() override;
    void paint(Graphics &g) noexcept override;
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
    // RollBase
    //===------------------------------------------------------------------===//

    void setChildrenInteraction(bool interceptsMouse, MouseCursor c) override;
    void updateRollSize();

    float findNextPlayheadAnchorBeat(float beat) const override;
    float findPreviousPlayheadAnchorBeat(float beat) const override;
    Range<float> findPlayheadHomeEndRange() const override;

    void updateAllSnapLines() override;

    void zoomRelative(const Point<float> &origin,
        const Point<float> &factor, bool isInertial) override;
    void zoomAbsolute(const Rectangle<float> &proportion) override;

private:

#if PLATFORM_DESKTOP
    static constexpr auto clipHeight = 55;
#elif PLATFORM_MOBILE
    static constexpr auto clipHeight = 37;
#endif

    static constexpr auto trackHeaderHeight = 3;
    static constexpr auto rowHeight = clipHeight + trackHeaderHeight;

private:

    Image rowPattern;
    static constexpr auto rowPatternHeight = rowHeight * 8;
    static Image renderRowsPattern(const HelioTheme &theme);
    void repaintBackgroundsCache();

    void reloadRollContent();

    void showNewTrackMenu(float beatToInsertAt);
    void showNewTrackDialog(const String &instrumentId, float beatToInsertAt);

private:

    UniquePointer<PatternRollCursor> cursor;

    bool addNewClipMode = false;
    SafePointer<ClipComponent> newClipDragging = nullptr;

    UniquePointer<ClipCutPointMark> knifeToolHelper;
    void startCuttingClips(const Point<float> &mousePosition);
    void continueCuttingClips(const Point<float> &mousePosition);
    void endCuttingClipsIfNeeded(bool shouldCut, bool shouldRenameNewTracks);

    UniquePointer<MergingClipsConnector> mergeToolHelper;
    void startMergingEvents(const Point<float> &mousePosition) override;
    void continueMergingEvents(const Point<float> &mousePosition) override;
    void endMergingEvents() override;

    void startErasingEvents(const Point<float> &mousePosition) override;
    void continueErasingEvents(const Point<float> &mousePosition) override;
    void endErasingEvents() override;
    Array<Clip> clipsToEraseOnMouseUp;

private:
    
    // needed for grouping:
    Array<String> rows;
    void reloadRowsGrouping();

    // for grouping and highlighting:
    Array<const MidiTrack *> tracks;

    String lastShownInstrumentId;

    OwnedArray<ChangeListener> selectionListeners;

    using ClipComponentsMap = FlatHashMap<Clip, UniquePointer<ClipComponent>, ClipHash>;
    ClipComponentsMap clipComponents;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternRoll)
};
