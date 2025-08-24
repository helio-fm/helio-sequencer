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
#include "CommandPalette.h"
#include "Workspace.h"
#include "MainLayout.h"
#include "ProjectNode.h"
#include "PianoRoll.h"
#include "ShadowDownwards.h"
#include "PanelBackground.h"
#include "ShadowLeftwards.h"
#include "ShadowRightwards.h"
#include "HotkeyScheme.h"
#include "HelioTheme.h"
#include "Config.h"
#include "SerializationKeys.h"
#include "ComponentIDs.h"
#include "CommandIDs.h"

CommandPalette::CommandPalette(ProjectNode *project, RollBase *roll, const String &defaultText) :
    roll(roll)
{
    this->setComponentID(ComponentIDs::commandPalette);

    this->background = make<PanelBackground>();
    this->addAndMakeVisible(this->background.get());
    this->shadowDown = make<ShadowDownwards>(ShadowType::Normal);
    this->addAndMakeVisible(this->shadowDown.get());
    this->shadowLeft = make<ShadowLeftwards>(ShadowType::Light);
    this->addAndMakeVisible(this->shadowLeft.get());
    this->shadowRight = make<ShadowRightwards>(ShadowType::Light);
    this->addAndMakeVisible(this->shadowRight.get());
    this->textEditor = make<CommandPaletteTextEditor>();
    this->addAndMakeVisible(this->textEditor.get());

    this->actionsList = make<ListBox>();
    this->addAndMakeVisible(this->actionsList.get());
    
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
    if (auto *pianoRoll = dynamic_cast<PianoRoll *>(this->roll.getComponent()))
    {
        if (pianoRoll->isShowing())
        {
            this->actionsProviders.addArray(pianoRoll->getCommandPaletteActionProviders());
        }
    }

    jassert(!this->actionsProviders.isEmpty());

    this->rootActionsProvider = this->actionsProviders.getFirst();

    // the root list to include some help on available prefixed lists:
    CommandPaletteActionsProvider::Actions prefixedActionsHelp;
    for (auto provider : this->actionsProviders)
    {
        if (!provider->hasPrefix())
        {
            continue;
        }

        const auto prefix = String::charToString(provider->getPrefix());
        prefixedActionsHelp.add(CommandPaletteAction::action(
            provider->getName(), prefix, provider->getPriority())->
            withCallback([prefix](TextEditor &ed) { ed.setText(prefix); return false; }));
    }

    this->rootActionsProvider->setAdditionalActions(prefixedActionsHelp);
    this->currentActionsProvider = this->rootActionsProvider;

    this->actionsList->setRowHeight(CommandPalette::rowHeight);
    //this->actionsList->setMouseMoveSelectsRows(true); // breaks keyboard select :(
    this->actionsList->getViewport()->setScrollBarThickness(2);
    this->actionsList->setModel(this);

    this->textEditor->setReturnKeyStartsNewLine(false);
    this->textEditor->setPopupMenuEnabled(false);
    this->textEditor->setScrollbarsShown(false);
    this->textEditor->setCaretVisible(true);
    this->textEditor->setMultiLine(false);
    this->textEditor->setReadOnly(false);
    this->textEditor->setFont(Globals::UI::Fonts::M);
    this->textEditor->addListener(this);
    this->textEditor->setText(defaultText, false);

    this->setSize(620, 100);

    // a hack to update the list on start synchronously, even if the text is empty:
    this->textEditorTextChanged(*this->textEditor);
}

CommandPalette::~CommandPalette()
{
    this->textEditor->removeListener(this);
}

void CommandPalette::resized()
{
    constexpr auto marginX = 8;
    constexpr auto marginBottom = 10; // top margin is 0

    this->background->setBounds(marginX, 0,
        this->getWidth() - marginX * 2,
        this->getHeight() - marginBottom);

    this->shadowDown->setBounds(marginX - 1,
        this->getHeight() - marginBottom,
        this->getWidth() - marginX * 2 + 2,
        marginBottom);

    this->shadowLeft->setBounds(0, 0, marginX,
        this->getHeight() - marginBottom);

    this->shadowRight->setBounds(this->getWidth() - marginX,
        0, marginX, this->getHeight() - marginBottom);

    constexpr auto editorHeight = 32;
    constexpr auto contentMargin = 4;
    this->textEditor->setBounds(marginX + contentMargin, contentMargin,
        this->getWidth() - (marginX + contentMargin) * 2, editorHeight);

    this->actionsList->setBounds(marginX + contentMargin,
        contentMargin + editorHeight + 3,
        this->getWidth() - (marginX + contentMargin) * 2,
        this->getHeight() - (contentMargin * 2) -
            editorHeight - marginBottom - 3);
}

void CommandPalette::parentHierarchyChanged()
{
    this->updatePosition();
}

void CommandPalette::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::CommandPaletteClear:
        // on escape keypress first try to erase the text, if any:
        if (!this->textEditor->isEmpty())
        {
            this->textEditor->setText({});
        }
        else
        {
            this->dismiss();
        }
        break;
    case CommandIDs::CommandPaletteDismiss:
        this->dismiss();
        break;
    case CommandIDs::CommandPaletteCursorUp:
        this->moveRowSelectionBy(-1);
        break;
    case CommandIDs::CommandPaletteCursorDown:
        this->moveRowSelectionBy(1);
        break;
    case CommandIDs::CommandPaletteCursorPageUp:
        this->moveRowSelectionBy(-this->getNumVisibleRows());
        break;
    case CommandIDs::CommandPaletteCursorPageDown:
        this->moveRowSelectionBy(this->getNumVisibleRows());
        break;
    default:
        break;
    }
}

bool CommandPalette::keyPressed(const KeyPress &key)
{
    App::Config().getHotkeySchemes()->getCurrent()->dispatchKeyPress(key, this, this);
    return true;
}

void CommandPalette::inputAttemptWhenModal()
{
    this->dismiss();
}

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

    if (rowNumber >= this->getNumRows())
    {
        return;
    }

    const auto labelColour = findDefaultColour(ListBox::textColourId);

    const auto action = this->currentActionsProvider->
        getFilteredActions().getUnchecked(rowNumber);

    const auto mainTextColour = action->getColor()
        .interpolatedWith(labelColour, 0.5f);

    const auto highlightingColour = action->getColor()
        .interpolatedWith(labelColour, 0.75f);

    if (rowIsSelected)
    {
        g.fillAll(highlightingColour.withAlpha(0.1f));
    }
    else if (rowNumber % 2)
    {
        g.fillAll(highlightingColour.withAlpha(0.015f));
    }

    g.setFont(Globals::UI::Fonts::M);

    // main text
    g.setColour(mainTextColour);

    auto glyphs = action->getGlyphArrangement();
    glyphs.justifyGlyphs(0, glyphs.getNumGlyphs(),
        4.f, 0.f, float(w), float(h),
        Justification::centredLeft);

    glyphs.draw(g);

    // hint text
    g.setColour(mainTextColour
        .interpolatedWith(labelColour, 0.5f).withMultipliedAlpha(0.5f));

    HelioTheme::drawFittedText(g,
        action->getHint(),
        0, 0, w - 4, h,
        Justification::centredRight, 1);
}

void CommandPalette::selectedRowsChanged(int lastRowSelected) {}

void CommandPalette::listBoxItemClicked(int row, const MouseEvent &)
{
    // too fast? switch this to double click?
    this->actionsList->selectRow(row);
    this->applySelectedCommand();
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
    bool foundValidPrefix = false;
    this->currentActionsProvider = this->rootActionsProvider;

    if (ed.getText().isNotEmpty())
    {
        const auto firstChar = ed.getText()[0];
        for (auto provider : this->actionsProviders)
        {
            if (provider->getPrefix() == firstChar)
            {
                foundValidPrefix = true;
                this->currentActionsProvider = provider;
                break;
            }
        }
    }

    if ((foundValidPrefix && ed.getText().length() == 1) ||
        (!foundValidPrefix && ed.getText().isEmpty()))
    {
        this->currentActionsProvider->clearFilter();
    }
    else
    {
        this->currentActionsProvider->updateFilter(ed.getText(), foundValidPrefix);
    }

    this->actionsList->updateContent();
    if (this->getNumRows() > 0)
    {
        this->actionsList->selectRow(0);
    }

    this->setSize(this->getWidth(), this->getHeightToFitActions());

    // force repaint, sometimes it doesn't update the underlined matches:
    this->actionsList->repaint();
}

void CommandPalette::textEditorReturnKeyPressed(TextEditor &ed)
{
    this->applySelectedCommand();
}

void CommandPalette::textEditorEscapeKeyPressed(TextEditor &)
{
    this->dismiss();
}

void CommandPalette::textEditorFocusLost(TextEditor &)
{
    // prevents close on mouse click:
    this->textEditor->grabKeyboardFocus();
}

void CommandPalette::dismiss()
{
    if (App::isOpenGLRendererEnabled())
    {
        App::animateComponent(this,
            this->getBounds().reduced(10).translated(0, -10),
                0.f, Globals::UI::fadeOutShort, true, 1.0, 0.0);
    }
    else
    {
        App::animateComponent(this, this->getBounds(),
            0.f, Globals::UI::fadeOutShort, true, 1.0, 0.0);
    }

    UniquePointer<Component> deleter(this);
}

void CommandPalette::updatePosition()
{
    const auto top = App::isUsingNativeTitleBar() ?
        Globals::UI::headlineHeight : Globals::UI::headlineHeight + 1;

    const auto centered = this->getBounds().withCentre(Point<int>(this->getParentWidth() / 2, 0)
        .transformedBy(this->getTransform().inverted()));

    this->setTopLeftPosition(centered.getX(), top);
}

Array<KeyPress> CommandPalette::getAllCommandPaletteHotkeys()
{
    static Array<KeyPress> commandPaletteKeyPresses =
        App::Config().getHotkeySchemes()->getCurrent()->
            findKeyPressesForReceiver(ComponentIDs::commandPalette);

    return commandPaletteKeyPresses;
}

bool CommandPaletteTextEditor::keyPressed(const KeyPress &key)
{
    // pass the hotkey keypresses up
    for (const auto &usedAsHotkey : CommandPalette::getAllCommandPaletteHotkeys())
    {
        if (key == usedAsHotkey)
        {
            return false;
        }
    }

    return TextEditor::keyPressed(key);
}

void CommandPalette::applySelectedCommand()
{
    const auto rowNumber = this->actionsList->getSelectedRow(0);
    if (rowNumber < 0 || rowNumber >= this->getNumRows())
    {
        return;
    }

    const auto action = this->currentActionsProvider->getFilteredActions().getUnchecked(rowNumber);
    const auto callback = action->getCallback();
    if (callback != nullptr)
    {
        const BailOutChecker checker(this);
        const bool shouldDismiss = callback(*this->textEditor);

        if (checker.shouldBailOut())
        {
            return;
        }

        if (shouldDismiss)
        {
            this->dismiss();
        }
    }
}

int CommandPalette::getHeightToFitActions()
{
    constexpr auto minRows = 4;
    const auto margin = this->getHeight() - this->actionsList->getHeight();
    const auto maxRows = (App::Layout().getHeight() / 3) / CommandPalette::rowHeight;
    return jlimit(minRows, maxRows, this->getNumRows()) * CommandPalette::rowHeight + margin;
}

int CommandPalette::getNumVisibleRows() const noexcept
{
    return this->actionsList->getHeight() / CommandPalette::rowHeight;
}
