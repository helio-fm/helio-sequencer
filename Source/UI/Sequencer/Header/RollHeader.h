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

#include "Lasso.h"
#include "SelectionComponent.h"
#include "ColourIDs.h"

class RollHeader final :
    public Component,
    public DrawableLassoSource<SelectableComponent *> // selecting timeline events
{
public:

    RollHeader(Transport &transport, RollBase &roll, Viewport &viewport) noexcept;
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
    Colour getRepriseColour() const noexcept;

    //===------------------------------------------------------------------===//
    // DrawableLassoSource
    //===------------------------------------------------------------------===//

    Lasso &getLassoSelection() override;
    Point<float> getLassoAnchor(const Point<float> &position) const override;
    Point<int> getLassoPosition(const Point<float> &anchorPoint) const override;
    void findLassoItemsInArea(Array<SelectableComponent *> &itemsFound,
        const Rectangle<int> &bounds) override;
    void findLassoItemsInPolygon(Array<SelectableComponent *> &itemsFound,
        const Rectangle<int> &bounds, const Array<Point<float>> &polygon) override;

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

private:

    Transport &transport;
    RollBase &roll;
    Viewport &viewport;

    Atomic<bool> soundProbeMode = false;
    Atomic<bool> recordingMode = false;

    void updateSoundProbeIndicatorPosition(SoundProbeIndicator *indicator, const MouseEvent &e);
    double getUnalignedAnchorForEvent(const MouseEvent &e) const;
    void updateTimeDistanceIndicator();
    void updateClipRangeIndicatorPositions();
    void updateSelectionRangeIndicatorPosition();

    OwnedArray<ClipRangeIndicator> clipRangeIndicators;
    UniquePointer<ClipRangeIndicator> selectionRangeIndicator;

    UniquePointer<SoundProbeIndicator> probeIndicator;
    UniquePointer<SoundProbeIndicator> pointingIndicator;
    UniquePointer<TimeDistanceIndicator> timeDistanceIndicator;

    UniquePointer<PlaybackLoopMarker> loopMarkerStart;
    UniquePointer<PlaybackLoopMarker> loopMarkerEnd;

    UniquePointer<TrackStartIndicator> projectStartIndicator;
    UniquePointer<TrackEndIndicator> projectEndIndicator;

    struct HeaderSelectionComponent final : public SelectionComponent
    {
        void paint(Graphics &g) override
        {
            g.setColour(this->fillColour);
            g.fillRect(0, this->getHeight() - 2, this->getWidth(), 2);
            g.fillRect(1, this->getHeight() - 3, jmax(0, this->getWidth() - 2), 1);
        }

        const Colour fillColour = findDefaultColour(ColourIDs::RollHeader::selection);
    };

    UniquePointer<HeaderSelectionIndicator> headerSelectionIndicator;
    UniquePointer<HeaderSelectionComponent> keysSelectionComponent;

    Colour barColour;
    Colour beatColour;
    Colour snapColour;
    Colour repriseColour;

    const Colour fillColour = findDefaultColour(ColourIDs::Roll::headerFill);
    const Colour bevelDarkColour = findDefaultColour(ColourIDs::Common::borderLineDark);
    const Colour bevelLightColour = findDefaultColour(ColourIDs::Roll::headerBorder);
    const Colour snapsPlaybackColour = findDefaultColour(ColourIDs::Roll::headerSnaps);
    const Colour reprisePlaybackColour = findDefaultColour(ColourIDs::Roll::headerReprise);
    const Colour recordingColour = findDefaultColour(ColourIDs::Roll::headerRecording);

    static constexpr auto minTimeDistanceIndicatorSize = 40;
};
