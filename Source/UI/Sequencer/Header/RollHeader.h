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

class Clip;
class RollBase;
class Transport;
class SoundProbeIndicator;
class TimeDistanceIndicator;
class HeaderSelectionIndicator;
class ClipRangeIndicator;
class PlaybackLoopMarker;
class TrackStartIndicator;
class TrackEndIndicator;

#include "ColourIDs.h"

class RollHeader final : public Component
{
public:

    RollHeader(Transport &transport, RollBase &roll, Viewport &viewport);
    ~RollHeader() override;
    
    void setSoundProbeMode(bool shouldProbeOnClick);

    void showPopupMenu();
    void showRecordingMode(bool showRecordingMarker);
    void showLoopMode(bool hasLoop, float startBeat, float endBeat);

    void updateProjectBeatRange(float projectFirstBeat, float projectLastBeat);
    void updateRollBeatRange(float viewFirstBeat, float viewLastBeat);
    
    void updateClipRangeIndicators(const Clip &activeClip);
    void updateSelectionRangeIndicator(const Colour &colour, float firstBeat, float lastBeat);

    void updateColours();
    Colour getBarColour() const noexcept;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void mouseUp(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseMove(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;
    void mouseDoubleClick(const MouseEvent &e) override;
    void paint(Graphics &g) override;
    void resized() override;

protected:
    
    Transport &transport;
    RollBase &roll;
    Viewport &viewport;

    Atomic<bool> soundProbeMode = false;
    Atomic<bool> recordingMode = false;

    const Colour fillColour = findDefaultColour(ColourIDs::Roll::headerFill);
    const Colour bevelDarkColour = findDefaultColour(ColourIDs::Common::borderLineDark);
    const Colour bevelLightColour = findDefaultColour(ColourIDs::Roll::headerBorder);

    const Colour snapsRecordingColour = findDefaultColour(ColourIDs::Roll::headerRecording);
    const Colour snapsPlaybackColour = findDefaultColour(ColourIDs::Roll::headerSnaps);

    Colour barColour;
    Colour beatColour;
    Colour snapColour;

    OwnedArray<ClipRangeIndicator> clipRangeIndicators;

    UniquePointer<ClipRangeIndicator> selectionRangeIndicator;

    UniquePointer<SoundProbeIndicator> probeIndicator;
    UniquePointer<SoundProbeIndicator> pointingIndicator;
    UniquePointer<TimeDistanceIndicator> timeDistanceIndicator;
    UniquePointer<HeaderSelectionIndicator> selectionIndicator;

    UniquePointer<PlaybackLoopMarker> loopMarkerStart;
    UniquePointer<PlaybackLoopMarker> loopMarkerEnd;

    UniquePointer<TrackStartIndicator> projectStartIndicator;
    UniquePointer<TrackEndIndicator> projectEndIndicator;

    static constexpr auto minTimeDistanceIndicatorSize = 40;

    void updateSoundProbeIndicatorPosition(SoundProbeIndicator *indicator, const MouseEvent &e);
    double getUnalignedAnchorForEvent(const MouseEvent &e) const;
    void updateTimeDistanceIndicator();
    void updateClipRangeIndicatorPositions();
    void updateSelectionRangeIndicatorPosition();
};
