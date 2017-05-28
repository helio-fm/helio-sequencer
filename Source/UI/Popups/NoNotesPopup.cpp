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

#include "NoNotesPopup.h"

//[MiscUserDefs]
#include "NotePopupListener.h"
#include "NoteComponent.h"
#include "PianoRoll.h"
#include "PianoLayer.h"
#include "Note.h"
#include "App.h"
#include "ChordTooltip.h"

#include "Supervisor.h"
#include "SerializationKeys.h"

#define NEWCHORD_POPUP_MENU_SIZE_DESKTOP   (500)
#define NEWCHORD_POPUP_LABEL_SIZE_DESKTOP  (32)
#define NEWCHORD_POPUP_MENU_SIZE_TABLET    (500)
#define NEWCHORD_POPUP_LABEL_SIZE_TABLET   (32)
#define NEWCHORD_POPUP_MENU_SIZE_PHONE     (350)
#define NEWCHORD_POPUP_LABEL_SIZE_PHONE    (24)
#define NEWCHORD_POPUP_DEFAULT_NOTE_LENGTH (4)

static Label *createLabel(const String &text)
{
#if HELIO_DESKTOP
    const int size = NEWCHORD_POPUP_LABEL_SIZE_DESKTOP;
#elif HELIO_MOBILE
    const int size = App::isRunningOnPhone() ? NEWCHORD_POPUP_LABEL_SIZE_PHONE : NEWCHORD_POPUP_LABEL_SIZE_TABLET;
#endif

    auto newLabel = new Label(text, text);
    newLabel->setJustificationType(Justification::centred);
    newLabel->setBounds(0, 0, size * 2, size);
    newLabel->setName(text + "_outline");

    const float autoFontSize = float(size - 5.f);
    newLabel->setFont(Font(Font::getDefaultSerifFontName(), autoFontSize, Font::plain));
    return newLabel;
}

//[/MiscUserDefs]

NoNotesPopup::NoNotesPopup(PianoRoll *caller, MidiLayer *layer)
    : PopupMenuComponent(caller),
      roll(caller),
      targetLayer(layer),
      hasMadeChanges(false),
      draggingStartPosition(0, 0),
      draggingEndPosition(0, 0)
{
    addAndMakeVisible (chordMinor1 = new PopupCustomButton (createLabel("t")));
    addAndMakeVisible (chordMajor1 = new PopupCustomButton (createLabel("T")));
    addAndMakeVisible (chordMinor2 = new PopupCustomButton (createLabel("DD")));
    addAndMakeVisible (chordMajor2 = new PopupCustomButton (createLabel("Sp")));
    addAndMakeVisible (chordMinor3 = new PopupCustomButton (createLabel("tP")));
    addAndMakeVisible (chordMajor3 = new PopupCustomButton (createLabel("Dp")));
    addAndMakeVisible (chordMinor4 = new PopupCustomButton (createLabel("s")));
    addAndMakeVisible (chordMajor4 = new PopupCustomButton (createLabel("S")));
    addAndMakeVisible (chordMinor5 = new PopupCustomButton (createLabel("d")));
    addAndMakeVisible (chordMajor5 = new PopupCustomButton (createLabel("D")));
    addAndMakeVisible (chordMinor6 = new PopupCustomButton (createLabel("sP")));
    addAndMakeVisible (chordMajor6 = new PopupCustomButton (createLabel("Tp")));
    addAndMakeVisible (chordMinor7 = new PopupCustomButton (createLabel("dP")));
    addAndMakeVisible (newNote = new PopupCustomButton (createLabel("+")));
    addAndMakeVisible (labelI = new Label (String(),
                                           TRANS("I")));
    labelI->setFont (Font (Font::getDefaultSerifFontName(), 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    labelI->setJustificationType (Justification::centred);
    labelI->setEditable (false, false, false);
    labelI->setColour (TextEditor::textColourId, Colours::black);
    labelI->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (labelII = new Label (String(),
                                            TRANS("II")));
    labelII->setFont (Font (Font::getDefaultSerifFontName(), 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    labelII->setJustificationType (Justification::centred);
    labelII->setEditable (false, false, false);
    labelII->setColour (TextEditor::textColourId, Colours::black);
    labelII->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (labelIII = new Label (String(),
                                             TRANS("III")));
    labelIII->setFont (Font (Font::getDefaultSerifFontName(), 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    labelIII->setJustificationType (Justification::centred);
    labelIII->setEditable (false, false, false);
    labelIII->setColour (TextEditor::textColourId, Colours::black);
    labelIII->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (labelIV = new Label (String(),
                                            TRANS("IV")));
    labelIV->setFont (Font (Font::getDefaultSerifFontName(), 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    labelIV->setJustificationType (Justification::centred);
    labelIV->setEditable (false, false, false);
    labelIV->setColour (TextEditor::textColourId, Colours::black);
    labelIV->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (labelV = new Label (String(),
                                           TRANS("V")));
    labelV->setFont (Font (Font::getDefaultSerifFontName(), 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    labelV->setJustificationType (Justification::centred);
    labelV->setEditable (false, false, false);
    labelV->setColour (TextEditor::textColourId, Colours::black);
    labelV->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (labelVI = new Label (String(),
                                            TRANS("VI")));
    labelVI->setFont (Font (Font::getDefaultSerifFontName(), 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    labelVI->setJustificationType (Justification::centred);
    labelVI->setEditable (false, false, false);
    labelVI->setColour (TextEditor::textColourId, Colours::black);
    labelVI->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (labelVII = new Label (String(),
                                             TRANS("VII")));
    labelVII->setFont (Font (Font::getDefaultSerifFontName(), 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    labelVII->setJustificationType (Justification::centred);
    labelVII->setEditable (false, false, false);
    labelVII->setColour (TextEditor::textColourId, Colours::black);
    labelVII->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (500, 500);

    //[Constructor]

#if HELIO_DESKTOP
    this->setSize(NEWCHORD_POPUP_MENU_SIZE_DESKTOP, NEWCHORD_POPUP_MENU_SIZE_DESKTOP);
#elif HELIO_MOBILE
    if (App::isRunningOnPhone())
    {
        this->setSize(NEWCHORD_POPUP_MENU_SIZE_PHONE, NEWCHORD_POPUP_MENU_SIZE_PHONE);
    }
    else
    {
        this->setSize(NEWCHORD_POPUP_MENU_SIZE_TABLET, NEWCHORD_POPUP_MENU_SIZE_TABLET);
    }
#endif

    this->setFocusContainer(true);
    this->grabKeyboardFocus();

    this->setInterceptsMouseClicks(false, true);
    this->enterModalState(true, nullptr, true);

    this->newNote->setMouseCursor(MouseCursor::DraggingHandCursor);
    //[/Constructor]
}

NoNotesPopup::~NoNotesPopup()
{
    //[Destructor_pre]
    this->targetLayer->allNotesOff();
    this->targetLayer->allSoundOff();
    //[/Destructor_pre]

    chordMinor1 = nullptr;
    chordMajor1 = nullptr;
    chordMinor2 = nullptr;
    chordMajor2 = nullptr;
    chordMinor3 = nullptr;
    chordMajor3 = nullptr;
    chordMinor4 = nullptr;
    chordMajor4 = nullptr;
    chordMinor5 = nullptr;
    chordMajor5 = nullptr;
    chordMinor6 = nullptr;
    chordMajor6 = nullptr;
    chordMinor7 = nullptr;
    newNote = nullptr;
    labelI = nullptr;
    labelII = nullptr;
    labelIII = nullptr;
    labelIV = nullptr;
    labelV = nullptr;
    labelVI = nullptr;
    labelVII = nullptr;

    //[Destructor]
    //[/Destructor]
}

void NoNotesPopup::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void NoNotesPopup::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    chordMinor1->setBounds (proportionOfWidth (0.5000f) - (proportionOfWidth (0.1120f) / 2), proportionOfHeight (0.2880f) - (proportionOfHeight (0.1120f) / 2), proportionOfWidth (0.1120f), proportionOfHeight (0.1120f));
    chordMajor1->setBounds (proportionOfWidth (0.5000f) - (proportionOfWidth (0.1440f) / 2), proportionOfHeight (0.1560f) - (proportionOfHeight (0.1440f) / 2), proportionOfWidth (0.1440f), proportionOfHeight (0.1440f));
    chordMinor2->setBounds (proportionOfWidth (0.6640f) - (proportionOfWidth (0.1120f) / 2), proportionOfHeight (0.3760f) - (proportionOfHeight (0.1120f) / 2), proportionOfWidth (0.1120f), proportionOfHeight (0.1120f));
    chordMajor2->setBounds (proportionOfWidth (0.7700f) - (proportionOfWidth (0.1440f) / 2), proportionOfHeight (0.2980f) - (proportionOfHeight (0.1440f) / 2), proportionOfWidth (0.1440f), proportionOfHeight (0.1440f));
    chordMinor3->setBounds (proportionOfWidth (0.7040f) - (proportionOfWidth (0.1120f) / 2), proportionOfHeight (0.5560f) - (proportionOfHeight (0.1120f) / 2), proportionOfWidth (0.1120f), proportionOfHeight (0.1120f));
    chordMajor3->setBounds (proportionOfWidth (0.8320f) - (proportionOfWidth (0.1440f) / 2), proportionOfHeight (0.5980f) - (proportionOfHeight (0.1440f) / 2), proportionOfWidth (0.1440f), proportionOfHeight (0.1440f));
    chordMinor4->setBounds (proportionOfWidth (0.5900f) - (proportionOfWidth (0.1120f) / 2), proportionOfHeight (0.7040f) - (proportionOfHeight (0.1120f) / 2), proportionOfWidth (0.1120f), proportionOfHeight (0.1120f));
    chordMajor4->setBounds (proportionOfWidth (0.6500f) - (proportionOfWidth (0.1440f) / 2), proportionOfHeight (0.8220f) - (proportionOfHeight (0.1440f) / 2), proportionOfWidth (0.1440f), proportionOfHeight (0.1440f));
    chordMinor5->setBounds (proportionOfWidth (0.4100f) - (proportionOfWidth (0.1120f) / 2), proportionOfHeight (0.7040f) - (proportionOfHeight (0.1120f) / 2), proportionOfWidth (0.1120f), proportionOfHeight (0.1120f));
    chordMajor5->setBounds (proportionOfWidth (0.3500f) - (proportionOfWidth (0.1440f) / 2), proportionOfHeight (0.8220f) - (proportionOfHeight (0.1440f) / 2), proportionOfWidth (0.1440f), proportionOfHeight (0.1440f));
    chordMinor6->setBounds (proportionOfWidth (0.2980f) - (proportionOfWidth (0.1120f) / 2), proportionOfHeight (0.5560f) - (proportionOfHeight (0.1120f) / 2), proportionOfWidth (0.1120f), proportionOfHeight (0.1120f));
    chordMajor6->setBounds (proportionOfWidth (0.1720f) - (proportionOfWidth (0.1440f) / 2), proportionOfHeight (0.5980f) - (proportionOfHeight (0.1440f) / 2), proportionOfWidth (0.1440f), proportionOfHeight (0.1440f));
    chordMinor7->setBounds (proportionOfWidth (0.3340f) - (proportionOfWidth (0.1120f) / 2), proportionOfHeight (0.3760f) - (proportionOfHeight (0.1120f) / 2), proportionOfWidth (0.1120f), proportionOfHeight (0.1120f));
    newNote->setBounds (proportionOfWidth (0.5000f) - (proportionOfWidth (0.1280f) / 2), proportionOfHeight (0.5000f) - (proportionOfHeight (0.1280f) / 2), proportionOfWidth (0.1280f), proportionOfHeight (0.1280f));
    labelI->setBounds (proportionOfWidth (0.5000f) - (proportionOfWidth (0.0640f) / 2), proportionOfHeight (0.3780f) - (proportionOfHeight (0.0480f) / 2), proportionOfWidth (0.0640f), proportionOfHeight (0.0480f));
    labelII->setBounds (proportionOfWidth (0.5900f) - (proportionOfWidth (0.0800f) / 2), proportionOfHeight (0.4300f) - (proportionOfHeight (0.0480f) / 2), proportionOfWidth (0.0800f), proportionOfHeight (0.0480f));
    labelIII->setBounds (proportionOfWidth (0.6180f) - (proportionOfWidth (0.0800f) / 2), proportionOfHeight (0.5360f) - (proportionOfHeight (0.0480f) / 2), proportionOfWidth (0.0800f), proportionOfHeight (0.0480f));
    labelIV->setBounds (proportionOfWidth (0.5540f) - (proportionOfWidth (0.0800f) / 2), proportionOfHeight (0.6260f) - (proportionOfHeight (0.0480f) / 2), proportionOfWidth (0.0800f), proportionOfHeight (0.0480f));
    labelV->setBounds (proportionOfWidth (0.4440f) - (proportionOfWidth (0.0800f) / 2), proportionOfHeight (0.6260f) - (proportionOfHeight (0.0480f) / 2), proportionOfWidth (0.0800f), proportionOfHeight (0.0480f));
    labelVI->setBounds (proportionOfWidth (0.3820f) - (proportionOfWidth (0.0800f) / 2), proportionOfHeight (0.5360f) - (proportionOfHeight (0.0480f) / 2), proportionOfWidth (0.0800f), proportionOfHeight (0.0480f));
    labelVII->setBounds (proportionOfWidth (0.4000f) - (proportionOfWidth (0.0800f) / 2), proportionOfHeight (0.4300f) - (proportionOfHeight (0.0480f) / 2), proportionOfWidth (0.0800f), proportionOfHeight (0.0480f));
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void NoNotesPopup::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->detectKeyAndBeat();
    this->buildNewNote(true);
    this->newNote->setState(true);
    //[/UserCode_parentHierarchyChanged]
}

void NoNotesPopup::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    Component::handleCommandMessage(commandId);

    if (commandId == CommandIDs::PopupMenuDismiss)
    {
        this->exitModalState(0);
    }
    //[/UserCode_handleCommandMessage]
}

bool NoNotesPopup::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...

    // Let's dismiss on any keypress
    //if (key.isKeyCode(KeyPress::escapeKey))
    {
        this->cancelChangesIfAny();
        this->dismissAsDone();
        return true;
    }

    return false;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
    //[/UserCode_keyPressed]
}

void NoNotesPopup::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->cancelChangesIfAny();
    this->dismissAsCancelled();
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

void NoNotesPopup::onPopupsResetState(PopupButton *button)
{
    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        if (PopupButton *pb = dynamic_cast<PopupButton *>(this->getChildComponent(i)))
        {
            const bool shouldBeTurnedOn = (pb == button);
            pb->setState(shouldBeTurnedOn);
        }
    }
}

static const char *kChordDescriptionsArray[] =
{
    "popup::chord::minor::1", "popup::chord::major::1",
    "popup::chord::minor::2", "popup::chord::major::2",
    "popup::chord::minor::3", "popup::chord::major::3",
    "popup::chord::minor::4", "popup::chord::major::4",
    "popup::chord::minor::5", "popup::chord::major::5",
    "popup::chord::minor::6", "popup::chord::major::6",
    "popup::chord::minor::7", ""
};

static const char *kFunctionDescriptionsArray[] =
{
    "popup::chord::function::1",
    "popup::chord::function::2",
    "popup::chord::function::3",
    "popup::chord::function::4",
    "popup::chord::function::5",
    "popup::chord::function::6",
    "popup::chord::function::7"
};

inline String keyName(int key)
{
    return MidiMessage::getMidiNoteName(key, true, true, 3);
}

#define SHOW_CHORD_TOOLTIP(FUNCTION_INDEX, CHORD_INDEX) \
if (! App::isRunningOnPhone()) { \
    Component *tip =\
    new ChordTooltip(rootKey,\
                     TRANS(functionDescriptions[FUNCTION_INDEX]),\
                     TRANS(chordDescriptions[CHORD_INDEX]));\
    App::Helio()->showTooltip(tip, this->getScreenBounds());\
}

void NoNotesPopup::onPopupButtonFirstAction(PopupButton *button)
{
    Array<int> chord;

    const String rootKey = keyName(this->targetKey);
    const StringArray chordDescriptions(kChordDescriptionsArray, 14);
    const StringArray functionDescriptions(kFunctionDescriptionsArray, 7);

    if (button == this->chordMinor1)
    {
        this->buildChord(0, 3, 7);
        SHOW_CHORD_TOOLTIP(0, 0);
    }
    else if (button == this->chordMajor1)
    {
        this->buildChord(0, 4, 7);
        SHOW_CHORD_TOOLTIP(0, 1);
    }
    else if (button == this->chordMinor2)
    {
        this->buildChord(1, 6, 9);
        SHOW_CHORD_TOOLTIP(1, 2);
    }
    else if (button == this->chordMajor2)
    {
        this->buildChord(2, 5, 9);
        SHOW_CHORD_TOOLTIP(1, 3);
    }
    else if (button == this->chordMinor3)
    {
        this->buildChord(3, 7, 10);
        SHOW_CHORD_TOOLTIP(2, 4);
    }
    else if (button == this->chordMajor3)
    {
        this->buildChord(4, 7, 11);
        SHOW_CHORD_TOOLTIP(2, 5);
    }
    else if (button == this->chordMinor4)
    {
        this->buildChord(0, 5, 8);
        SHOW_CHORD_TOOLTIP(3, 6);
    }
    else if (button == this->chordMajor4)
    {
        this->buildChord(0, 5, 9);
        SHOW_CHORD_TOOLTIP(3, 7);
    }
    else if (button == this->chordMinor5)
    {
        this->buildChord(2, 7, 10);
        SHOW_CHORD_TOOLTIP(4, 8);
    }
    else if (button == this->chordMajor5)
    {
        this->buildChord(2, 7, 11);
        SHOW_CHORD_TOOLTIP(4, 9);
    }
    else if (button == this->chordMinor6)
    {
        this->buildChord(0, 3, 8);
        SHOW_CHORD_TOOLTIP(5, 10);
    }
    else if (button == this->chordMajor6)
    {
        this->buildChord(0, 4, 9);
        SHOW_CHORD_TOOLTIP(5, 11);
    }
    else if (button == this->chordMinor7)
    {
        this->buildChord(2, 5, 10);
        SHOW_CHORD_TOOLTIP(6, 12);
    }
    else if (button == this->newNote)
    {
        const int dragDistance = this->draggingStartPosition.getDistanceFrom(this->draggingEndPosition);
        const bool dragPositionNotInitialized = (this->draggingStartPosition.getDistanceFromOrigin() == 0);
        const double retinaScale = Desktop::getInstance().getDisplays().getMainDisplay().scale;
        const bool draggedThePopup = (double(dragDistance) > retinaScale);
        if (draggedThePopup || dragPositionNotInitialized) {
            App::Helio()->showTooltip(nullptr); // hide if any
            this->buildNewNote(true);
        } else {
            //App::Helio()->showTooltip(createLabel(rootKey));
            this->buildNewNote(true);
            this->dismissAsDone();
        }
    }
}

void NoNotesPopup::onPopupButtonSecondAction(PopupButton *button)
{
    this->dismissAsDone();
}

void NoNotesPopup::onPopupButtonStartDragging(PopupButton *button)
{
    if (button == this->newNote)
    {
        this->draggingStartPosition = this->getPosition();
    }
}

bool NoNotesPopup::onPopupButtonDrag(PopupButton *button)
{
    if (button == this->newNote)
    {
        const Point<int> dragDelta = this->newNote->getDragDelta();
        this->setTopLeftPosition(this->getPosition() + dragDelta);
        const bool keyHasChanged = this->detectKeyAndBeat();
        this->buildNewNote(keyHasChanged);

        if (keyHasChanged)
        {
            const String rootKey = keyName(this->targetKey);
            App::Helio()->showTooltip(TRANS("popup::chord::rootkey") + ": " + rootKey);
        }

        // prevents it to be clicked and hid
        this->newNote->setState(false);
    }

    return false;
}

void NoNotesPopup::onPopupButtonEndDragging(PopupButton *button)
{
    if (button == this->newNote)
    {
        this->draggingEndPosition = this->getPosition();
    }
}


static const float kDefaultChordVelocity = 0.35f;

void NoNotesPopup::buildChord(int n1, int n2, int n3)
{
    if (PianoLayer *pianoLayer = dynamic_cast<PianoLayer *>(this->targetLayer))
    {
        this->cancelChangesIfAny();

        pianoLayer->allNotesOff();
        //pianoLayer->allControllersOff();
        //pianoLayer->allSoundOff();

        pianoLayer->checkpoint();

        const int key1 = jmin(128, jmax(0, this->targetKey + n1));
        const int key2 = jmin(128, jmax(0, this->targetKey + n2));
        const int key3 = jmin(128, jmax(0, this->targetKey + n3));

        Note note1(pianoLayer, key1, this->targetBeat, NEWCHORD_POPUP_DEFAULT_NOTE_LENGTH, kDefaultChordVelocity);
        Note note2(pianoLayer, key2, this->targetBeat, NEWCHORD_POPUP_DEFAULT_NOTE_LENGTH, kDefaultChordVelocity);
        Note note3(pianoLayer, key3, this->targetBeat, NEWCHORD_POPUP_DEFAULT_NOTE_LENGTH, kDefaultChordVelocity);

        pianoLayer->insert(note1, true);
        pianoLayer->insert(note2, true);
        pianoLayer->insert(note3, true);

        // a hack for stop sound events not mute the forthcoming notes
        //Time::waitForMillisecondCounter(Time::getMillisecondCounter() + 20);

        pianoLayer->sendMidiMessage(MidiMessage::noteOn(pianoLayer->getChannel(), key1, kDefaultChordVelocity));
        pianoLayer->sendMidiMessage(MidiMessage::noteOn(pianoLayer->getChannel(), key2, kDefaultChordVelocity));
        pianoLayer->sendMidiMessage(MidiMessage::noteOn(pianoLayer->getChannel(), key3, kDefaultChordVelocity));

        this->hasMadeChanges = true;
    }
}

void NoNotesPopup::buildNewNote(bool shouldSendMidiMessage)
{
    if (PianoLayer *pianoLayer = dynamic_cast<PianoLayer *>(this->targetLayer))
    {
        this->cancelChangesIfAny();

        if (shouldSendMidiMessage)
        {
            pianoLayer->allNotesOff();
        }

        pianoLayer->checkpoint();

        const int key = jmin(128, jmax(0, this->targetKey));

        Note note1(pianoLayer, key, this->targetBeat, NEWCHORD_POPUP_DEFAULT_NOTE_LENGTH, kDefaultChordVelocity);
        pianoLayer->insert(note1, true);

        if (shouldSendMidiMessage)
        {
            pianoLayer->sendMidiMessage(MidiMessage::noteOn(pianoLayer->getChannel(), key, kDefaultChordVelocity));
        }

        this->hasMadeChanges = true;
    }
}

void NoNotesPopup::cancelChangesIfAny()
{
    if (this->hasMadeChanges)
    {
        this->targetLayer->undo();
        this->hasMadeChanges = false;
    }
}

bool NoNotesPopup::detectKeyAndBeat()
{
    Point<int> myCentreRelativeToRoll = this->roll->getLocalPoint(this->getParentComponent(), this->getBounds().getCentre());
    int newKey = 0;
    this->roll->getRowsColsByMousePosition(myCentreRelativeToRoll.x, myCentreRelativeToRoll.y, newKey, this->targetBeat);
    const bool hasChanges = (newKey != this->targetKey);
    this->targetKey = newKey;
    return hasChanges;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="NoNotesPopup" template="../../Template"
                 componentName="" parentClasses="public PopupMenuComponent, public PopupButtonOwner"
                 constructorParams="PianoRoll *caller, MidiLayer *layer" variableInitialisers="PopupMenuComponent(caller),&#10;roll(caller),&#10;targetLayer(layer),&#10;hasMadeChanges(false),&#10;draggingStartPosition(0, 0),&#10;draggingEndPosition(0, 0)"
                 snapPixels="8" snapActive="0" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="500" initialHeight="500">
  <METHODS>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <IMAGE pos="100 96 200M 200M" resource="acute_heptagram_svg" opacity="0"
           mode="0"/>
  </BACKGROUND>
  <JUCERCOMP name="Minor, tonic" id="901299ec4e766469" memberName="chordMinor1"
             virtualName="" explicitFocusOrder="0" pos="50%c 28.8%c 11.2% 11.2%"
             sourceFile="PopupCustomButton.cpp" constructorParams="createLabel(&quot;t&quot;)"/>
  <JUCERCOMP name="Major, tonic" id="fd2a7dfd7daba8e3" memberName="chordMajor1"
             virtualName="" explicitFocusOrder="0" pos="50%c 15.6%c 14.4% 14.4%"
             sourceFile="PopupCustomButton.cpp" constructorParams="createLabel(&quot;T&quot;)"/>
  <JUCERCOMP name="Minor, double dominant" id="564252df06389d16" memberName="chordMinor2"
             virtualName="" explicitFocusOrder="0" pos="66.4%c 37.6%c 11.2% 11.2%"
             sourceFile="PopupCustomButton.cpp" constructorParams="createLabel(&quot;DD&quot;)"/>
  <JUCERCOMP name="Major, subdominant parallel" id="5ad43c37c16f6227" memberName="chordMajor2"
             virtualName="" explicitFocusOrder="0" pos="77%c 29.8%c 14.4% 14.4%"
             sourceFile="PopupCustomButton.cpp" constructorParams="createLabel(&quot;Sp&quot;)"/>
  <JUCERCOMP name="Minor, tonic parallel" id="dfdb8955515ce7b8" memberName="chordMinor3"
             virtualName="" explicitFocusOrder="0" pos="70.4%c 55.6%c 11.2% 11.2%"
             sourceFile="PopupCustomButton.cpp" constructorParams="createLabel(&quot;tP&quot;)"/>
  <JUCERCOMP name="Major, dominant parallel" id="dda8d5f3e6ab0fbc" memberName="chordMajor3"
             virtualName="" explicitFocusOrder="0" pos="83.2%c 59.8%c 14.4% 14.4%"
             sourceFile="PopupCustomButton.cpp" constructorParams="createLabel(&quot;Dp&quot;)"/>
  <JUCERCOMP name="Minor, subdominant" id="149c2590c43b7142" memberName="chordMinor4"
             virtualName="" explicitFocusOrder="0" pos="59%c 70.4%c 11.2% 11.2%"
             sourceFile="PopupCustomButton.cpp" constructorParams="createLabel(&quot;s&quot;)"/>
  <JUCERCOMP name="Major, subdominant" id="fb875d28ee5c2da0" memberName="chordMajor4"
             virtualName="" explicitFocusOrder="0" pos="65%c 82.2%c 14.4% 14.4%"
             sourceFile="PopupCustomButton.cpp" constructorParams="createLabel(&quot;S&quot;)"/>
  <JUCERCOMP name="Minor, dominant" id="b023353f06f731d2" memberName="chordMinor5"
             virtualName="" explicitFocusOrder="0" pos="41%c 70.4%c 11.2% 11.2%"
             sourceFile="PopupCustomButton.cpp" constructorParams="createLabel(&quot;d&quot;)"/>
  <JUCERCOMP name="Major, dominant" id="94f713292c1350d6" memberName="chordMajor5"
             virtualName="" explicitFocusOrder="0" pos="35%c 82.2%c 14.4% 14.4%"
             sourceFile="PopupCustomButton.cpp" constructorParams="createLabel(&quot;D&quot;)"/>
  <JUCERCOMP name="Minor, subdominant parallel" id="b1e7f5f3d76e413d" memberName="chordMinor6"
             virtualName="" explicitFocusOrder="0" pos="29.8%c 55.6%c 11.2% 11.2%"
             sourceFile="PopupCustomButton.cpp" constructorParams="createLabel(&quot;sP&quot;)"/>
  <JUCERCOMP name="Major, tonic parallel" id="63f32289c1859bc9" memberName="chordMajor6"
             virtualName="" explicitFocusOrder="0" pos="17.2%c 59.8%c 14.4% 14.4%"
             sourceFile="PopupCustomButton.cpp" constructorParams="createLabel(&quot;Tp&quot;)"/>
  <JUCERCOMP name="Minor, dominant parallel" id="2e1e071c2e754a69" memberName="chordMinor7"
             virtualName="" explicitFocusOrder="0" pos="33.4%c 37.6%c 11.2% 11.2%"
             sourceFile="PopupCustomButton.cpp" constructorParams="createLabel(&quot;dP&quot;)"/>
  <JUCERCOMP name="" id="6b3cbe21e2061b28" memberName="newNote" virtualName=""
             explicitFocusOrder="0" pos="50%c 50%c 12.8% 12.8%" sourceFile="PopupCustomButton.cpp"
             constructorParams="createLabel(&quot;+&quot;)"/>
  <LABEL name="" id="de0574d358157b64" memberName="labelI" virtualName=""
         explicitFocusOrder="0" pos="50%c 37.8%c 6.4% 4.8%" edTextCol="ff000000"
         edBkgCol="0" labelText="I" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="15"
         kerning="0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="2088d281aee92fe0" memberName="labelII" virtualName=""
         explicitFocusOrder="0" pos="59%c 43%c 8% 4.8%" edTextCol="ff000000"
         edBkgCol="0" labelText="II" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="15"
         kerning="0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="a832bb1acba79dab" memberName="labelIII" virtualName=""
         explicitFocusOrder="0" pos="61.8%c 53.6%c 8% 4.8%" edTextCol="ff000000"
         edBkgCol="0" labelText="III" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="15"
         kerning="0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="2c317738a2f9ec7f" memberName="labelIV" virtualName=""
         explicitFocusOrder="0" pos="55.4%c 62.6%c 8% 4.8%" edTextCol="ff000000"
         edBkgCol="0" labelText="IV" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="15"
         kerning="0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="38efc887d57017d3" memberName="labelV" virtualName=""
         explicitFocusOrder="0" pos="44.4%c 62.6%c 8% 4.8%" edTextCol="ff000000"
         edBkgCol="0" labelText="V" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="15"
         kerning="0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="b316e41f7b55b4d8" memberName="labelVI" virtualName=""
         explicitFocusOrder="0" pos="38.2%c 53.6%c 8% 4.8%" edTextCol="ff000000"
         edBkgCol="0" labelText="VI" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="15"
         kerning="0" bold="0" italic="0" justification="36"/>
  <LABEL name="" id="824d16e463bfc913" memberName="labelVII" virtualName=""
         explicitFocusOrder="0" pos="40%c 43%c 8% 4.8%" edTextCol="ff000000"
         edBkgCol="0" labelText="VII" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="15"
         kerning="0" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

//==============================================================================
// Binary resources - be careful not to edit any of these sections!

// JUCER_RESOURCE: acute_heptagram_svg, 370, "../../../../MainLayout/Acute_heptagram.svg"
static const unsigned char resource_NoNotesPopup_acute_heptagram_svg[] = { 60,33,68,79,67,84,89,80,69,32,115,118,103,32,80,85,66,76,73,67,32,34,45,47,47,87,51,67,47,47,68,84,68,32,83,86,71,32,49,46,49,
47,47,69,78,34,32,34,104,116,116,112,58,47,47,119,119,119,46,119,51,46,111,114,103,47,71,114,97,112,104,105,99,115,47,83,86,71,47,49,46,49,47,68,84,68,47,115,118,103,49,49,46,100,116,100,34,62,60,115,
118,103,32,118,101,114,115,105,111,110,61,34,49,46,49,34,32,120,109,108,110,115,61,34,104,116,116,112,58,47,47,119,119,119,46,119,51,46,111,114,103,47,50,48,48,48,47,115,118,103,34,32,104,101,105,103,
104,116,61,34,55,57,56,34,32,119,105,100,116,104,61,34,56,49,56,34,62,60,112,111,108,121,103,111,110,32,112,111,105,110,116,115,61,34,52,48,57,44,53,32,50,50,57,44,55,57,51,46,54,51,49,53,50,56,32,55,
51,51,46,51,52,56,55,57,50,44,49,54,49,46,49,57,56,49,52,54,32,52,46,53,52,51,54,55,49,44,53,49,50,46,49,55,50,49,57,52,32,56,49,51,46,52,53,54,51,50,57,44,53,49,50,46,49,55,50,49,57,52,32,56,52,46,54,
53,49,50,48,56,44,49,54,49,46,49,57,56,49,52,54,32,53,56,57,44,55,57,51,46,54,51,49,53,50,56,34,32,102,105,108,108,61,34,110,111,110,101,34,32,115,116,114,111,107,101,61,34,35,48,48,54,54,48,48,34,32,
115,116,114,111,107,101,45,119,105,100,116,104,61,34,54,34,47,62,60,47,115,118,103,62,10,0,0};

const char* NoNotesPopup::acute_heptagram_svg = (const char*) resource_NoNotesPopup_acute_heptagram_svg;
const int NoNotesPopup::acute_heptagram_svgSize = 370;
