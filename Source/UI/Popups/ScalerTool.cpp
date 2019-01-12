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

#include "ScalerTool.h"

//[MiscUserDefs]
#include "NotePopupListener.h"
#include "NoteComponent.h"
#include "PianoRoll.h"
#include "PianoSequence.h"
#include "Note.h"
#include "App.h"
#include "Config.h"
#include "ChordTooltip.h"
#include "Transport.h"
#include "SerializationKeys.h"
#include "ScalesManager.h"
#include "MenuPanel.h"
#include "BinaryData.h"
#include "Icons.h"
#include "ColourIDs.h"

#define SCALER_POPUP_MENU_SIZE            (500)
#define SCALER_POPUP_LABEL_SIZE           (32)
#define SCALER_POPUP_DEFAULT_NOTE_LENGTH  (4)

static Label *createLabel(const String &text)
{
    const int size = SCALER_POPUP_LABEL_SIZE;
    auto newLabel = new Label(text, text);
    newLabel->setJustificationType(Justification::centred);
    newLabel->setBounds(0, 0, size * 2, size);
    newLabel->setName(text + "_outline");

    const float autoFontSize = float(size - 5.f);
    newLabel->setFont(Font(Font::getDefaultSerifFontName(), autoFontSize, Font::plain));
    return newLabel;
}

class ScalesCommandPanel final : public MenuPanel
{
public:

    ScalesCommandPanel(const Array<Scale::Ptr> scales) : scales(scales)
    {
        MenuPanel::Menu cmds;
        for (int i = 0; i < scales.size(); ++i)
        {
            cmds.add(MenuItem::item(Icons::empty, CommandIDs::SelectScale + i,
                scales[i]->getLocalizedName())->withAlignment(MenuItem::Right));
        }
        this->updateContent(cmds, MenuPanel::SlideLeft, false);
    }

    void handleCommandMessage(int commandId) override
    {
        if (commandId >= CommandIDs::SelectScale &&
            commandId <= (CommandIDs::SelectScale + this->scales.size()))
        {
            const int scaleIndex = commandId - CommandIDs::SelectScale;
            if (auto *builder = dynamic_cast<ScalerTool *>(this->getParentComponent()))
            {
                builder->applyScale(this->scales[scaleIndex]);
            }
        }
    }

private:

    const Array<Scale::Ptr> scales;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScalesCommandPanel)
};

//[/MiscUserDefs]

ScalerTool::ScalerTool(PianoRoll *caller, MidiSequence *layer)
    : PopupMenuComponent(caller),
      roll(caller),
      sequence(layer),
      defaultScales(ScalesManager::getInstance().getScales()),
      hasMadeChanges(false),
      draggingStartPosition(0, 0),
      draggingEndPosition(0, 0),
      scale(defaultScales[0]),
      function(Scale::Tonic)
{
    this->newNote.reset(new PopupCustomButton(createLabel("+")));
    this->addAndMakeVisible(newNote.get());
    this->scalesList.reset(new ScalesCommandPanel(this->defaultScales));
    this->addAndMakeVisible(scalesList.get());

    internalPath1.startNewSubPath (275.0f, 140.0f);
    internalPath1.lineTo (350.0f, 150.0f);
    internalPath1.lineTo (340.0f, 380.0f);
    internalPath1.lineTo (160.0f, 380.0f);
    internalPath1.lineTo (150.0f, 150.0f);
    internalPath1.lineTo (225.0f, 140.0f);
    internalPath1.lineTo (250.0f, 100.0f);
    internalPath1.closeSubPath();


    //[UserPreSize]
    if (Config::contains(Serialization::Config::lastUsedScale))
    {
        Scale::Ptr s(new Scale());
        Config::load(s.get(), Serialization::Config::lastUsedScale);
        if (s->isValid())
        {
            this->scale = s;
        }
    }

    jassert(this->scale != nullptr);
    //[/UserPreSize]

    this->setSize(500, 500);

    //[Constructor]

    this->setSize(SCALER_POPUP_MENU_SIZE, SCALER_POPUP_MENU_SIZE);

    this->setFocusContainer(true);

    this->setInterceptsMouseClicks(false, true);
    this->enterModalState(false, nullptr, true);

    this->newNote->setMouseCursor(MouseCursor::DraggingHandCursor);
    //[/Constructor]
}

ScalerTool::~ScalerTool()
{
    //[Destructor_pre]
    this->stopSound();
    //[/Destructor_pre]

    newNote = nullptr;
    scalesList = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ScalerTool::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = 0, y = 0;
        Colour fillColour = Colour (0x77000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillPath (internalPath1, AffineTransform::translation(x, y));
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ScalerTool::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    newNote->setBounds(proportionOfWidth (0.5000f) - (proportionOfWidth (0.1280f) / 2), proportionOfHeight (0.1200f) - (proportionOfHeight (0.1280f) / 2), proportionOfWidth (0.1280f), proportionOfHeight (0.1280f));
    scalesList->setBounds((getWidth() / 2) - (172 / 2), (getHeight() / 2) + 14 - (224 / 2), 172, 224);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ScalerTool::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->detectKeyAndBeat();
    this->buildNewNote(true);
    this->newNote->setState(true);
    //[/UserCode_parentHierarchyChanged]
}

void ScalerTool::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    Component::handleCommandMessage(commandId);

    if (commandId == CommandIDs::PopupMenuDismiss)
    {
        this->exitModalState(0);
    }
    //[/UserCode_handleCommandMessage]
}

bool ScalerTool::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    if (key.isKeyCode(KeyPress::escapeKey))
    {
        this->cancelChangesIfAny();
    }

    this->dismissAsDone();
    return true;
    //[/UserCode_keyPressed]
}

void ScalerTool::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->dismissAsCancelled();
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

void ScalerTool::onPopupsResetState(PopupButton *button)
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

inline String keyName(int key)
{
    return MidiMessage::getMidiNoteName(key, true, true, 3);
}

#define SHOW_CHORD_TOOLTIP(ROOT_KEY, FUNCTION_NAME) \
if (! App::isRunningOnPhone()) { \
    auto *tip = new ChordTooltip(ROOT_KEY,\
        this->scale->getLocalizedName(), FUNCTION_NAME);\
    App::Layout().showTooltip(tip, this->getScreenBounds());\
}

void ScalerTool::onPopupButtonFirstAction(PopupButton *button)
{
    if (button == this->newNote.get())
    {
        const int dragDistance = this->draggingStartPosition.getDistanceFrom(this->draggingEndPosition);
        const bool dragPositionNotInitialized = (this->draggingStartPosition.getDistanceFromOrigin() == 0);
        const double retinaScale = Desktop::getInstance().getDisplays().getMainDisplay().scale;
        const bool draggedThePopup = (double(dragDistance) > retinaScale);
        if (draggedThePopup || dragPositionNotInitialized) {
            App::Layout().hideTooltipIfAny();
            this->buildNewNote(true);
        } else {
            //App::Helio()->showTooltip(createLabel(rootKey));
            this->buildNewNote(true);
            this->dismissAsDone();
        }
    }
}

void ScalerTool::onPopupButtonSecondAction(PopupButton *button)
{
    this->dismissAsDone();
}

void ScalerTool::onPopupButtonStartDragging(PopupButton *button)
{
    if (button == this->newNote.get())
    {
        this->draggingStartPosition = this->getPosition();
    }
}

bool ScalerTool::onPopupButtonDrag(PopupButton *button)
{
    if (button == this->newNote.get())
    {
        const Point<int> dragDelta = this->newNote->getDragDelta();
        this->setTopLeftPosition(this->getPosition() + dragDelta);
        const bool keyHasChanged = this->detectKeyAndBeat();
        this->buildNewNote(keyHasChanged);

        if (keyHasChanged)
        {
            const String rootKey = keyName(this->targetKey);
            App::Layout().showTooltip(TRANS("popup::chord::rootkey") + ": " + rootKey);
        }

        // prevents it to be clicked and hid
        this->newNote->setState(false);
    }

    return false;
}

void ScalerTool::onPopupButtonEndDragging(PopupButton *button)
{
    if (button == this->newNote.get())
    {
        this->draggingEndPosition = this->getPosition();
    }
}

void ScalerTool::applyScale(const Scale::Ptr scale)
{
    const auto funName = localizedFunctionNames();
    const String rootKey = keyName(this->targetKey);
    if (this->scale != scale)
    {
        this->scale = scale;
        Config::save(this->scale.get(), Serialization::Config::lastUsedScale);
        this->buildChord(this->scale->getTriad(this->function, true));
        SHOW_CHORD_TOOLTIP(rootKey, funName[this->function]);
    }
    else
    {
        // Alternate mode on second click
        this->buildChord(this->scale->getSeventhChord(this->function, false));
        SHOW_CHORD_TOOLTIP(rootKey, funName[this->function]);
    }
}

static const float kDefaultChordVelocity = 0.35f;

void ScalerTool::buildChord(Array<int> keys)
{
    if (keys.size() == 0) { return;  }

    if (PianoSequence *pianoLayer = dynamic_cast<PianoSequence *>(this->sequence))
    {
        this->cancelChangesIfAny();
        this->stopSound();
        pianoLayer->checkpoint();

        // a hack for stop sound events not mute the forthcoming notes
        //Time::waitForMillisecondCounter(Time::getMillisecondCounter() + 20);

        for (int offset : keys)
        {
            const int key = jmin(128, jmax(0, this->targetKey + offset));
            Note note(pianoLayer, key, this->targetBeat, SCALER_POPUP_DEFAULT_NOTE_LENGTH, kDefaultChordVelocity);
            pianoLayer->insert(note, true);
            this->sendMidiMessage(MidiMessage::noteOn(note.getTrackChannel(), key, kDefaultChordVelocity));
        }

        this->hasMadeChanges = true;
    }
}

void ScalerTool::buildNewNote(bool shouldSendMidiMessage)
{
    if (PianoSequence *pianoSequence = dynamic_cast<PianoSequence *>(this->sequence))
    {
        this->cancelChangesIfAny();

        if (shouldSendMidiMessage)
        {
            this->stopSound();
        }

        pianoSequence->checkpoint();

        const int key = jmin(128, jmax(0, this->targetKey));

        Note note1(pianoSequence, key, this->targetBeat, SCALER_POPUP_DEFAULT_NOTE_LENGTH, kDefaultChordVelocity);
        pianoSequence->insert(note1, true);

        if (shouldSendMidiMessage)
        {
            this->sendMidiMessage(MidiMessage::noteOn(note1.getTrackChannel(), key, kDefaultChordVelocity));
        }

        this->hasMadeChanges = true;
    }
}

void ScalerTool::cancelChangesIfAny()
{
    if (this->hasMadeChanges)
    {
        this->sequence->undo();
        this->hasMadeChanges = false;
    }
}

bool ScalerTool::detectKeyAndBeat()
{
    Point<int> myCentreRelativeToRoll = this->roll->getLocalPoint(this->getParentComponent(), this->getBounds().getCentre());
    int newKey = 0;
    this->roll->getRowsColsByMousePosition(myCentreRelativeToRoll.x, myCentreRelativeToRoll.y, newKey, this->targetBeat);
    const bool hasChanges = (newKey != this->targetKey);
    this->targetKey = newKey;
    return hasChanges;
}

//===----------------------------------------------------------------------===//
// Shorthands
//===----------------------------------------------------------------------===//

void ScalerTool::stopSound()
{
    this->roll->getTransport().allNotesControllersAndSoundOff();
}

void ScalerTool::sendMidiMessage(const MidiMessage &message)
{
    const String layerId = this->sequence->getTrackId();
    this->roll->getTransport().sendMidiMessage(layerId, message);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ScalerTool" template="../../Template"
                 componentName="" parentClasses="public PopupMenuComponent, public PopupButtonOwner"
                 constructorParams="PianoRoll *caller, MidiSequence *layer" variableInitialisers="PopupMenuComponent(caller),&#10;roll(caller),&#10;sequence(layer),&#10;defaultScales(ScalesManager::getInstance().getScales()),&#10;hasMadeChanges(false),&#10;draggingStartPosition(0, 0),&#10;draggingEndPosition(0, 0),&#10;scale(defaultScales[0]),&#10;function(Scale::Tonic)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="500" initialHeight="500">
  <METHODS>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <PATH pos="0 0 100 100" fill="solid: 77000000" hasStroke="0" nonZeroWinding="1">s 275 140 l 350 150 l 340 380 l 160 380 l 150 150 l 225 140 l 250 100 x</PATH>
  </BACKGROUND>
  <JUCERCOMP name="" id="6b3cbe21e2061b28" memberName="newNote" virtualName=""
             explicitFocusOrder="0" pos="50%c 12%c 12.8% 12.8%" sourceFile="PopupCustomButton.cpp"
             constructorParams="createLabel(&quot;+&quot;)"/>
  <GENERICCOMPONENT name="" id="5186723628bce1d6" memberName="scalesList" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 14Cc 172 224" class="ScalesCommandPanel"
                    params="this-&gt;defaultScales"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
