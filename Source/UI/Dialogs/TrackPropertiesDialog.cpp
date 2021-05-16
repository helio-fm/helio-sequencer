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
#include "TrackPropertiesDialog.h"

#include "CommandIDs.h"
#include "UndoStack.h"
#include "ProjectNode.h"
#include "MidiTrackActions.h"

TrackPropertiesDialog::TrackPropertiesDialog(ProjectNode &project,
    WeakReference<MidiTrack> track, const String &title, const String &confirmation) :
    project(project),
    track(track)
{
    this->messageLabel = make<Label>();
    this->addAndMakeVisible(this->messageLabel.get());
    this->messageLabel->setFont(Globals::UI::Fonts::L);
    this->messageLabel->setJustificationType(Justification::centred);

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

    this->colourSwatches = make<ColourSwatches>();
    this->addAndMakeVisible(this->colourSwatches.get());

    this->textEditor = make<TextEditor>();
    this->addAndMakeVisible(this->textEditor.get());
    this->textEditor->setMultiLine(false);
    this->textEditor->setReturnKeyStartsNewLine(false);
    this->textEditor->setReadOnly(false);
    this->textEditor->setScrollbarsShown(true);
    this->textEditor->setCaretVisible(true);
    this->textEditor->setPopupMenuEnabled(true);
    this->textEditor->setFont(Globals::UI::Fonts::L);
    this->textEditor->addListener(this);

    this->originalName = this->track->getTrackName();
    this->originalColour = this->track->getTrackColour();
    this->newName = this->originalName;
    this->newColour = this->originalColour;

    this->colourSwatches->setSelectedColour(this->originalColour);
    this->textEditor->setText(this->originalName, dontSendNotification);

    this->messageLabel->setText(title.isNotEmpty() ? title : TRANS(I18n::Dialog::renameTrackCaption), dontSendNotification);
    this->okButton->setButtonText(confirmation.isNotEmpty() ? confirmation : TRANS(I18n::Dialog::renameTrackProceed));
    this->cancelButton->setButtonText(TRANS(I18n::Dialog::cancel));

    this->messageLabel->setInterceptsMouseClicks(false, false);

    static constexpr auto colourButtonSize = 30;
    this->setSize(this->getPaddingAndMarginTotal() + 
        TrackPropertiesDialog::colourSwatchesMargin * 2 +
        colourButtonSize * this->colourSwatches->getNumButtons(), 220);

    this->updatePosition();
    this->updateOkButtonState();
}

TrackPropertiesDialog::~TrackPropertiesDialog()
{
    this->textEditor->removeListener(this);
}

void TrackPropertiesDialog::resized()
{
    this->messageLabel->setBounds(this->getCaptionBounds());

    const auto buttonsBounds(this->getButtonsBounds());
    const auto buttonWidth = buttonsBounds.getWidth() / 2;

    this->okButton->setBounds(buttonsBounds.withTrimmedLeft(buttonWidth));
    this->cancelButton->setBounds(buttonsBounds.withTrimmedRight(buttonWidth + 1));

    this->textEditor->setBounds(this->getRowBounds(0.3f, DialogBase::textEditorHeight));
    this->colourSwatches->setBounds(this->getRowBounds(0.7f, DialogBase::textEditorHeight,
        TrackPropertiesDialog::colourSwatchesMargin));
}

void TrackPropertiesDialog::parentHierarchyChanged()
{
    this->updatePosition();
}

void TrackPropertiesDialog::parentSizeChanged()
{
    this->updatePosition();
}

void TrackPropertiesDialog::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->doCancel();
    }
}

void TrackPropertiesDialog::inputAttemptWhenModal()
{
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
}

void TrackPropertiesDialog::updateOkButtonState()
{
    const bool textIsEmpty = this->textEditor->getText().trim().isEmpty();
    this->okButton->setAlpha(textIsEmpty ? 0.5f : 1.f);
    this->okButton->setEnabled(!textIsEmpty);
}

void TrackPropertiesDialog::onColourButtonClicked(ColourButton *clickedButton)
{
    this->newColour = clickedButton->getColour();
    this->applyChangesIfAny();
}

bool TrackPropertiesDialog::hasChanges() const
{
    return this->newColour != this->originalColour ||
        (this->newName != this->originalName && this->newName.isNotEmpty());
}

void TrackPropertiesDialog::cancelChangesIfAny()
{
    if (this->hasChanges())
    {
        this->project.getUndoStack()->undoCurrentTransactionOnly();
    }
}

void TrackPropertiesDialog::applyChangesIfAny()
{
    const auto &trackId = this->track->getTrackId();

    if (this->hasMadeChanges)
    {
        this->project.getUndoStack()->undoCurrentTransactionOnly();
    }

    this->project.getUndoStack()->beginNewTransaction();

    if (this->newName != this->originalName)
    {
        this->project.getUndoStack()->perform(new MidiTrackRenameAction(this->project, trackId, this->newName));
    }

    if (this->newColour != this->originalColour)
    {
        this->project.getUndoStack()->perform(new MidiTrackChangeColourAction(this->project, trackId, this->newColour));
    }

    this->hasMadeChanges = true;
}

void TrackPropertiesDialog::textEditorTextChanged(TextEditor&)
{
    this->updateOkButtonState();
    this->newName = this->textEditor->getText();
    this->applyChangesIfAny();
}

void TrackPropertiesDialog::textEditorReturnKeyPressed(TextEditor &ed)
{
    this->textEditorFocusLost(ed);
}

void TrackPropertiesDialog::textEditorEscapeKeyPressed(TextEditor&)
{
    this->doCancel();
}

void TrackPropertiesDialog::textEditorFocusLost(TextEditor&)
{
    this->updateOkButtonState();

    const auto *focusedComponent = Component::getCurrentlyFocusedComponent();
    if (this->textEditor->getText().isNotEmpty() &&
        focusedComponent != this->okButton.get() &&
        focusedComponent != this->cancelButton.get())
    {
        this->doOk(); // apply on return key
    }
    else
    {
        this->textEditor->grabKeyboardFocus();
    }
}

void TrackPropertiesDialog::doCancel()
{
    this->cancelChangesIfAny();

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

void TrackPropertiesDialog::doOk()
{
    if (textEditor->getText().isNotEmpty())
    {
        if (this->onOk != nullptr)
        {
            BailOutChecker checker(this);

            this->onOk();

            if (checker.shouldBailOut())
            {
                jassertfalse; // not expected
                return;
            }
        }

        this->dismiss();
    }
}
