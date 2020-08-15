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
#include "ModalDialogConfirmation.h"
#include "CommandIDs.h"

ModalDialogConfirmation::ModalDialogConfirmation(const String &message, const String &okText, const String &cancelText)
{
    this->messageLabel = make<Label>();
    this->addAndMakeVisible(this->messageLabel.get());
    this->messageLabel->setFont({ 21.f });
    this->messageLabel->setJustificationType(Justification::centred);

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
    
    this->messageLabel->setText(message, dontSendNotification);
    this->okButton->setButtonText(okText);
    this->cancelButton->setButtonText(cancelText);
    this->messageLabel->setInterceptsMouseClicks(false, false);

    this->setSize(410, 180);
    this->updatePosition();
}

ModalDialogConfirmation::~ModalDialogConfirmation() {}

void ModalDialogConfirmation::resized()
{
    this->messageLabel->setBounds(this->getContentBounds());

    const auto buttonsBounds(this->getButtonsBounds());
    const auto buttonWidth = buttonsBounds.getWidth() / 2;

    this->okButton->setBounds(buttonsBounds.withTrimmedLeft(buttonWidth));
    this->cancelButton->setBounds(buttonsBounds.withTrimmedRight(buttonWidth + 1));
}

void ModalDialogConfirmation::parentHierarchyChanged()
{
    this->updatePosition();
}

void ModalDialogConfirmation::parentSizeChanged()
{
    this->updatePosition();
}

void ModalDialogConfirmation::handleCommandMessage (int commandId)
{
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->cancel();
    }
}

bool ModalDialogConfirmation::keyPressed(const KeyPress &key)
{
    if (key.isKeyCode(KeyPress::escapeKey))
    {
        this->cancel();
        return true;
    }
    else if (key.isKeyCode(KeyPress::returnKey))
    {
        this->okay();
        return true;
    }

    return false;
}

void ModalDialogConfirmation::inputAttemptWhenModal()
{
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
}

void ModalDialogConfirmation::cancel()
{
    const BailOutChecker checker(this);

    if (this->onCancel != nullptr)
    {
        this->onCancel();
    }

    if (!checker.shouldBailOut())
    {
        this->dismiss();
    }
}

void ModalDialogConfirmation::okay()
{
    const BailOutChecker checker(this);

    if (this->onOk != nullptr)
    {
        this->onOk();
    }

    // a user might have created another dialog in onOk
    // and showed it as a modal component, which destroyed this,
    // because there can only be one model component:
    if (!checker.shouldBailOut())
    {
        this->dismiss();
    }
}

//===----------------------------------------------------------------------===//
// Presets
//===----------------------------------------------------------------------===//

UniquePointer<ModalDialogConfirmation> ModalDialogConfirmation::Presets::deleteProject()
{
    return make<ModalDialogConfirmation>(
        TRANS(I18n::Dialog::deleteProjectCaption),
        TRANS(I18n::Dialog::deleteProjectProceed),
        TRANS(I18n::Dialog::cancel));
}

UniquePointer<ModalDialogConfirmation> ModalDialogConfirmation::Presets::forceCheckout()
{
    return make<ModalDialogConfirmation>(
        TRANS(I18n::Dialog::vcsCheckoutWarning),
        TRANS(I18n::Dialog::vcsCheckoutProceed),
        TRANS(I18n::Dialog::cancel));
}

UniquePointer<ModalDialogConfirmation> ModalDialogConfirmation::Presets::resetChanges()
{
    return make<ModalDialogConfirmation>(
        TRANS(I18n::Dialog::vcsResetCaption),
        TRANS(I18n::Dialog::vcsResetProceed),
        TRANS(I18n::Dialog::cancel));
}

UniquePointer<ModalDialogConfirmation> ModalDialogConfirmation::Presets::confirmOpenGL()
{
    return make<ModalDialogConfirmation>(
        TRANS(I18n::Dialog::openglCaption),
        TRANS(I18n::Dialog::openglProceed),
        TRANS(I18n::Dialog::cancel));
}
