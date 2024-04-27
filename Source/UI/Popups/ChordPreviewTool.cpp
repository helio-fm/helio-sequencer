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

#include "PianoSequence.h"
#include "PianoRoll.h"
#include "KeySignatureEvent.h"
#include "KeySignaturesSequence.h"
#include "TimeSignaturesAggregator.h"
#include "MidiTrack.h"
#include "Config.h"

static Label *createPopupButtonLabel(const String &text)
{
    constexpr int size = 32;
    auto *newLabel = new Label(text, text);
    newLabel->setJustificationType(Justification::centred);
    newLabel->setBounds(0, 0, size * 2, size);
    newLabel->setName(text + "_outline");
    newLabel->setFont(App::isRunningOnPhone() ? Globals::UI::Fonts::XS : Globals::UI::Fonts::S);
    return newLabel;
}

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
    this->newChord = make<PopupCustomButton>(createPopupButtonLabel("+"));
    this->addAndMakeVisible(this->newChord.get());

    this->setSize(500, 500);

    const int numChordsToDisplay = jmin(16, this->defaultChords.size());
    for (int i = 0; i < numChordsToDisplay; ++i)
    {
        const auto chord = this->defaultChords.getUnchecked(i);
        const float radians = float(i) * (MathConstants<float>::twoPi / float(numChordsToDisplay));
        const auto defaultRadius = App::isRunningOnPhone() ? 110 : 150;
        // the more chords there are, the larger the raduis should be:
        const int radius = defaultRadius + jlimit(0, 8, numChordsToDisplay - 10) * 10;
        const auto centreOffset = Point<int>(0, -radius).transformedBy(AffineTransform::rotation(radians, 0, 0));
        const auto colour = Colour(float(i) / float(numChordsToDisplay), 0.65f, 1.f, 0.5f);
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

    this->newChord->setMouseCursor(MouseCursor::DraggingHandCursor);
}

ChordPreviewTool::~ChordPreviewTool()
{
    this->stopSound();
}

void ChordPreviewTool::resized()
{
    static constexpr auto yOffset = 0.14f;
    this->newChord->setBounds((this->getWidth() / 2) - (this->proportionOfWidth(yOffset) / 2),
        (this->getHeight() / 2) - (this->proportionOfHeight(yOffset) / 2),
        this->proportionOfWidth(yOffset), this->proportionOfHeight(yOffset));
}

void ChordPreviewTool::parentHierarchyChanged()
{
    this->detectKeyBeatAndContext();
    this->buildNewNote(true);
    this->newChord->setState(true);
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
    if (button != this->newChord.get())
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
    if (button == this->newChord.get())
    {
        const auto dragDelta = this->newChord->getDragDelta();
        this->setTopLeftPosition(this->getPosition() + dragDelta);
        const bool hasChanges = this->detectKeyBeatAndContext();
        if (hasChanges)
        {
            this->buildNewNote(true);
            const auto keyName = this->roll.getTemperament()->
                getMidiNoteName(this->targetKey + this->clip.getKey(),
                    this->scaleRootKey, this->scaleRootKeyName, true);

            App::Layout().showTooltip(TRANS(I18n::Popup::chordRootKey) + ": " + keyName);
        }

        // reset click state:
        this->newChord->setState(false);
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

    if (scaleDegree >= 0) // todo just use nearest neighbor key in scale?
    {
        this->undoChangesIfAny();
        this->stopSound();

        if (!this->hasMadeChanges)
        {
            this->sequence->checkpoint();
        }

        const auto temperament = this->roll.getTemperament();

        static const auto degreeNames = Chord::getLocalizedDegreeNames();

        const String tooltip =
            temperament->getMidiNoteName(periodOffset + this->scaleRootKey,
                this->scaleRootKey, this->scaleRootKeyName, true) + " "
            + this->scale->getLocalizedName() + ", "
            + degreeNames[scaleDegree] + ", "
            + temperament->getMidiNoteName(this->targetKey + this->clip.getKey(),
                this->scaleRootKey, this->scaleRootKeyName, false) + " "
            + chord->getName();

        App::Layout().showTooltip(tooltip);

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
        this->sequence->checkpoint();
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
