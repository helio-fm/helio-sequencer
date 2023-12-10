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
#include "TrackPropertiesDialog.h"
#include "CommandIDs.h"
#include "UndoStack.h"
#include "ProjectNode.h"
#include "IconComponent.h"

TrackPropertiesDialog::TrackPropertiesDialog(ProjectNode &project,
    WeakReference<MidiTrack> track,
    const String &title, const String &confirmation) :
    project(project),
    tracks(track)
{
    this->init(title, confirmation);
}

TrackPropertiesDialog::TrackPropertiesDialog(ProjectNode &project,
    Array<WeakReference<MidiTrack>> tracks) :
    project(project),
    tracks(tracks)
{
    this->init();
}

void TrackPropertiesDialog::init(const String &title, const String &confirmation)
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
    this->textEditor->setFont(Defaults::textEditorFont);
    this->textEditor->setIndents(Defaults::textEditorLeftIndent, Defaults::textEditorTopIndent);

    this->textEditor->onEscapeKey = [this]()
    {
        this->doCancel();
    };

    this->textEditor->onReturnKey = [this]()
    {
        if (this->textEditor->getText().isNotEmpty())
        {
            this->doOk(); // apply on return key
            return;
        }

        this->resetKeyboardFocus();
    };

    this->textEditor->onFocusLost = [this]()
    {
        this->updateControls();
        this->resetKeyboardFocus();
    };

    this->textEditor->onTextChange = [this]()
    {
        this->newName = this->textEditor->getText();
        this->updateControls();
        this->applyChangesIfAny();
    };

    this->multipleNamesIcon = make<IconComponent>(Icons::ellipsis);
    this->multipleNamesIcon->setIconAlphaMultiplier(0.5f);
    this->addChildComponent(this->multipleNamesIcon.get());

    if (this->tracks.size() == 1)
    {
        this->originalName = this->tracks.getFirst()->getTrackName();
        this->originalColour = this->tracks.getFirst()->getTrackColour();
    }

    this->newName = this->originalName;
    this->newColour = this->originalColour;

    this->colourSwatches->setSelectedColour(this->originalColour);
    this->textEditor->setText(this->originalName, dontSendNotification);
    // instead of selectAll(), which puts the caret at the start:
    this->textEditor->setCaretPosition(0);
    this->textEditor->moveCaretToEnd(true);

    this->messageLabel->setText(title.isNotEmpty() ?
        title : TRANS(I18n::Dialog::renameTrackCaption), dontSendNotification);
    this->okButton->setButtonText(confirmation.isNotEmpty() ?
        confirmation : TRANS(I18n::Dialog::apply));

    this->cancelButton->setButtonText(TRANS(I18n::Dialog::cancel));

    this->messageLabel->setInterceptsMouseClicks(false, false);

    static constexpr auto colourButtonSize = 30;
    this->setSize(this->getHorizontalSpacingExceptContent() +
        TrackPropertiesDialog::colourSwatchesMargin * 2 +
        colourButtonSize * this->colourSwatches->getNumButtons(),
        isPhoneLayout ? 100 : 190);

    this->updatePosition();
    this->updateControls();
}

TrackPropertiesDialog::~TrackPropertiesDialog() = default;

void TrackPropertiesDialog::resized()
{
    this->messageLabel->setBounds(this->getCaptionBounds());

    this->okButton->setBounds(this->getButton1Bounds());
    this->cancelButton->setBounds(this->getButton2Bounds());

    this->textEditor->setBounds(this->getRowBounds(0.225f, DialogBase::Defaults::textEditorHeight));
    this->colourSwatches->setBounds(this->getRowBounds(0.675f, DialogBase::Defaults::textEditorHeight,
        TrackPropertiesDialog::colourSwatchesMargin));

    constexpr auto iconSize = 16;
    this->multipleNamesIcon->setBounds(this->textEditor->getBounds()
        .withWidth(iconSize).translated(9, 6));
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
    if (commandId == CommandIDs::DismissDialog)
    {
        this->doCancel();
    }
}

void TrackPropertiesDialog::updateControls()
{
    const auto hasManyTracks = this->tracks.size() > 1;
    this->multipleNamesIcon->setVisible(hasManyTracks && this->newName.isEmpty());

    const auto colourIsOk = hasManyTracks || this->newColour != Colours::transparentBlack;
    const auto textIsOk = hasManyTracks || this->newName.trim().isNotEmpty();
    this->okButton->setEnabled(colourIsOk && textIsOk);
}

void TrackPropertiesDialog::onColourButtonClicked(ColourButton *clickedButton)
{
    this->newColour = clickedButton->getColour();
    this->updateControls();
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
    if (this->hasMadeChanges)
    {
        this->project.getUndoStack()->undoCurrentTransactionOnly();
    }

    this->project.getUndoStack()->beginNewTransaction();

    if (this->newName != this->originalName)
    {
        for (const auto &track : this->tracks)
        {
            track->setTrackName(this->newName, true, sendNotification);
        }
    }

    if (this->newColour != this->originalColour)
    {
        for (const auto &track : this->tracks)
        {
            track->setTrackColour(this->newColour, true, sendNotification);
        }
    }

    this->hasMadeChanges = true;
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

Component *TrackPropertiesDialog::getPrimaryFocusTarget()
{
#if PLATFORM_DESKTOP
    return this->textEditor.get();
#elif PLATFORM_MOBILE
    return this;
#endif
}
