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

class MidiSequence;
class NoteComponent;
class PianoRollSelectionMenuManager;
class CommandPaletteChordConstructor;
class CommandPaletteMoveNotesMenu;
class HelperRectangle;
class KnifeToolHelper;
class NoteNameGuidesBar;
class Scale;

#include "Note.h"
#include "Clip.h"
#include "RollBase.h"
#include "HelioTheme.h"
#include "NoteResizerLeft.h"
#include "NoteResizerRight.h"
#include "HighlightingScheme.h"
#include "CommandPaletteModel.h"
#include "MidiTrack.h"

class PianoRoll final : public RollBase, public CommandPaletteModel
{
public:

    PianoRoll(ProjectNode &parentProject,
        Viewport &viewportRef,
        WeakReference<AudioMonitor> clippingDetector);

    ~PianoRoll() override;

    WeakReference<MidiTrack> getActiveTrack() const noexcept;
    const Clip &getActiveClip() const noexcept;

    void setDefaultNoteVolume(float volume) noexcept;
    void setDefaultNoteLength(float length) noexcept;

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

    float getZoomFactorY() const noexcept override;
    void zoomRelative(const Point<float> &origin,
        const Point<float> &factor) override;

    void zoomToArea(int minKey, int maxKey, float minBeat, float maxBeat);

    //===------------------------------------------------------------------===//
    // Note management
    //===------------------------------------------------------------------===//

    Rectangle<float> getEventBounds(FloatBoundsComponent *mc) const override;
    Rectangle<float> getEventBounds(int key, float beat, float length) const;
    bool isNoteVisible(int key, float beat, float length) const;

    // Note that beat is returned relative to active clip's beat offset:
    void getRowsColsByComponentPosition(float x, float y, int &noteNumber, float &beatNumber) const;
    void getFineRowsColsByComponentPosition(float x, float y, int& noteNumber, float& beatNumber) const;
    void getBiasedRowsColsByMousePosition(int x, int y, int& noteNumber, float& beatNumber, float roundBias) const; //added roundBias variable to allow rounding bias to be set
    void getRowsColsByMousePosition(int x, int y, int &noteNumber, float &beatNumber) const;

    //===------------------------------------------------------------------===//
    // Drag helpers
    //===------------------------------------------------------------------===//

    void showDragHelpers();
    void hideDragHelpers();
    void moveDragHelpers(const float deltaBeat, const int deltaKey);

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
    void onChangeTrackBeatRange(MidiTrack *const track) override;

    void onChangeProjectInfo(const ProjectMetadata *info) override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewEditableScope(MidiTrack *const track,
        const Clip &clip, bool shouldFocus) override;

    //===------------------------------------------------------------------===//
    // UserInterfaceFlags::Listener
    //===------------------------------------------------------------------===//

    void onScalesHighlightingFlagChanged(bool enabled) override;
    void onNoteNameGuidesFlagChanged(bool enabled) override;

    //===------------------------------------------------------------------===//
    // LassoSource
    //===------------------------------------------------------------------===//

    void selectEventsInRange(float startBeat,
        float endBeat, bool shouldClearAllOthers) override;

    void findLassoItemsInArea(Array<SelectableComponent *> &itemsFound,
        const Rectangle<int> &rectangle) override;
    void selectLassoItemsInArea(const Rectangle<int>& rectangle, const MouseEvent &mouseEvent);

    float getLassoStartBeat() const;
    float getLassoEndBeat() const;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;
    void leftButtonDown(const MouseEvent& e) override;
    void rightButtonDown(const MouseEvent& e) override;
    void mouseDoubleClick(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void handleCommandMessage(int commandId) override;
    void resized() override;
    void paint(Graphics &g) override;
    
    //===------------------------------------------------------------------===//
    // RollBase's legacy
    //===------------------------------------------------------------------===//
    
    void handleAsyncUpdate() override;
    void changeListenerCallback(ChangeBroadcaster *source) override;

    //===------------------------------------------------------------------===//
    // Command Palette
    //===------------------------------------------------------------------===//

    Array<CommandPaletteActionsProvider *> getCommandPaletteActionProviders() const override;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;
    
protected:

    //===------------------------------------------------------------------===//
    // RollBase
    //===------------------------------------------------------------------===//

    void selectAll() override;
    float findNextAnchorBeat(float beat) const override;
    float findPreviousAnchorBeat(float beat) const override;

private:

    WeakReference<MidiTrack> activeTrack = nullptr;
    Clip activeClip;

    void updateActiveRangeIndicator() const;

private:

    void reloadRollContent();
    void loadTrack(const MidiTrack *const track);

    void updateSize();
    void updateChildrenBounds() override;
    void updateChildrenPositions() override;
    void setChildrenInteraction(bool interceptsMouse, MouseCursor c) override;

    void switchToClipInViewport() const;
    void insertNewNoteAt(const MouseEvent &e);
    int getYPositionByKey(int targetKey) const;

    UniquePointer<KnifeToolHelper> knifeToolHelper;
    void startCuttingEvents(const MouseEvent &e);
    void continueCuttingEvents(const MouseEvent &e);
    void endCuttingEventsIfNeeded();

    NoteComponent *newNoteDragging = nullptr;
    bool addNewNoteMode = false;
    float newNoteVolume = Globals::Defaults::newNoteVelocity;
    float newNoteLength = Globals::Defaults::newNoteLength;

    int rowHeight = PianoRoll::defaultRowHeight;
    void setRowHeight(int newRowHeight);
    inline int getRowHeight() const noexcept
    {
        return this->rowHeight;
    }

private:

    enum class ToolType : int8
    {
        ScalePreview,
        ChordPreview
    };

    void showChordTool(ToolType type, Point<int> position);

private:

    void updateBackgroundCachesAndRepaint();
    void updateBackgroundCacheFor(const KeySignatureEvent &key);
    void removeBackgroundCacheFor(const KeySignatureEvent &key);

    OwnedArray<HighlightingScheme> backgroundsCache;
    UniquePointer<HighlightingScheme> defaultHighlighting;
    int binarySearchForHighlightingScheme(const KeySignatureEvent *const e) const noexcept;
    friend class ThemeSettingsItem; // to be able to call renderRowsPattern
    
    bool scalesHighlightingEnabled = true;

private:

    friend class NoteNameGuidesBar;
    UniquePointer<NoteNameGuidesBar> noteNameGuides;

private:
    
    OwnedArray<NoteComponent> ghostNotes;
    UniquePointer<HelperRectangle> draggingHelper;

    UniquePointer<NoteResizerLeft> noteResizerLeft;
    UniquePointer<NoteResizerRight> noteResizerRight;

    UniquePointer<PianoRollSelectionMenuManager> selectedNotesMenuManager;
    
    UniquePointer<CommandPaletteMoveNotesMenu> consoleMoveNotesMenu;
    UniquePointer<CommandPaletteChordConstructor> consoleChordConstructor;

    using SequenceMap = FlatHashMap<Note, UniquePointer<NoteComponent>, MidiEventHash>;
    using PatternMap = FlatHashMap<Clip, UniquePointer<SequenceMap>, ClipHash>;
    PatternMap patternMap;
    SequenceMap sequenceMap;

private:

#if PLATFORM_DESKTOP
    static constexpr auto minRowHeight = 7;
    static constexpr auto defaultRowHeight = 12;
    static constexpr auto maxRowHeight = 26;
#elif PLATFORM_MOBILE
    static constexpr auto minRowHeight = 10;
    static constexpr auto defaultRowHeight = 15;
    static constexpr auto maxRowHeight = 35;
#endif

    friend class HighlightingScheme;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoRoll);
};
