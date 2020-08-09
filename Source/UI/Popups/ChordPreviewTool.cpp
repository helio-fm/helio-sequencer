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
#include "ChordPreviewTool.h"

#include "PianoSequence.h"
#include "PianoRoll.h"
#include "Transport.h"
#include "KeySignaturesSequence.h"
#include "KeySignatureEvent.h"
#include "Chord.h"
#include "CommandIDs.h"
#include "Config.h"

#define CHORD_BUILDER_LABEL_SIZE       (32)
#define CHORD_BUILDER_FONT_SIZE        (16)
#define CHORD_BUILDER_NOTE_LENGTH      (4)
#define CHORD_BUILDER_NOTE_VELOCITY    (0.35f)

static Label *createPopupButtonLabel(const String &text)
{
    const int size = CHORD_BUILDER_LABEL_SIZE;
    auto *newLabel = new Label(text, text);
    newLabel->setJustificationType(Justification::centred);
    newLabel->setBounds(0, 0, size * 2, size);
    newLabel->setName(text + "_outline");
    newLabel->setFont(CHORD_BUILDER_FONT_SIZE);
    return newLabel;
}

ChordPreviewTool::ChordPreviewTool(PianoRoll &caller, WeakReference<PianoSequence> target, const Clip &clip, WeakReference<KeySignaturesSequence> harmonicContext)
    : PopupMenuComponent(&caller),
      roll(caller),
      sequence(target),
      clip(clip),
      harmonicContext(harmonicContext),
      defaultChords(App::Config().getChords()->getAll())
{
    this->newChord = make<PopupCustomButton>(createPopupButtonLabel("+"));
    this->addAndMakeVisible(this->newChord.get());

    this->setSize(500, 500);

    this->defaultScale = Scale::getNaturalMajorScale();

    const int numChordsToDisplay = jmin(16, this->defaultChords.size());
    for (int i = 0; i < numChordsToDisplay; ++i)
    {
        const auto chord = this->defaultChords.getUnchecked(i);
        const float radians = float(i) * (MathConstants<float>::twoPi / float(numChordsToDisplay));
        // 10 items fit well in a radius of 150, but the more chords there are, the larger r should be:
        const int radius = 150 + jlimit(0, 8, numChordsToDisplay - 10) * 10;
        const auto centreOffset = Point<int>(0, -radius).transformedBy(AffineTransform::rotation(radians, 0, 0));
        const auto colour = Colour(float(i) / float(numChordsToDisplay), 0.65f, 1.f, 0.45f);
        auto *button = this->chordButtons.add(new PopupCustomButton(createPopupButtonLabel(chord->getName()), PopupButton::Hex, colour));
        button->setSize(this->proportionOfWidth(0.135f), this->proportionOfHeight(0.135f));
        button->setUserData(chord->getResourceId());
        const auto buttonSizeOffset = button->getLocalBounds().getCentre();
        const auto buttonPosition = this->getLocalBounds().getCentre() + centreOffset - buttonSizeOffset;
        button->setTopLeftPosition(buttonPosition);
        this->addAndMakeVisible(button);
    }

    this->setFocusContainer(true);
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

bool ChordPreviewTool::keyPressed (const KeyPress& key)
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
        const Point<int> dragDelta = this->newChord->getDragDelta();
        this->setTopLeftPosition(this->getPosition() + dragDelta);
        const bool hasChanges = this->detectKeyBeatAndContext();
        if (hasChanges)
        {
            this->buildNewNote(true);
            const auto rootKey = MidiMessage::getMidiNoteName(this->targetKey + this->clip.getKey(), true, true, 3);
            App::Layout().showTooltip(TRANS(I18n::Popup::chordRootKey) + ": " + rootKey);
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

    const auto period = (this->targetKey - this->root) / this->scale->getBasePeriod();
    const auto periodOffset = period * this->scale->getBasePeriod();
    const auto targetKeyOffset = (this->targetKey + this->clip.getKey()) % this->scale->getBasePeriod();
    const auto chromaticFnOffset = (targetKeyOffset - this->root);
    const auto scaleFnOffset = this->scale->getScaleKey(chromaticFnOffset);
    if (scaleFnOffset >= 0) // todo just use nearest neighbor key in scale?
    {
        this->undoChangesIfAny();
        this->stopSound();

        if (!this->hasMadeChanges)
        {
            this->sequence->checkpoint();
        }

        const auto temperament = this->roll.getTemperament();

        if (!App::isRunningOnPhone())
        {
            static const auto fnNames = Chord::getLocalizedFunctionNames();
            const String tooltip =
                temperament->getMidiNoteName(this->root, true) + " "
                + this->scale->getLocalizedName() + ", "
                + fnNames[scaleFnOffset] + ", "
                + temperament->getMidiNoteName(this->targetKey + this->clip.getKey(), true) + " "
                + chord->getName();

            App::Layout().showTooltip(tooltip);
        }

        for (const auto &chordKey : chord->getScaleKeys())
        {
            const auto inScaleKey = scaleFnOffset + chordKey.getInScaleKey();
            const auto finalRootOffset = periodOffset + this->root;
            const int key = jlimit(0, temperament->getNumKeys(), finalRootOffset +
                this->scale->getChromaticKey(inScaleKey, chordKey.getChromaticOffset(), false));

            const Note note(this->sequence.get(), key,
                this->targetBeat, CHORD_BUILDER_NOTE_LENGTH, CHORD_BUILDER_NOTE_VELOCITY);

            this->sequence->insert(note, true);
            this->sendMidiMessage(MidiMessage::noteOn(note.getTrackChannel(),
                key + this->clip.getKey(), CHORD_BUILDER_NOTE_VELOCITY));
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
        CHORD_BUILDER_NOTE_LENGTH, CHORD_BUILDER_NOTE_VELOCITY);

    this->sequence->insert(note, true);
    if (shouldSendMidiMessage)
    {
        this->sendMidiMessage(MidiMessage::noteOn(note.getTrackChannel(),
            key + this->clip.getKey(), CHORD_BUILDER_NOTE_VELOCITY));
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

    const KeySignatureEvent *context = nullptr;
    for (int i = 0; i < this->harmonicContext->size(); ++i)
    {
        const auto *event = this->harmonicContext->getUnchecked(i);
        if (context == nullptr || event->getBeat() <= (this->targetBeat + this->clip.getBeat()))
        {
            // Take the first one no matter where it resides;
            // If event is still before the sequence start, update the context anyway:
            context = static_cast<const KeySignatureEvent *>(event);
        }
        else if (event->getBeat() > (this->targetBeat + this->clip.getBeat()))
        {
            // No need to look further
            break;
        }
    }

    // We've found the only context that doesn't change within a sequence:
    if (context != nullptr)
    {
        this->scale = context->getScale();
        this->root = context->getRootKey();
    }
    else
    {
        this->scale = this->defaultScale;
        this->root = 0;
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

void ChordPreviewTool::sendMidiMessage(const MidiMessage &message)
{
    this->roll.getTransport().previewMidiMessage(this->sequence->getTrackId(), message);
}
