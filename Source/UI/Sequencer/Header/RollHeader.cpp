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

#include "Common.h"
#include "RollHeader.h"
#include "RollBase.h"
#include "Pattern.h"
#include "Transport.h"
#include "SelectionComponent.h"
#include "SoundProbeIndicator.h"
#include "TimeDistanceIndicator.h"
#include "HeaderSelectionIndicator.h"
#include "ClipRangeIndicator.h"
#include "TrackStartIndicator.h"
#include "TrackEndIndicator.h"
#include "ModalCallout.h"
#include "TimelineMenu.h"

class PlaybackLoopMarker final : public Component
{
public:

    enum class Type { LoopStart, LoopEnd };

    PlaybackLoopMarker(Transport &transport,
        RollHeader &header, RollBase &roll, Type type) :
        transport(transport),
        header(header),
        roll(roll),
        type(type)
    {
        //this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(true, false);
        this->setMouseCursor(MouseCursor::PointingHandCursor);
        this->setSize(16, 16);
    }

    void setBeat(float targetBeat)
    {
        this->beat = targetBeat;
        this->updatePosition();
    }

    void updatePosition()
    {
        const auto x = this->roll.getXPositionByBeat(this->beat);
        const auto w = this->getWidth();
        switch (this->type)
        {
        case Type::LoopStart:
            this->setBounds(x, 0, w, this->getParentHeight());
            break;
        case Type::LoopEnd:
            this->setBounds(x - w + 1, 0, w, this->getParentHeight());
            break;
        }
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->header.getBarColour());

        const auto p1 = roundf(float(this->getHeight()) * 0.33f - 1.f);
        const auto p2 = roundf(float(this->getHeight()) * 0.66f - 4.f);

        switch (this->type)
        {
        case Type::LoopStart:
            g.fillRect(0, 1, 3, this->getHeight() - 2);
            g.fillRect(5, 1, 1, this->getHeight() - 2);

            g.fillRect(float(9), p1, 2.f, 4.f);
            g.fillRect(float(8), p1 + 1, 4.f, 2.f);

            g.fillRect(float(9), p2, 2.f, 4.f);
            g.fillRect(float(8), p2 + 1, 4.f, 2.f);

            // some fancy shadows
            g.setColour(this->shadowColour);

            g.fillRect(3, 1, 1, this->getHeight() - 2);
            g.fillRect(6, 1, 1, this->getHeight() - 2);
            break;
        case Type::LoopEnd:
            const int x = this->getWidth();

            g.fillRect(x - 3, 1, 3, this->getHeight() - 2);
            g.fillRect(x - 6, 1, 1, this->getHeight() - 2);

            g.fillRect(float(x - 11), p1, 2.f, 4.f);
            g.fillRect(float(x - 12), p1 + 1, 4.f, 2.f);

            g.fillRect(float(x - 11), p2, 2.f, 4.f);
            g.fillRect(float(x - 12), p2 + 1, 4.f, 2.f);

            // some fancy shadows
            g.setColour(this->shadowColour);

            g.fillRect(x, 1, 1, this->getHeight() - 2);
            g.fillRect(x - 5, 1, 1, this->getHeight() - 2);
            break;
        }
    }

    void mouseUp(const MouseEvent &e) override
    {
        this->setMouseCursor(MouseCursor::PointingHandCursor);

        if (e.getOffsetFromDragStart().isOrigin())
        {
            if (!this->transport.isPlayingAndRecording())
            {
                this->transport.stopPlayback();
                this->transport.seekToBeat(this->beat);
            }
        }
    }

    void mouseDrag(const MouseEvent &e) override
    {
        this->setMouseCursor(MouseCursor::DraggingHandCursor);

        const auto dragAtHeader = e.getEventRelativeTo(this->getParentComponent());
        const auto beat = this->roll.getRoundBeatSnapByXPosition(dragAtHeader.x);

        switch (this->type)
        {
        case Type::LoopStart:
            this->transport.setPlaybackLoop(beat, this->transport.getPlaybackLoopEnd());
            break;
        case Type::LoopEnd:
            this->transport.setPlaybackLoop(this->transport.getPlaybackLoopStart(), beat);
            break;
        }
    }
    
private:

    const Type type;

    Transport &transport;
    RollHeader &header;
    RollBase &roll;

    float beat = 0.f;

    const Colour shadowColour = findDefaultColour(ColourIDs::Common::borderLineDark);
};

RollHeader::RollHeader(Transport &transport, RollBase &roll, Viewport &viewport) :
    transport(transport),
    roll(roll),
    viewport(viewport)
{
    this->setOpaque(true);
    this->setAlwaysOnTop(true);
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->updateColours();

    this->loopMarkerStart = make<PlaybackLoopMarker>(transport,
        *this, roll, PlaybackLoopMarker::Type::LoopStart);
    this->addChildComponent(this->loopMarkerStart.get());

    this->loopMarkerEnd = make<PlaybackLoopMarker>(transport,
        *this, roll, PlaybackLoopMarker::Type::LoopEnd);
    this->addChildComponent(this->loopMarkerEnd.get());

    this->selectionIndicator = make<HeaderSelectionIndicator>();
    this->addChildComponent(this->selectionIndicator.get());

    this->projectStartIndicator = make<TrackStartIndicator>();
    this->addAndMakeVisible(this->projectStartIndicator.get());

    this->projectEndIndicator = make<TrackEndIndicator>();
    this->addAndMakeVisible(this->projectEndIndicator.get());

    this->setSize(this->getParentWidth(), Globals::UI::rollHeaderHeight);
    this->selectionIndicator->setTopLeftPosition(0,
        this->getHeight() - this->selectionIndicator->getHeight());
}

RollHeader::~RollHeader() = default;

void RollHeader::updateColours()
{
    this->barColour = this->recordingMode.get() ?
        this->snapsRecordingColour : this->snapsPlaybackColour;

    this->beatColour = this->barColour
        .withMultipliedAlpha(0.9f * this->roll.getBeatLineAlpha());

    this->snapColour = this->barColour
        .withMultipliedAlpha(0.75f * this->roll.getSnapLineAlpha());
}

Colour RollHeader::getBarColour() const noexcept
{
    return this->barColour;
}

void RollHeader::showRecordingMode(bool showRecordingMarker)
{
    this->recordingMode = showRecordingMarker;
    this->updateColours();
    this->repaint();
}

void RollHeader::showLoopMode(bool hasLoop, float startBeat, float endBeat)
{
    this->loopMarkerStart->setBeat(startBeat);
    this->loopMarkerEnd->setBeat(endBeat);

    this->loopMarkerStart->setVisible(hasLoop);
    this->loopMarkerEnd->setVisible(hasLoop);

    this->resized(); // updates their positions
}

void RollHeader::setSoundProbeMode(bool shouldPreviewOnClick)
{
    if (this->soundProbeMode.get() == shouldPreviewOnClick)
    {
        return;
    }
    
    this->soundProbeMode = shouldPreviewOnClick;
    
    if (this->soundProbeMode.get())
    {
        this->setMouseCursor(MouseCursor::PointingHandCursor);
    }
    else
    {
        this->pointingIndicator = nullptr;
        this->timeDistanceIndicator = nullptr;
        this->setMouseCursor(MouseCursor::NormalCursor);
    }
}

void RollHeader::updateProjectBeatRange(float firstBeat, float lastBeat)
{
    this->projectStartIndicator->updatePosition(firstBeat);
    this->projectEndIndicator->updatePosition(lastBeat);
    this->projectStartIndicator->updateBounds();
    this->projectEndIndicator->updateBounds();
}

void RollHeader::updateRollBeatRange(float viewFirstBeat, float viewLastBeat)
{
    this->projectStartIndicator->updateViewRange(viewFirstBeat, viewLastBeat);
    this->projectEndIndicator->updateViewRange(viewFirstBeat, viewLastBeat);
    this->projectStartIndicator->updateBounds();
    this->projectEndIndicator->updateBounds();
}

void RollHeader::updateClipRangeIndicators(const Clip &activeClip)
{
    const auto *pattern = activeClip.getPattern();
    if (pattern == nullptr)
    {
        jassertfalse;
        return;
    }

    if (this->clipRangeIndicators.size() != pattern->size())
    {
        this->clipRangeIndicators.clearQuick(true);

        for (int i = 0; i < pattern->size(); ++i)
        {
            auto indicator = make<ClipRangeIndicator>();
            this->addAndMakeVisible(indicator.get());
            this->clipRangeIndicators.add(move(indicator));
        }
    }

    const auto colour = pattern->getTrack()->getTrackColour();
    const auto *sequence = pattern->getTrack()->getSequence();
    const auto sequenceFirstBeat = sequence->getFirstBeat();
    const auto sequenceLastBeat = sequence->isEmpty() ?
        sequenceFirstBeat + Globals::Defaults::emptyClipLength :
        sequence->getLastBeat();

    bool hasUpdates = false;
    for (int i = 0; i < pattern->size(); ++i)
    {
        const auto *clip = pattern->getUnchecked(i);
        const auto clipBeat = clip->getBeat();
        const bool isActive = *clip == activeClip;

        if (this->clipRangeIndicators.getUnchecked(i)->updateWith(colour,
            clipBeat + sequenceFirstBeat, clipBeat + sequenceLastBeat, isActive))
        {
            hasUpdates = true;
        }
    }

    if (hasUpdates)
    {
        this->updateClipRangeIndicatorPositions();
    }
}

void RollHeader::updateSelectionRangeIndicator(const Colour &colour, float firstBeat, float lastBeat)
{
    if (this->selectionRangeIndicator == nullptr)
    {
        this->selectionRangeIndicator = make<SelectionRangeIndicator>();
        this->addAndMakeVisible(this->selectionRangeIndicator.get());
    }

    if (this->selectionRangeIndicator->updateWith(colour, firstBeat, lastBeat, true))
    {
        this->updateSelectionRangeIndicatorPosition();
    }
}

void RollHeader::updateSoundProbeIndicatorPosition(SoundProbeIndicator *indicator, const MouseEvent &e)
{
    const auto parentEvent = e.getEventRelativeTo(&this->roll);
    const float roundBeat = this->roll.getBeatByXPosition(float(parentEvent.x));
    const int roundX = this->roll.getXPositionByBeat(roundBeat);
    const auto anchor = double(roundX) / double(this->roll.getWidth());
    indicator->setAnchoredAt(anchor);
}

double RollHeader::getUnalignedAnchorForEvent(const MouseEvent &e) const
{
    const auto parentEvent = e.getEventRelativeTo(&this->roll);
    const double absX = double(parentEvent.getPosition().getX()) / double(this->roll.getWidth());
    return absX;
}

void RollHeader::updateTimeDistanceIndicator()
{
    if (this->pointingIndicator == nullptr ||
        this->probeIndicator == nullptr ||
        this->timeDistanceIndicator == nullptr)
    {
        return;
    }
    
    const double anchor1 = this->pointingIndicator->getAnchor();
    const double anchor2 = this->probeIndicator->getAnchor();
    
    const auto seek1 = this->roll.getBeatByXPosition(float(this->pointingIndicator->getX()));
    const auto seek2 = this->roll.getBeatByXPosition(float(this->probeIndicator->getX()));

    this->timeDistanceIndicator->setAnchoredBetween(anchor1, anchor2);

    const auto timeMs1 = this->transport.findTimeAt(seek1);
    const auto timeMs2 = this->transport.findTimeAt(seek2);
    
    const double timeDelta = fabs(timeMs2 - timeMs1);
    const auto timeDeltaText = Transport::getTimeString(timeDelta);
    this->timeDistanceIndicator->getTimeLabel()->setText(timeDeltaText, dontSendNotification);
}

void RollHeader::updateClipRangeIndicatorPositions()
{
    for (auto *indicator : this->clipRangeIndicators)
    {
        const int x1 = this->roll.getXPositionByBeat(indicator->getFirstBeat());
        const int x2 = this->roll.getXPositionByBeat(indicator->getLastBeat());
        indicator->setBounds(x1, 0, jmax(x2 - x1, 1), 1);
    }
}

void RollHeader::updateSelectionRangeIndicatorPosition()
{
    jassert(this->selectionRangeIndicator != nullptr);
    const bool hasClipRangesDisplayed = !this->clipRangeIndicators.isEmpty();
    const int x1 = this->roll.getXPositionByBeat(this->selectionRangeIndicator->getFirstBeat());
    const int x2 = this->roll.getXPositionByBeat(this->selectionRangeIndicator->getLastBeat());
    this->selectionRangeIndicator->setBounds(x1, hasClipRangesDisplayed ? 2 : 0, x2 - x1 + 1, 1);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void RollHeader::mouseDown(const MouseEvent &e)
{
    if (this->roll.isMultiTouchEvent(e))
    {
        return;
    }

    if (this->soundProbeMode.get())
    {
        const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x);
        this->transport.probeSoundAtBeat(roundBeat, nullptr);
        
        this->probeIndicator = make<SoundProbeIndicator>();
        this->roll.addAndMakeVisible(this->probeIndicator.get());
        this->updateSoundProbeIndicatorPosition(this->probeIndicator.get(), e);
    }
    else
    {
        const auto parentEvent = e.getEventRelativeTo(&this->roll);
        const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);

        if ((e.mods.isAnyModifierKeyDown() || this->roll.isInSelectionMode()))
        {
            this->roll.getSelectionComponent()->beginLasso({ parentEvent.position.x, 0.f }, &this->roll);
            this->selectionIndicator->fadeIn();
            this->selectionIndicator->setStartAnchor(this->getUnalignedAnchorForEvent(e));
        }
        else if (!this->transport.isPlayingAndRecording())
        {
            this->transport.stopPlayback();
            this->roll.cancelPendingUpdate(); // why is it here?

            // two presses on mobile will emit the timeline menu,
            // on the desktop it is available via right click
#if PLATFORM_MOBILE
            if (this->transport.getSeekBeat() == roundBeat)
            {
                this->showPopupMenu();
            }
            else
            {
                this->transport.seekToBeat(roundBeat);
            }
#elif PLATFORM_DESKTOP
            this->transport.seekToBeat(roundBeat);
#endif
        }
    }
}

void RollHeader::mouseDrag(const MouseEvent &e)
{
    if (this->roll.isMultiTouchEvent(e))
    {
        return;
    }

    if (this->soundProbeMode.get())
    {
        if (this->pointingIndicator != nullptr)
        {
            this->updateSoundProbeIndicatorPosition(this->pointingIndicator.get(), e);

            if (this->probeIndicator != nullptr)
            {
                const int distance = abs(this->pointingIndicator->getX() - this->probeIndicator->getX());

                if (this->timeDistanceIndicator == nullptr)
                {
                    if (distance > RollHeader::minTimeDistanceIndicatorSize)
                    {
                        this->transport.stopPlaybackAndRecording();
                        this->transport.allNotesControllersAndSoundOff();

                        this->timeDistanceIndicator = make<TimeDistanceIndicator>();
                        this->roll.addAndMakeVisible(this->timeDistanceIndicator.get());
                        this->timeDistanceIndicator->setBounds(0, this->getBottom() + 4,
                            0, this->timeDistanceIndicator->getHeight());
                        this->updateTimeDistanceIndicator();
                    }
                }
                else
                {
                    if (distance <= RollHeader::minTimeDistanceIndicatorSize)
                    {
                        this->timeDistanceIndicator = nullptr;
                    }
                    else
                    {
                        this->updateTimeDistanceIndicator();
                    }
                }
            }
        }
    }
    else
    {
        if (this->roll.getSelectionComponent()->isDragging())
        {
            const auto parentEvent = e.getEventRelativeTo(&this->roll);
            const auto parentGlobalSelection = parentEvent.withNewPosition(Point<int>(parentEvent.x, this->roll.getHeight()));
            this->roll.getSelectionComponent()->dragLasso(parentGlobalSelection);
            this->selectionIndicator->setEndAnchor(this->getUnalignedAnchorForEvent(e));
        }
        else if (!this->transport.isPlayingAndRecording())
        {
            const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
            this->transport.stopPlayback();
            this->roll.cancelPendingUpdate();
            this->transport.seekToBeat(roundBeat);
        }
    }
}

void RollHeader::mouseUp(const MouseEvent &e)
{
    if (this->roll.isMultiTouchEvent(e))
    {
        return;
    }

    this->probeIndicator = nullptr;
    this->timeDistanceIndicator = nullptr;

    this->selectionIndicator->fadeOut();
    
    if (this->soundProbeMode.get())
    {
        this->transport.allNotesControllersAndSoundOff();
        return;
    }
    
    if (this->roll.getSelectionComponent()->isDragging())
    {
        this->roll.getSelectionComponent()->endLasso();
    }
    else if (!this->transport.isPlayingAndRecording())
    {
        const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
        this->transport.stopPlayback();
        this->transport.seekToBeat(roundBeat);
        
        if (e.mods.isRightButtonDown())
        {
            this->showPopupMenu();
        }
        else if (e.mods.isMiddleButtonDown())
        {
            this->transport.startPlayback();

            if (e.mods.isAnyModifierKeyDown())
            {
                this->transport.speedUpPlayback();
            }
        }
    }
}

void RollHeader::mouseMove(const MouseEvent &e)
{
    if (this->pointingIndicator != nullptr)
    {
        this->updateSoundProbeIndicatorPosition(this->pointingIndicator.get(), e);
    }
    else if (this->soundProbeMode.get())
    {
        this->pointingIndicator = make<SoundProbeIndicator>();
        this->roll.addAndMakeVisible(this->pointingIndicator.get());
        this->updateSoundProbeIndicatorPosition(this->pointingIndicator.get(), e);
    }
}

void RollHeader::mouseExit(const MouseEvent &e)
{
    if (this->pointingIndicator != nullptr)
    {
        this->pointingIndicator = nullptr;
    }
    
    if (this->timeDistanceIndicator != nullptr)
    {
        this->timeDistanceIndicator = nullptr;
    }
}

void RollHeader::mouseDoubleClick(const MouseEvent &e)
{
#if PLATFORM_DESKTOP
    if (this->soundProbeMode.get())
    {
        return;
    }

    const float roundBeat = this->roll.getRoundBeatSnapByXPosition(e.x); // skipped e.getEventRelativeTo(*this->roll);
    this->transport.stopPlaybackAndRecording();
    this->transport.seekToBeat(roundBeat);
    this->transport.startPlayback();

    if (e.mods.isAnyModifierKeyDown())
    {
        this->transport.speedUpPlayback();
    }
#endif
}

void RollHeader::paint(Graphics &g)
{
    const int paintStartX = this->viewport.getViewPositionX();
    const int paintEndX = this->viewport.getViewPositionX() + this->viewport.getViewWidth();

    g.setColour(this->fillColour);
    g.fillRect(paintStartX, 0, paintEndX - paintStartX, Globals::UI::rollHeaderHeight);

    g.setColour(this->barColour);
    for (const auto x : this->roll.getVisibleBars())
    {
        g.fillRect(floorf(x), float(this->getHeight() - 12), 1.f, 11.f);
    }

    g.setColour(this->beatColour);
    for (const auto x : this->roll.getVisibleBeats())
    {
        g.fillRect(floorf(x), float(this->getHeight() - 8), 1.f, 7.f);
    }

    g.setColour(this->snapColour);
    for (const auto x : this->roll.getVisibleSnaps())
    {
        g.fillRect(floorf(x), float(this->getHeight() - 5), 1.f, 4.f);
    }

    g.setColour(this->bevelLightColour);
    g.fillRect(0, this->getHeight() - 2, this->getWidth(), 1);

    g.setColour(this->bevelDarkColour);
    g.fillRect(0, this->getHeight() - 1, this->getWidth(), 1);

    g.setColour(this->barColour);
    if (this->recordingMode.get())
    {
        g.fillRect(0, this->getHeight() - 3, this->getWidth(), 2);
    }
}

void RollHeader::resized()
{
    this->updateClipRangeIndicatorPositions();

    if (this->selectionRangeIndicator != nullptr)
    {
        this->updateSelectionRangeIndicatorPosition();
    }

    this->loopMarkerStart->updatePosition();
    this->loopMarkerEnd->updatePosition();

    this->projectStartIndicator->updateBounds();
    this->projectEndIndicator->updateBounds();
}

void RollHeader::showPopupMenu()
{
    ModalCallout::emit(new TimelineMenu(this->roll.getProject()), this, true);
}
