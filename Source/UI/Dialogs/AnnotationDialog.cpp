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
#include "AnnotationDialog.h"
#include "AnnotationsSequence.h"
#include "HelioTheme.h"
#include "ColourIDs.h"

static Array<String> getDynamics()
{
    return
    {
        "Pianissimo",
        "Piano",
        "Mezzo-piano",
        "Mezzo-forte",
        "Forte",
        "Fortissimo",
        "Fortepiano",
        "In rilievo",
        "Al niente",
        "Calando",
        "Calmando",
        "Crescendo",
        "Dal niente",
        "Diminuendo",
        "Morendo",
        "Marcato",
        "Pianoforte",
        "Sotto voce",
        "Smorzando"
    };
}

AnnotationDialog::AnnotationDialog(Component &owner,
    AnnotationsSequence *sequence, const AnnotationEvent &editedEvent,
    bool shouldAddNewEvent, float targetBeat) :
    originalEvent(editedEvent),
    originalSequence(sequence),
    ownerComponent(owner),
    addsNewEvent(shouldAddNewEvent)
{
    const auto isPhoneLayout = App::isRunningOnPhone();

    this->presetsCombo = make<MobileComboBox::Container>();
    this->addAndMakeVisible(this->presetsCombo.get());

    this->messageLabel = make<Label>();
    this->addChildComponent(this->messageLabel.get());
    this->messageLabel->setFont(Globals::UI::Fonts::L);
    this->messageLabel->setJustificationType(Justification::centred);
    this->messageLabel->setVisible(!isPhoneLayout);

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
        if (this->textEditor->getText().isNotEmpty())
        {
            this->dismiss();
        }
    };

    const auto colourButtonSizeWithMargin = isPhoneLayout ? 25 : 29;
    this->colourSwatches = make<ColourSwatches>(colourButtonSizeWithMargin);
    this->addAndMakeVisible(this->colourSwatches.get());

    this->textEditor = HelioTheme::makeSingleLineTextEditor(true, DialogBase::Defaults::textEditorFont);
    this->addAndMakeVisible(this->textEditor.get());

    jassert(this->originalSequence != nullptr);
    jassert(this->addsNewEvent || this->originalEvent.getSequence() != nullptr);

    const auto dynamics(getDynamics());
    const auto colours(ColourIDs::getColoursList());

    if (this->addsNewEvent)
    {
        Random r;
        const int i = r.nextInt(dynamics.size());
        this->originalEvent = AnnotationEvent(this->originalSequence,
            targetBeat, dynamics[i], colours[i]);

        this->originalSequence->checkpoint();
        this->originalSequence->insert(this->originalEvent, true);

        this->messageLabel->setText(TRANS(I18n::Dialog::annotationAddCaption), dontSendNotification);
        this->removeEventButton->setButtonText(TRANS(I18n::Dialog::cancel));
        this->okButton->setButtonText(TRANS(I18n::Dialog::add));
    }
    else
    {
        this->messageLabel->setText(TRANS(I18n::Dialog::annotationEditCaption), dontSendNotification);
        this->removeEventButton->setButtonText(TRANS(I18n::Dialog::delete_));
        this->okButton->setButtonText(TRANS(I18n::Dialog::apply));
    }

    this->colourSwatches->setSelectedColour(this->originalEvent.getColour());

    this->textEditor->addListener(this);
    this->textEditor->setText(this->originalEvent.getDescription(), dontSendNotification);
    // instead of selectAll(), which puts the caret at the start:
    this->textEditor->setCaretPosition(0);
    this->textEditor->moveCaretToEnd(true);

    this->messageLabel->setInterceptsMouseClicks(false, false);

    MenuPanel::Menu menu;
    for (int i = 0; i < getDynamics().size(); ++i)
    {
        const auto cmd = MenuItem::item(Icons::annotation,
            CommandIDs::JumpToAnnotation + i, dynamics[i])->
            withColour(colours[i]);
        menu.add(cmd);
    }

    this->presetsCombo->initWith(this->textEditor.get(), menu);

    this->setSize(this->getHorizontalSpacingExceptContent() +
        AnnotationDialog::colourSwatchesMargin * 2 +
        (colourButtonSizeWithMargin * this->colourSwatches->getNumButtons()) + 1,
        isPhoneLayout ? 100 : 190);

    this->updatePosition();
    this->updateOkButtonState();
}

AnnotationDialog::~AnnotationDialog()
{
    this->textEditor->removeListener(this);
}

void AnnotationDialog::resized()
{
    this->presetsCombo->setBounds(this->getContentBounds(true));
    this->messageLabel->setBounds(this->getCaptionBounds());

    this->okButton->setBounds(this->getButton1Bounds());
    this->removeEventButton->setBounds(this->getButton2Bounds());

    this->textEditor->setBounds(this->getRowBounds(0.225f, Globals::UI::textEditorHeight));
    this->colourSwatches->setBounds(this->getRowBounds(0.675f,
        Globals::UI::textEditorHeight, AnnotationDialog::colourSwatchesMargin));
}

void AnnotationDialog::parentHierarchyChanged()
{
    this->updatePosition();
}

void AnnotationDialog::parentSizeChanged()
{
    this->updatePosition();
}

void AnnotationDialog::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::DismissDialog)
    {
        this->cancelAndDisappear();
    }
    else
    {
        const auto dynamics(getDynamics());
        const auto colours(ColourIDs::getColoursList());
        const int targetIndex = commandId - CommandIDs::JumpToAnnotation;
        if (targetIndex >= 0 && targetIndex < dynamics.size())
        {
            const String text = dynamics[targetIndex];
            this->colourSwatches->setSelectedColour(colours[targetIndex]);
            this->textEditor->setText(text, true);
            this->resetKeyboardFocus();
        }
    }
}

UniquePointer<Component> AnnotationDialog::editingDialog(Component &owner,
    const AnnotationEvent &event)
{
    return make<AnnotationDialog>(owner,
        static_cast<AnnotationsSequence *>(event.getSequence()), event, false, 0.f);
}

UniquePointer<Component> AnnotationDialog::addingDialog(Component &owner,
    AnnotationsSequence *annotationsLayer, float targetBeat)
{
    return make<AnnotationDialog>(owner,
        annotationsLayer, AnnotationEvent(), true, targetBeat);
}

void AnnotationDialog::updateOkButtonState()
{
    const bool textIsEmpty = this->textEditor->getText().isEmpty();
    this->okButton->setAlpha(textIsEmpty ? 0.5f : 1.f);
    this->okButton->setEnabled(!textIsEmpty);
}

void AnnotationDialog::onColourButtonClicked(ColourButton *clickedButton)
{
    const Colour c(clickedButton->getColour());
    const AnnotationEvent newEvent =
        this->originalEvent.withDescription(this->textEditor->getText()).withColour(c);
    this->sendEventChange(newEvent);
}

void AnnotationDialog::sendEventChange(const AnnotationEvent &newEvent)
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

void AnnotationDialog::removeEvent()
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

void AnnotationDialog::textEditorTextChanged(TextEditor&)
{
    this->updateOkButtonState();

    const String text(this->textEditor->getText());
    AnnotationEvent newEvent = this->originalEvent.withDescription(text);
    const Colour c(this->colourSwatches->getColour());
    this->colourSwatches->setSelectedColour(c);
    newEvent = newEvent.withColour(c);
    this->sendEventChange(newEvent);
}

void AnnotationDialog::textEditorReturnKeyPressed(TextEditor &ed)
{
    if (this->textEditor->getText().isNotEmpty())
    {
        this->dismiss(); // apply on return key
        return;
    }

    this->resetKeyboardFocus();
}

void AnnotationDialog::textEditorEscapeKeyPressed(TextEditor&)
{
    this->cancelAndDisappear();
}

void AnnotationDialog::textEditorFocusLost(TextEditor&)
{
    this->updateOkButtonState();

    if (nullptr != dynamic_cast<TextEditor *>(Component::getCurrentlyFocusedComponent()))
    {
        return; // some other editor is focused
    }

    this->resetKeyboardFocus();
}

void AnnotationDialog::cancelAndDisappear()
{
    jassert(this->originalSequence != nullptr);

    if (this->addsNewEvent || this->hasMadeChanges)
    {
        this->originalSequence->undo();
    }

    this->dismiss();
}

Component *AnnotationDialog::getPrimaryFocusTarget()
{
#if PLATFORM_DESKTOP
    return this->textEditor.get();
#elif PLATFORM_MOBILE
    return this;
#endif
}