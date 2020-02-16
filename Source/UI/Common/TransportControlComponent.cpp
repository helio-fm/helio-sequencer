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

#include "TransportControlComponent.h"

//[MiscUserDefs]
#include "MainLayout.h"
#include "HelioTheme.h"
#include "ColourIDs.h"
#include "CommandIDs.h"

class TransportControlRecordBg final : public Component
{
public:

    TransportControlRecordBg()
    {
        this->setOpaque(false);
        this->setWantsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
    }

    void resized() override
    {
        this->path.clear();
        this->path.startNewSubPath(0.0f, 0.0f);
        this->path.lineTo(static_cast<float> (getWidth()), 0.0f);
        this->path.lineTo(static_cast<float> (getWidth()), 30.0f);
        this->path.lineTo(0.0f, 36.0f);
        this->path.closeSubPath();
    }

    bool hitTest(int x, int y) override
    {
        return this->path.contains(float(x), float(y));
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->fillColour);
        g.fillPath(this->path, {});

        g.setColour(Colours::black.withAlpha(0.1f));
        g.drawLine(0.f, 35.f, float(this->getWidth()), 29.f);
    }

    void mouseEnter(const MouseEvent &e) override
    {
        this->fillColour = Colours::mediumvioletred.withAlpha(0.3f);
        this->repaint();
    }

    void mouseExit(const MouseEvent &e) override
    {
        this->fillColour = Colours::transparentBlack;
        this->repaint();
    }

private:

    Path path;
    Colour fillColour = Colours::transparentBlack;

};

class TransportControlPlayBg final : public Component
{
public:

    TransportControlPlayBg()
    {
        this->setOpaque(false);
        this->setWantsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
    }

    void resized() override
    {
        this->path.clear();
        this->path.startNewSubPath(0.0f, static_cast<float> (getHeight() - 42));
        this->path.lineTo(static_cast<float> (getWidth()), static_cast<float> (getHeight() - 48));
        this->path.lineTo(static_cast<float> (getWidth()), static_cast<float> (getHeight()));
        this->path.lineTo(0.0f, static_cast<float> (getHeight()));
        this->path.closeSubPath();
    }

    bool hitTest(int x, int y) override
    {
        return this->path.contains(float(x), float(y));
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->fillColour);
        g.fillPath(this->path, {});
        g.drawLine(0.f, float(this->getHeight() - 42), float(this->getWidth()), float(this->getHeight() - 48));
    }

    void mouseEnter(const MouseEvent &e) override
    {
        this->fillColour = Colour(0x20ffffff);
        this->repaint();
    }

    void mouseExit(const MouseEvent &e) override
    {
        this->fillColour = Colour(0x0cffffff);
        this->repaint();
    }

private:

    Path path;
    Colour fillColour = Colour(0x0cffffff);

};


//[/MiscUserDefs]

TransportControlComponent::TransportControlComponent(WeakReference<Component> eventReceiver)
    : eventReceiver(eventReceiver)
{
    this->recordBg.reset(new TransportControlRecordBg());
    this->addAndMakeVisible(recordBg.get());

    this->playBg.reset(new TransportControlPlayBg());
    this->addAndMakeVisible(playBg.get());

    this->playIcon.reset(new IconComponent(Icons::play));
    this->addAndMakeVisible(playIcon.get());

    this->stopIcon.reset(new IconComponent(Icons::stop));
    this->addAndMakeVisible(stopIcon.get());

    this->recordIcon.reset(new IconComponent(Icons::record));
    this->addAndMakeVisible(recordIcon.get());


    //[UserPreSize]
    this->playIcon->setVisible(true);
    this->stopIcon->setVisible(false);
    this->playIcon->setInterceptsMouseClicks(false, false);
    this->stopIcon->setInterceptsMouseClicks(false, false);
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);
    this->setOpaque(false);
    //[/UserPreSize]

    this->setSize(44, 78);

    //[Constructor]
    //[/Constructor]
}

TransportControlComponent::~TransportControlComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    recordBg = nullptr;
    playBg = nullptr;
    playIcon = nullptr;
    stopIcon = nullptr;
    recordIcon = nullptr;

    //[Destructor]
    //[/Destructor]
}

void TransportControlComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void TransportControlComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    recordBg->setBounds(0, 0, getWidth() - 0, 36);
    playBg->setBounds(0, getHeight() - 48, getWidth() - 0, 48);
    playIcon->setBounds((getWidth() / 2) + 2 - (24 / 2), (getHeight() / 2) + 16 - (24 / 2), 24, 24);
    stopIcon->setBounds((getWidth() / 2) - (22 / 2), (getHeight() / 2) + 16 - (22 / 2), 22, 22);
    recordIcon->setBounds((getWidth() / 2) - (18 / 2), 7, 18, 18);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void TransportControlComponent::mouseDown (const MouseEvent& e)
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

void TransportControlComponent::setPlaying(bool isPlaying)
{
    this->playing = isPlaying;

    if (this->playing)
    {
        MessageManagerLock lock;
        this->animator.fadeIn(this->stopIcon.get(), 100);
        this->animator.fadeOut(this->playIcon.get(), 100);
    }
    else
    {
        MessageManagerLock lock;
        this->animator.fadeIn(this->playIcon.get(), 150);
        this->animator.fadeOut(this->stopIcon.get(), 150);
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TransportControlComponent"
                 template="../../Template" componentName="" parentClasses="public Component"
                 constructorParams="WeakReference&lt;Component&gt; eventReceiver"
                 variableInitialisers="eventReceiver(eventReceiver)" snapPixels="8"
                 snapActive="1" snapShown="1" overlayOpacity="0.330" fixedSize="1"
                 initialWidth="44" initialHeight="78">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="0" nonZeroWinding="1">s 0 0 l 0R 0 l 0R 30 l 0 36 x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="0" nonZeroWinding="1">s 0 42R l 0R 48R l 0R 0R l 0 0R x</PATH>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="49831a57350a131d" memberName="recordBg" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 36" class="TransportControlRecordBg"
                    params=""/>
  <GENERICCOMPONENT name="" id="e71c47548b70a0b4" memberName="playBg" virtualName=""
                    explicitFocusOrder="0" pos="0 0Rr 0M 48" class="TransportControlPlayBg"
                    params=""/>
  <GENERICCOMPONENT name="" id="1a8a31abbc0f3c4e" memberName="playIcon" virtualName=""
                    explicitFocusOrder="0" pos="2Cc 16Cc 24 24" class="IconComponent"
                    params="Icons::play"/>
  <GENERICCOMPONENT name="" id="f10feab7d241bacb" memberName="stopIcon" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 16Cc 22 22" class="IconComponent"
                    params="Icons::stop"/>
  <GENERICCOMPONENT name="" id="554d8489a0447eda" memberName="recordIcon" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 7 18 18" class="IconComponent"
                    params="Icons::record"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



