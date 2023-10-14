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
#include "KeySignatureDialog.h"

#include "KeySignaturesSequence.h"
#include "SeparatorHorizontalFading.h"
#include "ProjectNode.h"
#include "ProjectMetadata.h"
#include "PlayButton.h"
#include "Temperament.h"
#include "Transport.h"
#include "CommandIDs.h"
#include "Config.h"
#include "App.h"

class ScalePreviewThread final : public Thread
{
public:

    ScalePreviewThread(const Transport &transport, Array<int> &&s) :
        Thread("ScalePreview"),
        transport(transport),
        sequence(s) {}

    void run() override
    {
        for (const auto key : this->sequence)
        {
            if (this->threadShouldExit())
            {
                this->transport.stopSound();
                return;
            }

            this->transport.stopSound();
            Thread::wait(25);
            this->transport.previewKey(String(), key,
                Globals::Defaults::previewNoteVelocity,
                Globals::Defaults::previewNoteLength);

            int c = 400;
            while (c > 0)
            {
                const auto a = Time::getMillisecondCounter();
                Thread::wait(25);
                const auto b = Time::getMillisecondCounter();
                c -= (b - a);

                if (this->threadShouldExit())
                {
                    this->transport.stopSound();
                    return;
                }
            }
        }

        this->transport.stopSound();
    }

private:

    const Transport &transport;
    const Array<int> sequence;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScalePreviewThread)
};

static inline const Temperament::Period &getPeriod(ProjectNode &project)
{
    return project.getProjectInfo()->getTemperament()->getPeriod();
}

KeySignatureDialog::KeySignatureDialog(ProjectNode &project, KeySignaturesSequence *keySequence,
    const KeySignatureEvent &editedEvent, bool shouldAddNewEvent, float targetBeat) :
    transport(project.getTransport()),
    originalEvent(editedEvent),
    originalSequence(keySequence),
    project(project),
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
        if (this->scaleNameEditor->getText().isNotEmpty())
        {
            this->dismiss();
        }
    };

    this->keySelector = make<KeySelector>(getPeriod(project));
    this->addAndMakeVisible(this->keySelector.get());

    this->separator = make<SeparatorHorizontalFading>();
    this->addAndMakeVisible(this->separator.get());

    this->scaleEditor = make<ScaleEditor>();
    this->addAndMakeVisible(this->scaleEditor.get());

    this->savePresetButton = make<IconButton>(Icons::commit, CommandIDs::SavePreset, this);
    this->addAndMakeVisible(this->savePresetButton.get());

    this->playButton = make<PlayButton>(this);
    this->addAndMakeVisible(this->playButton.get());

    this->scaleNameEditor = make<TextEditor>();
    this->addAndMakeVisible(this->scaleNameEditor.get());
    this->scaleNameEditor->setMultiLine(false);
    this->scaleNameEditor->setReturnKeyStartsNewLine(false);
    this->scaleNameEditor->setReadOnly(false);
    this->scaleNameEditor->setScrollbarsShown(true);
    this->scaleNameEditor->setCaretVisible(true);
    this->scaleNameEditor->setPopupMenuEnabled(true);

    this->scaleNameEditor->addListener(this);
    this->scaleNameEditor->setFont(Globals::UI::Fonts::L);

    this->reloadScalesList();

    this->transport.stopPlaybackAndRecording();

    jassert(this->originalSequence != nullptr);
    jassert(this->addsNewEvent || this->originalEvent.getSequence() != nullptr);

    if (this->addsNewEvent)
    {
        Random r;
        const auto i = r.nextInt(this->scales.size());
        this->rootKey = 0;
        this->scale = this->scales[i];
        this->scaleEditor->setScale(this->scale);
        this->keySelector->setSelectedKey(this->rootKey);
        this->scaleNameEditor->setText(this->scale->getLocalizedName());
        this->originalEvent = KeySignatureEvent(this->originalSequence, this->scale, targetBeat, this->rootKey);

        this->originalSequence->checkpoint();
        this->originalSequence->insert(this->originalEvent, true);

        this->messageLabel->setText(TRANS(I18n::Dialog::keySignatureAddCaption), dontSendNotification);
        this->okButton->setButtonText(TRANS(I18n::Dialog::add));
        this->removeEventButton->setButtonText(TRANS(I18n::Dialog::cancel));
    }
    else
    {
        this->rootKey = this->originalEvent.getRootKey();
        this->scale = this->originalEvent.getScale();
        this->scaleEditor->setScale(this->scale);
        this->keySelector->setSelectedKey(this->rootKey);
        this->scaleNameEditor->setText(this->scale->getLocalizedName(), dontSendNotification);

        this->messageLabel->setText(TRANS(I18n::Dialog::keySignatureEditCaption), dontSendNotification);
        this->removeEventButton->setButtonText(TRANS(I18n::Dialog::delete_));
        this->okButton->setButtonText(TRANS(I18n::Dialog::apply));
    }

    // instead of selectAll(), which puts the caret at the start:
    this->scaleNameEditor->setCaretPosition(0);
    this->scaleNameEditor->moveCaretToEnd(true);

    this->messageLabel->setInterceptsMouseClicks(false, false);

    static constexpr auto keyButtonSize = 34;
    const auto periodSize = getPeriod(project).size();

    this->setSize(this->getHorizontalSpacingExceptContent() + keyButtonSize * periodSize,
        isPhoneLayout ? DialogBase::Defaults::Phone::maxDialogHeight : 240);

    this->updatePosition();
    this->updateButtonsState();
}

KeySignatureDialog::~KeySignatureDialog()
{
    if (this->scalePreviewThread != nullptr)
    {
        this->scalePreviewThread->stopThread(500);
    }

    this->presetsCombo->cleanup();
    this->transport.stopPlayback();
    this->scaleNameEditor->removeListener(this);
}

void KeySignatureDialog::resized()
{
    this->presetsCombo->setBounds(this->getContentBounds(true));
    this->messageLabel->setBounds(this->getCaptionBounds());

    this->okButton->setBounds(this->getButton1Bounds());
    this->removeEventButton->setBounds(this->getButton2Bounds());

    this->keySelector->setBounds(this->getRowBounds(0.17f, DialogBase::Defaults::textEditorHeight));
    this->separator->setBounds(this->getRowBounds(0.34f, 4));

    this->scaleEditor->setBounds(this->getRowBounds(0.5f, DialogBase::Defaults::textEditorHeight));

    static constexpr auto scaleHelperButtonWidth = 36;
    static constexpr auto scaleEditorMargin = 4;
    const auto scaleEditorRow = this->getRowBounds(0.825f, DialogBase::Defaults::textEditorHeight, scaleEditorMargin);
    this->scaleNameEditor->setBounds(scaleEditorRow.withTrimmedRight((scaleHelperButtonWidth + scaleEditorMargin) * 2));

    auto buttonsArea = scaleEditorRow.withTrimmedLeft(this->scaleNameEditor->getWidth());
    this->playButton->setBounds(buttonsArea.removeFromRight(scaleHelperButtonWidth));
    this->savePresetButton->setBounds(buttonsArea.removeFromRight(scaleHelperButtonWidth).withSizeKeepingCentre(20, 20));
}

void KeySignatureDialog::parentHierarchyChanged()
{
    this->updatePosition();
}

void KeySignatureDialog::parentSizeChanged()
{
    this->updatePosition();
}

void KeySignatureDialog::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::DismissDialog)
    {
        this->cancelAndDisappear();
    }
    else if (commandId == CommandIDs::SavePreset)
    {
        this->savePreset();
    }
    else if (commandId == CommandIDs::TransportPlaybackStart)
    {
        const auto temperament =
            this->project.getProjectInfo()->getTemperament();

        // scale preview: simply play it forward and backward
        auto scaleKeys = this->scale->getUpScale();
        scaleKeys.addArray(this->scale->getDownScale());
        for (int i = 0; i < scaleKeys.size(); ++i)
        {
            const auto key = scaleKeys.getUnchecked(i);
            scaleKeys.getReference(i) = temperament->getMiddleC() + this->rootKey + key;
        }
        
        if (this->scalePreviewThread != nullptr)
        {
            this->scalePreviewThread->stopThread(500);
        }

        this->scalePreviewThread = make<ScalePreviewThread>(this->transport, move(scaleKeys));
        this->scalePreviewThread->startThread(5);

        this->playButton->setPlaying(true);
    }
    else if (commandId == CommandIDs::TransportStop)
    {
        if (this->scalePreviewThread != nullptr)
        {
            this->scalePreviewThread->stopThread(500);
        }

        this->playButton->setPlaying(false);
    }
    else
    {
        const int scaleIndex = commandId - CommandIDs::SelectScale;
        if (scaleIndex >= 0 && scaleIndex < this->scales.size())
        {
            this->playButton->setPlaying(false);
            this->scale = this->scales[scaleIndex];
            this->scaleEditor->setScale(this->scale);

            this->scaleNameEditor->setText(this->scale->getLocalizedName(), false);
            const auto newEvent = this->originalEvent
                .withRootKey(this->rootKey).withScale(this->scale);

            this->sendEventChange(newEvent);
            this->updateButtonsState();
            this->resetKeyboardFocus();
        }
    }
}

void KeySignatureDialog::savePreset()
{
    const auto scaleName = this->scaleNameEditor->getText();
    if (scaleName.isEmpty() || !this->scale->isValid())
    {
        jassertfalse;
        return;
    }

    Scale::Ptr scale(new Scale(scaleName,
        this->scale->getKeys(), this->scale->getBasePeriod()));

    App::Config().getScales()->updateUserResource(scale);

    this->reloadScalesList();
    this->updateButtonsState();
}

void KeySignatureDialog::reloadScalesList()
{
    this->scales.clearQuick();
    const auto periodSize = getPeriod(project).size();
    for (const auto &scale : App::Config().getScales()->getAll())
    {
        if (scale->getBasePeriod() == periodSize)
        {
            this->scales.add(scale);
        }
    }

    MenuPanel::Menu menu;
    for (int i = 0; i < this->scales.size(); ++i)
    {
        const auto &s = this->scales.getUnchecked(i);
        menu.add(MenuItem::item(Icons::ellipsis, CommandIDs::SelectScale + i, s->getLocalizedName()));
    }

    this->presetsCombo->initWith(this->scaleNameEditor.get(), menu);
}

void KeySignatureDialog::updateButtonsState()
{
    const bool scaleIsValid = this->scale->isValid();
    const bool nameIsNotEmpty = this->scaleNameEditor->getText().isNotEmpty();
    const bool buttonsEnabled = scaleIsValid && nameIsNotEmpty;
    this->okButton->setEnabled(buttonsEnabled);

    bool hasSameScaleInList = false;
    for (const auto &s : this->scales)
    {
        hasSameScaleInList |=
            (s->getUnlocalizedName() == this->scale->getUnlocalizedName() &&
             s->getKeys() == this->scale->getKeys());
    }

    this->savePresetButton->setEnabled(buttonsEnabled && !hasSameScaleInList);
}

UniquePointer<Component> KeySignatureDialog::editingDialog(ProjectNode &project,
    const KeySignatureEvent &event)
{
    return make<KeySignatureDialog>(project,
        static_cast<KeySignaturesSequence *>(event.getSequence()), event, false, 0.f);
}

UniquePointer<Component> KeySignatureDialog::addingDialog(ProjectNode &project,
    KeySignaturesSequence *annotationsLayer, float targetBeat)
{
    return make<KeySignatureDialog>(project,
        annotationsLayer, KeySignatureEvent(), true, targetBeat);
}

void KeySignatureDialog::sendEventChange(const KeySignatureEvent &newEvent)
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

void KeySignatureDialog::removeEvent()
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

void KeySignatureDialog::cancelAndDisappear()
{
    jassert(this->originalSequence != nullptr);

    if (this->addsNewEvent || this->hasMadeChanges)
    {
        this->originalSequence->undo();
    }

    this->dismiss();
}

void KeySignatureDialog::previewNote(int keyRelative) const
{
    const auto temperament = this->project.getProjectInfo()->getTemperament();
    const int key = temperament->getMiddleC() + keyRelative;
    this->transport.stopSound();
    this->transport.previewKey(String(), key,
        Globals::Defaults::previewNoteVelocity,
        Globals::Defaults::previewNoteLength);
}

//===----------------------------------------------------------------------===//
// KeySelector::Listener
//===----------------------------------------------------------------------===//

void KeySignatureDialog::onKeyChanged(int key)
{
    if (this->rootKey != key)
    {
        this->rootKey = key;
        const auto newEvent = this->originalEvent
            .withRootKey(key).withScale(this->scale);

        this->sendEventChange(newEvent);
    }
}

void KeySignatureDialog::onRootKeyPreview(int key)
{
    this->previewNote(key);
}

//===----------------------------------------------------------------------===//
// ScaleEditor::Listener
//===----------------------------------------------------------------------===//

void KeySignatureDialog::onScaleChanged(const Scale::Ptr scale)
{
    if (!this->scale->isEquivalentTo(scale))
    {
        this->scale = scale;

        // Update name, if found equivalent:
        for (int i = 0; i < this->scales.size(); ++i)
        {
            const auto &s = this->scales.getUnchecked(i);
            if (s->isEquivalentTo(scale))
            {
                this->scaleNameEditor->setText(s->getLocalizedName());
                this->scaleEditor->setScale(s);
                this->scale = s;
                break;
            }
        }

        const auto newEvent = this->originalEvent
            .withRootKey(this->rootKey).withScale(this->scale);

        this->sendEventChange(newEvent);
        this->updateButtonsState();
    }
}

void KeySignatureDialog::onScaleNotePreview(int key)
{
    this->previewNote(this->rootKey + key);
}

//===----------------------------------------------------------------------===//
// TextEditor::Listener
//===----------------------------------------------------------------------===//

void KeySignatureDialog::textEditorTextChanged(TextEditor &ed)
{
    this->scale = this->scale->withName(this->scaleNameEditor->getText());
    this->scaleEditor->setScale(this->scale);
    const auto newEvent = this->originalEvent
        .withRootKey(this->rootKey).withScale(scale);

    this->sendEventChange(newEvent);
    this->updateButtonsState();
}

void KeySignatureDialog::textEditorReturnKeyPressed(TextEditor &ed)
{
    if (this->scaleNameEditor->getText().isNotEmpty())
    {
        this->dismiss(); // apply on return key
        return;
    }

    this->resetKeyboardFocus();
}

void KeySignatureDialog::textEditorEscapeKeyPressed(TextEditor &)
{
    this->cancelAndDisappear();
}

void KeySignatureDialog::textEditorFocusLost(TextEditor &)
{
    this->updateButtonsState();

    if (nullptr != dynamic_cast<TextEditor *>(Component::getCurrentlyFocusedComponent()))
    {
        return; // some other editor is focused
    }

    this->resetKeyboardFocus();
}

Component *KeySignatureDialog::getPrimaryFocusTarget()
{
#if PLATFORM_DESKTOP
    return this->scaleNameEditor.get();
#elif PLATFORM_MOBILE
    return this;
#endif
}
