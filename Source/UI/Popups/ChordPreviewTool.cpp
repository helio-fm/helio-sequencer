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
#include "ChordPreviewTool.h"
#include "SequencerOperations.h"
#include "PianoSequence.h"
#include "PianoRoll.h"
#include "KeySignatureEvent.h"
#include "KeySignaturesSequence.h"
#include "TimeSignaturesAggregator.h"
#include "NoteNameComponent.h"
#include "MidiTrack.h"
#include "HotkeyScheme.h"
#include "ColourIDs.h"
#include "UndoActionIDs.h"
#include "ComponentIDs.h"

static UniquePointer<Label> makePopupButtonLabel(const String &text)
{
    constexpr int size = 50;
    auto newLabel = make<Label>(text, text);
    newLabel->setJustificationType(Justification::centred);
    newLabel->setBounds(0, 0, size, size);
    newLabel->setFont(App::isRunningOnPhone() ? Globals::UI::Fonts::XS : Globals::UI::Fonts::S);
    return newLabel;
}

class ChordButton final : public PopupButton
{
public:

    ChordButton(UniquePointer<Component> &&newOwnedComponent,
        PopupButton::Shape shapeType, Colour colour) :
        PopupButton(shapeType, colour),
        ownedComponent(move(newOwnedComponent))
    {
        this->ownedComponent->setInterceptsMouseClicks(false, false);
        this->addAndMakeVisible(this->ownedComponent.get());
    }

    void resized() override
    {
        PopupButton::resized();

        this->ownedComponent->
            setTopLeftPosition((this->getWidth() / 2) - (ownedComponent->getWidth() / 2),
                               (this->getHeight() / 2) - (ownedComponent->getHeight() / 2));
    }

private:

    UniquePointer<Component> ownedComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordButton)
};

class ChordRootKeyAimMark final : public Component
{
public:

    ChordRootKeyAimMark()
    {
        this->setAccessible(false);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(false, false);

        constexpr int size = 9;
        this->setBounds(0, 0, size, size);
    }

    void paint(Graphics &g) override
    {
        const auto w = float(this->getWidth());
        const auto h = float(this->getHeight());

        g.setColour(this->outlineColour);
        g.fillRect(0.f, h / 2.f - 1.5f, w, 3.f);
        g.fillRect(w / 2.f - 1.5f, 0.f, 3.f, h);

        g.setColour(this->fillColour);
        g.fillRect(1.f, h / 2.f - 0.5f, w - 2.f, 1.f);
        g.fillRect(w / 2.f - 0.5f, 1.f, 1.f, h - 2.f);
    }

private:

    const Colour fillColour =
        findDefaultColour(Label::textColourId).withMultipliedAlpha(0.75f);

    const Colour outlineColour =
        findDefaultColour(ColourIDs::Roll::blackKey).withMultipliedAlpha(0.5f);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordRootKeyAimMark)
};

class ChordTooltip final : public Component
{
public:

    ChordTooltip(const String &chordName, const String &keyDegree, const String &keyName)
    {
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
        this->setWantsKeyboardFocus(false);

        const auto useFixedDo = App::Config().getUiFlags()->isUsingFixedDoNotation();
        const auto fontSize = App::isRunningOnPhone() ?
            Globals::UI::Fonts::S : Globals::UI::Fonts::M;

        this->chordNameLabel = make<Label>(String(), chordName);
        this->addAndMakeVisible(this->chordNameLabel.get());
        this->chordNameLabel->setFont(fontSize);
        this->chordNameLabel->setJustificationType(Justification::centredLeft);
        this->chordNameLabel->setBorderSize({});
        this->chordNameLabel->setInterceptsMouseClicks(false, false);
        this->chordNameLabel->setAlpha(0.75f);

        this->keyDegreeLabel = make<Label>(String(), keyDegree);
        this->addAndMakeVisible(this->keyDegreeLabel.get());
        this->keyDegreeLabel->setFont(fontSize);
        this->keyDegreeLabel->setJustificationType(Justification::centredLeft);
        this->keyDegreeLabel->setBorderSize({});
        this->keyDegreeLabel->setInterceptsMouseClicks(false, false);

        this->rootKeyNameComponent = make<NoteNameComponent>(false, fontSize);
        this->addAndMakeVisible(this->rootKeyNameComponent.get());
        this->rootKeyNameComponent->setNoteName(keyName, String(), useFixedDo);

        const auto keyDegreeWidth = this->keyDegreeLabel->getFont().
            getStringWidth(keyDegree) + (keyDegree.isEmpty() ? 0 : 3);
        const auto rootKeyWidth = this->rootKeyNameComponent->getRequiredWidth();
        const auto chordNameWidth = this->chordNameLabel->getFont().getStringWidth(chordName);
        const auto contentWidth = jmax(chordNameWidth, keyDegreeWidth + rootKeyWidth);

        constexpr int lineHeight = int(Globals::UI::Fonts::M);
        if (chordName.isEmpty())
        {
            this->setSize(contentWidth, lineHeight);
            auto localBounds = this->getLocalBounds();
            this->keyDegreeLabel->setBounds(localBounds.removeFromLeft(keyDegreeWidth));
            this->rootKeyNameComponent->setBounds(localBounds);
        }
        else
        {
            this->setSize(contentWidth, lineHeight * 2);
            auto localBounds = this->getLocalBounds();
            const auto topLine = localBounds.removeFromTop(lineHeight).withSizeKeepingCentre(chordNameWidth, lineHeight);
            this->chordNameLabel->setBounds(topLine);
            auto bottomLine = localBounds.withSizeKeepingCentre(keyDegreeWidth + rootKeyWidth, lineHeight);
            this->keyDegreeLabel->setBounds(bottomLine.removeFromLeft(keyDegreeWidth));
            this->rootKeyNameComponent->setBounds(bottomLine);
        }
    }

private:

    UniquePointer<Label> chordNameLabel;
    UniquePointer<Label> keyDegreeLabel;
    UniquePointer<NoteNameComponent> rootKeyNameComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordTooltip)
};

ChordPreviewTool::ChordPreviewTool(PianoRoll &roll,
    WeakReference<PianoSequence> sequence, const Clip &clip,
    WeakReference<KeySignaturesSequence> harmonicContext,
    WeakReference<TimeSignaturesAggregator> timeContext) :
    roll(roll),
    sequence(sequence),
    clip(clip),
    harmonicContext(harmonicContext),
    timeContext(timeContext)
{
    this->setComponentID(ComponentIDs::chordTool);

    this->setAccessible(false);
    this->setInterceptsMouseClicks(false, true);
    this->setWantsKeyboardFocus(true);
    this->setFocusContainerType(Component::FocusContainerType::focusContainer);
    this->setSize(420, 420);

    const auto bgColour = findDefaultColour(ColourIDs::Roll::blackKey);

    const int buttonSize = App::isRunningOnPhone() ? 57 : 67;
    this->centreButton = make<ChordButton>(make<ChordRootKeyAimMark>(),
        PopupButton::Shape::Circle, bgColour.withMultipliedAlpha(0.2f));
    this->centreButton->setSize(buttonSize, buttonSize);
    this->centreButton->setMouseCursor(MouseCursor::DraggingHandCursor);
    this->addAndMakeVisible(this->centreButton.get());
    this->centreButton->setCentreRelative(0.5f, 0.5f);

    const int numChordsToDisplay = jmin(16, this->chords.size());
    for (int i = 0; i < numChordsToDisplay; ++i)
    {
        const auto chord = this->chords.getUnchecked(i);
        const auto radians = float(i) * (MathConstants<float>::twoPi / float(numChordsToDisplay));
        const auto defaultRadius = App::isRunningOnPhone() ? 95 : 130;
        // the more chords there are, the larger the raduis should be:
        const auto radius = defaultRadius + jlimit(0, 8, numChordsToDisplay - 10) * 10;
        const auto centreOffset = Point<int>(0, -radius).transformedBy(AffineTransform::rotation(radians, 0, 0));
        const auto colour = Colour(float(i) / float(numChordsToDisplay), 0.69f, 1.f, 1.f).interpolatedWith(bgColour, 0.3f);
        auto *button = this->chordButtons.add(new ChordButton(makePopupButtonLabel(chord->getName()), PopupButton::Shape::Hex, colour));
        button->setSize(buttonSize, buttonSize);
        button->setUserData(chord->getResourceId());
        const auto buttonSizeOffset = button->getLocalBounds().getCentre();
        const auto buttonPosition = this->getLocalBounds().getCentre() + centreOffset - buttonSizeOffset;
        button->setTopLeftPosition(buttonPosition);
        this->addAndMakeVisible(button);
    }
}

ChordPreviewTool::~ChordPreviewTool()
{
#if PLATFORM_MOBILE
    if (!App::isWorkspaceInitialized())
    {
        // this can happen when the app is manually shut down
        // while this modal component is still showing,
        // and the workspace doesn't exist at this point:
        return;
    }
#endif

    this->stopSound();

    App::fadeOutComponent(this, Globals::UI::fadeOutShort);
}

void ChordPreviewTool::parentHierarchyChanged()
{
    this->detectContextAndRebuild();
}

void ChordPreviewTool::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::ChordToolDismissApply:
        this->dismiss();
        break;
    case CommandIDs::ChordToolDismissCancel:
        this->undoChangesIfAny();
        this->dismiss();
        break;
    case CommandIDs::ChordToolRootKeyUp:
        this->setBounds(this->getBounds().translated(0, -this->roll.getRowHeight()));
        this->detectContextAndRebuild();
        break;
    case CommandIDs::ChordToolRootKeyDown:
        this->setBounds(this->getBounds().translated(0, this->roll.getRowHeight()));
        this->detectContextAndRebuild();
        break;
    case CommandIDs::ChordToolBeatShiftLeft:
        this->setBounds(this->getBounds().translated(
            roundToIntAccurate(-this->roll.getMinVisibleBeatForCurrentZoomLevel() * this->roll.getBeatWidth()), 0));
        this->detectContextAndRebuild();
        break;
    case CommandIDs::ChordToolBeatShiftRight:
        this->setBounds(this->getBounds().translated(
            roundToIntAccurate(this->roll.getMinVisibleBeatForCurrentZoomLevel() * this->roll.getBeatWidth()), 0));
        this->detectContextAndRebuild();
        break;
    case CommandIDs::ChordToolNextPreset:
        if (this->lastBuiltChord != nullptr)
        {
            this->selectPreset((this->chords.indexOf(this->lastBuiltChord) + 1) % this->chords.size());
        }
        else
        {
            this->selectPreset(0);
        }
        break;
    case CommandIDs::ChordToolPreviousPreset:
        if (this->lastBuiltChord != nullptr)
        {
            this->selectPreset((this->chords.indexOf(this->lastBuiltChord) +
                this->chords.size() - 1) % this->chords.size());
        }
        else
        {
            this->selectPreset(0);
        }
        break;
    case CommandIDs::ChordToolPreset1:
        this->selectPreset(0);
        break;
    case CommandIDs::ChordToolPreset2:
        this->selectPreset(1);
        break;
    case CommandIDs::ChordToolPreset3:
        this->selectPreset(2);
        break;
    case CommandIDs::ChordToolPreset4:
        this->selectPreset(3);
        break;
    case CommandIDs::ChordToolPreset5:
        this->selectPreset(4);
        break;
    case CommandIDs::ChordToolPreset6:
        this->selectPreset(5);
        break;
    case CommandIDs::ChordToolPreset7:
        this->selectPreset(6);
        break;
    case CommandIDs::ChordToolPreset8:
        this->selectPreset(7);
        break;
    case CommandIDs::ChordToolPreset9:
        this->selectPreset(8);
        break;
    case CommandIDs::ChordToolPreset10:
        this->selectPreset(9);
        break;
    case CommandIDs::ChordToolPreset11:
        this->selectPreset(10);
        break;
    case CommandIDs::ChordToolPreset12:
        this->selectPreset(11);
        break;
    default:
        break;
    }
}

void ChordPreviewTool::selectPreset(int presetIndex)
{
    App::Layout().hideTooltipIfAny();

    if (auto chord = this->chords[presetIndex])
    {
        this->buildChord(chord);
    }

    if (auto *button = this->chordButtons[presetIndex])
    {
        this->onPopupsResetState(button);
    }
}

void ChordPreviewTool::detectContextAndRebuild()
{
    const bool hasChanges = this->detectKeyBeatAndContext();
    if (!hasChanges)
    {
        return;
    }

    const auto absKey = this->targetKey + this->clip.getKey();
    const auto targetKeyOffset = absKey % this->scale->getBasePeriod();
    const auto chromaticOffset = targetKeyOffset - this->scaleRootKey;
    const auto scaleDegree = this->scale->getScaleKey(chromaticOffset);

    if (scaleDegree >= 0)
    {
        const auto degreeName = this->degreeNames[scaleDegree];
        int periodNumber = 0;
        auto keyName = this->roll.getTemperament()->
            getMidiNoteName(absKey, this->scaleRootKey, this->scaleRootKeyName, periodNumber);

        if (this->lastBuiltChord != nullptr)
        {
            this->buildChord(this->lastBuiltChord);
            auto tooltip = make<ChordTooltip>(this->lastBuiltChord->getName(), degreeName, keyName);
            App::Layout().showTooltip(move(tooltip));
        }
        else
        {
            this->buildNewNote(true);
            auto tooltip = make<ChordTooltip>(String(), degreeName, keyName);
            App::Layout().showTooltip(move(tooltip));
        }
    }
    else
    {
        this->buildNewNote(true);
        App::Layout().hideTooltipIfAny();
    }
}

bool ChordPreviewTool::keyPressed(const KeyPress &key)
{
    // all modal components have to process hotkeys like this:
    App::Config().getHotkeySchemes()->getCurrent()->dispatchKeyPress(key, this, this);
    return true;
}

bool ChordPreviewTool::keyStateChanged(bool isKeyDown)
{
    return true;
}

void ChordPreviewTool::inputAttemptWhenModal()
{
    // a hack, see the same in CommandPalette, DialogBase and ModalCallout:
    this->roll.resetDraggingAnchors();

    this->dismiss();
}

void ChordPreviewTool::onPopupsResetState(PopupButton *button)
{
    if (button == this->centreButton.get())
    {
        return;
    }

    // reset all buttons except for the clicked one:
    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        if (auto *pb = dynamic_cast<PopupButton *>(this->getChildComponent(i)))
        {
            const bool shouldBeTurnedOn = (pb == button);
            pb->setState(shouldBeTurnedOn);
        }
    }
}

void ChordPreviewTool::onPopupButtonFirstAction(PopupButton *button)
{
    if (button == this->centreButton.get())
    {
        return;
    }

    App::Layout().hideTooltipIfAny();

    if (auto chord = this->findChordFor(button))
    {
        this->buildChord(chord);
    }
}

void ChordPreviewTool::onPopupButtonSecondAction(PopupButton *button)
{
    this->dismiss();
}

bool ChordPreviewTool::onPopupButtonDrag(PopupButton *button)
{
    if (button == this->centreButton.get())
    {
        const auto dragDelta = this->centreButton->getDragDelta();
        this->setTopLeftPosition(this->getPosition() + dragDelta);
        this->detectContextAndRebuild();
    }

    return false; // let the button return to anchor
}

Chord::Ptr ChordPreviewTool::findChordFor(PopupButton *button) const
{
    for (const auto &chord : this->chords)
    {
        if (chord->getResourceId() == button->getUserData())
        {
            return chord;
        }
    }

    return nullptr;
}

void ChordPreviewTool::buildChord(const Chord::Ptr chord)
{
    if (!chord->isValid())
    {
        jassertfalse;
        return;
    }

    this->lastBuiltChord = chord;

    const auto absKey = this->targetKey + this->clip.getKey();
    const auto period = (absKey - this->scaleRootKey) / this->scale->getBasePeriod();
    const auto periodOffset = period * this->scale->getBasePeriod();
    const auto targetKeyOffset = absKey % this->scale->getBasePeriod();
    const auto chromaticOffset = targetKeyOffset - this->scaleRootKey;
    const auto scaleDegree = this->scale->getScaleKey(chromaticOffset);
    if (scaleDegree < 0) // todo some indication that the key is not in scale?
    {
        return;
    }

    this->undoChangesIfAny();
    this->stopSound();

    if (!this->hasMadeChanges)
    {
        this->sequence->checkpoint(UndoActionIDs::MakeChord);
    }

    const auto temperament = this->roll.getTemperament();

    int periodNumber = 0;
    const auto keyName =
        temperament->getMidiNoteName(absKey,
            this->scaleRootKey, this->scaleRootKeyName, periodNumber);

    const auto degreeName = this->degreeNames[scaleDegree];

    auto tooltip = make<ChordTooltip>(chord->getName(), degreeName, keyName);
    App::Layout().showTooltip(move(tooltip));

    for (const auto &chordKey : chord->getScaleKeys())
    {
        const auto inScaleKey = scaleDegree + chordKey.getInScaleKey();
        const auto finalRootOffset = periodOffset + this->scaleRootKey;
        const auto key = jlimit(0, temperament->getNumKeys(), finalRootOffset +
            this->scale->getChromaticKey(inScaleKey, chordKey.getChromaticOffset(), false));

        const Note note(this->sequence.get(),
            key - this->clip.getKey(),
            this->targetBeat,
            this->barLengthInBeats,
            this->roll.getDefaultNoteVolume());

        this->sequence->insert(note, true);

        const auto *track = this->sequence->getTrack();
        this->roll.getTransport().previewKey(track->getTrackId(),
            track->getTrackChannel(),
            key,
            note.getVelocity(),
            note.getLength());
    }

    this->hasMadeChanges = true;
}

void ChordPreviewTool::buildNewNote(bool shouldSendMidiMessage)
{
    this->undoChangesIfAny();

    if (shouldSendMidiMessage)
    {
        this->stopSound();
    }

    if (!this->hasMadeChanges)
    {
        this->sequence->checkpoint(UndoActionIDs::MakeChord);
    }
    
    const int key = jlimit(0, this->roll.getNumKeys(), this->targetKey);
    const Note note(this->sequence.get(), key, this->targetBeat,
        this->barLengthInBeats,
        this->roll.getDefaultNoteVolume());

    this->sequence->insert(note, true);

    if (shouldSendMidiMessage)
    {
        const auto *track = this->sequence->getTrack();
        this->roll.getTransport().previewKey(track->getTrackId(),
            track->getTrackChannel(),
            key + this->clip.getKey(),
            note.getVelocity(),
            note.getLength());
    }

    this->hasMadeChanges = true;
}

void ChordPreviewTool::undoChangesIfAny()
{
    if (this->hasMadeChanges)
    {
        this->sequence->undo();
        this->hasMadeChanges = false;
    }
}

bool ChordPreviewTool::detectKeyBeatAndContext()
{
    int newKey = 0;
    float newBeat = 0.f;
    const auto myCentreRelativeToRoll =
        this->roll.getLocalPoint(this->getParentComponent(),
            this->getBounds().getCentre());

    this->roll.getRowsColsByMousePosition(myCentreRelativeToRoll.x,
        myCentreRelativeToRoll.y, newKey, newBeat);

    const bool hasChanges = (newKey != this->targetKey) || (newBeat != this->targetBeat);
    this->targetKey = newKey;
    this->targetBeat = newBeat;

    // detect key signature
    const auto absBeat = this->targetBeat + this->clip.getBeat();
    if (!SequencerOperations::findHarmonicContext(absBeat, absBeat, this->harmonicContext,
        this->scale, this->scaleRootKey, this->scaleRootKeyName))
    {
        this->scale = this->defaultScale;
        this->scaleRootKey = 0;
        this->scaleRootKeyName = {};
    }

    // detect time signature
    // (todo move this to SequencerOperations as well)
    const TimeSignatureEvent *timeSignature = nullptr;
    for (int i = 0; i < this->timeContext->getSequence()->size(); ++i)
    {
        const auto *event = static_cast<const TimeSignatureEvent *>
            (this->timeContext->getSequence()->getUnchecked(i));

        if (timeSignature == nullptr ||
            event->getBeat() <= (this->targetBeat + this->clip.getBeat()))
        {
            timeSignature = event;
        }
        else if (event->getBeat() > (this->targetBeat + this->clip.getBeat()))
        {
            break;
        }
    }

    if (timeSignature != nullptr)
    {
        this->barLengthInBeats = timeSignature->getBarLengthInBeats();
    }
    else
    {
        this->barLengthInBeats = Globals::beatsPerBar;
    }

    return hasChanges;
}

//===----------------------------------------------------------------------===//
// Shorthands
//===----------------------------------------------------------------------===//

void ChordPreviewTool::stopSound()
{
    this->roll.getTransport().stopSound(this->sequence->getTrackId());
}

void ChordPreviewTool::dismiss()
{
    App::Layout().hideTooltipIfAny();
    this->exitModalState(0);
}
