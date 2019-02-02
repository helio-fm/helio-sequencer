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

//[Headers]
#include "Common.h"
//[/Headers]

#include "PlayButton.h"

//[MiscUserDefs]
#include "MainLayout.h"
#include "HelioTheme.h"
#include "ColourIDs.h"
#include "CommandIDs.h"

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
        const Colour colour1(findDefaultColour(ColourIDs::Icons::fill).withAlpha(0.1f));
        const int h = this->getHeight();
        const Rectangle<float> r(this->getLocalBounds()
                                 .withSizeKeepingCentre(h, h)
                                 .reduced(3)
                                 .toFloat());

        HelioTheme::drawDashedRectangle(g, r, colour1, 5.5f, 1.0f, 0.5f, float(h / 2));
    }
};
//[/MiscUserDefs]

PlayButton::PlayButton(WeakReference<Component> eventReceiver)
    : HighlightedComponent(),
      eventReceiver(eventReceiver),
      playing(false)
{
    this->playIcon.reset(new IconComponent(Icons::play));
    this->addAndMakeVisible(playIcon.get());
    playIcon->setName ("playIcon");

    this->pauseIcon.reset(new IconComponent(Icons::pause));
    this->addAndMakeVisible(pauseIcon.get());
    pauseIcon->setName ("pauseIcon");


    //[UserPreSize]
    this->playIcon->setVisible(true);
    this->pauseIcon->setVisible(false);
    this->playIcon->setInterceptsMouseClicks(false, false);
    this->pauseIcon->setInterceptsMouseClicks(false, false);
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->setOpaque(false);
    //[/UserPreSize]

    this->setSize(64, 64);

    //[Constructor]
    //[/Constructor]
}

PlayButton::~PlayButton()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    playIcon = nullptr;
    pauseIcon = nullptr;

    //[Destructor]
    //[/Destructor]
}

void PlayButton::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void PlayButton::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    playIcon->setBounds((getWidth() / 2) + 1 - ((getWidth() - 24) / 2), (getHeight() / 2) - ((getHeight() - 24) / 2), getWidth() - 24, getHeight() - 24);
    pauseIcon->setBounds((getWidth() / 2) + -1 - ((getWidth() - 24) / 2), (getHeight() / 2) - ((getHeight() - 24) / 2), getWidth() - 24, getHeight() - 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void PlayButton::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    const auto command = this->playing ?
        CommandIDs::TransportPausePlayback :
        CommandIDs::TransportStartPlayback;

    if (this->eventReceiver != nullptr)
    {
        this->eventReceiver->postCommandMessage(command);
    }
    else
    {
        App::Layout().broadcastCommandMessage(command);
    }
    //[/UserCode_mouseDown]
}


//[MiscUserCode]

void PlayButton::setPlaying(bool isPlaying)
{
    this->playing = isPlaying;

    if (this->playing)
    {
        MessageManagerLock lock;
        this->animator.fadeIn(this->pauseIcon.get(), 100);
        this->animator.fadeOut(this->playIcon.get(), 100);
    }
    else
    {
        MessageManagerLock lock;
        this->animator.fadeIn(this->playIcon.get(), 150);
        this->animator.fadeOut(this->pauseIcon.get(), 150);
    }
}

Component *PlayButton::createHighlighterComponent()
{
    return new PlayButtonHighlighter();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PlayButton" template="../../Template"
                 componentName="" parentClasses="public HighlightedComponent"
                 constructorParams="WeakReference&lt;Component&gt; eventReceiver"
                 variableInitialisers="HighlightedComponent(),&#10;eventReceiver(eventReceiver),&#10;playing(false)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="64" initialHeight="64">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="playIcon" id="1a8a31abbc0f3c4e" memberName="playIcon" virtualName=""
                    explicitFocusOrder="0" pos="1Cc 0Cc 24M 24M" class="IconComponent"
                    params="Icons::play"/>
  <GENERICCOMPONENT name="pauseIcon" id="f10feab7d241bacb" memberName="pauseIcon"
                    virtualName="" explicitFocusOrder="0" pos="-1Cc 0Cc 24M 24M"
                    class="IconComponent" params="Icons::pause"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
