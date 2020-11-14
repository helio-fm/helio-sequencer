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
class MidiTrackNode;
class SelectionComponent;
class LongTapController;
class SmartDragController;
class SmoothPanController;
class SmoothZoomController;
class MultiTouchController;
class OverlayShadow;
class HybridRollHeader;
class AutomationStepsSequenceMap;
class Transport;
class HybridRollListener;
class TimelineWarningMarker;

#include "ComponentFader.h"
#include "AnnotationsProjectMap.h"
#include "TimeSignaturesProjectMap.h"
#include "KeySignaturesProjectMap.h"
#include "Playhead.h"
#include "TransportListener.h"
#include "MidiEventComponent.h"
#include "LongTapListener.h"
#include "SmoothPanListener.h"
#include "SmoothZoomListener.h"
#include "MultiTouchListener.h"
#include "ProjectNode.h"
#include "ProjectListener.h"
#include "Lasso.h"
#include "HybridRollEditMode.h"
#include "UserInterfaceFlags.h"
#include "AudioMonitor.h"
#include "HeadlineContextMenuController.h"
#include "Temperament.h"

#if PLATFORM_MOBILE
#   define HYBRID_ROLL_LISTENS_LONG_TAP 1
#endif

#define HYBRID_ROLL_BULK_REPAINT_START \
    if (this->isEnabled()) { this->setVisible(false); }

#define HYBRID_ROLL_BULK_REPAINT_END \
    if (this->isEnabled()) { this->setVisible(true); }

class HybridRoll :
    public Component,
    public Serializable,
    public LongTapListener,
    public SmoothPanListener,
    public SmoothZoomListener,
    public MultiTouchListener,
    public ProjectListener,
    public LassoSource<SelectableComponent *>,
    public Playhead::Listener, // for smooth scrolling to seek position
    protected UserInterfaceFlags::Listener, // global UI options
    protected ChangeListener, // listens to HybridRollEditMode,
    protected TransportListener, // for positioning the playhead component and auto-scrolling
    protected AsyncUpdater, // coalesce multiple transport events ^^ into a single async view change
    protected HighResolutionTimer, // for smooth scrolling to seek position
    protected AudioMonitor::ClippingListener // for displaying clipping indicator components
{
public:
    
    HybridRoll(ProjectNode &project, Viewport &viewport,
        WeakReference<AudioMonitor> audioMonitor,
        bool hasAnnotationsTrack = true,
        bool hasKeySignaturesTrack = true,
        bool hasTimeSignaturesTrack = true);

    ~HybridRoll() override;

    Viewport &getViewport() const noexcept;
    Transport &getTransport() const noexcept;
    ProjectNode &getProject() const noexcept;
    HybridRollEditMode &getEditMode() noexcept;

    virtual void selectAll() = 0;
    virtual Rectangle<float> getEventBounds(FloatBoundsComponent *nc) const = 0;
    
    void scrollToSeekPosition();
    float getPositionForNewTimelineEvent() const;
    void insertAnnotationWithinScreen(const String &annotation);
    void insertTimeSignatureWithinScreen(int numerator, int denominator);
    
    //===------------------------------------------------------------------===//
    // Custom maps
    //===------------------------------------------------------------------===//
    
    void addOwnedMap(Component *newTrackMap);
    void removeOwnedMap(Component *existingTrackMap);
    template<typename T> inline T *findOwnedMapOfType() const
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
    // Temperament info
    //===------------------------------------------------------------------===//

    int getNumKeys() const noexcept;
    int getPeriodSize() const noexcept;
    Note::Key getMiddleC() const noexcept;
    Temperament::Ptr getTemperament() const noexcept;

    //===------------------------------------------------------------------===//
    // HybridRoll listeners management
    //===------------------------------------------------------------------===//
    
    void addRollListener(HybridRollListener *listener);
    void removeRollListener(HybridRollListener *listener);
    void removeAllRollListeners();
    
    //===------------------------------------------------------------------===//
    // MultiTouchListener
    //===------------------------------------------------------------------===//

    void longTapEvent(const Point<float> &position,
        const WeakReference<Component> &target) override;

    void multiTouchZoomEvent(const Point<float> &origin, const Point<float> &zoom) override;
    void multiTouchPanEvent(const Point<float> &offset) override;
    void multiTouchCancelZoom() override;
    void multiTouchCancelPan() override;
    Point<float> getMultiTouchOrigin(const Point<float> &from) override;

    //===------------------------------------------------------------------===//
    // SmoothPanListener
    //===------------------------------------------------------------------===//

    void panByOffset(int offsetX, int offsetY) override;
    void panProportionally(float absX, float absY) override;

    Point<int> getPanOffset() const override;

    //===------------------------------------------------------------------===//
    // SmoothZoomListener
    //===------------------------------------------------------------------===//

    float getZoomFactorX() const noexcept override;
    float getZoomFactorY() const noexcept override;
    void zoomRelative(const Point<float> &origin,
        const Point<float> &factor) override;

    void zoomInImpulse(float factor = 1.f);
    void zoomOutImpulse(float factor = 1.f);
    void zoomToArea(float minBeat, float maxBeat);
    void startSmoothZoom(const Point<float> &origin, const Point<float> &factor);

    //===------------------------------------------------------------------===//
    // Misc
    //===------------------------------------------------------------------===//

    int getXPositionByBeat(float targetBeat) const noexcept;
    int getPlayheadPositionByBeat(double targetBeat, double parentWidth) const;
    float getRoundBeatSnapByXPosition(int x) const;

    inline float getLastBeat() const noexcept { return this->lastBeat; }
    inline float getFirstBeat() const noexcept { return this->firstBeat; }
    
    void setBeatRange(float first, float last);
    inline float getNumBeats() const noexcept { return this->lastBeat - this->firstBeat; }

    virtual void setBeatWidth(float newBeatWidth);
    inline float getBeatWidth() const noexcept { return this->beatWidth; }

    float getMinVisibleBeatForCurrentZoomLevel() const;

    inline const Array<float> &getVisibleBars() const noexcept  { return this->visibleBars; }
    inline const Array<float> &getVisibleBeats() const noexcept { return this->visibleBeats; }
    inline const Array<float> &getVisibleSnaps() const noexcept { return this->visibleSnaps; }
    
    bool isUsingAnyAltMode() const;
    void setSpaceDraggingMode(bool dragMode);
    bool isUsingSpaceDraggingMode() const;
    void setAltDrawingMode(bool drawMode);
    bool isUsingAltDrawingMode() const;
    
    void triggerBatchRepaintFor(FloatBoundsComponent *target);

    void startFollowingPlayhead();
    void stopFollowingPlayhead();
    
    //===------------------------------------------------------------------===//
    // LassoSource
    //===------------------------------------------------------------------===//

    virtual void selectEventsInRange(float startBeat,
        float endBeat, bool shouldClearAllOthers) = 0;

    Lasso &getLassoSelection() override;
    void selectEvent(SelectableComponent *event, bool shouldClearAllOthers);
    void deselectEvent(SelectableComponent *event);
    void deselectAll();
    
    SelectionComponent *getSelectionComponent() const noexcept;
    
    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;
    void onChangeProjectBeatRange(float firstBeat, float lastBeat) override;
    void onChangeViewBeatRange(float firstBeat, float lastBeat) override;
    void onChangeProjectInfo(const ProjectMetadata *info) override;
    void onBeforeReloadProjectContent() override;
    void onReloadProjectContent(const Array<MidiTrack *> &tracks,
        const ProjectMetadata *meta) override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &wheel) override;

    void handleCommandMessage(int commandId) override;
    void resized() override;
    void paint(Graphics &g) override;

protected:
    
    ListenerList<HybridRollListener> listeners;
    
    void broadcastRollMoved();
    void broadcastRollResized();
    
protected:
    
    //===------------------------------------------------------------------===//
    // Playhead::Listener
    //===------------------------------------------------------------------===//
    
    void onPlayheadMoved(int indicatorX) override;
    
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
    
    void onSeek(float beat, double currentTimeMs, double totalTimeMs) override;
    void onTempoChanged(double msPerQuarter) override {}
    void onTotalTimeChanged(double timeMs) override {}
    void onLoopModeChanged(bool hasLoop, float start, float end) override;

    void onPlay() override;
    void onRecord() override;
    void onStop() override;

    Atomic<float> lastTransportBeat = 0.f; // modified from the player thread

    Atomic<double> playheadOffset = 0.0;
    bool shouldFollowPlayhead = false;

    //===------------------------------------------------------------------===//
    // AsyncUpdater
    //===------------------------------------------------------------------===//
    
    void handleAsyncUpdate() override;

    double findPlayheadOffsetFromViewCentre() const;
    friend class HybridRollHeader;
    
    //===------------------------------------------------------------------===//
    // Timer
    //===------------------------------------------------------------------===//

    void hiResTimerCallback() override;
    
protected:
    
    // These two methods are supposed to layout non-midi-event children
    virtual void updateChildrenBounds();
    virtual void updateChildrenPositions();

    virtual void setChildrenInteraction(bool interceptMouse, MouseCursor c) = 0;

    virtual float findNextAnchorBeat(float beat) const;
    virtual float findPreviousAnchorBeat(float beat) const;

    void addTrackInteractively(MidiTrackNode *preset, UndoActionId whereToRollback, bool refocus,
        const String &defaultTrackName, const String &dialogTitle, const String &dialogOk);

    float getFloorBeatSnapByXPosition(int x) const;
    inline float getBeatByXPosition(float x) const
    {
        const float beatNumber = roundBeat(x / this->beatWidth + this->firstBeat);
        return jlimit(this->firstBeat, this->lastBeat, beatNumber);
    }

    void updateBounds();
    
    WeakReference<AudioMonitor> clippingDetector;
    ProjectNode &project;
    Viewport &viewport;
    
    Temperament::Ptr temperament;

    OwnedArray<Component> trackMaps;

    Point<int> viewportAnchor = { 0, 0 };
    Point<float> clickAnchor = { 0, 0 };
    Point<float> zoomAnchor = { 0, 0 };
    UniquePointer<Component> zoomMarker;
    
    void resetDraggingAnchors();
    void continueDragging(const MouseEvent &e);
    Point<float> getMouseOffset(Point<float> mouseScreenPosition) const;

    Point<int> getDefaultPositionForPopup() const;

    void startZooming();
    void continueZooming(const MouseEvent &e);
    void endZooming();
    
    Lasso selection;

    bool isViewportZoomEvent(const MouseEvent &e) const;
    bool isViewportDragEvent(const MouseEvent &e) const;
    bool isAddEvent(const MouseEvent &e) const;
    bool isLassoEvent(const MouseEvent &e) const;
    bool isKnifeToolEvent(const MouseEvent &e) const;

    float firstBeat = 0.f;
    float lastBeat = Globals::Defaults::projectLength;

    float projectFirstBeat = 0.f;
    float projectLastBeat = Globals::Defaults::projectLength;

    float beatWidth = 0.f;
    
    bool altDrawMode = false;
    bool spaceDragMode = false;
    int draggedDistance = 0;
    Time timeEnteredDragMode;

    ComponentFader fader;

    UniquePointer<HybridRollHeader> header;
    UniquePointer<Component> headerShadow;
    UniquePointer<Playhead> playhead;
    
    UniquePointer<AnnotationsProjectMap> annotationsMap;
    UniquePointer<TimeSignaturesProjectMap> timeSignaturesMap;
    UniquePointer<KeySignaturesProjectMap> keySignaturesMap;

    UniquePointer<SelectionComponent> lassoComponent;

protected:
    
    Array<float> visibleBars;
    Array<float> visibleBeats;
    Array<float> visibleSnaps;
    // contains all three above so that it' easier to iterate them
    Array<float> allSnaps;

    const Colour barLineColour;
    const Colour barLineBevelColour;
    const Colour beatLineColour;
    const Colour snapLineColour;

    void computeVisibleBeatLines();

protected:

    UniquePointer<LongTapController> longTapController;
    UniquePointer<MultiTouchController> multiTouchController;
    UniquePointer<SmoothPanController> smoothPanController;
    UniquePointer<SmoothZoomController> smoothZoomController;
    UniquePointer<HeadlineContextMenuController> contextMenuController;

    Array<SafePointer<FloatBoundsComponent>> batchRepaintList;

protected:
    
    void changeListenerCallback(ChangeBroadcaster *source) override;
    void applyEditModeUpdates();

};
