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

#include "CommandPalette.h"

//[MiscUserDefs]
#include "CommandIDs.h"
#include "SerializationKeys.h"

#include "Headline.h"
#include "Config.h"

#include "Workspace.h"
#include "MainLayout.h"
#include "ProjectNode.h"
#include "PianoRoll.h"

static const juce_wchar kTildaKey = '`';
//[/MiscUserDefs]

CommandPalette::CommandPalette(ProjectNode *project, HybridRoll *roll)
{
    this->shadowDn.reset(new ShadowDownwards(ShadowType::Normal));
    this->addAndMakeVisible(shadowDn.get());
    this->bg.reset(new PanelBackgroundC());
    this->addAndMakeVisible(bg.get());
    this->shadowL.reset(new ShadowLeftwards(ShadowType::Normal));
    this->addAndMakeVisible(shadowL.get());
    this->shadowR.reset(new ShadowRightwards(ShadowType::Normal));
    this->addAndMakeVisible(shadowR.get());
    this->textEditor.reset(new CommandPaletteTextEditor());
    this->addAndMakeVisible(textEditor.get());

    this->actionsList.reset(new ListBox());
    this->addAndMakeVisible(actionsList.get());


    //[UserPreSize]
    const auto lastText = App::Config().getProperty(Serialization::Config::lastSearch);

    // some help and hotkey commands list (depending on the current page):
    this->actionsProviders.addArray(App::Layout().getCommandPaletteActionProviders());

    // projects list
    this->actionsProviders.addArray(App::Workspace().getCommandPaletteActionProviders());

    // timeline events and version control, if opened in a project sub-node:
    if (project != nullptr)
    {
        this->actionsProviders.addArray(project->getCommandPaletteActionProviders());
    }

    // chord tools, if also opened in a piano roll:
    if (auto *pianoRoll = dynamic_cast<PianoRoll *>(roll))
    {
        if (pianoRoll->isShowing())
        {
            this->actionsProviders.addArray(pianoRoll->getCommandPaletteActionProviders());
        }
    }

    jassert(!this->actionsProviders.isEmpty());

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

CommandPalette::~CommandPalette()
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

void CommandPalette::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CommandPalette::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    shadowDn->setBounds(12 + 0, 0 + 224, (getWidth() - 24) - 0, 8);
    bg->setBounds(12, 0, getWidth() - 24, 224);
    shadowL->setBounds(0, 0, 12, 174 - -44);
    shadowR->setBounds(getWidth() - 12, 0, 12, 174 - -44);
    textEditor->setBounds(18, 6, getWidth() - 36, 32);
    actionsList->setBounds(18 + 0, 6 + 32 - -4, (getWidth() - 36) - 0, 174);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void CommandPalette::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->updatePosition();
    //[/UserCode_parentHierarchyChanged]
}

void CommandPalette::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::DismissModalDialogAsync)
    {
        this->cancelAndDismiss();
    }
    //[/UserCode_handleCommandMessage]
}

bool CommandPalette::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    if (key.isKeyCode(KeyPress::escapeKey))
    {
        // todo test
        // if escape, palette will first erase the text, if any, and then close?
        if (this->textEditor->isEmpty())
        {
            this->cancelAndDismiss();
        }
        else
        {
            this->textEditor->setText({});
        }
        return true;
    }
    else if (key.isKeyCode(kTildaKey))
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
    else if (key.isKeyCode(KeyPress::pageDownKey))
    {
        this->moveRowSelectionBy(10); // fixme - not a hardcoded page size pls!
        return true;
    }
    else if (key.isKeyCode(KeyPress::pageUpKey))
    {
        this->moveRowSelectionBy(-10); // fixme - not a hardcoded page size pls!
        return true;
    }

    return false;
    //[/UserCode_keyPressed]
}

void CommandPalette::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->postCommandMessage(CommandIDs::DismissModalDialogAsync);
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

int CommandPalette::getNumRows()
{
    return this->currentActionsProvider->getFilteredActions().size();
}

void CommandPalette::paintListBoxItem(int rowNumber, Graphics &g, int w, int h, bool rowIsSelected)
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

    const auto colour = findDefaultColour(ListBox::textColourId)
        .interpolatedWith(action->getColor(), 0.25f);

    // main text
    g.setColour(colour);

    auto glyphs = action->getGlyphArrangement();
    glyphs.justifyGlyphs(0, glyphs.getNumGlyphs(),
        (margin * 2), 0.f, float(w), float(h),
        Justification::centredLeft);

    glyphs.draw(g);

    // hint text
    g.setColour(colour.withMultipliedAlpha(0.75f));

    g.drawFittedText(action->getHint(),
        0, 0, w - int(margin * 2), h,
        Justification::centredRight, 1);
}

void CommandPalette::selectedRowsChanged(int lastRowSelected)
{
    // so what?
}

void CommandPalette::listBoxItemClicked(int row, const MouseEvent &)
{
    // apply and dismiss? or just highlight?
    //
}

void CommandPalette::moveRowSelectionBy(int offset)
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

void CommandPalette::textEditorTextChanged(TextEditor &ed)
{
    if (ed.getText().isNotEmpty())
    {
        bool foundValidPrefix = false;
        const auto firstChar = ed.getText()[0];
        for (auto provider : this->actionsProviders)
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

void CommandPalette::textEditorReturnKeyPressed(TextEditor &ed)
{
    // since the command was applied, clear the remembered search
    // ?

    const auto rowNumber = this->actionsList->getSelectedRow(0);
    if (rowNumber < 0 || rowNumber >= this->currentActionsProvider->getFilteredActions().size())
    {
        return;
    }

    const auto action = this->currentActionsProvider->getFilteredActions().getUnchecked(rowNumber);
    const auto callback = action->getCallback();
    if (callback != nullptr)
    {
        const BailOutChecker checker(this);
        const bool shouldDismiss = callback(ed);

        if (checker.shouldBailOut())
        {
            return;
        }

        if (shouldDismiss)
        {
            App::Config().setProperty(Serialization::Config::lastSearch, "");
            this->dismiss();
        }
    }
}

void CommandPalette::textEditorEscapeKeyPressed(TextEditor &)
{
    this->cancelAndDismiss();
}

void CommandPalette::textEditorFocusLost(TextEditor&)
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

void CommandPalette::dismiss()
{
    this->fadeOut();
    delete this;
}

void CommandPalette::fadeOut()
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

void CommandPalette::updatePosition()
{
    this->setTopLeftPosition(this->getParentWidth() / 2 - this->getWidth() / 2, HEADLINE_HEIGHT + 1);
}

void CommandPalette::cancelAndDismiss()
{
    // todo cancel
    this->dismiss();
}


bool CommandPaletteTextEditor::keyPressed(const KeyPress &key)
{
    // if escape or ~, just pass the keypress up
    if (key.isKeyCode(kTildaKey) ||
        key.isKeyCode(KeyPress::escapeKey) ||
        key.isKeyCode(KeyPress::downKey) ||
        key.isKeyCode(KeyPress::upKey) ||
        key.isKeyCode(KeyPress::pageDownKey) ||
        key.isKeyCode(KeyPress::pageUpKey))
    {
        return false;
    }

    return TextEditor::keyPressed(key);
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="CommandPalette" template="../../Template"
                 componentName="" parentClasses="public Component, public TextEditor::Listener, public ListBoxModel"
                 constructorParams="ProjectNode *project, HybridRoll *roll" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="620" initialHeight="400">
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
             explicitFocusOrder="0" pos="12 0 24M 224" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="72514a37e5bc1299" memberName="shadowL" virtualName=""
             explicitFocusOrder="0" pos="0 0 12 -44M" posRelativeH="2362db9f8826e90f"
             sourceFile="../Themes/ShadowLeftwards.cpp" constructorParams="ShadowType::Normal"/>
  <JUCERCOMP name="" id="3cd9e82f6261eff1" memberName="shadowR" virtualName=""
             explicitFocusOrder="0" pos="0Rr 0 12 -44M" posRelativeH="2362db9f8826e90f"
             sourceFile="../Themes/ShadowRightwards.cpp" constructorParams="ShadowType::Normal"/>
  <GENERICCOMPONENT name="" id="3abf9d1982e1c63b" memberName="textEditor" virtualName=""
                    explicitFocusOrder="0" pos="18 6 36M 32" class="CommandPaletteTextEditor"
                    params=""/>
  <GENERICCOMPONENT name="" id="2362db9f8826e90f" memberName="actionsList" virtualName=""
                    explicitFocusOrder="0" pos="0 -4R 0M 174" posRelativeX="3abf9d1982e1c63b"
                    posRelativeY="3abf9d1982e1c63b" posRelativeW="3abf9d1982e1c63b"
                    class="ListBox" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



