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
#include "Workspace.h"
#include "AudioCore.h"

#include "MainLayout.h"
#include "HelioTheme.h"

#include "MenuPanel.h"
#include "HelioCallout.h"
#include "ColourIDs.h"

class TransportControlButton : public Component, private Timer
{
public:

    TransportControlButton(TransportControlComponent &owner) : owner(owner)
    {
        this->setOpaque(false);
        this->setWantsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);
    }

    bool hitTest(int x, int y) override
    {
        return this->path.contains(float(x), float(y));
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->currentColour);
        g.fillPath(this->path, {});
    }

    void mouseEnter(const MouseEvent &e) override
    {
        if (this->state != State::Active)
        {
            this->setHighlighted();
        }
    }

    void mouseExit(const MouseEvent &e) override
    {
        if (this->state != State::Active)
        {
            this->setInactive();
        }
    }

    void setInactive()
    {
        this->state = State::Inactive;
        this->targetColour = this->inactiveColour;
        this->startTimerHz(60);
    }

    void setHighlighted()
    {
        this->state = State::Highlighted;
        this->targetColour = this->highlightColour;
        this->startTimerHz(60);
    }

    void setActive()
    {
        this->state = State::Active;
        this->targetColour = this->activeColour;
        this->startTimerHz(60);
    }

protected:

    Path path;

    Colour inactiveColour;
    Colour highlightColour;
    Colour activeColour;

    Colour currentColour;

    TransportControlComponent &owner;

private:

    enum class State : uint8
    {
        Inactive,
        Highlighted,
        Active
    };

    State state = State::Inactive;
    Colour targetColour;

    void timerCallback() override
    {
        const auto newColour = this->currentColour.interpolatedWith(this->targetColour, 0.3f);
        //DBG(this->currentColour.toDisplayString(true));

        if (this->currentColour == newColour)
        {
            //DBG("--- stop timer");
            this->stopTimer();
        }
        else
        {
            this->currentColour = newColour;
            this->repaint();
        }
    }

    JUCE_DECLARE_WEAK_REFERENCEABLE(TransportControlButton)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportControlButton)
};

class TransportControlRecordBg final : public TransportControlButton
{
public:

    TransportControlRecordBg(TransportControlComponent &owner) :
        TransportControlButton(owner)
    {
        this->inactiveColour = findDefaultColour(ColourIDs::TransportControl::recordInactive);
        this->highlightColour = findDefaultColour(ColourIDs::TransportControl::recordHighlight);
        this->activeColour = findDefaultColour(ColourIDs::TransportControl::recordActive);
        this->lineColour = findDefaultColour(ColourIDs::Common::borderLineDark);
        this->currentColour = this->inactiveColour;
    }

    inline float getTiltStart() const noexcept { return 28.f; }
    inline float getTiltEnd() const noexcept { return 35.f; }

    void resized() override
    {
        this->path.clear();
        this->path.startNewSubPath(0.0f, 0.0f);
        this->path.lineTo(float(this->getWidth()), 0.0f);
        this->path.lineTo(float(this->getWidth()), this->getTiltStart());
        this->path.lineTo(0.0f, this->getTiltEnd());
        this->path.closeSubPath();
    }

    void paint(Graphics &g) override
    {
        TransportControlButton::paint(g);
        g.setColour(this->lineColour);
        g.drawLine(0.f, this->getTiltEnd(), float(this->getWidth()), this->getTiltStart());
    }

    void mouseDown(const MouseEvent &e)
    {
        TransportControlButton::mouseDown(e);
        this->owner.recordButtonPressed();
    }

    Colour lineColour;
};

class TransportControlPlayBg final : public TransportControlButton
{
public:

    TransportControlPlayBg(TransportControlComponent &owner) :
        TransportControlButton(owner)
    {
        this->inactiveColour = findDefaultColour(ColourIDs::TransportControl::playInactive);
        this->highlightColour = findDefaultColour(ColourIDs::TransportControl::playHighlight);
        this->activeColour = findDefaultColour(ColourIDs::TransportControl::playActive);
        this->lineColour = findDefaultColour(ColourIDs::Common::borderLineLight);
        this->currentColour = this->inactiveColour;
    }

    inline float getTiltStart() const noexcept
    {
        return float(this->getHeight() - 43);
    }

    inline float getTiltEnd() const noexcept
    {
        return float(this->getHeight() - 50);
    }

    void resized() override
    {
        this->path.clear();
        this->path.startNewSubPath(0.0f, this->getTiltStart());
        this->path.lineTo(float(this->getWidth()), this->getTiltEnd());
        this->path.lineTo(float(this->getWidth()), float(this->getHeight()));
        this->path.lineTo(0.0f, float(this->getHeight()));
        this->path.closeSubPath();
    }

    void paint(Graphics &g) override
    {
        TransportControlButton::paint(g);
        g.setColour(this->lineColour);
        g.drawLine(0.f, this->getTiltStart(), float(this->getWidth()), this->getTiltEnd());
    }

    void mouseDown(const MouseEvent &e)
    {
        TransportControlButton::mouseDown(e);
        this->owner.playButtonPressed();
    }

    Colour lineColour;
};

class RecordButtonBlinkAnimator final : public Timer
{
public:

    explicit RecordButtonBlinkAnimator(WeakReference<TransportControlButton> button) :
        button(button) {}

    void timerCallback() override
    {
        if (this->state)
        {
            this->button->setActive();
        }
        else
        {
            this->button->setInactive();
        }

        this->blinks++;
        this->state = !this->state;

        constexpr auto numBlinks = 3;
        if (this->blinks > numBlinks * 2)
        {
            this->blinks = 0;
            this->state = false;
            this->stopTimer();
        }
    }

private:

    int blinks = 0;
    bool state = false;
    WeakReference<TransportControlButton> button;

};


//[/MiscUserDefs]

TransportControlComponent::TransportControlComponent(WeakReference<Component> eventReceiver)
    : eventReceiver(eventReceiver)
{
    this->recordBg.reset(new TransportControlRecordBg(*this));
    this->addAndMakeVisible(recordBg.get());

    this->playBg.reset(new TransportControlPlayBg(*this));
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

    this->setSize(44, 79);

    //[Constructor]
    this->recordButtonBlinkAnimator = makeUnique<RecordButtonBlinkAnimator>(this->recordBg.get());
    //[/Constructor]
}

TransportControlComponent::~TransportControlComponent()
{
    //[Destructor_pre]
    this->recordButtonBlinkAnimator = nullptr;
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

    recordBg->setBounds(0, 0, getWidth() - 0, 35);
    playBg->setBounds(0, getHeight() - 50, getWidth() - 0, 50);
    playIcon->setBounds((getWidth() / 2) + 2 - (24 / 2), (getHeight() / 2) + 15 - (24 / 2), 24, 24);
    stopIcon->setBounds((getWidth() / 2) - (22 / 2), (getHeight() / 2) + 15 - (22 / 2), 22, 22);
    recordIcon->setBounds((getWidth() / 2) - (18 / 2), 7, 18, 18);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void TransportControlComponent::mouseDown (const MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    //[/UserCode_mouseDown]
}


//[MiscUserCode]

void TransportControlComponent::showPlayingMode(bool isPlaying)
{
    this->isPlaying = isPlaying;

    if (this->isPlaying.get())
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

    if (isPlaying)
    {
        this->playBg->setActive();
    }
    else
    {
        this->playBg->setInactive();
    }
}

void TransportControlComponent::showRecordingMode(bool isRecording)
{
    // stop the animation if any
    if (this->recordButtonBlinkAnimator->isTimerRunning())
    {
        this->recordButtonBlinkAnimator->stopTimer();
    }

    this->isRecording = isRecording;
    if (isRecording)
    {
        this->recordBg->setActive();
    }
    else
    {
        this->recordBg->setInactive();
    }
}

void TransportControlComponent::showRecordingError()
{
    this->recordBg->setActive(); // the initial "blink"
    this->recordButtonBlinkAnimator->startTimer(100);
}

void TransportControlComponent::showRecordingMenu(const Array<MidiDeviceInfo> &devices)
{
    jassert(devices.size() > 1);

    // if more than 1 devices available, provide a choice
    MenuPanel::Menu menu;
    auto panel = makeUnique<MenuPanel>();

    for (const auto &midiInput : devices)
    {
        menu.add(MenuItem::item(Icons::piano, midiInput.name)->
            closesMenu()->
            withAction([this, midiInput]()
        {
            App::Workspace().getAudioCore().getDevice()
                .setMidiInputDeviceEnabled(midiInput.identifier, true);

            this->recordButtonPressed();
        }));
    }

    panel->updateContent(menu, MenuPanel::SlideLeft, true);

    HelioCallout::emit(panel.release(), this->recordBg.get());
}

// button callbacks:

void TransportControlComponent::playButtonPressed()
{
    this->broadcastCommandMessage(this->isPlaying.get() ?
        CommandIDs::TransportStop :
        CommandIDs::TransportPlaybackStart);
}

void TransportControlComponent::recordButtonPressed()
{
    this->broadcastCommandMessage(this->isRecording.get() ?
        CommandIDs::TransportStop :
        CommandIDs::TransportRecordingAwait);
}

void TransportControlComponent::broadcastCommandMessage(CommandIDs::Id command)
{
    if (this->eventReceiver != nullptr)
    {
        this->eventReceiver->postCommandMessage(command);
    }
    else
    {
        App::Layout().broadcastCommandMessage(command);
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
                 initialWidth="44" initialHeight="79">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="0" nonZeroWinding="1">s 0 0 l 0R 0 l 0R 30 l 0 36 x</PATH>
    <PATH pos="0 0 100 100" fill="solid: 0" hasStroke="0" nonZeroWinding="1">s 0 42R l 0R 48R l 0R 0R l 0 0R x</PATH>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="49831a57350a131d" memberName="recordBg" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 35" class="TransportControlRecordBg"
                    params="*this"/>
  <GENERICCOMPONENT name="" id="e71c47548b70a0b4" memberName="playBg" virtualName=""
                    explicitFocusOrder="0" pos="0 0Rr 0M 50" class="TransportControlPlayBg"
                    params="*this"/>
  <GENERICCOMPONENT name="" id="1a8a31abbc0f3c4e" memberName="playIcon" virtualName=""
                    explicitFocusOrder="0" pos="2Cc 15Cc 24 24" class="IconComponent"
                    params="Icons::play"/>
  <GENERICCOMPONENT name="" id="f10feab7d241bacb" memberName="stopIcon" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 15Cc 22 22" class="IconComponent"
                    params="Icons::stop"/>
  <GENERICCOMPONENT name="" id="554d8489a0447eda" memberName="recordIcon" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 7 18 18" class="IconComponent"
                    params="Icons::record"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



