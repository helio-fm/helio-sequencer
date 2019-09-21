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

#include "Console.h"

//[MiscUserDefs]
#include "CommandIDs.h"
#include "SerializationKeys.h"

#include "Headline.h"
#include "Config.h"
#include "ConsoleHelp.h"
#include "ConsoleProjectsList.h"
//[/MiscUserDefs]

Console::Console()
{
    this->shadowDn.reset(new ShadowDownwards(ShadowType::Normal));
    this->addAndMakeVisible(shadowDn.get());
    this->bg.reset(new PanelBackgroundC());
    this->addAndMakeVisible(bg.get());
    this->shadowL.reset(new ShadowLeftwards(ShadowType::Normal));
    this->addAndMakeVisible(shadowL.get());
    this->shadowR.reset(new ShadowRightwards(ShadowType::Normal));
    this->addAndMakeVisible(shadowR.get());
    this->textEditor.reset(new ConsoleTextEditor());
    this->addAndMakeVisible(textEditor.get());

    this->actionsList.reset(new ListBox());
    this->addAndMakeVisible(actionsList.get());


    //[UserPreSize]

    // todo get last edited text
    const String lastText =
        App::Config().getProperty(Serialization::Config::lastSearch);

    // fill action providers
    // set default provider (which exactly?)
    this->actionsProviders.add(new ConsoleHelp());
    this->actionsProviders.add(new ConsoleProjectsList());
    this->defaultActionsProvider = this->actionsProviders.getFirst();
    this->currentActionsProvider = this->defaultActionsProvider;

    this->actionsList->setRowHeight(28);
    //this->actionsList->setMouseMoveSelectsRows(true); // fucks up keyboard select :(
    this->actionsList->getViewport()->setScrollBarThickness(2);
    this->actionsList->setModel(this);

    this->textEditor->setReturnKeyStartsNewLine(false);
    this->textEditor->setPopupMenuEnabled(false);
    this->textEditor->setScrollbarsShown(false);
    this->textEditor->setCaretVisible(true);
    this->textEditor->setMultiLine(false);
    this->textEditor->setReadOnly(false);
    this->textEditor->setFont(21.f);

    this->textEditor->addListener(this);
    this->textEditor->setText(lastText, true);
    if (lastText.isEmpty())
    {
        // a hack to explicitly update the list on start, even if empty:
        this->textEditorTextChanged(*this->textEditor);
    }
    //[/UserPreSize]

    this->setSize(620, 400);

    //[Constructor]
    //[/Constructor]
}

Console::~Console()
{
    //[Destructor_pre]
    this->textEditor->removeListener(this);
    //[/Destructor_pre]

    shadowDn = nullptr;
    bg = nullptr;
    shadowL = nullptr;
    shadowR = nullptr;
    textEditor = nullptr;
    actionsList = nullptr;

    //[Destructor]
    //[/Destructor]
}

void Console::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void Console::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    shadowDn->setBounds(12 + 0, 0 + 160, (getWidth() - 24) - 0, 8);
    bg->setBounds(12, 0, getWidth() - 24, 160);
    shadowL->setBounds(0, 0, 12, 114 - -44);
    shadowR->setBounds(getWidth() - 12, 0, 12, 114 - -44);
    textEditor->setBounds(18, 6, getWidth() - 36, 32);
    actionsList->setBounds(18 + 0, 6 + 32 - -4, (getWidth() - 36) - 0, 114);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void Console::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentHierarchyChanged]
}

void Console::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->cancelAndDismiss();
    }
    //[/UserCode_handleCommandMessage]
}

bool Console::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    if (key.isKeyCode(KeyPress::escapeKey))
    {
        this->cancelAndDismiss();
        return true;
    }
    else if (key.isKeyCode(KeyPress::downKey))
    {
        this->moveRowSelectionBy(1);
        return true;
    }
    else if (key.isKeyCode(KeyPress::upKey))
    {
        this->moveRowSelectionBy(-1);
        return true;
    }

    return false;
    //[/UserCode_keyPressed]
}

void Console::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

int Console::getNumRows()
{
    return this->currentActionsProvider->getFilteredActions().size();
}

void Console::paintListBoxItem(int rowNumber, Graphics &g, int w, int h, bool rowIsSelected)
{
    jassert(this->currentActionsProvider != nullptr);

    if (rowIsSelected)
    {
        g.fillAll(Colours::white.withAlpha(0.05f));
    }
    else if (rowNumber % 2)
    {
        g.fillAll(Colours::black.withAlpha(0.05f));
    }

    if (rowNumber >= this->currentActionsProvider->getFilteredActions().size())
    {
        return;
    }

    const auto action = this->currentActionsProvider->getFilteredActions().getUnchecked(rowNumber);

    g.setFont(21);
    const float margin = float(h / 12.f);

    g.setColour(findDefaultColour(ListBox::textColourId).withMultipliedAlpha(0.85f));

    auto glyphs = action->getGlyphArrangement();
    glyphs.justifyGlyphs(0, glyphs.getNumGlyphs(),
        (margin * 2), margin, float(w), float(h - (margin * 2)),
        Justification::centredLeft);
    
    glyphs.draw(g);
}

void Console::selectedRowsChanged(int lastRowSelected)
{
    // so what?
}

void Console::listBoxItemClicked(int row, const MouseEvent &)
{
    // apply and dismiss? or just highlight?
    //
}

void Console::moveRowSelectionBy(int offset)
{
    if (this->getNumRows() > 0)
    {
        const auto selectedRow = this->actionsList->getSelectedRow(0);
        const auto newSelectedRow = jlimit(0, this->getNumRows() - 1, selectedRow + offset);
        this->actionsList->selectRow(newSelectedRow);
    }
}

//===----------------------------------------------------------------------===//
// TextEditor::Listener
//===----------------------------------------------------------------------===//

void Console::textEditorTextChanged(TextEditor &ed)
{
    if (ed.getText().isNotEmpty())
    {
        bool foundValidPrefix = false;
        const auto firstChar = ed.getText()[0];
        for (auto *provider : this->actionsProviders)
        {
            if (provider->usesPrefix(firstChar))
            {
                foundValidPrefix = true;
                this->currentActionsProvider = provider;
                break;
            }
        }

        if (foundValidPrefix && ed.getText().length() == 1)
        {
            this->currentActionsProvider->clearFilter();
        }
        else
        {
            this->currentActionsProvider->updateFilter(ed.getText(), foundValidPrefix);
        }
    }
    else
    {
        this->currentActionsProvider = this->defaultActionsProvider;
        this->currentActionsProvider->clearFilter();
    }

    this->actionsList->updateContent();
    if (this->getNumRows() > 0)
    {
        this->actionsList->selectRow(0);
    }

    // force repaint, sometimes it doesn't update underlined matches:
    this->actionsList->repaint();

    // todo only save at exit?
    App::Config().setProperty(Serialization::Config::lastSearch, ed.getText());
}

void Console::textEditorReturnKeyPressed(TextEditor &)
{
    // todo apply selected action

    // since the command was applied, clear the remembered search
    // ?
    App::Config().setProperty(Serialization::Config::lastSearch, "");

    this->dismiss();
}

void Console::textEditorEscapeKeyPressed(TextEditor &)
{
    this->cancelAndDismiss();
}

void Console::textEditorFocusLost(TextEditor&)
{
    //const auto *focusedComponent = Component::getCurrentlyFocusedComponent();
    if (this->textEditor->getText().isNotEmpty()
        //&& focusedComponent != this->okButton.get()
        )
    {
        this->dismiss();
    }
    else
    {
        this->textEditor->grabKeyboardFocus();
    }
}

void Console::dismiss()
{
    this->fadeOut();
    delete this;
}

void Console::fadeOut()
{
    const int fadeoutTime = 200;
    auto &animator = Desktop::getInstance().getAnimator();
    if (App::isOpenGLRendererEnabled())
    {
        animator.animateComponent(this, this->getBounds().reduced(30).translated(0, -30), 0.f, fadeoutTime, true, 0.0, 0.0);
    }
    else
    {
        animator.animateComponent(this, this->getBounds(), 0.f, fadeoutTime, true, 0.0, 0.0);
    }
}

void Console::updatePosition()
{
    this->setTopLeftPosition(this->getParentWidth() / 2 - this->getWidth() / 2, HEADLINE_HEIGHT + 1);
}

void Console::cancelAndDismiss()
{
    // todo cancel
    this->dismiss();
}


bool ConsoleTextEditor::keyPressed(const KeyPress &key)
{
    static const juce_wchar tildaKey = '`';
    if (key.isKeyCode(tildaKey))
    {
        this->escapePressed();
        return true;
    }
    else if (key.isKeyCode(KeyPress::downKey) || key.isKeyCode(KeyPress::upKey))
    {
        return false;
    }

    return TextEditor::keyPressed(key);
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Console" template="../../Template"
                 componentName="" parentClasses="public Component, public TextEditor::Listener, public ListBoxModel"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="620"
                 initialHeight="400">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="inputAttemptWhenModal()"/>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ffffff"/>
  <JUCERCOMP name="" id="5d11a023e4905d74" memberName="shadowDn" virtualName=""
             explicitFocusOrder="0" pos="0 0R 0M 8" posRelativeX="80e2df40dfc6307e"
             posRelativeY="80e2df40dfc6307e" posRelativeW="80e2df40dfc6307e"
             sourceFile="../Themes/ShadowDownwards.cpp" constructorParams="ShadowType::Normal"/>
  <JUCERCOMP name="" id="80e2df40dfc6307e" memberName="bg" virtualName=""
             explicitFocusOrder="0" pos="12 0 24M 160" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="72514a37e5bc1299" memberName="shadowL" virtualName=""
             explicitFocusOrder="0" pos="0 0 12 -44M" posRelativeH="2362db9f8826e90f"
             sourceFile="../Themes/ShadowLeftwards.cpp" constructorParams="ShadowType::Normal"/>
  <JUCERCOMP name="" id="3cd9e82f6261eff1" memberName="shadowR" virtualName=""
             explicitFocusOrder="0" pos="0Rr 0 12 -44M" posRelativeH="2362db9f8826e90f"
             sourceFile="../Themes/ShadowRightwards.cpp" constructorParams="ShadowType::Normal"/>
  <GENERICCOMPONENT name="" id="3abf9d1982e1c63b" memberName="textEditor" virtualName=""
                    explicitFocusOrder="0" pos="18 6 36M 32" class="ConsoleTextEditor"
                    params=""/>
  <GENERICCOMPONENT name="" id="2362db9f8826e90f" memberName="actionsList" virtualName=""
                    explicitFocusOrder="0" pos="0 -4R 0M 114" posRelativeX="3abf9d1982e1c63b"
                    posRelativeY="3abf9d1982e1c63b" posRelativeW="3abf9d1982e1c63b"
                    class="ListBox" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



