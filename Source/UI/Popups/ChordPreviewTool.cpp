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
#include "MenuPanel.h"
#include "CommandIDs.h"

#define CHORD_BUILDER_LABEL_SIZE           (32)

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
    return{
        TRANS("popup::chord::function::1"),
        TRANS("popup::chord::function::2"),
        TRANS("popup::chord::function::3"),
        TRANS("popup::chord::function::4"),
        TRANS("popup::chord::function::5"),
        TRANS("popup::chord::function::6"),
        TRANS("popup::chord::function::7")
    };
}

class ChordsCommandPanel final : public MenuPanel
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
            if (auto *builder = dynamic_cast<ChordBuilderTool *>(this->getParentComponent()))
            {
                builder->applyFunction((Scale::Function)functionIndex);
            }
        }
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FunctionsCommandPanel)
};

//[/MiscUserDefs]

ChordBuilderTool::ChordBuilderTool()
{
    this->newNote.reset(new PopupCustomButton(createLabel("+")));
    this->addAndMakeVisible(newNote.get());
    this->chordsList.reset(new ChordsCommandPanel(this->defaultChords));
    this->addAndMakeVisible(chordsList.get());

    internalPath1.startNewSubPath (275.0f, 140.0f);
    internalPath1.lineTo (350.0f, 150.0f);
    internalPath1.lineTo (340.0f, 380.0f);
    internalPath1.lineTo (160.0f, 380.0f);
    internalPath1.lineTo (150.0f, 150.0f);
    internalPath1.lineTo (225.0f, 140.0f);
    internalPath1.lineTo (250.0f, 100.0f);
    internalPath1.closeSubPath();


    //[UserPreSize]
    //[/UserPreSize]

    this->setSize(500, 500);

    //[Constructor]
    //[/Constructor]
}

ChordBuilderTool::~ChordBuilderTool()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    newNote = nullptr;
    chordsList = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ChordBuilderTool::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

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

void ChordBuilderTool::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    newNote->setBounds(proportionOfWidth (0.5000f) - (proportionOfWidth (0.1280f) / 2), proportionOfHeight (0.1200f) - (proportionOfHeight (0.1280f) / 2), proportionOfWidth (0.1280f), proportionOfHeight (0.1280f));
    chordsList->setBounds((getWidth() / 2) - (172 / 2), (getHeight() / 2) + 14 - (224 / 2), 172, 224);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ChordBuilderTool::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    //[/UserCode_parentHierarchyChanged]
}

void ChordBuilderTool::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    Component::handleCommandMessage(commandId);
    if (commandId == CommandIDs::PopupMenuDismiss)
    {
        this->exitModalState(0);
    }
    //[/UserCode_handleCommandMessage]
}

bool ChordBuilderTool::keyPressed (const KeyPress& key)
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

void ChordBuilderTool::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->dismissAsCancelled();
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

void ChordBuilderTool::onPopupsResetState(PopupButton *button)
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

void ChordBuilderTool::onPopupButtonFirstAction(PopupButton *button)
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
        }
        else {
            //App::Helio()->showTooltip(createLabel(rootKey));
            this->buildNewNote(true);
            this->dismissAsDone();
        }
    }
}

void ChordBuilderTool::onPopupButtonSecondAction(PopupButton *button)
{
    this->dismissAsDone();
}

void ChordBuilderTool::onPopupButtonStartDragging(PopupButton *button)
{
    if (button == this->newNote.get())
    {
        this->draggingStartPosition = this->getPosition();
    }
}

bool ChordBuilderTool::onPopupButtonDrag(PopupButton *button)
{
    if (button == this->newNote.get())
    {
        const Point<int> dragDelta = this->newNote->getDragDelta();
        this->setTopLeftPosition(this->getPosition() + dragDelta);
        const bool keyHasChanged = this->detectKeyAndBeat();
        this->buildNewChord(keyHasChanged);

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

void ChordBuilderTool::onPopupButtonEndDragging(PopupButton *button)
{
    if (button == this->newNote.get())
    {
        this->draggingEndPosition = this->getPosition();
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ChordBuilderTool" template="../../Template"
                 componentName="" parentClasses="public PopupMenuComponent, public PopupButtonOwner"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="500"
                 initialHeight="500">
  <METHODS>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="ff323e44">
    <PATH pos="0 0 100 100" fill="solid: 77000000" hasStroke="0" nonZeroWinding="1">s 275 140 l 350 150 l 340 380 l 160 380 l 150 150 l 225 140 l 250 100 x</PATH>
  </BACKGROUND>
  <JUCERCOMP name="" id="6b3cbe21e2061b28" memberName="newNote" virtualName=""
             explicitFocusOrder="0" pos="50%c 12%c 12.8% 12.8%" sourceFile="PopupCustomButton.cpp"
             constructorParams="createLabel(&quot;+&quot;)"/>
  <GENERICCOMPONENT name="" id="5186723628bce1d6" memberName="chordsList" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 14Cc 172 224" class="ChordsCommandPanel"
                    params="this-&gt;defaultChords"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
