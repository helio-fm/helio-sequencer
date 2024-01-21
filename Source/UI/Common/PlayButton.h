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

#include "IconComponent.h"
#include "HighlightedComponent.h"

class PlayButton final : public HighlightedComponent
{
public:

    PlayButton(WeakReference<Component> eventReceiver);
    ~PlayButton();

    void setPlaying(bool isPlaying);

    void resized() override;
    void mouseDown(const MouseEvent &e) override;

    void enablementChanged() override
    {
        this->setAlpha(this->isEnabled() ? 1.0f : 0.5f);
    }

private:

    Component *createHighlighterComponent() override;

    ComponentAnimator animator;
    WeakReference<Component> eventReceiver;
    bool playing = false;

    UniquePointer<IconComponent> playIcon;
    UniquePointer<IconComponent> stopIcon;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayButton)
};
