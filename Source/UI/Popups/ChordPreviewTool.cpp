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
#include "Config.h"
#include "ColourIDs.h"
#include "UndoActionIDs.h"

static Label *createPopupButtonLabel(const String &text)
{
    constexpr int size = 50;
    auto *newLabel = new Label(text, text);
    newLabel->setJustificationType(Justification::centred);
    newLabel->setBounds(0, 0, size, size);
    newLabel->setFont(App::isRunningOnPhone() ? Globals::UI::Fonts::XS : Globals::UI::Fonts::S);
    return newLabel;
}

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
    PopupMenuComponent(&roll),
    roll(roll),
    sequence(sequence),
    clip(clip),
    harmonicContext(harmonicContext),
    timeContext(timeContext),
    defaultChords(App::Config().getChords()->getAll())
{
    const auto bgColour = findDefaultColour(ColourIDs::Roll::blackKey).withMultipliedAlpha(0.65f);

    this->centreButton = make<PopupCustomButton>(createPopupButtonLabel("+"),
        PopupButton::Shape::Circle, bgColour.withMultipliedAlpha(0.5f));
    this->addAndMakeVisible(this->centreButton.get());

    this->setSize(500, 500);

    const int numChordsToDisplay = jmin(16, this->defaultChords.size());
    for (int i = 0; i < numChordsToDisplay; ++i)
    {
        const auto chord = this->defaultChords.getUnchecked(i);
        const auto radians = float(i) * (MathConstants<float>::twoPi / float(numChordsToDisplay));
        const auto defaultRadius = App::isRunningOnPhone() ? 100 : 140;
        // the more chords there are, the larger the raduis should be:
        const auto radius = defaultRadius + jlimit(0, 8, numChordsToDisplay - 10) * 10;
        const auto centreOffset = Point<int>(0, -radius).transformedBy(AffineTransform::rotation(radians, 0, 0));
        const auto colour = Colour(float(i) / float(numChordsToDisplay), 0.675f, 1.f, 1.f).interpolatedWith(bgColour, 0.4f);
        auto *button = this->chordButtons.add(new PopupCustomButton(createPopupButtonLabel(chord->getName()), PopupButton::Shape::Hex, colour));
        const int buttonSize = App::isRunningOnPhone() ? 60 : 67;
        button->setSize(buttonSize, buttonSize);
        button->setUserData(chord->getResourceId());
        const auto buttonSizeOffset = button->getLocalBounds().getCentre();
        const auto buttonPosition = this->getLocalBounds().getCentre() + centreOffset - buttonSizeOffset;
        button->setTopLeftPosition(buttonPosition);
        this->addAndMakeVisible(button);
    }

    this->setFocusContainerType(Component::FocusContainerType::focusContainer);
    this->setInterceptsMouseClicks(false, true);
    this->enterModalState(false, nullptr, true); // deleted when dismissed

    this->centreButton->setMouseCursor(MouseCursor::DraggingHandCursor);
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
}

void ChordPreviewTool::resized()
{
    static constexpr auto yOffset = 0.14f;
    this->centreButton->setBounds((this->getWidth() / 2) - (this->proportionOfWidth(yOffset) / 2),
        (this->getHeight() / 2) - (this->proportionOfHeight(yOffset) / 2),
        this->proportionOfWidth(yOffset), this->proportionOfHeight(yOffset));
}

void ChordPreviewTool::parentHierarchyChanged()
{
    this->detectKeyBeatAndContext();
    this->buildNewNote(true);
    this->centreButton->setState(true);
}

void ChordPreviewTool::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::PopupMenuDismiss)
    {
        this->exitModalState(0);
    }
}

bool ChordPreviewTool::keyPressed(const KeyPress &key)
{
    if (key.isKeyCode(KeyPress::escapeKey))
    {
        this->undoChangesIfAny();
    }

    this->dismissAsDone();
    return true;
}

void ChordPreviewTool::inputAttemptWhenModal()
{
    this->dismissAsCancelled();
}

void ChordPreviewTool::onPopupsResetState(PopupButton *button)
{
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
    if (button != this->centreButton.get())
    {
        App::Layout().hideTooltipIfAny();
        if (auto chord = this->findChordFor(button))
        {
            this->buildChord(chord);
        }
    }
}

void ChordPreviewTool::onPopupButtonSecondAction(PopupButton *button)
{
    // TODO alt chords?
    this->dismissAsDone();
}

bool ChordPreviewTool::onPopupButtonDrag(PopupButton *button)
{
    if (button == this->centreButton.get())
    {
        const auto dragDelta = this->centreButton->getDragDelta();
        this->setTopLeftPosition(this->getPosition() + dragDelta);
        const bool hasChanges = this->detectKeyBeatAndContext();
        if (hasChanges)
        {
            this->buildNewNote(true);

            const auto targetKeyOffset = (this->targetKey + this->clip.getKey()) % this->scale->getBasePeriod();
            const auto chromaticOffset = targetKeyOffset - this->scaleRootKey;
            const auto scaleDegree = this->scale->getScaleKey(chromaticOffset);

            if (scaleDegree >= 0)
            {
                const auto degreeName = this->degreeNames[scaleDegree];

                int periodNumber = 0;
                auto keyName = this->roll.getTemperament()->
                    getMidiNoteName(this->targetKey + this->clip.getKey(),
                        this->scaleRootKey, this->scaleRootKeyName, periodNumber);

                auto tooltip = make<ChordTooltip>(String(), degreeName, keyName);
                App::Layout().showTooltip(move(tooltip));
            }
            else
            {
                App::Layout().hideTooltipIfAny();
            }
        }

        // reset click state:
        this->centreButton->setState(false);
        for (auto *b : this->chordButtons)
        {
            b->setState(false);
        }
    }

    return false;
}

Chord::Ptr ChordPreviewTool::findChordFor(PopupButton *button) const
{
    for (const auto &chord : this->defaultChords)
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
    if (!chord->isValid()) { return; }

    const auto period = (this->targetKey - this->scaleRootKey) / this->scale->getBasePeriod();
    const auto periodOffset = period * this->scale->getBasePeriod();
    const auto targetKeyOffset = (this->targetKey + this->clip.getKey()) % this->scale->getBasePeriod();
    const auto chromaticOffset = targetKeyOffset - this->scaleRootKey;
    const auto scaleDegree = this->scale->getScaleKey(chromaticOffset);

    if (scaleDegree >= 0) // todo some indication that the key is not in scale?
    {
        this->undoChangesIfAny();
        this->stopSound();

        if (!this->hasMadeChanges)
        {
            this->sequence->checkpoint(UndoActionIDs::MakeChord);
        }

        const auto temperament = this->roll.getTemperament();

        int periodNumber = 0;
        const auto keyName =
            temperament->getMidiNoteName(this->targetKey + this->clip.getKey(),
                this->scaleRootKey, this->scaleRootKeyName, periodNumber);

        const auto degreeName = this->degreeNames[scaleDegree];

        auto tooltip = make<ChordTooltip>(chord->getName(), degreeName, keyName);
        App::Layout().showTooltip(move(tooltip));

        for (const auto &chordKey : chord->getScaleKeys())
        {
            const auto inScaleKey = scaleDegree + chordKey.getInScaleKey();
            const auto finalRootOffset = periodOffset + this->scaleRootKey;
            const int key = jlimit(0, temperament->getNumKeys(), finalRootOffset +
                this->scale->getChromaticKey(inScaleKey, chordKey.getChromaticOffset(), false));

            const Note note(this->sequence.get(), key, this->targetBeat,
                this->barLengthInBeats,
                this->roll.getDefaultNoteVolume());

            this->sequence->insert(note, true);

            const auto *track = this->sequence->getTrack();
            this->roll.getTransport().previewKey(track->getTrackId(),
                track->getTrackChannel(),
                key + this->clip.getKey(),
                note.getVelocity(),
                note.getLength());
        }

        this->hasMadeChanges = true;
    }
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
