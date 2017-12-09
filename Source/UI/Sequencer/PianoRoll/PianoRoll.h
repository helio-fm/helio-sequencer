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

#if HELIO_DESKTOP
#   define PIANOROLL_HAS_NOTE_RESIZERS 0
#   define PIANOROLL_MIN_ROW_HEIGHT (6)
#   define PIANOROLL_MAX_ROW_HEIGHT (30)
#elif HELIO_MOBILE
#   define PIANOROLL_HAS_NOTE_RESIZERS 1
#   define PIANOROLL_MIN_ROW_HEIGHT (10)
#   define PIANOROLL_MAX_ROW_HEIGHT (35)
#endif

class MidiSequence;
class NoteComponent;
class PianoRollReboundThread;
class PianoRollCellHighlighter;
class HelperRectangle;
class NoteResizerLeft;
class NoteResizerRight;
class Scale;

#include "HelioTheme.h"
#include "HybridRoll.h"
#include "Note.h"
#include "Clip.h"

class PianoRoll : public HybridRoll
{
public:

    PianoRoll(ProjectTreeItem &parentProject,
              Viewport &viewportRef,
              WeakReference<AudioMonitor> clippingDetector);

    ~PianoRoll() override;

    void deleteSelection();
    
    void reloadRollContent() override;
    int getNumActiveLayers() const noexcept;
    MidiSequence *getActiveMidiLayer(int index) const noexcept;
    MidiSequence *getPrimaryActiveMidiLayer() const noexcept;
    void setActiveMidiLayers(Array<MidiSequence *> tracks,
        MidiSequence *primaryLayer);

    void setRowHeight(const int newRowHeight);
    inline int getRowHeight() const
    { return this->rowHeight; }

    inline int getNumRows() const
    { return this->numRows; }

    //===------------------------------------------------------------------===//
    // HybridRoll
    //===------------------------------------------------------------------===//

    void selectAll() override;
    void setChildrenInteraction(bool interceptsMouse, MouseCursor c) override;

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
    Rectangle<float> getEventBounds(FloatBoundsComponent *mc) const override;
    Rectangle<float> getEventBounds(int key, float beat, float length) const;
    void getRowsColsByComponentPosition(float x, float y, int &noteNumber, float &beatNumber) const;
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

    void onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;

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
    void mouseDoubleClick(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void handleCommandMessage(int commandId) override;
    void resized() override;
    void paint(Graphics &g) override;

    
    //===------------------------------------------------------------------===//
    // HybridRoll's legacy
    //===------------------------------------------------------------------===//
    
    void handleAsyncUpdate() override;

    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:

    Array<MidiSequence *> activeLayers;
    MidiSequence *primaryActiveLayer;

private:

    void updateChildrenBounds() override;
    void updateChildrenPositions() override;

    void insertNewNoteAt(const MouseEvent &e);
    bool dismissDraggingNoteIfNeeded();

    bool mouseDownWasTriggered; // juce mouseUp weirdness workaround

    NoteComponent *draggingNote;
    bool addNewNoteMode;
    
    int numRows;
    int rowHeight;

private:

    class HighlightingScheme final
    {
    public:
        HighlightingScheme(int rootKey, const Scale &scale);
        
        template<typename T1, typename T2>
        static int compareElements(const T1 *const l, const T2 *const r)
        {
            const int keyDiff = l->getRootKey() - r->getRootKey();
            const int keyResult = (keyDiff > 0) - (keyDiff < 0);
            if (keyResult != 0) { return keyDiff; }

            if (l->getScale().isEquivalentTo(r->getScale())) { return 0; }

            const int scaleDiff = l->getScale().hashCode() - r->getScale().hashCode();
            return (scaleDiff > 0) - (scaleDiff < 0);
        }

        const Scale &getScale() const noexcept { return this->scale; }
        const int getRootKey() const noexcept { return this->rootKey; }
        const Image getUnchecked(int i) const noexcept { return this->rows.getUnchecked(i); }
        void setRows(Array<Image> val) { this->rows = val; }

    private:
        Scale scale;
        int rootKey;
        Array<Image> rows;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighlightingScheme);
    };

    void updateBackgroundCacheFor(const KeySignatureEvent &key);
    void removeBackgroundCacheFor(const KeySignatureEvent &key);
    Array<Image> renderBackgroundCacheFor(const HighlightingScheme *const scheme) const;
    static Image renderRowsPattern(const HelioTheme &, const Scale &, int root, int height);
    OwnedArray<HighlightingScheme> backgroundsCache;
    ScopedPointer<HighlightingScheme> defaultHighlighting;
    int binarySearchForHighlightingScheme(const KeySignatureEvent *const e) const noexcept;
    friend class ThemeSettingsItem; // to be able to call renderRowsPattern

private:

    void focusToRegionAnimated(int startKey, int endKey, float startBeat, float endBeat);
    class FocusToRegionAnimator;
    ScopedPointer<Timer> focusToRegionAnimator;
    
private:
    
    OwnedArray<NoteComponent> ghostNotes;
    
    ScopedPointer<HelperRectangle> helperHorizontal;

    ScopedPointer<NoteResizerLeft> noteResizerLeft;
    ScopedPointer<NoteResizerRight> noteResizerRight;
    
    typedef SparseHashMap<Note, NoteComponent *, NoteHashFunction> EventComponentsMap;
    EventComponentsMap componentsMap;

    //typedef SparseHashMap<Clip, EventComponentsMap, ClipHashFunction> ClipsMap;
    //EventComponentsMap clipsMap;

};
