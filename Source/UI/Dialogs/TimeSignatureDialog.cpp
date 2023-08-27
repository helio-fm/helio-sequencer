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
#include "MidiTrackActions.h"
#include "Config.h"
#include "CommandIDs.h"
#include "MetronomeEditor.h"
#include "ProjectNode.h"

#include "App.h"
#include "AudioCore.h"
#include "Workspace.h"

TimeSignatureDialog::TimeSignatureDialog(Component &owner,
    ProjectNode &project, WeakReference<MidiTrack> targetTrack,
    WeakReference<TimeSignaturesSequence> targetSequence,
    const TimeSignatureEvent &editedTimeSignature, bool shouldAddNewEvent) :
    undoStack(project.getUndoStack()),
    targetTrack(targetTrack),
    targetSequence(targetSequence),
    originalEvent(editedTimeSignature),
    editedEvent(editedTimeSignature),
    ownerComponent(owner),
    defaultMeters(App::Config().getMeters()->getAll()),
    mode(shouldAddNewEvent ? Mode::AddTimelineTimeSignature :
        (targetTrack != nullptr ? Mode::EditTrackTimeSignature : Mode::EditTimelineTimeSignature))
{
    jassert(this->originalEvent.isValid() || this->targetTrack != nullptr);
    jassert(this->targetSequence != nullptr || this->targetTrack != nullptr);
    jassert(this->targetTrack != nullptr || this->originalEvent.getSequence() != nullptr);

    const auto isPhoneLayout = App::isRunningOnPhone();

    this->messageLabel = make<Label>();
    this->addChildComponent(this->messageLabel.get());
    this->messageLabel->setFont(Globals::UI::Fonts::L);
    this->messageLabel->setJustificationType(Justification::centred);
    this->messageLabel->setInterceptsMouseClicks(false, false);
    this->messageLabel->setVisible(!isPhoneLayout);

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

    if (!this->originalEvent.isValid() || this->mode == Mode::AddTimelineTimeSignature)
    {
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

    this->textEditor = make<TextEditor>();
    this->addAndMakeVisible(this->textEditor.get());
    this->textEditor->setFont(Globals::UI::Fonts::L);

    this->textEditor->onReturnKey = [this]()
    {
        if (this->textEditor->getText().isNotEmpty())
        {
            this->dismiss(); // apply on return key
            return;
        }

        this->resetKeyboardFocus();
    };

    this->textEditor->onFocusLost = [this]()
    {
        this->updateOkButtonState();

        if (nullptr != dynamic_cast<TextEditor *>(Component::getCurrentlyFocusedComponent()))
        {
            return; // some other editor is focused
        }

        this->resetKeyboardFocus();
    };

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
        Meter::parseString(meterString, numerator, denominator);

        auto newEvent = this->editedEvent
            .withNumerator(numerator)
            .withDenominator(denominator);

        const auto metronome = this->editedEvent.getMeter().getMetronome();
        if (numerator != metronome.getSize())
        {
            newEvent = newEvent.withMetronome(metronome.resized(numerator));
        }

        this->sendEventChange(newEvent);
    };

    this->textEditor->onEscapeKey = [this]()
    {
        this->undoAndDismiss();
    };

    MenuPanel::Menu menu;
    for (int i = 0; i < this->defaultMeters.size(); ++i)
    {
        const auto meter = this->defaultMeters[i];
        menu.add(MenuItem::item(Icons::empty,
            CommandIDs::SelectTimeSignature + i, meter->getLocalizedName()));
    }

    this->presetsCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->presetsCombo.get());
    this->presetsCombo->initWith(this->textEditor.get(), menu);

    this->metronomeEditor = make<MetronomeEditor>(project.getTransport(),
        App::Workspace().getAudioCore().getMetronomeInstrument());
    this->addAndMakeVisible(this->metronomeEditor.get());
    this->metronomeEditor->setMetronome(this->originalEvent.getMeter().getMetronome());
    this->metronomeEditor->onTap = [this, &transport = project.getTransport()]
        (const MetronomeScheme &metronome, int tappedSyllableIndex)
    {
        const auto tappedSyllable = metronome.getSyllableAt(tappedSyllableIndex);
        const auto nextSyllable = MetronomeScheme::getNextSyllable(tappedSyllable);
        const auto newMetronome = metronome.withSyllableAt(tappedSyllableIndex, nextSyllable);

        const auto key = MetronomeSynth::getKeyForSyllable(nextSyllable);
        auto *metronomeInstrument = App::Workspace().getAudioCore().getMetronomeInstrument();
        transport.previewKey(metronomeInstrument, key, 1.f, float(Globals::beatsPerBar));

        this->sendEventChange(this->editedEvent.withMetronome(newMetronome));
    };

    if (this->mode == Mode::EditTrackTimeSignature && !this->originalEvent.isValid())
    {
        jassert(this->targetTrack != nullptr);

        // this is the case when we're adding a time signature to a track,
        // so let's try to suggest the one that seems to fit by bar length:
        auto newMeter = this->defaultMeters.getFirst();
        for (int i = this->defaultMeters.size(); i --> 0 ;)
        {
            if (fmodf(this->targetTrack->getSequence()->getLengthInBeats(),
                this->defaultMeters[i]->getBarLengthInBeats()) == 0.f)
            {
                newMeter = this->defaultMeters[i];
                break;
            }
        }

        this->sendEventChange(this->originalEvent.withMeter(*newMeter));
    }
    else if (this->mode == Mode::AddTimelineTimeSignature)
    {
        jassert(this->targetSequence != nullptr);
        this->undoStack->beginNewTransaction();
        this->targetSequence->insert(this->originalEvent, true);
    }

    this->textEditor->setText(this->editedEvent.toString(), dontSendNotification);
    // instead of selectAll(), which puts the caret at the start:
    this->textEditor->setCaretPosition(0);
    this->textEditor->moveCaretToEnd(true);

    this->updatePosition();
    this->updateSize();
    this->updateOkButtonState();
}

TimeSignatureDialog::~TimeSignatureDialog() = default;

void TimeSignatureDialog::updateSize()
{
    const auto isPhoneLayout = App::isRunningOnPhone();
    const auto oldWidth = this->getWidth();
    const auto metronomeEditorPadding = 34;
    this->setSize(this->getHorizontalSpacingExceptContent() +
        this->metronomeEditor->getAllButtonsArea().getWidth() + metronomeEditorPadding,
        isPhoneLayout ? DialogBase::Defaults::Phone::maxDialogHeight : 210);
    this->setTopLeftPosition(this->getPosition().translated((oldWidth - this->getWidth()) / 2, 0));
}

void TimeSignatureDialog::resized()
{
    this->presetsCombo->setBounds(this->getContentBounds(true));
    this->messageLabel->setBounds(this->getCaptionBounds());

    this->okButton->setBounds(this->getButton1Bounds());
    this->removeEventButton->setBounds(this->getButton2Bounds());

    this->textEditor->setBounds(this->getRowBounds(0.2f, DialogBase::Defaults::textEditorHeight));

    constexpr auto metronomeEditorHeight = 55;
    this->metronomeEditor->setBounds(this->getRowBounds(0.65f, metronomeEditorHeight));
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
    this->metronomeEditor->postCommandMessage(CommandIDs::TransportStop);

    if (commandId == CommandIDs::DismissDialog)
    {
        this->undoAndDismiss();
    }
    else
    {
        const int targetIndex = commandId - CommandIDs::SelectTimeSignature;
        if (targetIndex >= 0 && targetIndex < this->defaultMeters.size())
        {
            const auto meter = this->defaultMeters[targetIndex];
            this->textEditor->setText(meter->getTimeAsString(), false);
            this->resetKeyboardFocus();
            this->sendEventChange(this->editedEvent.withMeter(*meter));
        }
    }
}

void TimeSignatureDialog::updateOkButtonState()
{
    this->okButton->setEnabled(this->textEditor->getText().isNotEmpty());
}

void TimeSignatureDialog::sendEventChange(const TimeSignatureEvent &newEvent)
{
    this->metronomeEditor->postCommandMessage(CommandIDs::TransportStop);

    switch (this->mode)
    {
    case Mode::EditTrackTimeSignature:
        jassert(this->targetTrack != nullptr);
        if (this->hasMadeChanges)
        {
            this->undoStack->undo();
        }

        this->undoStack->beginNewTransaction();
        this->targetTrack->setTimeSignatureOverride(newEvent, true, sendNotification);
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
        break;
    }

    this->editedEvent = newEvent;
    this->metronomeEditor->setMetronome(newEvent.getMeter().getMetronome());
    this->updateSize();
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
        this->targetTrack->setTimeSignatureOverride({}, true, sendNotification);
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

Component *TimeSignatureDialog::getPrimaryFocusTarget()
{
#if PLATFORM_DESKTOP
    return this->textEditor.get();
#elif PLATFORM_MOBILE
    return this;
#endif
}

UniquePointer<Component> TimeSignatureDialog::editingDialog(Component &owner,
    ProjectNode &project, const TimeSignatureEvent &event)
{
    if (event.getTrack() != nullptr)
    {
        return make<TimeSignatureDialog>(owner, project, event.getTrack(), nullptr, event, false);
    }
    else
    {
        auto *sequence = static_cast<TimeSignaturesSequence *>(event.getSequence());
        jassert(sequence != nullptr);
        return make<TimeSignatureDialog>(owner, project, nullptr, sequence, event, false);
    }
}

UniquePointer<Component> TimeSignatureDialog::addingDialog(Component &owner,
    ProjectNode &project, WeakReference<TimeSignaturesSequence> targetSequence, float targetBeat)
{
    return make<TimeSignatureDialog>(owner, project, nullptr,
        targetSequence, TimeSignatureEvent(targetSequence.get(), targetBeat), true);
}
