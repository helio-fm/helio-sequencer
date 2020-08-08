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
#include "TimeSignatureDialog.h"
#include "TimeSignaturesSequence.h"
#include "CommandIDs.h"

static StringPairArray getDefaultMeters()
{
    StringPairArray c;
    c.set("Common time", "4/4");
    c.set("Alla breve", "2/4");
    c.set("Waltz time", "3/4");
    c.set("5/4", "5/4");
    c.set("6/4", "6/4");
    c.set("7/4", "7/4");
    c.set("5/8", "5/8");
    c.set("6/8", "6/8");
    c.set("7/8", "7/8");
    c.set("9/8", "9/8");
    c.set("12/8", "12/8");
    return c;
}

TimeSignatureDialog::TimeSignatureDialog(Component &owner,
    TimeSignaturesSequence *timeSequence, const TimeSignatureEvent &editedEvent,
    bool shouldAddNewEvent, float targetBeat) : 
    originalEvent(editedEvent),
    originalSequence(timeSequence),
    ownerComponent(owner),
    defailtMeters(getDefaultMeters()),
    addsNewEvent(shouldAddNewEvent)
{
    this->comboPrimer = make<MobileComboBox::Primer>();
    this->addAndMakeVisible(this->comboPrimer.get());

    this->messageLabel = make<Label>();
    this->addAndMakeVisible(this->messageLabel.get());
    this->messageLabel->setFont({ 21.f });
    this->messageLabel->setJustificationType(Justification::centred);

    this->removeEventButton = make<TextButton>();
    this->addAndMakeVisible(this->removeEventButton.get());
    this->removeEventButton->onClick = [this]()
    {
        if (this->addsNewEvent)
        {
            this->cancelAndDisappear();
        }
        else
        {
            this->removeEvent();
            this->dismiss();
        }
    };

    this->okButton = make<TextButton>();
    this->addAndMakeVisible(this->okButton.get());
    this->okButton->onClick = [this]()
    {
        if (textEditor->getText().isNotEmpty())
        {
            this->dismiss();
        }
    };

    this->textEditor = make<TextEditor>();
    this->addAndMakeVisible(textEditor.get());
    this->textEditor->setMultiLine(false);
    this->textEditor->setReturnKeyStartsNewLine(false);
    this->textEditor->setReadOnly(false);
    this->textEditor->setScrollbarsShown(true);
    this->textEditor->setCaretVisible(true);
    this->textEditor->setPopupMenuEnabled(true);

    jassert(this->originalSequence != nullptr);
    jassert(this->addsNewEvent || this->originalEvent.getSequence() != nullptr);

    const auto &meterNames = this->defailtMeters.getAllKeys();
    const auto &meterValues = this->defailtMeters.getAllValues();

    if (this->addsNewEvent)
    {
        Random r;
        const String meter(meterValues[r.nextInt(this->defailtMeters.size())]);
        int numerator;
        int denominator;
        TimeSignatureEvent::parseString(meter, numerator, denominator);
        this->originalEvent = TimeSignatureEvent(this->originalSequence, targetBeat, numerator, denominator);

        this->originalSequence->checkpoint();
        this->originalSequence->insert(this->originalEvent, true);

        this->messageLabel->setText(TRANS(I18n::Dialog::timeSignatureAddCaption), dontSendNotification);
        this->okButton->setButtonText(TRANS(I18n::Dialog::timeSignatureAddProceed));
        this->removeEventButton->setButtonText(TRANS(I18n::Dialog::cancel));
    }
    else
    {
        this->messageLabel->setText(TRANS(I18n::Dialog::timeSignatureEditCaption), dontSendNotification);
        this->okButton->setButtonText(TRANS(I18n::Dialog::timeSignatureEditApply));
        this->removeEventButton->setButtonText(TRANS(I18n::Dialog::timeSignatureEditDelete));
    }

    this->textEditor->addListener(this);
    this->textEditor->setFont(21.f);
    this->textEditor->setText(this->originalEvent.toString(), dontSendNotification);

    this->messageLabel->setInterceptsMouseClicks(false, false);

    MenuPanel::Menu menu;
    for (int i = 0; i < this->defailtMeters.size(); ++i)
    {
        const auto &s = meterNames[i];
        menu.add(MenuItem::item(Icons::empty, CommandIDs::SelectTimeSignature + i, s));
    }
    this->comboPrimer->initWith(this->textEditor.get(), menu);

    this->setSize(370, 185);
    this->updatePosition();
    this->updateOkButtonState();
}

TimeSignatureDialog::~TimeSignatureDialog()
{
    this->textEditor->removeListener(this);
}

void TimeSignatureDialog::resized()
{
    this->comboPrimer->setBounds(this->getContentBounds(0.5f));
    this->messageLabel->setBounds(this->getCaptionBounds());

    const auto buttonsBounds(this->getButtonsBounds());
    const auto buttonWidth = buttonsBounds.getWidth() / 2;

    this->okButton->setBounds(buttonsBounds.withTrimmedLeft(buttonWidth));
    this->removeEventButton->setBounds(buttonsBounds.withTrimmedRight(buttonWidth + 1));

    this->textEditor->setBounds(this->getRowBounds(0.5f, DialogBase::textEditorHeight));
}

void TimeSignatureDialog::parentHierarchyChanged()
{
    this->updatePosition();
}

void TimeSignatureDialog::parentSizeChanged()
{
    this->updatePosition();
}

void TimeSignatureDialog::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->cancelAndDisappear();
    }
    else
    {
        const int targetIndex = commandId - CommandIDs::SelectTimeSignature;
        if (targetIndex >= 0 && targetIndex < this->defailtMeters.size())
        {
            const String title = this->defailtMeters.getAllKeys()[targetIndex];
            const String time(this->defailtMeters[title]);
            this->textEditor->setText(time, true);
        }
    }
}

void TimeSignatureDialog::inputAttemptWhenModal()
{
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
}

UniquePointer<Component> TimeSignatureDialog::editingDialog(Component &owner, const TimeSignatureEvent &event)
{
    return make<TimeSignatureDialog>(owner, static_cast<TimeSignaturesSequence *>(event.getSequence()), event, false, 0.f);
}

UniquePointer<Component> TimeSignatureDialog::addingDialog(Component &owner, TimeSignaturesSequence *annotationsLayer, float targetBeat)
{
    return make<TimeSignatureDialog>(owner, annotationsLayer, TimeSignatureEvent(), true, targetBeat);
}

void TimeSignatureDialog::updateOkButtonState()
{
    const bool textIsEmpty = this->textEditor->getText().isEmpty();
    this->okButton->setAlpha(textIsEmpty ? 0.5f : 1.f);
    this->okButton->setEnabled(!textIsEmpty);
}

void TimeSignatureDialog::sendEventChange(const TimeSignatureEvent &newEvent)
{
    jassert(this->originalSequence != nullptr);

    if (this->addsNewEvent)
    {
        this->originalSequence->undo();
        this->originalSequence->insert(newEvent, true);
        this->originalEvent = newEvent;
    }
    else
    {
        if (this->hasMadeChanges)
        {
            this->originalSequence->undo();
            this->hasMadeChanges = false;
        }

        this->originalSequence->checkpoint();
        this->originalSequence->change(this->originalEvent, newEvent, true);
        this->hasMadeChanges = true;
    }
}

void TimeSignatureDialog::removeEvent()
{
    jassert(this->originalSequence != nullptr);

    if (this->addsNewEvent)
    {
        this->originalSequence->undo();
    }
    else
    {
        if (this->hasMadeChanges)
        {
            this->originalSequence->undo();
            this->hasMadeChanges = false;
        }

        this->originalSequence->checkpoint();
        this->originalSequence->remove(this->originalEvent, true);
        this->hasMadeChanges = true;
    }
}

void TimeSignatureDialog::textEditorTextChanged(TextEditor&)
{
    this->updateOkButtonState();

    const String meterString(this->textEditor->getText());
    if (meterString.isNotEmpty())
    {
        int numerator;
        int denominator;
        TimeSignatureEvent::parseString(meterString, numerator, denominator);

        TimeSignatureEvent newEvent = this->originalEvent
            .withNumerator(numerator)
            .withDenominator(denominator);

        this->sendEventChange(newEvent);
    }
}

void TimeSignatureDialog::textEditorReturnKeyPressed(TextEditor &ed)
{
    this->textEditorFocusLost(ed);
}

void TimeSignatureDialog::textEditorEscapeKeyPressed(TextEditor&)
{
    this->cancelAndDisappear();
}

void TimeSignatureDialog::textEditorFocusLost(TextEditor&)
{
    this->updateOkButtonState();

    const auto *focusedComponent = Component::getCurrentlyFocusedComponent();
    if (this->textEditor->getText().isNotEmpty() &&
        focusedComponent != this->okButton.get() &&
        focusedComponent != this->removeEventButton.get())
    {
        this->dismiss();
    }
    else
    {
        this->textEditor->grabKeyboardFocus();
    }
}

void TimeSignatureDialog::cancelAndDisappear()
{
    jassert(this->originalSequence != nullptr);

    if (this->addsNewEvent || this->hasMadeChanges)
    {
        this->originalSequence->undo();
    }

    this->dismiss();
}
