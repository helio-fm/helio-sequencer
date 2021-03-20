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

#include "Common.h"
#include "TransportControlComponent.h"

#include "Workspace.h"
#include "AudioCore.h"
#include "MainLayout.h"
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

#if PLATFORM_DESKTOP

        App::Layout().showTooltip(this->getTooltipText(),
            MainLayout::TooltipIcon::None, Globals::UI::tooltipDelayMs);

#endif
    }

    void mouseExit(const MouseEvent &e) override
    {
        if (this->state != State::Active)
        {
            this->setInactive();
        }

#if PLATFORM_DESKTOP

        App::Layout().hideTooltipIfAny();

#endif
    }

    void mouseDown(const MouseEvent &e) override
    {
#if PLATFORM_DESKTOP

        App::Layout().hideTooltipIfAny();

#endif
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

    virtual const String &getTooltipText() const = 0;

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

    explicit TransportControlRecordBg(TransportControlComponent &owner) :
        TransportControlButton(owner),
        lineColour(findDefaultColour(ColourIDs::Common::borderLineDark)),
        tooltipText(MenuItem::createTooltip(TRANS(I18n::Tooltips::recordingMode), CommandIDs::TransportRecordingAwait))
    {
        this->inactiveColour = findDefaultColour(ColourIDs::TransportControl::recordInactive);
        this->highlightColour = findDefaultColour(ColourIDs::TransportControl::recordHighlight);
        this->activeColour = findDefaultColour(ColourIDs::TransportControl::recordActive);
        this->currentColour = this->inactiveColour;
    }

    static constexpr float getTiltStart()
    {
        return float(TransportControlComponent::recordButtonSize -
            TransportControlComponent::buttonSkew);
    }

    static constexpr float getTiltEnd()
    {
        return float(TransportControlComponent::recordButtonSize);
    }

    void resized() override
    {
        this->path.clear();
        this->path.startNewSubPath(0.f, 0.f);
        this->path.lineTo(float(this->getWidth()), 0.f);
        this->path.lineTo(float(this->getWidth()), TransportControlRecordBg::getTiltStart());
        this->path.lineTo(0.f, TransportControlRecordBg::getTiltEnd());
        this->path.closeSubPath();
    }

    void paint(Graphics &g) override
    {
        TransportControlButton::paint(g);
        g.setColour(this->lineColour);
        g.drawLine(0.f,
            TransportControlRecordBg::getTiltEnd(),
            float(this->getWidth()),
            TransportControlRecordBg::getTiltStart());
    }

    void mouseDown(const MouseEvent &e) override
    {
        TransportControlButton::mouseDown(e);
        this->owner.recordButtonPressed();
    }

    const String &getTooltipText() const override
    {
        return this->tooltipText;
    }

    const Colour lineColour;
    const String tooltipText;
};

class TransportControlPlayBg final : public TransportControlButton
{
public:

    explicit TransportControlPlayBg(TransportControlComponent &owner) :
        TransportControlButton(owner),
        lineColour(findDefaultColour(ColourIDs::Common::borderLineLight)),
        tooltipText(MenuItem::createTooltip(TRANS(I18n::Tooltips::playbackMode), KeyPress(' ')))
    {
        this->inactiveColour = findDefaultColour(ColourIDs::TransportControl::playInactive);
        this->highlightColour = findDefaultColour(ColourIDs::TransportControl::playHighlight);
        this->activeColour = findDefaultColour(ColourIDs::TransportControl::playActive);
        this->currentColour = this->inactiveColour;
    }

    inline float getTiltStart() const noexcept
    {
        constexpr auto dh = TransportControlComponent::playButtonSize - TransportControlComponent::buttonSkew;
        return float(this->getHeight() - dh);
    }

    inline float getTiltEnd() const noexcept
    {
        return float(this->getHeight() - TransportControlComponent::playButtonSize);
    }

    void resized() override
    {
        this->path.clear();
        this->path.startNewSubPath(0.f, this->getTiltStart());
        this->path.lineTo(float(this->getWidth()), this->getTiltEnd());
        this->path.lineTo(float(this->getWidth()), float(this->getHeight()));
        this->path.lineTo(0.f, float(this->getHeight()));
        this->path.closeSubPath();
    }

    void paint(Graphics &g) override
    {
        TransportControlButton::paint(g);
        g.setColour(this->lineColour);
        g.drawLine(0.f, this->getTiltStart(), float(this->getWidth()), this->getTiltEnd());
    }

    void mouseDown(const MouseEvent &e) override
    {
        TransportControlButton::mouseDown(e);
        this->owner.playButtonPressed();
    }

    const String &getTooltipText() const override
    {
        return this->tooltipText;
    }

    const Colour lineColour;
    const String tooltipText;
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

TransportControlComponent::TransportControlComponent(WeakReference<Component> eventReceiver) :
    eventReceiver(eventReceiver)
{
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(true, false);
    this->setMouseClickGrabsKeyboardFocus(false);

    this->recordBg = make<TransportControlRecordBg>(*this);
    this->addAndMakeVisible(this->recordBg.get());

    this->playBg = make<TransportControlPlayBg>(*this);
    this->addAndMakeVisible(this->playBg.get());

    this->playIcon = make<IconComponent>(Icons::play);
    this->addAndMakeVisible(this->playIcon.get());

    this->stopIcon = make<IconComponent>(Icons::stop);
    this->addChildComponent(this->stopIcon.get());

    this->recordIcon = make<IconComponent>(Icons::record);
    this->addAndMakeVisible(this->recordIcon.get());

    this->playIcon->setInterceptsMouseClicks(false, false);
    this->stopIcon->setInterceptsMouseClicks(false, false);

    this->recordButtonBlinkAnimator = make<RecordButtonBlinkAnimator>(this->recordBg.get());
}

TransportControlComponent::~TransportControlComponent() = default;

void TransportControlComponent::resized()
{
    this->recordBg->setBounds(0, 0, this->getWidth(), TransportControlComponent::recordButtonSize);
    this->playBg->setBounds(0, this->getHeight() - TransportControlComponent::playButtonSize,
        this->getWidth(), TransportControlComponent::playButtonSize);

    const auto cw = this->getWidth() / 2;
    const auto ch = this->getHeight() / 2;

    constexpr auto playIconSize = 24;
    this->playIcon->setBounds(cw - (playIconSize / 2) + 2,
        ch - (playIconSize / 2) + 15, playIconSize, playIconSize);

    constexpr auto stopIconSize = 22;
    this->stopIcon->setBounds(cw - (stopIconSize / 2),
        ch - (stopIconSize / 2) + 15, stopIconSize, stopIconSize);

    constexpr auto recordIconSize = 18;
    this->recordIcon->setBounds(cw - (recordIconSize / 2),
        7, recordIconSize, recordIconSize);
}

void TransportControlComponent::showPlayingMode(bool isPlaying)
{
    this->isPlaying = isPlaying;

    if (this->isPlaying.get())
    {
        const MessageManagerLock lock;
        this->animator.fadeIn(this->stopIcon.get(), Globals::UI::fadeInShort);
        this->animator.fadeOut(this->playIcon.get(), Globals::UI::fadeOutShort);
    }
    else
    {
        const MessageManagerLock lock;
        this->animator.fadeIn(this->playIcon.get(), Globals::UI::fadeInLong);
        this->animator.fadeOut(this->stopIcon.get(), Globals::UI::fadeOutLong);
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

void TransportControlComponent::showRecordingError() // just blinks red a few times
{
    this->recordBg->setActive(); // the initial "blink"
    this->recordButtonBlinkAnimator->startTimer(100);
}

void TransportControlComponent::showRecordingMenu(const Array<MidiDeviceInfo> &devices)
{
    jassert(devices.size() > 1);

    // if more than 1 devices available, provide a choice
    MenuPanel::Menu menu;
    auto panel = make<MenuPanel>();

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
