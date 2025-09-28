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
#include "ColourIDs.h"

static Array<String> getAnnotationExamples()
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

AnnotationDialog::AnnotationDialog(AnnotationsSequence *sequence,
    const AnnotationEvent &editedEvent,
    bool shouldAddNewEvent, float targetBeat) :
    originalEvent(editedEvent),
    originalSequence(sequence),
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
        this->dialogDeleteAction();
    };

    this->okButton = make<TextButton>();
    this->addAndMakeVisible(this->okButton.get());
    this->okButton->onClick = [this]()
    {
        this->dialogApplyAction();
    };

    const auto colourButtonSizeWithMargin = isPhoneLayout ? 25 : 29;
    this->colourSwatches = make<ColourSwatches>(colourButtonSizeWithMargin);
    this->addAndMakeVisible(this->colourSwatches.get());

    this->textEditor = DialogBase::makeSingleLineTextEditor();
    this->addAndMakeVisible(this->textEditor.get());

    jassert(this->originalSequence != nullptr);
    jassert(this->addsNewEvent || this->originalEvent.getSequence() != nullptr);

    const auto annotationExamples = getAnnotationExamples();
    const auto colours = ColourIDs::getColoursList();

    if (this->addsNewEvent)
    {
        Random r;
        const int i = r.nextInt(jmin(colours.size(), annotationExamples.size()));
        this->originalEvent = AnnotationEvent(this->originalSequence,
            targetBeat, annotationExamples[i], colours[i]);

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

    this->textEditor->onTextChange = [this]()
    {
        this->updateOkButtonState();
        this->updateEvent();
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

    this->textEditor->setText(this->originalEvent.getDescription(), dontSendNotification);
    // instead of selectAll(), which puts the caret at the start:
    this->textEditor->setCaretPosition(0);
    this->textEditor->moveCaretToEnd(true);

    this->messageLabel->setInterceptsMouseClicks(false, false);

    MenuPanel::Menu menu;
    for (int i = 0; i < annotationExamples.size(); ++i)
    {
        menu.add(MenuItem::item(Icons::annotation,
            CommandIDs::SelectPreset + i, annotationExamples[i])->withColour(colours[i]));
    }

    this->presetsCombo->initWith(this->textEditor.get(), menu);

    this->setSize(this->getHorizontalSpacingExceptContent() +
        AnnotationDialog::colourSwatchesMargin * 2 +
        (colourButtonSizeWithMargin * this->colourSwatches->getNumButtons()) + 1,
        isPhoneLayout ? 100 : 190);

    this->updatePosition();
    this->updateOkButtonState();
}

AnnotationDialog::~AnnotationDialog() = default;

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
    const auto annotationExamples = getAnnotationExamples();
    const int targetIndex = commandId - CommandIDs::SelectPreset;
    if (targetIndex >= 0 && targetIndex < annotationExamples.size())
    {
        const auto colours = ColourIDs::getColoursList();
        this->colourSwatches->setSelectedColour(colours[targetIndex]);
        this->textEditor->setText(annotationExamples[targetIndex], true);
        this->updateEvent();
        this->resetKeyboardFocus();
    }
    else if (commandId == CommandIDs::DialogNextPreset)
    {
        if (const auto colour = this->colourSwatches->selectNextColour())
        {
            this->sendEventChanges(this->originalEvent.
                withDescription(this->textEditor->getText()).withColour(*colour));
        }
    }
    else if (commandId == CommandIDs::DialogPreviousPreset)
    {
        if (const auto colour = this->colourSwatches->selectPreviousColour())
        {
            this->sendEventChanges(this->originalEvent.
                withDescription(this->textEditor->getText()).withColour(*colour));
        }
    }
    else if (commandId == CommandIDs::DialogShowPresetsList)
    {
        this->presetsCombo->postCommandMessage(CommandIDs::ToggleShowHideCombo);
    }
    else
    {
        DialogBase::handleCommandMessage(commandId);
    }
}

bool AnnotationDialog::keyPressed(const KeyPress &key)
{
    if (this->presetsCombo->isShowingMenu())
    {
        getHotkeyScheme()->dispatchKeyPress(key,
            this->presetsCombo.get(), this->presetsCombo.get());
    }
    else
    {
        getHotkeyScheme()->dispatchKeyPress(key, this, this);
    }

    return true;
}

bool AnnotationDialog::keyStateChanged(bool isKeyDown)
{
    if (this->presetsCombo->isShowingMenu())
    {
        getHotkeyScheme()->dispatchKeyStateChange(isKeyDown,
            this->presetsCombo.get(), this->presetsCombo.get());
    }
    else
    {
        getHotkeyScheme()->dispatchKeyStateChange(isKeyDown, this, this);
    }

    return true;
}

void AnnotationDialog::dialogCancelAction()
{
    jassert(this->originalSequence != nullptr);

    if (this->addsNewEvent || this->hasMadeChanges)
    {
        this->originalSequence->undo();
    }

    this->dismiss();
}

void AnnotationDialog::dialogApplyAction()
{
    if (this->textEditor->getText().isNotEmpty())
    {
        this->dismiss();
        return;
    }

    this->resetKeyboardFocus();
}

void AnnotationDialog::dialogDeleteAction()
{
    if (this->addsNewEvent)
    {
        this->dialogCancelAction();
    }
    else
    {
        this->removeEvent();
        this->dismiss();
    }
}

UniquePointer<Component> AnnotationDialog::editingDialog(const AnnotationEvent &event)
{
    return make<AnnotationDialog>
        (static_cast<AnnotationsSequence *>(event.getSequence()), event, false, 0.f);
}

UniquePointer<Component> AnnotationDialog::addingDialog(AnnotationsSequence *annotationsSequence, float targetBeat)
{
    return make<AnnotationDialog>
        (annotationsSequence, AnnotationEvent(), true, targetBeat);
}

void AnnotationDialog::updateOkButtonState()
{
    const bool textIsEmpty = this->textEditor->getText().isEmpty();
    this->okButton->setAlpha(textIsEmpty ? 0.5f : 1.f);
    this->okButton->setEnabled(!textIsEmpty);
}

void AnnotationDialog::onColourButtonClicked(ColourButton *button)
{
    this->sendEventChanges(this->originalEvent.
        withDescription(this->textEditor->getText()).withColour(button->getColour()));
}

void AnnotationDialog::sendEventChanges(const AnnotationEvent &newEvent)
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

void AnnotationDialog::updateEvent()
{
    const auto text = this->textEditor->getText();
    const auto colour = this->colourSwatches->getColour();
    this->sendEventChanges(this->originalEvent.withDescription(text).withColour(colour));
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

Component *AnnotationDialog::getPrimaryFocusTarget()
{
#if PLATFORM_DESKTOP
    return this->textEditor.get();
#elif PLATFORM_MOBILE
    return this;
#endif
}