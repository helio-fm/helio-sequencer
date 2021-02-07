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
#include "HeadlineItem.h"
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
#include "ProjectNode.h"
#include "PianoTrackNode.h"
#include "PatternEditorNode.h"
#include "VersionControlNode.h"
#include "SequencerLayout.h"
#include "ComponentIDs.h"
#include "CommandIDs.h"
#include "ColourIDs.h"
#include "ColourSchemesManager.h"
#include "CommandPaletteCommonActions.h"

class InitScreen final : public Component, private Timer
{
public:

    InitScreen() : fillColour(findDefaultColour(ColourIDs::BackgroundA::fill))
    {
        this->setOpaque(false);
        this->setWantsKeyboardFocus(false);
        this->setInterceptsMouseClicks(true, false);
        this->setPaintingIsUnclipped(true);
    }

    void paint(Graphics &g) override
    {
        g.setColour(this->fillColour);
        g.fillRect(this->getLocalBounds());
    }

    void visibilityChanged() override
    {
        if (this->isVisible())
        {
            // now that this dummy screen is visible, init the workspace
            this->postCommandMessage(CommandIDs::InitWorkspace);
        }
    }

    void handleCommandMessage(int commandId) override
    {
        if (commandId == CommandIDs::InitWorkspace)
        {
            App::Workspace().init();
            App::Layout().setVisible(true);
            this->startFadeOut();
        }
    }

private:

    void timerCallback() override
    {
        const auto newFill = this->fillColour.interpolatedWith(Colours::transparentBlack, 0.2f);

        if (this->fillColour == newFill)
        {
            this->stopTimer();
            App::Layout().clearInitScreen();
        }
        else
        {
            this->fillColour = newFill;
            this->repaint();
        }
    }

    inline void startFadeOut()
    {
        this->toFront(false);
        this->startTimerHz(60);
    }

    Colour fillColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InitScreen)
};

MainLayout::MainLayout()
{
    this->setComponentID(ComponentIDs::mainLayoutId);
    this->setPaintingIsUnclipped(true);
    this->setVisible(false);
    this->setOpaque(true);

    this->setInterceptsMouseClicks(false, true);
    this->setMouseClickGrabsKeyboardFocus(true);
    this->setWantsKeyboardFocus(true);
    this->setFocusContainer(true);

    this->tooltipContainer = make<TooltipContainer>();
    this->addChildComponent(this->tooltipContainer.get());

    this->headline = make<Headline>();

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

    this->consoleCommonActions = make<CommandPaletteCommonActions>();

    if (const bool quickStartMode = App::Workspace().isInitialized())
    {
        this->setVisible(true);
        this->clearInitScreen();
    }
    else
    {
#if PLATFORM_DESKTOP
        this->initScreen = make<InitScreen>();
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

void MainLayout::clearInitScreen()
{
    if (this->initScreen != nullptr)
    {
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

static ProjectNode *findProjectForSelectedNode(TreeNode *source)
{
    jassert(source != nullptr);

    if (auto *projectNode = dynamic_cast<ProjectNode *>(source))
    {
        return projectNode;
    }

    return source->findParentOfType<ProjectNode>();
}


void MainLayout::showPage(Component *page, TreeNode *source)
{
    jassert(page != nullptr);

    App::dismissAllModalComponents();

    if (source != nullptr)
    {
        App::Workspace().getNavigationHistory().addItemIfNeeded(source);
        this->headline->syncWithTree(App::Workspace().getNavigationHistory(), source);
        App::Config().setProperty(Serialization::Config::lastShownPageId,
            source->getNodeIdentifier(), false);

        // keep track of currently active project
        auto *newParentProject = findProjectForSelectedNode(source);
        if (newParentProject != this->currentProject)
        {
            if (auto *oldParentProject = dynamic_cast<ProjectNode *>(this->currentProject.get()))
            {
                oldParentProject->broadcastDeactivateProjectSubtree();
            }

            this->currentProject = newParentProject;

            if (newParentProject != nullptr)
            {
                newParentProject->broadcastActivateProjectSubtree();
            }
        }
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
    this->visibleCommandReceivers.clearQuick();
    findVisibleCommandReceivers(this->currentContent.getComponent(), this->visibleCommandReceivers);
    this->consoleCommonActions->setActiveCommandReceivers(this->visibleCommandReceivers);
}

//===----------------------------------------------------------------------===//
// Breadcrumbs
//===----------------------------------------------------------------------===//

HeadlineItem *MainLayout::getMenuTail() const
{
    return this->headline->getTailItem();
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
        this->tooltipContainer->showWithComponent(make<GenericTooltip>(message), timeoutMs);
    }

    if (type == TooltipType::Success)
    {
        App::showModalComponent(make<SuccessTooltip>());
    }
    else if (type == TooltipType::Failure)
    {
        App::showModalComponent(make<FailTooltip>());
    }
}

void MainLayout::hideTooltipIfAny()
{
    this->tooltipContainer->showWithComponent(nullptr);
}

Rectangle<int> MainLayout::getBoundsForPopups() const
{
    Rectangle<int> r(this->getLocalBounds());

    r.removeFromLeft(Globals::UI::sidebarWidth);
    r.removeFromRight(Globals::UI::sidebarWidth);

    if (App::isUsingNativeTitleBar())
    {
        r.removeFromTop(Globals::UI::headlineHeight);
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
        this->headline->setBounds(r.removeFromTop(Globals::UI::headlineHeight));
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
#if PLATFORM_MOBILE
    return false;
#elif PLATFORM_DESKTOP

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
    if (auto *project = findParentProjectOfSelectedNode())
    {
        project->getTransport().stopPlaybackAndRecording();

        HybridRoll *activeRoll = nullptr;
        auto *activeNode = App::Workspace().getTreeRoot()->findActiveNode();
        if (nullptr != dynamic_cast<PianoTrackNode *>(activeNode))
        {
            activeRoll = project->getLastFocusedRoll();
        }

        // activeRoll is ok to be null
        // (project is too, but there'll be no useful content shown):
        App::showModalComponent(make<CommandPalette>(project, activeRoll));
    }
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
                project->selectFirstChildOfType<PianoTrackNode>();
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
            project->selectFirstChildOfType<PatternEditorNode>();
        }
        break;
    case CommandIDs::SwitchToVersioningMode:
        if (auto *project = findParentProjectOfSelectedNode())
        {
            project->selectFirstChildOfType<VersionControlNode>();
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

void MainLayout::broadcastCommandMessage(int commandId)
{
    this->postCommandMessage(commandId);

    this->visibleCommandReceivers.clearQuick();
    findVisibleCommandReceivers(this->currentContent.getComponent(), this->visibleCommandReceivers);

    for (auto *receiver : this->visibleCommandReceivers)
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
