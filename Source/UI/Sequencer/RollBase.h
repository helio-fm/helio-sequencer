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

class MidiSequence;
class MidiTrackNode;
class SelectionComponent;
class LongTapController;
class SmoothPanController;
class SmoothZoomController;
class MultiTouchController;
class RollHeader;
class Transport;
class RollListener;
class TimelineWarningMarker;

#include "ComponentFader.h"
#include "FloatBoundsComponent.h"
#include "AnnotationsProjectMap.h"
#include "TimeSignaturesProjectMap.h"
#include "KeySignaturesProjectMap.h"
#include "Playhead.h"
#include "TransportListener.h"
#include "LongTapListener.h"
#include "SmoothPanListener.h"
#include "SmoothZoomListener.h"
#include "MultiTouchListener.h"
#include "ProjectNode.h"
#include "ProjectListener.h"
#include "Lasso.h"
#include "RollEditMode.h"
#include "UserInterfaceFlags.h"
#include "AudioMonitor.h"
#include "HeadlineContextMenuController.h"
#include "TimeSignaturesAggregator.h"
#include "Temperament.h"
#include "ColourIDs.h"

#if PLATFORM_MOBILE
#   define ROLL_LISTENS_LONG_TAP 1
#endif

#define ROLL_BATCH_REPAINT_START \
    if (this->isEnabled()) { this->setVisible(false); }

#define ROLL_BATCH_REPAINT_END \
    if (this->isEnabled()) { this->setVisible(true); }

class RollBase :
    public Component,
    public Serializable,
    public LongTapListener,
    public SmoothPanListener,
    public SmoothZoomListener,
    public MultiTouchListener,
    public ProjectListener,
    public Playhead::Listener,
    public DrawableLassoSource<SelectableComponent *>,
    protected UserInterfaceFlags::Listener,
    protected RollEditMode::Listener,
    protected TransportListener, // for positioning the playhead component and auto-scrolling
    protected AsyncUpdater, // coalesce multiple transport events ^^ into a single async view change
    protected Timer, // for smooth scrolling to seek position,
    protected TimeSignaturesAggregator::Listener, // when the editable scope changes, active time signatures may change
    protected AudioMonitor::ClippingListener // for displaying clipping indicator components
{
public:
    
    RollBase(ProjectNode &project, Viewport &viewport,
        WeakReference<AudioMonitor> audioMonitor,
        bool hasAnnotationsTrack = true,
        bool hasKeySignaturesTrack = true,
        bool hasTimeSignaturesTrack = true);

    ~RollBase() override;

    Viewport &getViewport() const noexcept;
    Transport &getTransport() const noexcept;
    ProjectNode &getProject() const noexcept;
    RollEditMode &getEditMode() noexcept;

    virtual void selectAll() = 0;
    virtual Rectangle<float> getEventBounds(FloatBoundsComponent *nc) const = 0;
    
    float getPositionForNewTimelineEvent() const;

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
    // RollListeners management
    //===------------------------------------------------------------------===//
    
    void addRollListener(RollListener *listener);
    void removeRollListener(RollListener *listener);
    void removeAllRollListeners();
    
    //===------------------------------------------------------------------===//
    // MultiTouchListener
    //===------------------------------------------------------------------===//

    void multiTouchStartZooming() override;
    void multiTouchContinueZooming(
        const Rectangle<float> &relativePosition,
        const Rectangle<float> &relativePositionAnchor,
        const Rectangle<float> &absolutePositionAnchor) override;
    void multiTouchEndZooming(const MouseEvent &anchorEvent) override;

    Point<float> getMultiTouchRelativeAnchor(const MouseEvent &e) override;
    Point<float> getMultiTouchAbsoluteAnchor(const MouseEvent &e) override;

    bool isMultiTouchEvent(const MouseEvent &e) const noexcept;

    void onLongTap(const Point<float> &position,
        const WeakReference<Component> &target) override;

    //===------------------------------------------------------------------===//
    // SmoothPanListener
    //===------------------------------------------------------------------===//

    bool panByOffset(int offsetX, int offsetY) override;
    void panProportionally(float absX, float absY) override;
    Point<int> getPanOffset() const override;

    //===------------------------------------------------------------------===//
    // SmoothZoomListener
    //===------------------------------------------------------------------===//

    float getZoomFactorX() const noexcept override;
    float getZoomFactorY() const noexcept override;
    void zoomRelative(const Point<float> &origin,
        const Point<float> &factor, bool isInertial) override;
    void zoomAbsolute(const Rectangle<float> &proportion) override;

    void zoomInImpulse(float factor = 1.f);
    void zoomOutImpulse(float factor = 1.f);
    void zoomToArea(float minBeat, float maxBeat);
    void startSmoothZoom(const Point<float> &origin, const Point<float> &factor);

    //===------------------------------------------------------------------===//
    // TimeSignaturesAggregator::Listener
    //===------------------------------------------------------------------===//

    void onTimeSignaturesUpdated() override;

    //===------------------------------------------------------------------===//
    // Misc
    //===------------------------------------------------------------------===//

    inline int getXPositionByBeat(float targetBeat) const noexcept
    {
        return int((targetBeat - this->firstBeat) * this->beatWidth);
    }

    inline int getXPositionByBeat(float targetBeat, float parentWidth) const noexcept
    {
        const auto widthRatio = parentWidth / jmax(1.f, float(this->getWidth()));
        return int((targetBeat - this->firstBeat) * this->beatWidth * widthRatio);
    }

    inline float getBeatByXPosition(float x) const noexcept
    {
        jassert(this->beatWidth > 0.f);
        const float beatNumber = roundBeat(x / this->beatWidth + this->firstBeat);
        return jlimit(this->firstBeat, this->lastBeat, beatNumber);
    }

    float getFloorBeatSnapByXPosition(int x) const noexcept;
    float getRoundBeatSnapByXPosition(int x) const noexcept;

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
    inline float getBeatLineAlpha() const noexcept { return this->beatLineAlpha; }
    inline float getSnapLineAlpha() const noexcept { return this->snapLineAlpha; }
    
    void setSpaceDraggingMode(bool dragMode);
    bool isUsingSpaceDraggingMode() const;
    
    void triggerBatchRepaintFor(FloatBoundsComponent *target);

    bool scrollToPlayheadPositionIfNeeded(int edgeMargin = 50);
    void startFollowingPlayhead(bool forceScrollToPlayhead = false);
    void stopFollowingPlayhead();

    void resetDraggingAnchors();
    void resetDraggingAnchors(const MouseEvent &e);
    
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
    RollHeader *getHeaderComponent() const noexcept;

    //===------------------------------------------------------------------===//
    // ProjectListener
    //===------------------------------------------------------------------===//

    void onChangeMidiEvent(const MidiEvent &oldEvent, const MidiEvent &newEvent) override;
    void onAddMidiEvent(const MidiEvent &event) override;
    void onRemoveMidiEvent(const MidiEvent &event) override;
    void onChangeClip(const Clip &oldClip, const Clip &newClip) override;
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
    void paint(Graphics &g) noexcept override;

    //===------------------------------------------------------------------===//
    // Edit mode helpers
    //===------------------------------------------------------------------===//

    bool isViewportDragEvent(const MouseEvent &e) const;
    bool isAddEvent(const MouseEvent &e) const;
    bool isLassoEvent(const MouseEvent &e) const;
    bool isKnifeToolEvent(const MouseEvent &e) const;
    bool isMergeToolEvent(const MouseEvent &e) const;
    bool isErasingEvent(const MouseEvent &e) const;

protected:
    
    ListenerList<RollListener> listeners;
    
    void broadcastRollMoved();
    void broadcastRollResized();
    
protected:

    //===------------------------------------------------------------------===//
    // RollEditMode::Listener
    //===------------------------------------------------------------------===//

    void onChangeEditMode(const RollEditMode &mode) override;
    void applyEditModeUpdates();
    
    //===------------------------------------------------------------------===//
    // Playhead::Listener
    //===------------------------------------------------------------------===//
    
    void onMovePlayhead(int oldX, int newX) override;
    
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
    // UserInterfaceFlags::Listener
    //===------------------------------------------------------------------===//

    void onFollowPlayheadFlagChanged(bool following) override;
    void onUiAnimationsFlagChanged(bool enabled) override;
    void onMouseWheelFlagsChanged(UserInterfaceFlags::MouseWheelFlags flags) override;
    void onLockZoomLevelFlagChanged(bool zoomLocked) override;

    //===------------------------------------------------------------------===//
    // TransportListener
    //===------------------------------------------------------------------===//
    
    void onSeek(float beatPosition) override;
    void onCurrentTempoChanged(double msPerQuarter) override {}
    void onTotalTimeChanged(double timeMs) override {}
    void onLoopModeChanged(bool hasLoop, float start, float end) override;

    void onPlay() override;
    void onRecord() override;
    void onStop() override;

    Atomic<float> lastPlayheadBeat = 0.f; // modified from the player thread

    enum class PlayheadFollowMode { Disabled, Free, CatchWhenOffscreen, Follow };
    PlayheadFollowMode playheadFollowMode = PlayheadFollowMode::Free;

    //===------------------------------------------------------------------===//
    // AsyncUpdater
    //===------------------------------------------------------------------===//
    
    void handleAsyncUpdate() override;

    double findPlayheadOffsetFromViewCentre() const;
    friend class RollHeader;
    
    //===------------------------------------------------------------------===//
    // Timer
    //===------------------------------------------------------------------===//

    void timerCallback() override;
    
protected:
    
    // These two methods are supposed to layout non-midi-event children
    virtual void updateChildrenBounds();
    virtual void updateChildrenPositions();

    virtual void setChildrenInteraction(bool interceptMouse, MouseCursor c) = 0;

    virtual float findNextPlayheadAnchorBeat(float beat) const = 0;
    virtual float findPreviousPlayheadAnchorBeat(float beat) const = 0;
    virtual Range<float> findPlayheadHomeEndRange() const = 0;

    void updateWidth();
    
    WeakReference<AudioMonitor> clippingDetector;
    ProjectNode &project;
    Viewport &viewport;
    
    Temperament::Ptr temperament;

    Point<int> viewportAnchor = { 0, 0 };
    Point<float> clickAnchor = { 0, 0 };
    float beatWidthAnchor = 0;

    void continueDragging(const MouseEvent &e);
    Point<int> getMouseOffset(Point<int> mouseScreenPosition) const;

    Lasso selection;

    virtual void startErasingEvents(const Point<float> &mousePosition) = 0;
    virtual void continueErasingEvents(const Point<float> &mousePosition) = 0;
    virtual void endErasingEvents() = 0;

    virtual void startMergingEvents(const Point<float> &mousePosition) = 0;
    virtual void continueMergingEvents(const Point<float> &mousePosition) = 0;
    virtual void endMergingEvents() = 0;

    float firstBeat = -Globals::beatsPerBar * 4;
    float lastBeat = Globals::Defaults::projectLength + (Globals::beatsPerBar * 4);

    float projectFirstBeat = 0.f;
    float projectLastBeat = Globals::Defaults::projectLength;

    float beatWidth = Globals::UI::defaultBeatWidth;
    
    bool spaceDragMode = false;
    int draggedDistance = 0;
    uint32 timeEnteredDragMode = 0;
    uint32 timeStartedPlayback = 0;

    UserInterfaceFlags::MouseWheelFlags mouseWheelFlags;
    bool zoomLevelLocked = false;

    ComponentFader fader;

    UniquePointer<RollHeader> header;
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

    const Colour barLineColour = findDefaultColour(ColourIDs::Roll::barLine);
    const Colour barLineBevelColour = findDefaultColour(ColourIDs::Roll::barLineBevel);
    const Colour beatLineColour = findDefaultColour(ColourIDs::Roll::beatLine);
    const Colour snapLineColour = findDefaultColour(ColourIDs::Roll::snapLine);

    float beatLineAlpha = 1.f;
    float snapLineAlpha = 1.f;

    virtual void updateAllSnapLines();

protected:

    UniquePointer<LongTapController> longTapController;
    UniquePointer<MultiTouchController> multiTouchController;
    UniquePointer<SmoothPanController> smoothPanController;
    UniquePointer<SmoothZoomController> smoothZoomController;
    UniquePointer<HeadlineContextMenuController> contextMenuController;

    Array<SafePointer<FloatBoundsComponent>> batchRepaintList;

};
