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
#include "ModalDialogInput.h"
#include "CommandIDs.h"

ModalDialogInput::ModalDialogInput(const String &text, const String &message,
    const String &okText, const String &cancelText) :
    input(text)
{
    const auto isPhoneLayout = App::isRunningOnPhone();

    this->messageLabel = make<Label>();
    this->addAndMakeVisible(this->messageLabel.get());
    this->messageLabel->setFont(Globals::UI::Fonts::L);
    this->messageLabel->setJustificationType(Justification::centred);
    this->messageLabel->setText(message, dontSendNotification);
    this->messageLabel->setInterceptsMouseClicks(false, false);

    this->cancelButton = make<TextButton>();
    this->addAndMakeVisible(this->cancelButton.get());
    this->cancelButton->onClick = [this]()
    {
        this->cancel();
    };

    this->okButton = make<TextButton>();
    this->addAndMakeVisible(this->okButton.get());
    this->okButton->onClick = [this]()
    {
        this->okay();
    };

    this->textEditor = make<TextEditor>();
    this->addAndMakeVisible(this->textEditor.get());
    this->textEditor->setMultiLine(false);
    this->textEditor->setReturnKeyStartsNewLine(false);
    this->textEditor->setReadOnly(false);
    this->textEditor->setScrollbarsShown(false);
    this->textEditor->setCaretVisible(true);
    this->textEditor->setPopupMenuEnabled(true);

    this->textEditor->setTextToShowWhenEmpty(message, Colours::black.withAlpha(0.5f));
    this->textEditor->setFont(Globals::UI::Fonts::L);
    this->textEditor->setText(this->input, dontSendNotification);
    this->textEditor->addListener(this);

    this->okButton->setButtonText(okText);
    this->cancelButton->setButtonText(cancelText);

    this->setSize(460, isPhoneLayout ? 120 : 160);

    this->updatePosition();
    this->updateOkButtonState();
}

ModalDialogInput::~ModalDialogInput()
{
    this->textEditor->removeListener(this);
}

void ModalDialogInput::resized()
{
    const auto contentBounds = this->getContentBounds();
    this->messageLabel->setBounds(contentBounds
        .withTrimmedBottom(DialogBase::Defaults::textEditorHeight + 16));

    this->textEditor->setBounds(contentBounds
        .withTrimmedTop(this->messageLabel->getHeight())
        .withSizeKeepingCentre(contentBounds.getWidth(), DialogBase::Defaults::textEditorHeight));

    this->okButton->setBounds(this->getButton1Bounds());
    this->cancelButton->setBounds(this->getButton2Bounds());
}

void ModalDialogInput::parentHierarchyChanged()
{
    this->updatePosition();
}

void ModalDialogInput::parentSizeChanged()
{
    this->updatePosition();
}

void ModalDialogInput::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::DismissDialog)
    {
        this->cancel();
    }
}

void ModalDialogInput::textEditorTextChanged(TextEditor &editor)
{
    this->input = editor.getText();
    this->updateOkButtonState();
}

void ModalDialogInput::textEditorReturnKeyPressed(TextEditor &editor)
{
    if (this->input.isNotEmpty())
    {
        this->okay(); // apply on return key
        return;
    }

    this->resetKeyboardFocus();
}

void ModalDialogInput::textEditorEscapeKeyPressed(TextEditor &)
{
    this->cancel();
}

void ModalDialogInput::textEditorFocusLost(TextEditor &ed)
{
    this->updateOkButtonState();
    this->resetKeyboardFocus();
}

void ModalDialogInput::cancel()
{
    const BailOutChecker checker(this);

    if (this->onCancel != nullptr)
    {
        this->onCancel(this->input);
    }

    if (!checker.shouldBailOut())
    {
        this->dismiss();
    }
}

void ModalDialogInput::okay()
{
    if (textEditor->getText().isEmpty())
    {
        return;
    }

    const BailOutChecker checker(this);

    if (this->onOk != nullptr)
    {
        this->onOk(this->input);
    }

    if (!checker.shouldBailOut())
    {
        this->dismiss();
    }
}

void ModalDialogInput::updateOkButtonState()
{
    this->okButton->setEnabled(this->input.isNotEmpty());
}

Component *ModalDialogInput::getPrimaryFocusTarget()
{
#if PLATFORM_DESKTOP
    return this->textEditor.get();
#elif PLATFORM_MOBILE
    return this;
#endif
}

void ModalDialogInput::visibilityChanged()
{
    if (this->isShowing())
    {
        this->textEditor->grabKeyboardFocus();
    }
}

//===----------------------------------------------------------------------===//
// Presets
//===----------------------------------------------------------------------===//

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::renameAnnotation(const String &name)
{
    return make<ModalDialogInput>(name,
        TRANS(I18n::Dialog::annotationRenameCaption),
        TRANS(I18n::Dialog::annotationRenameProceed),
        TRANS(I18n::Dialog::annotationRenameCancel));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::renameInstrument(const String &name)
{
    return make<ModalDialogInput>(name,
        TRANS(I18n::Dialog::instrumentRenameCaption),
        TRANS(I18n::Dialog::instrumentRenameProceed),
        TRANS(I18n::Dialog::cancel));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::changeTimeSignature(const String &name)
{
    return make<ModalDialogInput>(name,
        TRANS(I18n::Dialog::timeSignatureEditCaption),
        TRANS(I18n::Dialog::apply),
        TRANS(I18n::Dialog::cancel));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::renameTrack(const String &name)
{
    return make<ModalDialogInput>(name,
        TRANS(I18n::Dialog::renameTrackCaption),
        TRANS(I18n::Dialog::apply),
        TRANS(I18n::Dialog::cancel));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::newTrack()
{
    return make<ModalDialogInput>(TRANS(I18n::Defaults::midiTrackName),
        TRANS(I18n::Dialog::addTrackCaption),
        TRANS(I18n::Dialog::add),
        TRANS(I18n::Dialog::cancel));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::deleteProjectConfirmation()
{
    return make<ModalDialogInput>(String(),
        TRANS(I18n::Dialog::deleteProjectConfirmCaption),
        TRANS(I18n::Dialog::delete_),
        TRANS(I18n::Dialog::cancel));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::commit(const String &name)
{
    return make<ModalDialogInput>(name,
        TRANS(I18n::Dialog::vcsCommitCaption),
        TRANS(I18n::Dialog::vcsCommitProceed),
        TRANS(I18n::Dialog::cancel));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::newArpeggiator()
{
    return make<ModalDialogInput>(String(),
        TRANS(I18n::Dialog::addArpCaption),
        TRANS(I18n::Dialog::addArpProceed),
        TRANS(I18n::Dialog::cancel));
}

UniquePointer<ModalDialogInput> ModalDialogInput::Presets::savePreset()
{
    return make<ModalDialogInput>(String(),
        TRANS(I18n::Menu::savePreset),
        TRANS(I18n::Dialog::save),
        TRANS(I18n::Dialog::cancel));
}
