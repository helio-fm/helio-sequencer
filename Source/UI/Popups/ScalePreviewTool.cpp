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

#include "ScalePreviewTool.h"

//[MiscUserDefs]
#include "NoteComponent.h"
#include "PianoRoll.h"
#include "PianoSequence.h"
#include "Note.h"
#include "Config.h"
#include "Transport.h"
#include "SerializationKeys.h"
#include "MenuPanel.h"
#include "BinaryData.h"
#include "Icons.h"
#include "ColourIDs.h"

#define NEWCHORD_POPUP_MENU_SIZE            (500)
#define NEWCHORD_POPUP_LABEL_SIZE           (32)
#define NEWCHORD_POPUP_DEFAULT_NOTE_LENGTH  (4)

static Label *createLabel(const String &text)
{
    const int size = NEWCHORD_POPUP_LABEL_SIZE;
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

class ScalesCommandPanel final : public MenuPanel
{
public:

    ScalesCommandPanel(const Array<Scale::Ptr> scales) : scales(scales)
    {
        MenuPanel::Menu cmds;
        for (int i = 0; i < scales.size(); ++i)
        {
            cmds.add(MenuItem::item(Icons::empty,
                CommandIDs::SelectScale + i, scales[i]->getLocalizedName())->withAlignment(MenuItem::Right));
        }
        this->updateContent(cmds, MenuPanel::SlideLeft, false);
    }

    void handleCommandMessage(int commandId) override
    {
        if (commandId >= CommandIDs::SelectScale &&
            commandId <= (CommandIDs::SelectScale + this->scales.size()))
        {
            const int scaleIndex = commandId - CommandIDs::SelectScale;
            if (auto *builder = dynamic_cast<ScalePreviewTool *>(this->getParentComponent()))
            {
                builder->applyScale(this->scales[scaleIndex]);
            }
        }
    }

private:

    const Array<Scale::Ptr> scales;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScalesCommandPanel)
};

class FunctionsCommandPanel final : public MenuPanel
{
public:

    FunctionsCommandPanel()
    {
        const auto funName = localizedFunctionNames();
        MenuPanel::Menu cmds;
        cmds.add(MenuItem::item(Icons::empty,
            CommandIDs::SelectFunction + 6, "VII - " + funName[6]));
        cmds.add(MenuItem::item(Icons::empty,
            CommandIDs::SelectFunction + 5, "VI - " + funName[5]));
        cmds.add(MenuItem::item(Icons::empty,
            CommandIDs::SelectFunction + 4, "V - " + funName[4]));
        cmds.add(MenuItem::item(Icons::empty,
            CommandIDs::SelectFunction + 3, "IV - " + funName[3]));
        cmds.add(MenuItem::item(Icons::empty,
            CommandIDs::SelectFunction + 2, "III - " + funName[2]));
        cmds.add(MenuItem::item(Icons::empty,
            CommandIDs::SelectFunction + 1, "II - " + funName[1]));
        cmds.add(MenuItem::item(Icons::empty,
            CommandIDs::SelectFunction, "I - " + funName[0]));
        this->updateContent(cmds, MenuPanel::SlideRight, false);
    }

    void handleCommandMessage(int commandId) override
    {
        if (commandId >= CommandIDs::SelectFunction &&
            commandId <= (CommandIDs::SelectFunction + 7))
        {
            const int functionIndex = commandId - CommandIDs::SelectFunction;
            if (auto *builder = dynamic_cast<ScalePreviewTool *>(this->getParentComponent()))
            {
                builder->applyFunction((Scale::Function)functionIndex);
            }
        }
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FunctionsCommandPanel)
};

//[/MiscUserDefs]

ScalePreviewTool::ScalePreviewTool(PianoRoll *caller, MidiSequence *layer)
    : PopupMenuComponent(caller),
      roll(caller),
      sequence(layer),
      defaultScales(App::Config().getScales()->getAll()),
      hasMadeChanges(false),
      draggingStartPosition(0, 0),
      draggingEndPosition(0, 0),
      scale(defaultScales[0]),
      function(Scale::Tonic)
{
    this->scalesList.reset(new ScalesCommandPanel(this->defaultScales));
    this->addAndMakeVisible(scalesList.get());

    this->functionsList.reset(new FunctionsCommandPanel());
    this->addAndMakeVisible(functionsList.get());

    this->newNote.reset(new PopupCustomButton(createLabel("+")));
    this->addAndMakeVisible(newNote.get());


    //[UserPreSize]
    if (App::Config().containsProperty(Serialization::Config::lastUsedScale))
    {
        Scale::Ptr s(new Scale());
        App::Config().load(s.get(), Serialization::Config::lastUsedScale);
        if (s->isValid())
        {
            this->scale = s;
        }
    }

    jassert(this->scale != nullptr);
    //[/UserPreSize]

    this->setSize(500, 500);

    //[Constructor]

    this->setSize(NEWCHORD_POPUP_MENU_SIZE, NEWCHORD_POPUP_MENU_SIZE);

    this->setFocusContainer(true);

    this->setInterceptsMouseClicks(false, true);
    this->enterModalState(false, nullptr, true);

    this->newNote->setMouseCursor(MouseCursor::DraggingHandCursor);
    //[/Constructor]
}

ScalePreviewTool::~ScalePreviewTool()
{
    //[Destructor_pre]
    this->stopSound();
    //[/Destructor_pre]

    scalesList = nullptr;
    functionsList = nullptr;
    newNote = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ScalePreviewTool::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = static_cast<float> ((getWidth() / 2) + 140 - (180 / 2)), y = static_cast<float> ((getHeight() / 2) - (232 / 2)), width = 180.0f, height = 232.0f;
        Colour fillColour = Colour (0x77000000);
        Colour strokeColour = Colour (0x33000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour = findDefaultColour(ColourIDs::Callout::fill);
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 2.000f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 2.000f, 1.000f);
    }

    {
        float x = static_cast<float> ((getWidth() / 2) + -140 - (180 / 2)), y = static_cast<float> ((getHeight() / 2) - (232 / 2)), width = 180.0f, height = 232.0f;
        Colour fillColour = Colour (0x77000000);
        Colour strokeColour = Colour (0x33000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        fillColour = findDefaultColour(ColourIDs::Callout::fill);
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 2.000f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 2.000f, 1.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ScalePreviewTool::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    scalesList->setBounds((getWidth() / 2) + -140 - (172 / 2), (getHeight() / 2) - (224 / 2), 172, 224);
    functionsList->setBounds((getWidth() / 2) + 140 - (176 / 2), (getHeight() / 2) - (224 / 2), 176, 224);
    newNote->setBounds((getWidth() / 2) - (proportionOfWidth (0.1400f) / 2), (getHeight() / 2) - (proportionOfHeight (0.1400f) / 2), proportionOfWidth (0.1400f), proportionOfHeight (0.1400f));
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ScalePreviewTool::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->detectKeyAndBeat();
    this->buildNewNote(true);
    this->newNote->setState(true);
    //[/UserCode_parentHierarchyChanged]
}

void ScalePreviewTool::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    Component::handleCommandMessage(commandId);

    if (commandId == CommandIDs::PopupMenuDismiss)
    {
        this->exitModalState(0);
    }
    //[/UserCode_handleCommandMessage]
}

bool ScalePreviewTool::keyPressed (const KeyPress& key)
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

void ScalePreviewTool::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    //this->cancelChangesIfAny();
    this->dismissAsCancelled();
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

void ScalePreviewTool::onPopupsResetState(PopupButton *button)
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
    const String tip = ROOT_KEY + " " + this->scale->getLocalizedName() + ", " + FUNCTION_NAME; \
    App::Layout().showTooltip(tip); \
}

void ScalePreviewTool::onPopupButtonFirstAction(PopupButton *button)
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
            //App::Layout()->showTooltip(createLabel(rootKey));
            this->buildNewNote(true);
            this->dismissAsDone();
        }
    }
}

void ScalePreviewTool::onPopupButtonSecondAction(PopupButton *button)
{
    this->dismissAsDone();
}

void ScalePreviewTool::onPopupButtonStartDragging(PopupButton *button)
{
    if (button == this->newNote.get())
    {
        this->draggingStartPosition = this->getPosition();
    }
}

bool ScalePreviewTool::onPopupButtonDrag(PopupButton *button)
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

void ScalePreviewTool::onPopupButtonEndDragging(PopupButton *button)
{
    if (button == this->newNote.get())
    {
        this->draggingEndPosition = this->getPosition();
    }
}

void ScalePreviewTool::applyScale(const Scale::Ptr scale)
{
    const auto funName = localizedFunctionNames();
    const String rootKey = keyName(this->targetKey);
    if (this->scale != scale)
    {
        this->scale = scale;
        App::Config().save(this->scale.get(), Serialization::Config::lastUsedScale);
        this->buildChord(this->scale->getChord(Chord::getTriad(), this->function, true));
        SHOW_CHORD_TOOLTIP(rootKey, funName[this->function]);
    }
    else
    {
        // Alternate mode on second click
        this->buildChord(this->scale->getChord(Chord::getSeventhChord(), this->function, false));
        SHOW_CHORD_TOOLTIP(rootKey, funName[this->function]);
    }
}

void ScalePreviewTool::applyFunction(Scale::Function function)
{
    const auto funName = localizedFunctionNames();
    const String rootKey = keyName(this->targetKey);
    if (this->function != function)
    {
        this->function = function;
        this->buildChord(this->scale->getChord(Chord::getTriad(), this->function, true));
        SHOW_CHORD_TOOLTIP(rootKey, funName[this->function]);
    }
    else
    {
        // Alternate mode on second click
        this->buildChord(this->scale->getChord(Chord::getSeventhChord(), this->function, false));
        SHOW_CHORD_TOOLTIP(rootKey, funName[this->function]);
    }
}

static const float kDefaultChordVelocity = 0.35f;

void ScalePreviewTool::buildChord(Array<int> keys)
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
            Note note(pianoLayer, key, this->targetBeat, NEWCHORD_POPUP_DEFAULT_NOTE_LENGTH, kDefaultChordVelocity);
            pianoLayer->insert(note, true);
            this->sendMidiMessage(MidiMessage::noteOn(note.getTrackChannel(), key, kDefaultChordVelocity));
        }

        this->hasMadeChanges = true;
    }
}

void ScalePreviewTool::buildNewNote(bool shouldSendMidiMessage)
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

        Note note1(pianoSequence, key, this->targetBeat, NEWCHORD_POPUP_DEFAULT_NOTE_LENGTH, kDefaultChordVelocity);
        pianoSequence->insert(note1, true);

        if (shouldSendMidiMessage)
        {
            this->sendMidiMessage(MidiMessage::noteOn(note1.getTrackChannel(), key, kDefaultChordVelocity));
        }

        this->hasMadeChanges = true;
    }
}

void ScalePreviewTool::cancelChangesIfAny()
{
    if (this->hasMadeChanges)
    {
        this->sequence->undo();
        this->hasMadeChanges = false;
    }
}

bool ScalePreviewTool::detectKeyAndBeat()
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

void ScalePreviewTool::stopSound()
{
    this->roll->getTransport().allNotesControllersAndSoundOff();
}

void ScalePreviewTool::sendMidiMessage(const MidiMessage &message)
{
    const String layerId = this->sequence->getTrackId();
    this->roll->getTransport().sendMidiMessage(layerId, message);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ScalePreviewTool" template="../../Template"
                 componentName="" parentClasses="public PopupMenuComponent, public PopupButtonOwner"
                 constructorParams="PianoRoll *caller, MidiSequence *layer" variableInitialisers="PopupMenuComponent(caller),&#10;roll(caller),&#10;sequence(layer),&#10;defaultScales(App::Config().getScales()->getAll()),&#10;hasMadeChanges(false),&#10;draggingStartPosition(0, 0),&#10;draggingEndPosition(0, 0),&#10;scale(defaultScales[0]),&#10;function(Scale::Tonic)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="500" initialHeight="500">
  <METHODS>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="140Cc 0Cc 180 232" cornerSize="2.00000000000000000000" fill="solid: 77000000"
               hasStroke="1" stroke="1, mitered, butt" strokeColour="solid: 33000000"/>
    <ROUNDRECT pos="-140Cc 0Cc 180 232" cornerSize="2.00000000000000000000"
               fill="solid: 77000000" hasStroke="1" stroke="1, mitered, butt"
               strokeColour="solid: 33000000"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="5186723628bce1d6" memberName="scalesList" virtualName=""
                    explicitFocusOrder="0" pos="-140Cc 0Cc 172 224" class="ScalesCommandPanel"
                    params="this-&gt;defaultScales"/>
  <GENERICCOMPONENT name="" id="2b87eb6e536e9c5b" memberName="functionsList" virtualName=""
                    explicitFocusOrder="0" pos="140Cc 0Cc 176 224" class="FunctionsCommandPanel"
                    params=""/>
  <GENERICCOMPONENT name="" id="e7f368456de9aae7" memberName="newNote" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 0Cc 14% 14%" class="PopupCustomButton"
                    params="createLabel(&quot;+&quot;)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
