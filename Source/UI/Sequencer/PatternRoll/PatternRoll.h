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
#   define PATTERNROLL_HAS_PRERENDERED_BACKGROUND 1
#   define PATTERNROLL_ROW_HEIGHT 96
#else
#   define PATTERNROLL_HAS_PRERENDERED_BACKGROUND 0
#   define PATTERNROLL_ROW_HEIGHT 64
#endif

class Pattern;
class ClipComponent;
class PianoRollReboundThread;
class PianoRollCellHighlighter;
class HelperRectangle;

#include "HelioTheme.h"
#include "HybridRoll.h"
#include "Clip.h"

class PatternRoll : public HybridRoll
{
public:

    PatternRoll(ProjectTreeItem &parentProject,
              Viewport &viewportRef,
              WeakReference<AudioMonitor> clippingDetector);

    ~PatternRoll() override;

    void deleteSelection();
    void reloadRollContent() override;
	int getNumRows() const noexcept;


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
    Rectangle<float> getEventBounds(Pattern *pattern, float beat) const;
    float getBeatByComponentPosition(float x) const;
    float getBeatByMousePosition(int x) const;
	Pattern *getPatternByMousePosition(int y) const;


    //===------------------------------------------------------------------===//
    // Drag helpers
    //===------------------------------------------------------------------===//

    void showHelpers();
    void hideHelpers();
    void moveHelpers(const float deltaBeat, const int deltaKey);


    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

	void onAddMidiEvent(const MidiEvent &event) override;
	void onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
	void onRemoveMidiEvent(const MidiEvent &event) override;
	void onPostRemoveMidiEvent(const MidiLayer *layer) override;

	void onAddClip(const Clip &clip) override;
	void onChangeClip(const Clip &oldClip, const Clip &newClip) override;
	void onRemoveClip(const Clip &clip) override;
	void onPostRemoveClip(const Pattern *pattern) override;

	void onAddTrack(const MidiLayer *layer, const Pattern *pattern = nullptr) override;
	void onChangeTrack(const MidiLayer *layer, const Pattern *pattern = nullptr) override;
	void onRemoveTrack(const MidiLayer *layer, const Pattern *pattern = nullptr) override;


    //===------------------------------------------------------------------===//
    // LassoSource
    //===------------------------------------------------------------------===//

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
    bool keyPressed(const KeyPress &key) override;
    void resized() override;
    void paint(Graphics &g) override;

    
    //===------------------------------------------------------------------===//
    // MidiRoll's legacy
    //===------------------------------------------------------------------===//
    
    void handleAsyncUpdate() override;
    
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
public:

		static CachedImage::Ptr rowPattern;
		static CachedImage::Ptr renderRowsPattern(HelioTheme &theme, int height);
		static void repaintBackgroundsCache(HelioTheme &theme)
		{ rowPattern = PatternRoll::renderRowsPattern(theme, PATTERNROLL_ROW_HEIGHT); }

private:

    void updateChildrenBounds() override;
    void updateChildrenPositions() override;

    void insertNewClipAt(const MouseEvent &e);
	
private:
    
    void focusToRegionAnimated(int startKey, int endKey, float startBeat, float endBeat);
    class FocusToRegionAnimator;
    ScopedPointer<Timer> focusToRegionAnimator;
    
private:
    
	// layer's path : pattern
	HashMap<String, Pattern *> patterns;
	HashMap<String, MidiLayer *> layers;
	StringArray sortedPaths;

    OwnedArray<ClipComponent> ghostClips;
    
    ScopedPointer<HelperRectangle> helperHorizontal;
    
    HashMap<Clip, ClipComponent *, ClipHashFunction> componentsHashTable;

};
