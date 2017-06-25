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

class MidiLayer;
class MidiEventComponentLasso;
class ProjectTreeItem;
class LongTapController;
class SmartDragController;
class SmoothPanController;
class SmoothZoomController;
class MultiTouchController;
class OverlayShadow;
class HybridRollHeader;
class TriggersTrackMap;
class Transport;
class HybridRollListener;
class WipeSpaceHelper;
class InsertSpaceHelper;
class TimelineWarningMarker;

#include "ComponentFader.h"
#include "AnnotationsTrackMap.h"
#include "AnnotationLargeComponent.h"
#include "TimeSignaturesTrackMap.h"
#include "TimeSignatureLargeComponent.h"
#include "TransportIndicator.h"
#include "TransportListener.h"
#include "MidiEventComponent.h"
#include "ClipboardOwner.h"
#include "LongTapListener.h"
#include "SmoothPanListener.h"
#include "SmoothZoomListener.h"
#include "MultiTouchListener.h"
#include "ProjectListener.h"
#include "Lasso.h"
#include "HybridRollEditMode.h"
#include "AudioMonitor.h"
#include "Serializable.h"

#if HELIO_DESKTOP
#   define MAX_BAR_WIDTH (250)
#   define MIDIROLL_HEADER_HEIGHT (48)
#elif HELIO_MOBILE
#   define MAX_BAR_WIDTH (300)
#   define MIDIROLL_HEADER_HEIGHT (48)
#endif


// Track is measured in quarter beats
#define NUM_BEATS_IN_BAR 4

// For the empty project:
#define DEFAULT_NUM_BARS 8


// Dirty hack for speedup, supposed to be used every time I need to repaint a lot of child components. 
// Component::unfocusAllComponents() - prevents keyboard focus traverse hell
// this->setVisible(false) - prevents redraw hell
#define MIDI_ROLL_BULK_REPAINT_START \
    Component::unfocusAllComponents();  \
    this->setVisible(false);

#define MIDI_ROLL_BULK_REPAINT_END \
    this->setVisible(true); \
    this->grabKeyboardFocus();


class HybridRoll :
    public Component,
    public Serializable,
    public LongTapListener,
    public SmoothPanListener,
    public SmoothZoomListener,
    public MultiTouchListener,
    public ProjectListener,
    public ClipboardOwner,
    public LassoSource<SelectableComponent *>,
    protected ChangeListener, // listens to HybridRollEditMode,
    protected TransportListener,
    protected AsyncUpdater, // for async scrolling on transport listener events
    protected HighResolutionTimer, // for smooth scrolling to seek position
    protected TransportIndicator::MovementListener, // for smooth scrolling to seek position
    protected AudioMonitor::ClippingListener // for displaying clipping indicator components
{
public:

    enum ColourIds
    {
        blackKeyColourId                 = 0x99002001,
        blackKeyBrightColourId           = 0x99002002,
        whiteKeyColourId                 = 0x99002003,
        whiteKeyBrightColourId           = 0x99002004,
        rowLineColourId                  = 0x99002005,
        barLineColourId                  = 0x99002006,
        barLineBevelColourId             = 0x99002007,
        beatLineColourId                 = 0x99002008,
        snapLineColourId                 = 0x99002009,
        headerColourId                   = 0x99002010,
        indicatorColourId                = 0x99002011
    };
    
    HybridRoll(ProjectTreeItem &parentProject,
             Viewport &viewportRef,
             WeakReference<AudioMonitor> AudioMonitor);

    ~HybridRoll() override;

    Viewport &getViewport() const noexcept;
    Transport &getTransport() const noexcept;
    ProjectTreeItem &getProject() const noexcept;
    MidiLayer *getPrimaryActiveMidiLayer() const noexcept;
    HybridRollEditMode getEditMode() const;

    int getNumActiveLayers() const noexcept;
    MidiLayer *getActiveMidiLayer(int index) const noexcept;
    
    virtual void reloadMidiTrack() = 0;
    virtual void setActiveMidiLayers(Array<MidiLayer *> tracks, MidiLayer *primaryLayer) = 0;
    virtual Rectangle<float> getEventBounds(FloatBoundsComponent *nc) const = 0;
    
    void scrollToSeekPosition();
	float getPositionForNewTimelineEvent() const;
    void insertAnnotationWithinScreen(const String &annotation);
	void insertTimeSignatureWithinScreen(int numerator, int denominator);
	void selectAll();
    
    //===------------------------------------------------------------------===//
    // Custom maps
    //===------------------------------------------------------------------===//
    
    void addOwnedMap(Component *newTrackMap);
    
    void removeOwnedMap(Component *existingTrackMap);
    
    template<typename T>
    T *findOwnedMapOfType()
    {
        for (int i = 0; i < this->trackMaps.size(); ++i)
        {
            if (T *target = dynamic_cast<T *>(this->trackMaps.getUnchecked(i)))
            {
                return target;
            }
        }
        
        return nullptr;
    }
    
    //===------------------------------------------------------------------===//
    // Modes
    //===------------------------------------------------------------------===//
    
    bool isInSelectionMode() const;
    bool isInDragMode() const;
    // ...
    
    //===------------------------------------------------------------------===//
    // HybridRoll listeners management
    //===------------------------------------------------------------------===//
    
    void addRollListener(HybridRollListener *listener);
    void removeRollListener(HybridRollListener *listener);
    void removeAllRollListeners();
    
    //===------------------------------------------------------------------===//
    // MultiTouchListener
    //===------------------------------------------------------------------===//

    void multiTouchZoomEvent(const Point<float> &origin, const Point<float> &zoom) override;
    void multiTouchPanEvent(const Point<float> &offset) override;
    void multiTouchCancelZoom() override;
    void multiTouchCancelPan() override;
    Point<float> getMultiTouchOrigin(const Point<float> &from) override;

    //===------------------------------------------------------------------===//
    // SmoothPanListener
    //===------------------------------------------------------------------===//

    void panByOffset(const int offsetX, const int offsetY) override;
    void panProportionally(const float absX, const float absY) override;

    Point<int> getPanOffset() const override;

    //===------------------------------------------------------------------===//
    // SmoothZoomListener
    //===------------------------------------------------------------------===//

    void zoomAbsolute(const Point<float> &zoom) override;
    void zoomRelative(const Point<float> &origin, const Point<float> &factor) override;
    float getZoomFactorX() const override;
    float getZoomFactorY() const override;

    void startSmoothZoom(const Point<float> &origin, const Point<float> &factor);
    void zoomInImpulse();
    void zoomOutImpulse();

    //===------------------------------------------------------------------===//
    // Misc
    //===------------------------------------------------------------------===//
    
    int getXPositionByTransportPosition(double absPosition, double canvasWidth) const;
    double getTransportPositionByXPosition(int xPosition, double canvasWidth) const;

    double getTransportPositionByBeat(float targetBeat) const;
    float getBeatByTransportPosition(double absSeekPosition) const;

    float getBarByXPosition(int xPosition) const;
    int getXPositionByBar(float targetBar) const;

    int getXPositionByBeat(float targetBeat) const;
    float getFloorBeatByXPosition(int x) const;
    float getRoundBeatByXPosition(int x) const;
    
    virtual void setLastBar(int bar);
    inline int getLastBar() const noexcept { return this->lastBar; }
    inline float getLastBeat() const noexcept { return float(this->lastBar * NUM_BEATS_IN_BAR); }
    
    virtual void setFirstBar(int bar);
    inline int getFirstBar() const noexcept { return this->firstBar; }
    inline float getFirstBeat() const noexcept { return float(this->firstBar * NUM_BEATS_IN_BAR); }
    
    void setBarRange(int first, int last);
    inline int getNumBars() const { return this->lastBar - this->firstBar; }
    inline int getNumBeats() const { return this->getNumBars() * NUM_BEATS_IN_BAR; }

    virtual void setBarWidth(const float newBarWidth);
    float getBarWidth() const noexcept { return this->barWidth; }

    Array<float> &getVisibleBars() noexcept { return this->visibleBars; }
    Array<float> &getVisibleBeats() noexcept { return this->visibleBeats; }
    Array<float> &getVisibleSnaps() noexcept { return this->visibleSnaps; }
    
    bool isUsingAnyAltMode() const;
    void setSpaceDraggingMode(bool dragMode);
    bool isUsingSpaceDraggingMode() const;
    void setAltDrawingMode(bool drawMode);
    bool isUsingAltDrawingMode() const;
    
    void triggerBatchRepaintFor(FloatBoundsComponent *target);

    void startFollowingIndicator();
    void stopFollowingIndicator();
    
    //===------------------------------------------------------------------===//
    // LassoSource
    //===------------------------------------------------------------------===//

    Lasso &getLassoSelection() override;
    void selectEventsInRange(float startBeat, float endBeat, bool shouldClearAllOthers);
    void selectEvent(SelectableComponent *event, bool shouldClearAllOthers);
    void deselectEvent(SelectableComponent *event);
    void deselectAll();
    
    MidiEventComponentLasso *getLasso() const;
    
    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;
    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void longTapEvent(const MouseEvent &e) override;

    void focusGained(FocusChangeType cause) override;
    void focusLost(FocusChangeType cause) override;

    bool keyPressed(const KeyPress &key) override;
    bool keyStateChanged(bool isKeyDown) override;
    void modifierKeysChanged(const ModifierKeys &modifiers) override;

    void mouseMove(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &wheel) override;
    void moved() override;

    void resized() override;
    void paint(Graphics &g) override;

protected:
    
    ListenerList<HybridRollListener> listeners;
    
    void broadcastRollMoved();
    void broadcastRollResized();
    
protected:
    
    //===------------------------------------------------------------------===//
    // TransportIndicator::MovementListener
    //===------------------------------------------------------------------===//
    
    void onTransportIndicatorMoved(int indicatorX) override;
    
    //===------------------------------------------------------------------===//
    // VolumeCallback::ClippingListener
    //===------------------------------------------------------------------===//
    
    void onClippingWarning() override;
    void resetAllClippingIndicators();
    OwnedArray<TimelineWarningMarker> clippingIndicators;
    
    void onOversaturationWarning() override;
    void resetAllOversaturationIndicators();
    OwnedArray<TimelineWarningMarker> oversaturationIndicators;

    //===------------------------------------------------------------------===//
    // TransportListener
    //===------------------------------------------------------------------===//
    
    void onSeek(const double absolutePosition, const double currentTimeMs, const double totalTimeMs) override;
    void onTempoChanged(const double newTempo) override;
    void onTotalTimeChanged(const double timeMs) override;
    void onPlay() override;
    void onStop() override;

    ReadWriteLock transportLastCorrectPositionLock;
    double transportLastCorrectPosition;
    double transportIndicatorOffset;
    bool shouldFollowIndicator;
    
    //===------------------------------------------------------------------===//
    // AsyncUpdater
    //===------------------------------------------------------------------===//
    
    void handleAsyncUpdate() override;
	void handleCommandMessage(int commandId) override;

    double findIndicatorOffsetFromViewCentre() const;
    friend class HybridRollHeader;
    
    //===------------------------------------------------------------------===//
    // Timer
    //===------------------------------------------------------------------===//

    void hiResTimerCallback() override;
    
protected:
    
    virtual void updateChildrenBounds();
    virtual void updateChildrenPositions();
    void updateBounds();
    
    WeakReference<AudioMonitor> clippingDetector;
    ProjectTreeItem &project;
    Viewport &viewport;
    
    OwnedArray<Component> trackMaps;

    Point<int> viewportAnchor;
    Point<float> clickAnchor;
    Point<float> zoomAnchor;
    ScopedPointer<Component> zoomMarker;
    
    void resetDraggingAnchors();
    void continueDragging(const MouseEvent &e);
    Point<float> getMouseOffset(Point<float> mouseScreenPosition) const;

    void startZooming();
    void continueZooming(const MouseEvent &e);
    void endZooming();
    
    
    void initWipeSpaceHelper(int xPosition);
    void updateWipeSpaceHelperIfNeeded(const MouseEvent &e);
    void removeWipeSpaceHelper();

    void startWipingSpace(const MouseEvent &e);
    void continueWipingSpace(const MouseEvent &e);
    void endWipingSpaceIfNeeded();
    
    ScopedPointer<WipeSpaceHelper> wipeSpaceHelper;

    
    void initInsertSpaceHelper(int xPosition);
    void updateInsertSpaceHelperIfNeeded(const MouseEvent &e);
    void removeInsertSpaceHelper();
    
    void startInsertingSpace(const MouseEvent &e);
    void continueInsertingSpace(const MouseEvent &e);
    void endInsertingSpaceIfNeeded();
    
    ScopedPointer<InsertSpaceHelper> insertSpaceHelper;

    
    Array<MidiLayer *> activeLayers;
    MidiLayer *primaryActiveLayer;

    Lasso selection;

    OwnedArray<MidiEventComponent> eventComponents;

    bool isViewportZoomEvent(const MouseEvent &e) const;
    bool isViewportDragEvent(const MouseEvent &e) const;
    bool isAddEvent(const MouseEvent &e) const;
    bool isLassoEvent(const MouseEvent &e) const;
    bool isWipeSpaceEvent(const MouseEvent &e) const;
    bool isInsertSpaceEvent(const MouseEvent &e) const;

    int firstBar;
    int lastBar;

    float trackFirstBeat;
    float trackLastBeat;

    float barWidth;
    float beatDelta;
    
    bool altDrawMode;
    bool spaceDragMode;
    int draggedDistance;
    Time timeEnteredDragMode;

    ComponentFader fader;

    ScopedPointer<HybridRollHeader> header;
    ScopedPointer<TransportIndicator> indicator;
    
    typedef AnnotationsTrackMap<AnnotationLargeComponent> AnnotationsLargeMap;
    ScopedPointer<AnnotationsLargeMap> annotationsTrack;

    typedef TimeSignaturesTrackMap<TimeSignatureLargeComponent> TimeSignaturesLargeMap;
    ScopedPointer<TimeSignaturesLargeMap> timeSignaturesTrack;

    ScopedPointer<Component> topShadow;
    ScopedPointer<Component> bottomShadow;

    ScopedPointer<MidiEventComponentLasso> lassoComponent;

protected:
    
    Array<float> visibleBars;
    Array<float> visibleBeats;
    Array<float> visibleSnaps;

    void computeVisibleBeatLines();

protected:

    ScopedPointer<LongTapController> longTapController;
    ScopedPointer<MultiTouchController> multiTouchController;
    ScopedPointer<SmoothPanController> smoothPanController;
    ScopedPointer<SmoothZoomController> smoothZoomController;

    Array<SafePointer<FloatBoundsComponent>> batchRepaintList;

protected:
    
    void changeListenerCallback(ChangeBroadcaster *source) override;

};
