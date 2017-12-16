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

#if JUCE_IOS
#   define PATTERNROLL_ROW_HEIGHT 96
#else
#   define PATTERNROLL_ROW_HEIGHT 128
#endif

class ClipComponent;
class PianoRollReboundThread;
class PianoRollCellHighlighter;

#include "HelioTheme.h"
#include "HybridRoll.h"
#include "Pattern.h"
#include "Clip.h"

class PatternRoll : public HybridRoll
{
public:

    PatternRoll(ProjectTreeItem &parentProject,
        Viewport &viewportRef,
        WeakReference<AudioMonitor> clippingDetector);

    void deleteSelection();
    void selectAll() override;
    int getNumRows() const noexcept;

    //===------------------------------------------------------------------===//
    // HybridRoll
    //===------------------------------------------------------------------===//

    void setChildrenInteraction(bool interceptsMouse, MouseCursor c) override;

    //===------------------------------------------------------------------===//
    // Ghost notes
    //===------------------------------------------------------------------===//
    
    void showGhostClipFor(ClipComponent *targetClipComponent);
    void hideAllGhostClips();

    //===------------------------------------------------------------------===//
    // Note management
    //===------------------------------------------------------------------===//

    void addClip(Pattern *pattern, float beat);
    Rectangle<float> getEventBounds(FloatBoundsComponent *mc) const override;
    Rectangle<float> getEventBounds(const Clip &clip, float beat) const;
    float getBeatByComponentPosition(float x) const;
    float getBeatByMousePosition(int x) const;
    Pattern *getPatternByMousePosition(int y) const;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onAddMidiEvent(const MidiEvent &event) override;
    void onChangeMidiEvent(const MidiEvent &e1, const MidiEvent &e2) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;
    void onPostRemoveMidiEvent(MidiSequence *const layer) override;

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
    // ClipboardOwner
    //===------------------------------------------------------------------===//

    XmlElement *clipboardCopy() const override;
    void clipboardPaste(const XmlElement &xml) override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void handleCommandMessage(int commandId) override;
    void resized() override;
    void paint(Graphics &g) override;
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
public:

    Image rowPattern;
    Image renderRowsPattern(const HelioTheme &theme, int height) const;
    void repaintBackgroundsCache();

    void reloadRollContent();
    void insertNewClipAt(const MouseEvent &e);

    void focusToRegionAnimated(int startKey, int endKey, float startBeat, float endBeat);
    class FocusToRegionAnimator;
    ScopedPointer<Timer> focusToRegionAnimator;
    
private:
    
    Array<MidiTrack *> tracks;

    OwnedArray<ClipComponent> ghostClips;
    
    typedef SparseHashMap<Clip, UniquePointer<ClipComponent>, ClipHash> ClipComponentsMap;
    ClipComponentsMap componentsMap;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatternRoll)
};
