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
#include "PlayButton.h"

#include "MainLayout.h"
#include "HelioTheme.h"
#include "ColourIDs.h"

class PlayButtonHighlighter final : public Component
{
public:

    PlayButtonHighlighter()
    {
        this->setInterceptsMouseClicks(false, false);
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        const int size = jmin(this->getWidth(), this->getHeight());
        const auto bounds = this->getLocalBounds()
            .withSizeKeepingCentre(size, size)
            .reduced(3).toFloat();

        g.setColour(findDefaultColour(ColourIDs::Icons::fill).withAlpha(0.2f));
        g.drawEllipse(bounds, 1.f);
    }
};

PlayButton::PlayButton(WeakReference<Component> eventReceiver) :
    HighlightedComponent(),
    eventReceiver(eventReceiver)
{
    this->setOpaque(false);
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->playIcon = make<IconComponent>(Icons::play);
    this->addAndMakeVisible(this->playIcon.get());
    this->playIcon->setInterceptsMouseClicks(false, false);

    this->pauseIcon = make<IconComponent>(Icons::pause);
    this->addChildComponent(this->pauseIcon.get()); // not visible
    this->pauseIcon->setInterceptsMouseClicks(false, false);

    this->setSize(64, 64);
}

PlayButton::~PlayButton() = default;

void PlayButton::resized()
{
    const auto buttonSize = jmin(this->getWidth() / 2, this->getHeight() / 2);

    this->playIcon->setBounds(this->getLocalBounds()
        .withSizeKeepingCentre(buttonSize, buttonSize).translated(1, 0));

    this->pauseIcon->setBounds(this->getLocalBounds()
        .withSizeKeepingCentre(buttonSize, buttonSize));
}

void PlayButton::mouseDown(const MouseEvent &e)
{
    const auto command = this->playing ?
        CommandIDs::TransportStop :
        CommandIDs::TransportPlaybackStart;

    if (this->eventReceiver != nullptr)
    {
        this->eventReceiver->postCommandMessage(command);
    }
    else
    {
        App::Layout().broadcastCommandMessage(command);
    }
}

void PlayButton::setPlaying(bool isPlaying)
{
    this->playing = isPlaying;

    if (this->playing)
    {
        MessageManagerLock lock;
        this->animator.fadeIn(this->pauseIcon.get(), Globals::UI::fadeInShort);
        this->animator.fadeOut(this->playIcon.get(), Globals::UI::fadeOutShort);
    }
    else
    {
        MessageManagerLock lock;
        this->animator.fadeIn(this->playIcon.get(), Globals::UI::fadeInLong);
        this->animator.fadeOut(this->pauseIcon.get(), Globals::UI::fadeOutLong);
    }
}

Component *PlayButton::createHighlighterComponent()
{
    return new PlayButtonHighlighter();
}
