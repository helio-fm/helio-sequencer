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
#   define PIANOROLL_MIN_ROW_HEIGHT (7)
#   define PIANOROLL_MAX_ROW_HEIGHT (26)
#elif HELIO_MOBILE
#   define PIANOROLL_HAS_NOTE_RESIZERS 1
#   define PIANOROLL_MIN_ROW_HEIGHT (10)
#   define PIANOROLL_MAX_ROW_HEIGHT (35)
#endif

class MidiSequence;
class NoteComponent;
class PianoRollCellHighlighter;
class PianoRollSelectionMenuManager;
class HelperRectangle;
class KnifeToolHelper;
class Scale;

#include "HybridRoll.h"
#include "HelioTheme.h"
#include "NoteResizerLeft.h"
#include "NoteResizerRight.h"
#include "Note.h"
#include "Clip.h"

class PianoRoll final : public HybridRoll
{
public:

    PianoRoll(ProjectNode &parentProject,
              Viewport &viewportRef,
              WeakReference<AudioMonitor> clippingDetector);
    
    void setEditableScope(WeakReference<MidiTrack> activeTrack,
        const Clip &activeClip, bool zoomToArea);

    WeakReference<MidiTrack> getActiveTrack() const noexcept;
    const Clip &getActiveClip() const noexcept;

    void setDefaultNoteVolume(float volume) noexcept;

    void setRowHeight(int newRowHeight);
    inline int getRowHeight() const noexcept
    { return this->rowHeight; }

    inline int getNumRows() const noexcept
    { return this->numRows; }

    //===------------------------------------------------------------------===//
    // HybridRoll
    //===------------------------------------------------------------------===//

    void selectAll() override;

    //===------------------------------------------------------------------===//
    // Ghost notes
    //===------------------------------------------------------------------===//
    
    void showGhostNoteFor(NoteComponent *targetNoteComponent);
    void hideAllGhostNotes();
    
    //===------------------------------------------------------------------===//
    // Input Listeners
    //===------------------------------------------------------------------===//

    void longTapEvent(const Point<float> &position,
        const WeakReference<Component> &target) override;

    void zoomRelative(const Point<float> &origin, const Point<float> &factor) override;
    void zoomAbsolute(const Point<float> &zoom) override;
    float getZoomFactorY() const noexcept override;

    void zoomToArea(int minKey, int maxKey, float minBeat, float maxBeat);

    //===------------------------------------------------------------------===//
    // Note management
    //===------------------------------------------------------------------===//

    void addNote(int key, float beat, float length, float velocity);
    Rectangle<float> getEventBounds(FloatBoundsComponent *mc) const override;
    Rectangle<float> getEventBounds(int key, float beat, float length) const;

    int getYPositionByKey(int targetKey) const;

    // Note that beat is returned relative to active clip's beat offset:
    void getRowsColsByComponentPosition(float x, float y, int &noteNumber, float &beatNumber) const;
    void getRowsColsByMousePosition(int x, int y, int &noteNumber, float &beatNumber) const;

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

    void onAddClip(const Clip &clip) override;
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override;
    void onRemoveClip(const Clip &clip) override;

    void onAddTrack(MidiTrack *const track) override;
    void onRemoveTrack(MidiTrack *const track) override;
    void onChangeTrackProperties(MidiTrack *const track) override;

    void onReloadProjectContent(const Array<MidiTrack *> &tracks) override;
    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;

    //===------------------------------------------------------------------===//
    // LassoSource
    //===------------------------------------------------------------------===//

    void selectEventsInRange(float startBeat,
        float endBeat, bool shouldClearAllOthers) override;

    void findLassoItemsInArea(Array<SelectableComponent *> &itemsFound,
        const Rectangle<int> &rectangle) override;

    float getLassoStartBeat() const;
    float getLassoEndBeat() const;

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
    void changeListenerCallback(ChangeBroadcaster *source) override;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:

    // Piano roll restricts editing to a single clip of one track at time.
    // I've cut the feature to edit multiple tracks at once,
    // mainly because it makes headline menu unclear (like that is the current track).
    // I never implemented the ability to edit multiple clips at once:
    // so far, all the code that works with selection, assumes that selected notes are unique,
    // and there are no multiple instances of the same note within different clips selected.

    WeakReference<MidiTrack> activeTrack;
    Clip activeClip;

    void updateActiveRangeIndicator() const;

private:

    void reloadRollContent();
    void loadTrack(const MidiTrack *const track);

    void updateChildrenBounds() override;
    void updateChildrenPositions() override;
    void setChildrenInteraction(bool interceptsMouse, MouseCursor c) override;

    void insertNewNoteAt(const MouseEvent &e);

    UniquePointer<KnifeToolHelper> knifeToolHelper;
    void startCuttingEvents(const MouseEvent &e);
    void continueCuttingEvents(const MouseEvent &e);
    void endCuttingEventsIfNeeded();

    NoteComponent *newNoteDragging;
    bool addNewNoteMode;
    float newNoteVolume;

    int numRows;
    int rowHeight;

private:

    enum ToolType
    {
        ScalePreview,
        ChordPreview
    };

    void showChordTool(ToolType type, Point<int> position);

private:

    class HighlightingScheme final
    {
    public:
        HighlightingScheme(int rootKey, const Scale::Ptr scale) noexcept;
        
        template<typename T1, typename T2>
        static int compareElements(const T1 *const l, const T2 *const r)
        {
            const int keyDiff = l->getRootKey() - r->getRootKey();
            const int keyResult = (keyDiff > 0) - (keyDiff < 0);
            if (keyResult != 0) { return keyDiff; }

            if (l->getScale()->isEquivalentTo(r->getScale())) { return 0; }

            const int scaleDiff = l->getScale()->hashCode() - r->getScale()->hashCode();
            return (scaleDiff > 0) - (scaleDiff < 0);
        }

        const Scale::Ptr getScale() const noexcept { return this->scale; }
        const int getRootKey() const noexcept { return this->rootKey; }
        const Image getUnchecked(int i) const noexcept { return this->rows.getUnchecked(i); }
        void setRows(Array<Image> val) noexcept { this->rows = val; }

    private:
        Scale::Ptr scale;
        int rootKey;
        Array<Image> rows;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighlightingScheme);
    };

    void updateBackgroundCacheFor(const KeySignatureEvent &key);
    void removeBackgroundCacheFor(const KeySignatureEvent &key);
    Array<Image> renderBackgroundCacheFor(const HighlightingScheme *const scheme) const;
    static Image renderRowsPattern(const HelioTheme &, const Scale::Ptr, int root, int height);
    OwnedArray<HighlightingScheme> backgroundsCache;
    UniquePointer<HighlightingScheme> defaultHighlighting;
    int binarySearchForHighlightingScheme(const KeySignatureEvent *const e) const noexcept;
    friend class ThemeSettingsItem; // to be able to call renderRowsPattern

private:

    void focusToRegionAnimated(int startKey, int endKey, float startBeat, float endBeat);
    class FocusToRegionAnimator;
    UniquePointer<Timer> focusToRegionAnimator;
    
private:
    
    OwnedArray<NoteComponent> ghostNotes;
    UniquePointer<HelperRectangle> draggingHelper;

    UniquePointer<NoteResizerLeft> noteResizerLeft;
    UniquePointer<NoteResizerRight> noteResizerRight;

    UniquePointer<PianoRollSelectionMenuManager> selectedNotesMenuManager;
    
    using SequenceMap = FlatHashMap<Note, UniquePointer<NoteComponent>, MidiEventHash>;
    using PatternMap = FlatHashMap<Clip, UniquePointer<SequenceMap>, ClipHash>;
    PatternMap patternMap;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRoll);
};
