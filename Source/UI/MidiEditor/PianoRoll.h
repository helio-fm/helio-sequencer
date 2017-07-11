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
#   define PIANOROLL_HAS_PRERENDERED_BACKGROUND 1
#else
#   define PIANOROLL_HAS_PRERENDERED_BACKGROUND 0
#endif

#if HELIO_DESKTOP
#   define PIANOROLL_HAS_NOTE_RESIZERS 0
#   if PIANOROLL_HAS_PRERENDERED_BACKGROUND
#       define MAX_ROW_HEIGHT (30)
#       define MIN_ROW_HEIGHT (8)
#   else
#       define MAX_ROW_HEIGHT (35)
#       define MIN_ROW_HEIGHT (6)
#   endif
#elif HELIO_MOBILE
#   define PIANOROLL_HAS_NOTE_RESIZERS 1
#   if PIANOROLL_HAS_PRERENDERED_BACKGROUND
#       define MAX_ROW_HEIGHT (40)
#       define MIN_ROW_HEIGHT (10)
#   else
#       define MAX_ROW_HEIGHT (60)
#       define MIN_ROW_HEIGHT (8)
#   endif
#endif

class MidiLayer;
class NoteComponent;
class PianoRollReboundThread;
class PianoRollCellHighlighter;
class HelperRectangle;
class NoteResizerLeft;
class NoteResizerRight;

#include "HelioTheme.h"
#include "MidiRoll.h"
#include "Note.h"

class PianoRoll : public MidiRoll
{
public:

    static void repaintBackgroundsCache(HelioTheme &theme);
    static CachedImage::Ptr renderRowsPattern(HelioTheme &theme, int height);
    
public:

    PianoRoll(ProjectTreeItem &parentProject,
              Viewport &viewportRef,
              WeakReference<AudioMonitor> clippingDetector);

    ~PianoRoll() override;

    void deleteSelection();
    
    void reloadMidiTrack() override;
    void setActiveMidiLayers(Array<MidiLayer *> tracks, MidiLayer *primaryLayer) override;

    void setRowHeight(const int newRowHeight);

    inline int getRowHeight() const
    { return this->rowHeight; }

    inline int getNumRows() const
    { return this->numRows; }
    
    inline float getDefaultNoteVelocity() const
    { return this->defaultNoteVelocity; }

    inline void setDefaultNoteVelocity(float val)
    { this->defaultNoteVelocity = val; }

    
    //===------------------------------------------------------------------===//
    // Ghost notes
    //===------------------------------------------------------------------===//
    
    void showGhostNoteFor(NoteComponent *targetNoteComponent);
    void hideAllGhostNotes();
    

    //===------------------------------------------------------------------===//
    // SmoothZoomListener
    //===------------------------------------------------------------------===//

    void zoomRelative(const Point<float> &origin, const Point<float> &factor) override;
    void zoomAbsolute(const Point<float> &zoom) override;
    float getZoomFactorY() const override;


    //===------------------------------------------------------------------===//
    // Note management
    //===------------------------------------------------------------------===//

    void addNote(int key, float beat, float length, float velocity);
    Rectangle<float> getEventBounds(MidiEventComponent *mc) const override;
    Rectangle<float> getEventBounds(const int key, const float beat, const float length) const;
    void getRowsColsByComponentPosition(const float x, const float y, int &noteNumber, float &beatNumber) const;
    void getRowsColsByMousePosition(int x, int y, int &noteNumber, float &beatNumber) const;
    int getYPositionByKey(int targetKey) const;
    

    //===------------------------------------------------------------------===//
    // Drag helpers
    //===------------------------------------------------------------------===//

    void showHelpers();
    void hideHelpers();
    void moveHelpers(const float deltaBeat, const int deltaKey);


    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
    void onEventAdded(const MidiEvent &event) override;
    void onEventRemoved(const MidiEvent &event) override;
    void onLayerChanged(const MidiLayer *layer) override;
    void onLayerAdded(const MidiLayer *layer) override;
    void onLayerRemoved(const MidiLayer *layer) override;


    //===------------------------------------------------------------------===//
    // LassoSource
    //===------------------------------------------------------------------===//

    void findLassoItemsInArea(Array<MidiEventComponent *> &itemsFound,
                                      const Rectangle<int> &rectangle) override;


    //===------------------------------------------------------------------===//
    // ClipboardOwner
    //===------------------------------------------------------------------===//

    XmlElement *clipboardCopy() const override;
    void clipboardPaste(const XmlElement &xml) override;


    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void longTapEvent(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDoubleClick(const MouseEvent &e) override;
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

    void insertNewNoteAt(const MouseEvent &e);
    bool dismissDraggingNoteIfNeeded();

    bool mouseDownWasTriggered; // juce mouseUp wierdness workaround

    NoteComponent *draggingNote;
    bool addNewNoteMode;
    
    int numRows;
    int rowHeight;

    float defaultNoteLength;
    float defaultNoteVelocity;
    
    bool usingFullRender;
    
private:
    
    void focusToRegionAnimated(int startKey, int endKey, float startBeat, float endBeat);
    class FocusToRegionAnimator;
    ScopedPointer<Timer> focusToRegionAnimator;
    
private:
    
    OwnedArray<NoteComponent> ghostNotes;
    
    //ScopedPointer<HelperRectangle> helperVertical;
    ScopedPointer<HelperRectangle> helperHorizontal;

    ScopedPointer<NoteResizerLeft> noteResizerLeft;
    ScopedPointer<NoteResizerRight> noteResizerRight;
    
    HashMap<Note, NoteComponent *, NoteHashFunction> componentsHashTable;

};
