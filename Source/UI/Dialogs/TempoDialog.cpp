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
#include "TempoDialog.h"
#include "CommandIDs.h"
#include "HelioTheme.h"
#include "ColourIDs.h"

// a hack to pass the TextEditor's mousewheel to the dialog,
// which needs it for adjusting the tempo with mouse wheel:
class TempoTextEditor final : public TextEditor, private AsyncUpdater
{
public:

    Function<void(const MouseWheelDetails &wheel)> onWheelMove;

    void mouseWheelMove(const MouseEvent &e,
        const MouseWheelDetails &wheel) override
    {
        TextEditor::mouseWheelMove(e, wheel);

        if (!wheel.isInertial)
        {
            // doing it asynchronously to avoid weird
            // double mouseWheelMove callbacks in some cases:
            this->details = wheel;
            this->triggerAsyncUpdate();
        }
    }

private:

    void handleAsyncUpdate() override
    {
        if (this->onWheelMove != nullptr)
        {
            this->onWheelMove(this->details);
        }
    }

    MouseWheelDetails details;
};

class TapTempoComponent final : public Component, private Timer
{
public:

    TapTempoComponent()
    {
        this->setWantsKeyboardFocus(false);
        this->setPaintingIsUnclipped(true);

        this->currentFillColour = this->targetColour;

        this->label = make<Label>();
        this->addAndMakeVisible(this->label.get());
        this->label->setInterceptsMouseClicks(false, false);
        this->label->setJustificationType(Justification::centred);
        this->label->setText(TRANS(I18n::Dialog::setTempoTapLabel), dontSendNotification);
        this->label->setFont(Globals::UI::Fonts::S);
    }

    Function<void(int newTempoBpm)> onTempoChanged;

    void resized() override
    {
        this->label->setBounds(this->getLocalBounds());
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->currentFillColour);
        g.fillRect(this->getLocalBounds());
        HelioTheme::drawStripes(this->getLocalBounds().toFloat().reduced(2.5f), g, 0.2f);
        g.setColour(this->outlineColour);
        HelioTheme::drawDashedFrame(g, this->getLocalBounds());
    }

    void mouseDown(const MouseEvent &e) override
    {
        this->detectAndSendTapTempo();
        this->currentFillColour = this->highlightColour; // then animate
        this->startTimerHz(60);
    }

private:

    void detectAndSendTapTempo()
    {
        if (this->onTempoChanged == nullptr)
        {
            return;
        }

        const auto now = Time::getMillisecondCounterHiRes();

        if (this->lastTapMs == 0.0)
        {
            this->lastTapMs = now;
            return; // first tap is kinda useless
        }

        const auto delay = now - this->lastTapMs;
        this->tapIntervalsMs.add(delay);
        this->lastTapMs = now;

        if (this->tapIntervalsMs.size() == 1)
        {
            return; // second tap is also kinda useless
        }

        // now check the last 2 delays: if they differ too much,
        // then everything up to the latest tap delay doesn't seem relevant:
        const auto d1 = this->tapIntervalsMs[this->tapIntervalsMs.size() - 1];
        const auto d2 = this->tapIntervalsMs[this->tapIntervalsMs.size() - 2];
        if (abs(d1 - d2) > 1000) // todo experiment more with this
        {
            this->tapIntervalsMs.clearQuick();
            this->tapIntervalsMs.add(delay);
            return;
        }

        // at this point we're pretty sure that all tap intervals are ok,
        // lets take average and compute BPM from that:
        double average = 0.0;
        for (const auto val : this->tapIntervalsMs)
        {
            average += val;
        }
        average /= double(this->tapIntervalsMs.size());

        const int bpm = int(60000.0 / average);
        this->onTempoChanged(bpm);
    }

    void timerCallback() override
    {
        const auto newColour = this->currentFillColour.
            interpolatedWith(this->targetColour, 0.1f);

        if (this->currentFillColour == newColour)
        {
            this->stopTimer();
        }
        else
        {
            this->currentFillColour = newColour;
            this->repaint();
        }
    }

    Colour currentFillColour;

    const Colour targetColour = findDefaultColour(ColourIDs::TapTempoControl::fill);
    const Colour highlightColour = findDefaultColour(ColourIDs::TapTempoControl::fillHighlighted);
    const Colour outlineColour = findDefaultColour(ColourIDs::TapTempoControl::outline);

    double lastTapMs = 0.0;
    Array<double> tapIntervalsMs;

    UniquePointer<Label> label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TapTempoComponent)
};

TempoDialog::TempoDialog(int bpmValue)
{
    const auto isPhoneLayout = App::isRunningOnPhone();

    this->messageLabel = make<Label>();
    this->addChildComponent(this->messageLabel.get());
    this->messageLabel->setFont(Globals::UI::Fonts::L);
    this->messageLabel->setJustificationType(Justification::centred);
    this->messageLabel->setVisible(!isPhoneLayout);

    this->cancelButton = make<TextButton>();
    this->addAndMakeVisible(this->cancelButton.get());
    this->cancelButton->onClick = [this]()
    {
        this->doCancel();
    };

    this->okButton = make<TextButton>();
    this->addAndMakeVisible(this->okButton.get());
    this->okButton->onClick = [this]()
    {
        this->doOk();
    };

    this->tapTempo = make<TapTempoComponent>();
    this->addAndMakeVisible(this->tapTempo.get());
    this->tapTempo->onTempoChanged = [this](int newTempoBpm)
    {
        // todo clamp value (15 .. 240)?
        this->textEditor->setText(String(newTempoBpm), sendNotification);
    };

    this->textEditor = make<TempoTextEditor>();
    this->addAndMakeVisible(this->textEditor.get());
    this->textEditor->setMultiLine(false);
    this->textEditor->setReturnKeyStartsNewLine(false);
    this->textEditor->setReadOnly(false);
    this->textEditor->setScrollbarsShown(true);
    this->textEditor->setCaretVisible(true);
    this->textEditor->setPopupMenuEnabled(true);
    this->textEditor->setFont(DialogBase::Defaults::textEditorFont);
    this->textEditor->setJustification(Justification::centredLeft);
    this->textEditor->setIndents(4, 0);

    this->textEditor->onWheelMove = [this](const MouseWheelDetails &wheel)
    {
        if (wheel.deltaY == 0)
        {
            return;
        }

        const auto delta = wheel.deltaY > 0 ? 1 : -1;
        const auto newTempoBpm = this->textEditor->getText().getIntValue() + delta;
        this->textEditor->setText(String(newTempoBpm));

        this->updateOkButtonState();
    };

    this->textEditor->onTextChange = [this]()
    {
        this->updateOkButtonState();
    };

    this->textEditor->onFocusLost = [this]()
    {
        this->updateOkButtonState();
        this->resetKeyboardFocus();
    };

    this->textEditor->onReturnKey = [this]()
    {
        if (this->textEditor->getText().isNotEmpty())
        {
            this->doOk();
            return;
        }

        this->resetKeyboardFocus();
    };

    this->textEditor->onEscapeKey = [this]()
    {
        this->doCancel();
    };

    this->textEditor->setText(String(bpmValue), dontSendNotification);
    // instead of selectAll(), which puts the caret at the start:
    this->textEditor->setCaretPosition(0);
    this->textEditor->moveCaretToEnd(true);

    this->messageLabel->setText(TRANS(I18n::Dialog::setTempoCaption), dontSendNotification);
    this->messageLabel->setInterceptsMouseClicks(false, false);

    this->okButton->setButtonText(TRANS(I18n::Dialog::apply));
    this->cancelButton->setButtonText(TRANS(I18n::Dialog::cancel));

    this->setSize(460, isPhoneLayout ? DialogBase::Defaults::Phone::maxDialogHeight : 210);

    this->updatePosition();
    this->updateOkButtonState();
}

// still need to have this empty dtor here,
// so that compiler can resolve the forward-declared TapTempo
TempoDialog::~TempoDialog() = default;

void TempoDialog::resized()
{
    this->messageLabel->setBounds(this->getCaptionBounds());

    this->okButton->setBounds(this->getButton1Bounds());
    this->cancelButton->setBounds(this->getButton2Bounds());

    this->textEditor->setBounds(this->getRowBounds(0.2f, Globals::UI::textEditorHeight));
    this->tapTempo->setBounds(this->getRowBounds(0.65f,
        TempoDialog::tapTempoHeight, TempoDialog::tapTempoMargin));
}

void TempoDialog::parentHierarchyChanged()
{
    this->updatePosition();
}

void TempoDialog::parentSizeChanged()
{
    this->updatePosition();
}

void TempoDialog::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::DismissDialog)
    {
        this->doCancel();
    }
}

void TempoDialog::updateOkButtonState()
{
    const bool textIsEmpty = this->textEditor->getText().isEmpty();
    const auto newTempoBpm = this->textEditor->getText().getIntValue();
    this->okButton->setEnabled(!textIsEmpty && newTempoBpm != 0);

    if (!textIsEmpty && (newTempoBpm > 300 || newTempoBpm <= 0))
    {
        static const auto fletcherMsg = "Not quite my tempo!"; // intentionally untranslated
        this->messageLabel->setText(fletcherMsg, dontSendNotification);
    }
    else
    {
        this->messageLabel->setText(TRANS(I18n::Dialog::setTempoCaption), dontSendNotification);
    }
}

void TempoDialog::doCancel()
{
    if (this->onCancel != nullptr)
    {
        BailOutChecker checker(this);

        this->onCancel();

        if (checker.shouldBailOut())
        {
            jassertfalse; // not expected
            return;
        }
    }

    this->dismiss();
}

void TempoDialog::doOk()
{
    if (this->textEditor->getText().isNotEmpty())
    {
        if (this->onOk != nullptr)
        {
            BailOutChecker checker(this);

            const auto newValue =
                this->textEditor->getText().getIntValue();

            this->onOk(newValue);

            if (checker.shouldBailOut())
            {
                jassertfalse; // not expected
                return;
            }
        }

        this->dismiss();
    }
}

Component *TempoDialog::getPrimaryFocusTarget()
{
#if PLATFORM_DESKTOP
    return this->textEditor.get();
#elif PLATFORM_MOBILE
    return this;
#endif
}
