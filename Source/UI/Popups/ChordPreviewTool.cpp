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

//[Headers]
#include "Common.h"
//[/Headers]

#include "ChordPreviewTool.h"

//[MiscUserDefs]
#include "PianoSequence.h"
#include "PianoRoll.h"
#include "Transport.h"
#include "KeySignaturesSequence.h"
#include "KeySignatureEvent.h"
#include "CommandIDs.h"
#include "Config.h"

#define CHORD_BUILDER_LABEL_SIZE           (28)
#define CHORD_BUILDER_NOTE_LENGTH          (4)

static Label *createLabel(const String &text)
{
    const int size = CHORD_BUILDER_LABEL_SIZE;
    auto newLabel = new Label(text, text);
    newLabel->setJustificationType(Justification::centred);
    newLabel->setBounds(0, 0, size * 2, size);
    newLabel->setName(text + "_outline");

    const float autoFontSize = float(size - 5.f);
    newLabel->setFont(Font(Font::getDefaultSerifFontName(), autoFontSize, Font::plain));
    return newLabel;
}

static Array<String> localizedFunctionNames()
{
    return {
        TRANS("popup::chord::function::1"),
        TRANS("popup::chord::function::2"),
        TRANS("popup::chord::function::3"),
        TRANS("popup::chord::function::4"),
        TRANS("popup::chord::function::5"),
        TRANS("popup::chord::function::6"),
        TRANS("popup::chord::function::7")
    };
}
//[/MiscUserDefs]

ChordPreviewTool::ChordPreviewTool(PianoRoll &caller, WeakReference<PianoSequence> target, const Clip &clip, WeakReference<KeySignaturesSequence> harmonicContext)
    : PopupMenuComponent(&caller),
      roll(caller),
      sequence(target),
      clip(clip),
      harmonicContext(harmonicContext),
      defaultChords(App::Config().getChords()->getAll()),
      hasMadeChanges(false),
      draggingStartPosition(0, 0),
      draggingEndPosition(0, 0)
{
    this->newChord.reset(new PopupCustomButton(createLabel("+")));
    this->addAndMakeVisible(newChord.get());


    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(500, 500);

    //[Constructor]

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
        auto *button = this->chordButtons.add(new PopupCustomButton(createLabel(chord->getName()), PopupButton::Hex, colour));
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
    //[/Constructor]
}

ChordPreviewTool::~ChordPreviewTool()
{
    //[Destructor_pre]
    this->stopSound();
    //[/Destructor_pre]

    newChord = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ChordPreviewTool::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ChordPreviewTool::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    newChord->setBounds((getWidth() / 2) - (proportionOfWidth (0.1400f) / 2), (getHeight() / 2) - (proportionOfHeight (0.1400f) / 2), proportionOfWidth (0.1400f), proportionOfHeight (0.1400f));
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ChordPreviewTool::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->detectKeyBeatAndContext();
    this->buildNewNote(true);
    this->newChord->setState(true);
    //[/UserCode_parentHierarchyChanged]
}

void ChordPreviewTool::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::PopupMenuDismiss)
    {
        this->exitModalState(0);
    }
    //[/UserCode_handleCommandMessage]
}

bool ChordPreviewTool::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    if (key.isKeyCode(KeyPress::escapeKey))
    {
        this->undoChangesIfAny();
    }

    this->dismissAsDone();
    return true;
    //[/UserCode_keyPressed]
}

void ChordPreviewTool::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->dismissAsCancelled();
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

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

void ChordPreviewTool::onPopupButtonStartDragging(PopupButton *button)
{
    if (button == this->newChord.get())
    {
        this->draggingStartPosition = this->getPosition();
    }
}

bool ChordPreviewTool::onPopupButtonDrag(PopupButton *button)
{
    if (button == this->newChord.get())
    {
        const Point<int> dragDelta = this->newChord->getDragDelta();
        this->setTopLeftPosition(this->getPosition() + dragDelta);
        const bool keyHasChanged = this->detectKeyBeatAndContext();
        this->buildNewNote(keyHasChanged);

        if (keyHasChanged)
        {
            const auto rootKey = MidiMessage::getMidiNoteName(this->targetKey + this->clip.getKey(), true, true, 3);
            App::Layout().showTooltip(TRANS("popup::chord::rootkey") + ": " + rootKey);
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

void ChordPreviewTool::onPopupButtonEndDragging(PopupButton *button)
{
    if (button == this->newChord.get())
    {
        this->draggingEndPosition = this->getPosition();
    }
}

static const float kDefaultChordVelocity = 0.35f;

Chord::Ptr ChordPreviewTool::findChordFor(PopupButton *button) const
{
    for (const auto chord : this->defaultChords)
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

    const auto period = this->targetKey / this->scale->getBasePeriod();
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

        // a hack for stop sound events not mute the forthcoming notes
        Time::waitForMillisecondCounter(Time::getMillisecondCounter() + 20);

        if (!App::isRunningOnPhone())
        {
            static const auto fnNames = localizedFunctionNames();
            const String tooltip =
                MidiMessage::getMidiNoteName(this->root, true, false, 3) + " "
                + this->scale->getLocalizedName() + ", "
                + fnNames[scaleFnOffset] + ", "
                + MidiMessage::getMidiNoteName(this->targetKey + this->clip.getKey(), true, true, 3) + " "
                + chord->getName();

            App::Layout().showTooltip(tooltip);
        }

        for (const auto inScaleChordKey : chord->getScaleKeys())
        {
            const auto inScaleKey = scaleFnOffset + inScaleChordKey;
            const auto finalOffset = periodOffset + this->root - this->clip.getKey();
            const int key = jlimit(0, 128, finalOffset + this->scale->getChromaticKey(inScaleKey));
            const Note note(this->sequence.get(), key, this->targetBeat, CHORD_BUILDER_NOTE_LENGTH, kDefaultChordVelocity);
            this->sequence->insert(note, true);
            this->sendMidiMessage(MidiMessage::noteOn(note.getTrackChannel(),
                key + this->clip.getKey(), kDefaultChordVelocity));
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

    // a hack for stop sound events not mute the forthcoming notes
    Time::waitForMillisecondCounter(Time::getMillisecondCounter() + 9);

    const int key = jlimit(0, 128, this->targetKey);
    const Note note(this->sequence.get(), key, this->targetBeat,
        CHORD_BUILDER_NOTE_LENGTH, kDefaultChordVelocity);

    this->sequence->insert(note, true);
    if (shouldSendMidiMessage)
    {
        this->sendMidiMessage(MidiMessage::noteOn(note.getTrackChannel(),
            key + this->clip.getKey(), kDefaultChordVelocity));
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
    const auto myCentreRelativeToRoll =
        this->roll.getLocalPoint(this->getParentComponent(),
            this->getBounds().getCentre());

    this->roll.getRowsColsByMousePosition(myCentreRelativeToRoll.x,
        myCentreRelativeToRoll.y, newKey, this->targetBeat);

    bool hasChanges = (newKey != this->targetKey);
    this->targetKey = newKey;

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
        hasChanges = hasChanges || this->root != context->getRootKey() ||
            !this->scale->isEquivalentTo(context->getScale());

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
    this->roll.getTransport().allNotesControllersAndSoundOff();
}

void ChordPreviewTool::sendMidiMessage(const MidiMessage &message)
{
    const String layerId = this->sequence->getTrackId();
    this->roll.getTransport().sendMidiMessage(layerId, message);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ChordPreviewTool" template="../../Template"
                 componentName="" parentClasses="public PopupMenuComponent, public PopupButtonOwner"
                 constructorParams="PianoRoll &amp;caller, WeakReference&lt;PianoSequence&gt; target, const Clip &amp;clip, WeakReference&lt;KeySignaturesSequence&gt; harmonicContext"
                 variableInitialisers="PopupMenuComponent(&amp;caller),&#10;roll(caller),&#10;sequence(target),&#10;clip(clip),&#10;harmonicContext(harmonicContext),&#10;defaultChords(App::Config().getChords()->getAll()),&#10;hasMadeChanges(false),&#10;draggingStartPosition(0, 0),&#10;draggingEndPosition(0, 0)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="500" initialHeight="500">
  <METHODS>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="e7f368456de9aae7" memberName="newChord" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 0Cc 14% 14%" class="PopupCustomButton"
                    params="createLabel(&quot;+&quot;)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
