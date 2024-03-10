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

class TransportControlPlayBg;
class TransportControlRecordBg;
class LongTapController;

#include "LongTapListener.h"
#include "IconComponent.h"
#include "CommandIDs.h"

class TransportControlComponent final : public Component, public LongTapListener
{
public:

    explicit TransportControlComponent(WeakReference<Component> eventReceiver);
    ~TransportControlComponent();

    void showPlayingMode(bool isPlaying);
    void showRecordingMode(bool isRecording);
    void showRecordingError();
    void showRecordingMenu(const Array<MidiDeviceInfo> &devices);

    void resized() override;

    void onLongTap(const Point<float> &position,
        const WeakReference<Component> &target) override;
private:

    WeakReference<Component> eventReceiver;

    Atomic<bool> isPlaying = false;
    Atomic<bool> isRecording = false;

    static constexpr auto buttonSkew = 6;
    static constexpr auto playButtonSize =
        int(Globals::UI::projectMapHeight * 0.6) + buttonSkew / 2;
    static constexpr auto recordButtonSize =
        Globals::UI::projectMapHeight - playButtonSize + buttonSkew / 2 + 1;

    friend class TransportControlRecordBg;
    friend class TransportControlPlayBg;

    void playButtonPressed();
    void recordButtonPressed();
    void broadcastCommandMessage(CommandIDs::Id command);

    UniquePointer<TransportControlRecordBg> recordBg;
    UniquePointer<TransportControlPlayBg> playBg;
    UniquePointer<IconComponent> playIcon;
    UniquePointer<IconComponent> stopIcon;
    UniquePointer<IconComponent> recordIcon;

    ComponentAnimator animator;
    UniquePointer<Timer> recordButtonBlinkAnimator;

    UniquePointer<LongTapController> longTapController;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportControlComponent)
};
