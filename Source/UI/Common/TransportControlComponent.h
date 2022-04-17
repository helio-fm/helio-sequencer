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

class TransportControlPlayBg;
class TransportControlRecordBg;

#include "IconComponent.h"
#include "CommandIDs.h"

class TransportControlComponent final : public Component
{
public:

    explicit TransportControlComponent(WeakReference<Component> eventReceiver);
    ~TransportControlComponent();

    void showPlayingMode(bool isPlaying);
    void showRecordingMode(bool isRecording);
    void showRecordingError();
    void showRecordingMenu(const Array<MidiDeviceInfo> &devices);

    void resized() override;

private:

    WeakReference<Component> eventReceiver;

    Atomic<bool> isPlaying = false;
    Atomic<bool> isRecording = false;

    static constexpr auto playButtonSize = 48;
    static constexpr auto recordButtonSize = 33;
    static constexpr auto buttonSkew = 7;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportControlComponent)
};
