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
#include "CommandIDs.h"

// fixme: create a separate resource config
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
    WeakReference<UndoStack> undoStack,
    WeakReference<MidiTrack> targetTrack,
    WeakReference<TimeSignaturesSequence> targetSequence,
    const TimeSignatureEvent &editedEvent, bool shouldAddNewEvent) :
    undoStack(undoStack),
    targetTrack(targetTrack),
    targetSequence(targetSequence),
    originalEvent(editedEvent),
    ownerComponent(owner),
    defaultMeters(getDefaultMeters()),
    mode(shouldAddNewEvent ? Mode::AddTimelineTimeSignature :
        (targetSequence != nullptr ? Mode::EditTimelineTimeSignature : Mode::EditTrackTimeSignature))
{
    jassert(this->originalEvent.isValid());
    jassert(this->targetSequence != nullptr || this->targetTrack != nullptr);
    jassert(this->targetTrack != nullptr || this->originalEvent.getSequence() != nullptr);

    this->messageLabel = make<Label>();
    this->addAndMakeVisible(this->messageLabel.get());
    this->messageLabel->setFont(Globals::UI::Fonts::L);
    this->messageLabel->setJustificationType(Justification::centred);
    this->messageLabel->setInterceptsMouseClicks(false, false);

    this->removeEventButton = make<TextButton>();
    this->addAndMakeVisible(this->removeEventButton.get());
    this->removeEventButton->onClick = [this]()
    {
        switch (this->mode)
        {
        case Mode::EditTrackTimeSignature:
        case Mode::EditTimelineTimeSignature:
            this->removeTimeSignature();
            this->dismiss();
            break;
        case Mode::AddTimelineTimeSignature:
            this->undoAndDismiss();
            break;
        }
    };

    this->okButton = make<TextButton>();
    this->addAndMakeVisible(this->okButton.get());
    this->okButton->onClick = [this]()
    {
        if (this->textEditor->getText().isNotEmpty())
        {
            this->dismiss();
        }
    };

    this->textEditor = make<TextEditor>();
    this->addAndMakeVisible(this->textEditor.get());
    this->textEditor->setFont(Globals::UI::Fonts::L);

    const auto onLostFocus = [this]()
    {
        this->updateOkButtonState();

        auto *focusedComponent = Component::getCurrentlyFocusedComponent();

        if (nullptr != dynamic_cast<TextEditor *>(focusedComponent) &&
            this->textEditor.get() != focusedComponent)
        {
            return; // other editor is focused
        }

        if (this->textEditor->getText().isNotEmpty() &&
            focusedComponent != this->okButton.get() &&
            focusedComponent != this->removeEventButton.get())
        {
            this->dismiss(); // apply on return key
        }
        else
        {
            this->textEditor->grabKeyboardFocus();
        }
    };

    this->textEditor->onReturnKey = onLostFocus;
    this->textEditor->onFocusLost = onLostFocus;

    this->textEditor->onTextChange = [this]()
    {
        this->updateOkButtonState();

        const auto meterString = this->textEditor->getText();
        if (meterString.isEmpty())
        {
            return;
        }
        
        int numerator;
        int denominator;
        TimeSignatureEvent::parseString(meterString, numerator, denominator);

        const auto newEvent = this->originalEvent
            .withNumerator(numerator)
            .withDenominator(denominator);

        this->sendEventChange(newEvent);
    };

    this->textEditor->onEscapeKey = [this]()
    {
        this->undoAndDismiss();
    };

    const auto &meterNames = this->defaultMeters.getAllKeys();
    MenuPanel::Menu menu;
    for (int i = 0; i < this->defaultMeters.size(); ++i)
    {
        const auto &s = meterNames[i];
        menu.add(MenuItem::item(Icons::empty, CommandIDs::SelectTimeSignature + i, s));
    }

    this->presetsCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->presetsCombo.get());
    this->presetsCombo->initWith(this->textEditor.get(), menu);

    if (this->mode == Mode::AddTimelineTimeSignature)
    {
        jassert(this->targetSequence != nullptr);
        this->targetSequence->checkpoint();
        this->targetSequence->insert(this->originalEvent, true);

        this->messageLabel->setText(TRANS(I18n::Dialog::timeSignatureAddCaption), dontSendNotification);
        this->removeEventButton->setButtonText(TRANS(I18n::Dialog::cancel));
        this->okButton->setButtonText(TRANS(I18n::Dialog::add));
    }
    else
    {
        this->messageLabel->setText(TRANS(I18n::Dialog::timeSignatureEditCaption), dontSendNotification);
        this->removeEventButton->setButtonText(TRANS(I18n::Dialog::delete_));
        this->okButton->setButtonText(TRANS(I18n::Dialog::apply));
    }

    this->textEditor->setText(this->originalEvent.toString(), dontSendNotification);
    
    this->setSize(370, 185);
    this->updatePosition();
    this->updateOkButtonState();
}

TimeSignatureDialog::~TimeSignatureDialog() = default;

void TimeSignatureDialog::resized()
{
    this->messageLabel->setBounds(this->getCaptionBounds());
    this->presetsCombo->setBounds(this->getContentBounds(0.5f));

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
        this->undoAndDismiss();
    }
    else
    {
        const int targetIndex = commandId - CommandIDs::SelectTimeSignature;
        if (targetIndex >= 0 && targetIndex < this->defaultMeters.size())
        {
            const auto title = this->defaultMeters.getAllKeys()[targetIndex];
            const auto time = this->defaultMeters[title];

            this->textEditor->grabKeyboardFocus();
            this->textEditor->setText(time, true);
        }
    }
}

void TimeSignatureDialog::inputAttemptWhenModal()
{
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
}

void TimeSignatureDialog::updateOkButtonState()
{
    this->okButton->setEnabled(this->textEditor->getText().isNotEmpty());
}

void TimeSignatureDialog::sendEventChange(const TimeSignatureEvent &newEvent)
{
    switch (this->mode)
    {
    case Mode::EditTrackTimeSignature:
        jassert(this->targetTrack != nullptr);
        if (this->hasMadeChanges)
        {
            this->undoStack->undo();
        }

        this->undoStack->beginNewTransaction();
        this->targetTrack->setTimeSignatureOverride(newEvent, true);
        this->hasMadeChanges = true;
        break;
    case Mode::EditTimelineTimeSignature:
        jassert(this->targetSequence != nullptr);
        if (this->hasMadeChanges)
        {
            this->undoStack->undo();
        }

        this->undoStack->beginNewTransaction();
        this->targetSequence->change(this->originalEvent, newEvent, true);
        this->hasMadeChanges = true;
        break;
    case Mode::AddTimelineTimeSignature:
        jassert(this->targetSequence != nullptr);
        this->undoStack->undo();
        this->targetSequence->insert(newEvent, true);
        this->originalEvent = newEvent;
        break;
    }
}

void TimeSignatureDialog::removeTimeSignature()
{
    switch (this->mode)
    {
    case Mode::EditTrackTimeSignature:
        jassert(this->targetTrack != nullptr);
        if (this->hasMadeChanges)
        {
            this->undoStack->undo();
        }

        this->undoStack->beginNewTransaction();
        this->targetTrack->setTimeSignatureOverride({}, true);
        this->hasMadeChanges = true;
        break;
    case Mode::EditTimelineTimeSignature:
        jassert(this->targetSequence != nullptr);
        if (this->hasMadeChanges)
        {
            this->undoStack->undo();
        }

        this->undoStack->beginNewTransaction();
        this->targetSequence->remove(this->originalEvent, true);
        this->hasMadeChanges = true;
        break;
    case Mode::AddTimelineTimeSignature:
        jassert(this->targetSequence != nullptr);
        this->undoStack->undo();
        break;
    }
}

void TimeSignatureDialog::undoAndDismiss()
{
    switch (this->mode)
    {
    case Mode::EditTrackTimeSignature:
    case Mode::EditTimelineTimeSignature:
        if (this->hasMadeChanges)
        {
            this->undoStack->undo();
        }
        break;
    case Mode::AddTimelineTimeSignature:
        // undo anyway to cancel new event insertion at the start:
        this->undoStack->undo();
        break;
    }

    this->dismiss();
}

UniquePointer<Component> TimeSignatureDialog::editingDialog(Component &owner,
    WeakReference<UndoStack> undoStack, const TimeSignatureEvent &event)
{
    if (auto *sequence = static_cast<TimeSignaturesSequence *>(event.getSequence()))
    {
        return make<TimeSignatureDialog>(owner, undoStack,
            nullptr, sequence, event, false);
    }
    else
    {
        jassert(event.getTrack() != nullptr);
        return make<TimeSignatureDialog>(owner, undoStack,
            event.getTrack(), nullptr, event, false);
    }
}

UniquePointer<Component> TimeSignatureDialog::addingDialog(Component &owner,
    WeakReference<UndoStack> undoStack,
    WeakReference<TimeSignaturesSequence> targetSequence, float targetBeat)
{
    return make<TimeSignatureDialog>(owner, undoStack, nullptr,
        targetSequence, TimeSignatureEvent(targetSequence.get(), targetBeat), true);
}
