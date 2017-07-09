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
#else
#   define PATTERNROLL_HAS_PRERENDERED_BACKGROUND 0
#endif

#if HELIO_DESKTOP
#   if PATTERNROLL_HAS_PRERENDERED_BACKGROUND
#       define MAX_ROW_HEIGHT (30)
#       define MIN_ROW_HEIGHT (8)
#   else
#       define MAX_ROW_HEIGHT (35)
#       define MIN_ROW_HEIGHT (6)
#   endif
#elif HELIO_MOBILE
#   if PATTERNROLL_HAS_PRERENDERED_BACKGROUND
#       define MAX_ROW_HEIGHT (40)
#       define MIN_ROW_HEIGHT (10)
#   else
#       define MAX_ROW_HEIGHT (60)
#       define MIN_ROW_HEIGHT (8)
#   endif
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

    static void repaintBackgroundsCache(HelioTheme &theme);
    static CachedImage::Ptr renderRowsPattern(HelioTheme &theme, int height);
    
public:

    PatternRoll(ProjectTreeItem &parentProject,
              Viewport &viewportRef,
              WeakReference<AudioMonitor> clippingDetector);

    ~PatternRoll() override;

    void deleteSelection();
    
    void reloadMidiTrack() override;

    void setRowHeight(const int newRowHeight);
    inline int getRowHeight() const noexcept
    { return this->rowHeight; }

	int getNumRows() const noexcept;
    
    //===------------------------------------------------------------------===//
    // Ghost notes
    //===------------------------------------------------------------------===//
    
    void showGhostClipFor(ClipComponent *targetClipComponent);
    void hideAllGhostClips();
    

    //===------------------------------------------------------------------===//
    // SmoothZoomListener
    //===------------------------------------------------------------------===//

    void zoomRelative(const Point<float> &origin, const Point<float> &factor) override;
    void zoomAbsolute(const Point<float> &zoom) override;
    float getZoomFactorY() const override;


    //===------------------------------------------------------------------===//
    // Note management
    //===------------------------------------------------------------------===//

	void addClip(Pattern *pattern, float beat);
    Rectangle<float> getEventBounds(FloatBoundsComponent *mc) const override;
    Rectangle<float> getEventBounds(Pattern *pattern, float beat) const;
    float getBeatByComponentPosition(float x) const;
    float getBeatByMousePosition(int x) const;
    

    //===------------------------------------------------------------------===//
    // Drag helpers
    //===------------------------------------------------------------------===//

    void showHelpers();
    void hideHelpers();
    void moveHelpers(const float deltaBeat, const int deltaKey);


    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

	void onAddClip(const Clip &clip) override;
	void onChangeClip(const Clip &oldClip, const Clip &newClip) override;
	void onRemoveClip(const Clip &clip) override;
	void onPostRemoveClip(const Pattern *pattern) override;

	void onAddPattern(const Pattern *pattern) override;
	void onChangePattern(const Pattern *pattern) override;
	void onRemovePattern(const Pattern *pattern) override;


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
    
private:

    void updateChildrenBounds() override;
    void updateChildrenPositions() override;

    void insertNewClipAt(const MouseEvent &e);

    int rowHeight;
	
private:
    
    void focusToRegionAnimated(int startKey, int endKey, float startBeat, float endBeat);
    class FocusToRegionAnimator;
    ScopedPointer<Timer> focusToRegionAnimator;
    
private:
    
    OwnedArray<ClipComponent> ghostClips;
    
    ScopedPointer<HelperRectangle> helperHorizontal;
    
    HashMap<Clip, ClipComponent *, ClipHashFunction> componentsHashTable;

};
