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
#include "MainLayout.h"
#include "TooltipContainer.h"
#include "Headline.h"
#include "HelioTheme.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "Config.h"
#include "SerializationKeys.h"
#include "RootNode.h"
#include "GenericTooltip.h"
#include "SuccessTooltip.h"
#include "FailTooltip.h"
#include "CommandPalette.h"
#include "MidiTrackNode.h"
#include "ProjectNode.h"
#include "PianoTrackNode.h"
#include "PatternEditorNode.h"
#include "VersionControlNode.h"
#include "InitScreen.h"
#include "SequencerLayout.h"
#include "JsonSerializer.h"
#include "ComponentIDs.h"
#include "CommandIDs.h"
#include "Workspace.h"
#include "Config.h"
#include "ColourSchemesManager.h"
#include "CommandPaletteCommonActions.h"

MainLayout::MainLayout() :
    currentContent(nullptr)
{
    this->setComponentID(ComponentIDs::mainLayoutId);
    this->setPaintingIsUnclipped(true);
    this->setVisible(false);
    this->setOpaque(true);

    this->setInterceptsMouseClicks(false, true);
    this->setMouseClickGrabsKeyboardFocus(true);
    this->setWantsKeyboardFocus(true);
    this->setFocusContainer(true);

    this->tooltipContainer = makeUnique<TooltipContainer>();
    this->addChildComponent(this->tooltipContainer.get());

    this->headline = makeUnique<Headline>();

    if (App::isUsingNativeTitleBar())
    {
        this->addAndMakeVisible(this->headline.get());
    }
    else
    {
        App::setTitleBarComponent(this->headline.get());
    }

    // TODO make it able for user to select a scheme in settings page
    this->hotkeyScheme = App::Config().getHotkeySchemes()->getCurrent();

    this->consoleCommonActions = makeUnique<CommandPaletteCommonActions>();

    if (const bool quickStartMode = App::Workspace().isInitialized())
    {
        this->show();
    }
    else
    {
#if HELIO_DESKTOP
        this->initScreen = makeUnique<InitScreen>();
        this->addAndMakeVisible(this->initScreen.get());
#endif
    }
}

MainLayout::~MainLayout()
{
    this->removeAllChildren();
    this->consoleCommonActions = nullptr;
    this->hotkeyScheme = nullptr;
    this->headline = nullptr;
}

void MainLayout::show()
{
    this->setVisible(true);

    if (this->initScreen != nullptr)
    {
        this->initScreen->toFront(false);
        this->fader.fadeOut(this->initScreen.get(), 200);
        this->initScreen = nullptr;
    }
}

void MainLayout::restoreLastOpenedPage()
{
    const auto lastPageId = App::Config().getProperty(Serialization::Config::lastShownPageId);
    App::Workspace().selectTreeNodeWithId(lastPageId);
}

//===----------------------------------------------------------------------===//
// Pages
//===----------------------------------------------------------------------===//

static void findVisibleCommandReceivers(Component *root, Array<Component *> &outArray)
{
    if (root == nullptr)
    {
        return;
    }

    if (root->isEnabled() && root->isShowing() &&
        root->getComponentID().isNotEmpty())
    {
        outArray.add(root);
    }

    // avoid iterating children of complex components like piano roll
    if (root->getNumChildComponents() < 16)
    {
        for (const auto child : root->getChildren())
        {
            findVisibleCommandReceivers(child, outArray);
        }
    }
}

bool MainLayout::isShowingPage(Component *page) const noexcept
{
    jassert(page != nullptr);
    return (this->currentContent == page);
}

void MainLayout::showPage(Component *page, TreeNode *source)
{
    jassert(page != nullptr);

    App::dismissAllModalComponents();

    if (source != nullptr)
    {
        App::Workspace().getNavigationHistory().addItemIfNeeded(source);
        this->headline->syncWithTree(App::Workspace().getNavigationHistory(), source);
        App::Config().setProperty(Serialization::Config::lastShownPageId, source->getNodeIdentifier(), false);
    }

    if (this->currentContent != nullptr)
    {
        this->currentContent->setVisible(false);
        this->removeChildComponent(this->currentContent);
    }
    
    this->currentContent = page;    

    this->addAndMakeVisible(this->currentContent);
    this->resized();

    this->currentContent->setExplicitFocusOrder(1);
    this->currentContent->toFront(false);

    // fill up console commands for visible command targets
    this->visibleCommandReceiversCache.clearQuick();
    findVisibleCommandReceivers(this->currentContent.getComponent(), this->visibleCommandReceiversCache);
    this->consoleCommonActions->setActiveCommandReceivers(this->visibleCommandReceiversCache);
}

void MainLayout::showSelectionMenu(WeakReference<HeadlineItemDataSource> menuSource)
{
    this->headline->showSelectionMenu(menuSource);
}

void MainLayout::hideSelectionMenu()
{
    this->headline->hideSelectionMenu();
}

//===----------------------------------------------------------------------===//
// Tooltips
//===----------------------------------------------------------------------===//

void MainLayout::showTooltip(const String &message, TooltipType type, int timeoutMs)
{
    if (message.isNotEmpty())
    {
        this->tooltipContainer->showWithComponent(makeUnique<GenericTooltip>(message), timeoutMs);
    }

    if (type == TooltipType::Success)
    {
        App::showModalComponent(makeUnique<SuccessTooltip>());
    }
    else if (type == TooltipType::Failure)
    {
        App::showModalComponent(makeUnique<FailTooltip>());
    }
}

void MainLayout::hideTooltipIfAny()
{
    this->tooltipContainer->showWithComponent(nullptr);
}

// a hack!
Rectangle<int> MainLayout::getPageBounds() const
{
    Rectangle<int> r(this->getLocalBounds());

    r.removeFromLeft(SEQUENCER_SIDEBAR_WIDTH);
    r.removeFromRight(SEQUENCER_SIDEBAR_WIDTH);

    if (App::isUsingNativeTitleBar())
    {
        r.removeFromTop(HEADLINE_HEIGHT);
    }

    return r;
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void MainLayout::resized()
{
    Rectangle<int> r(this->getLocalBounds());
    if (r.isEmpty())
    {
        return;
    }

    if (App::isUsingNativeTitleBar())
    {
        this->headline->setBounds(r.removeFromTop(HEADLINE_HEIGHT));
    }

    if (this->currentContent)
    {
        this->currentContent->setBounds(r);
    }

    if (this->initScreen)
    {
        this->initScreen->setBounds(this->getLocalBounds());
    }
}

void MainLayout::lookAndFeelChanged()
{
    this->repaint();
}

bool MainLayout::keyPressed(const KeyPress &key)
{
    // under Android, it sends here the ';' key, 
    // when you type a capital letter or a char like @, !, ?
    // a piece of juce.
#if HELIO_MOBILE
    return false;
#elif HELIO_DESKTOP

    if (Component::getNumCurrentlyModalComponents() > 0)
    {
        return false;
    }

    if (this->hotkeyScheme->dispatchKeyPress(key,
        this, this->currentContent.getComponent()))
    {
        return true;
    }

#if JUCE_ENABLE_LIVE_CONSTANT_EDITOR

    if (key == KeyPress::createFromDescription("f1"))
    {
        if (HelioTheme *ht = dynamic_cast<HelioTheme *>(&this->getLookAndFeel()))
        {
            auto scheme = App::Config().getColourSchemes()->getCurrent();
            ht->initColours(scheme);
            this->repaint();

            scheme->syncWithLiveConstantEditor();
            const auto schemeNode(scheme->serialize());
            String schemeNodeString;
            JsonSerializer serializer;
            serializer.saveToString(schemeNodeString, schemeNode);
            SystemClipboard::copyTextToClipboard(schemeNodeString);
            return true;
        }
    }

#endif
    
    return false;
#endif
}

bool MainLayout::keyStateChanged(bool isKeyDown)
{
    if (Component::getNumCurrentlyModalComponents() > 0)
    {
        return false;
    }

    return this->hotkeyScheme->dispatchKeyStateChange(isKeyDown,
        this, this->currentContent.getComponent());
}

void MainLayout::modifierKeysChanged(const ModifierKeys &modifiers)
{
    // TODO do I need to handle this?
}

static ProjectNode *findParentProjectOfSelectedNode()
{
    if (auto *active = App::Workspace().getTreeRoot()->findActiveNode())
    {
        if (auto *projectItself = dynamic_cast<ProjectNode *>(active))
        {
            return projectItself;
        }
        else if (auto *projectParent = active->findParentOfType<ProjectNode>())
        {
            return projectParent;
        }
    }

    return nullptr;
}

static void emitCommandPalette()
{
    auto *project = findParentProjectOfSelectedNode();
    if (project != nullptr)
    {
        project->getTransport().stopPlayback();
        auto *activeNode = App::Workspace().getTreeRoot()->findActiveNode();
        if (nullptr != dynamic_cast<PianoTrackNode *>(activeNode))
        {
            auto *activeRoll = project->getLastFocusedRoll();
            App::showModalComponent(makeUnique<CommandPalette>(project, activeRoll));
            return;
        }
    }

    jassertfalse;
}

void MainLayout::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
    case CommandIDs::SwitchToEditMode:
        if (auto *project = findParentProjectOfSelectedNode())
        {
            if (project->getLastShownTrack() == nullptr)
            {
                project->selectChildOfType<PianoTrackNode>();
            }
            else
            {
                project->getLastShownTrack()->setSelected();
            }
        }
        break;
    case CommandIDs::SwitchToArrangeMode:
        if (auto *project = findParentProjectOfSelectedNode())
        {
            project->selectChildOfType<PatternEditorNode>();
        }
        break;
    case CommandIDs::SwitchToVersioningMode:
        if (auto *project = findParentProjectOfSelectedNode())
        {
            project->selectChildOfType<VersionControlNode>();
        }
        break;
    case CommandIDs::ShowRootPage:
        App::Workspace().getTreeRoot()->setSelected();
        break;
    case CommandIDs::ShowPreviousPage:
        App::Workspace().navigateBackwardIfPossible();
        break;
    case CommandIDs::ShowNextPage:
        App::Workspace().navigateForwardIfPossible();
        break;
    case CommandIDs::CommandPalette:
        emitCommandPalette();
        break;
    case CommandIDs::CommandPaletteWithMode:
    {
        const auto modeKey = this->hotkeyScheme->getLastKeyPress().getTextCharacter();
        App::Config().setProperty(Serialization::Config::lastSearch, String::charToString(modeKey));
        emitCommandPalette();
        break;
    }
    default:
        break;
    }
}

static inline void broadcastMessage(Component *root, int commandId)
{
    if (root == nullptr)
    {
        return;
    }

    if (root->isEnabled() && root->isShowing() &&
        root->getComponentID().isNotEmpty())
    {
        root->postCommandMessage(commandId);
    }

    for (const auto child : root->getChildren())
    {
        broadcastMessage(child, commandId);
    }
}

void MainLayout::broadcastCommandMessage(int commandId)
{
    this->postCommandMessage(commandId);

    this->visibleCommandReceiversCache.clearQuick();
    findVisibleCommandReceivers(this->currentContent.getComponent(), this->visibleCommandReceiversCache);

    for (auto *receiver : this->visibleCommandReceiversCache)
    {
        receiver->postCommandMessage(commandId);
    }
}

//===----------------------------------------------------------------------===//
// Command Palette
//===----------------------------------------------------------------------===//

Array<CommandPaletteActionsProvider *> MainLayout::getCommandPaletteActionProviders() const
{
    return { this->consoleCommonActions.get() };
}
