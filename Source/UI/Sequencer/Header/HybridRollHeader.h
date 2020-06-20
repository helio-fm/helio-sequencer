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

class HybridRoll;
class Transport;
class SoundProbeIndicator;
class TimeDistanceIndicator;
class HeaderSelectionIndicator;
class ClipRangeIndicator;

class HybridRollHeader final : public Component
{
public:

    HybridRollHeader(Transport &transport, HybridRoll &roll, Viewport &viewport);
    
    void setSoundProbeMode(bool shouldProbeOnClick);

    void showPopupMenu();
    void showRecordingMode(bool showRecordingMarker);
    void showLoopMode(bool hasLoop, float startBeat, float endBeat);
    void updateSubrangeIndicator(const Colour &colour, float firstBeat, float lastBeat);

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
    HybridRoll &roll;
    Viewport &viewport;

    Atomic<bool> soundProbeMode = false;
    Atomic<bool> recordingMode = false;

    Atomic<bool> loopMode = false;
    Atomic<float> loopStartBeat = 0.f;
    Atomic<float> loopEndBeat = Globals::projectDefaultNumBeats;

    Colour backColour;
    Colour barColour;
    Colour barShadeColour;
    Colour beatColour;
    Colour snapColour;
    Colour bevelLightColour;
    Colour bevelDarkColour;
    Colour recordingColour;

    void updateColours();

    UniquePointer<ClipRangeIndicator> clipRangeIndicator;
    UniquePointer<SoundProbeIndicator> playingIndicator;
    UniquePointer<SoundProbeIndicator> pointingIndicator;
    UniquePointer<TimeDistanceIndicator> timeDistanceIndicator;
    UniquePointer<HeaderSelectionIndicator> selectionIndicator;

    void updateIndicatorPosition(SoundProbeIndicator *indicator, const MouseEvent &e);
    double getUnalignedAnchorForEvent(const MouseEvent &e) const;
    double getAlignedAnchorForEvent(const MouseEvent &e) const;
    void updateTimeDistanceIndicator();
    void updateClipRangeIndicator();

};
