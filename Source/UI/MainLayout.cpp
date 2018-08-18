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
#include "App.h"
#include "Workspace.h"
#include "AudioCore.h"
#include "Config.h"
#include "SerializationKeys.h"
#include "MainWindow.h"
#include "RootTreeItem.h"
#include "GenericTooltip.h"
#include "MidiTrackTreeItem.h"
#include "ProjectTreeItem.h"
#include "PianoTrackTreeItem.h"
#include "PatternEditorTreeItem.h"
#include "VersionControlTreeItem.h"
#include "InitScreen.h"
#include "SequencerLayout.h"
#include "ColourSchemesManager.h"
#include "HotkeySchemesManager.h"
#include "JsonSerializer.h"
#include "ComponentIDs.h"
#include "CommandIDs.h"
#include "Workspace.h"
#include "App.h"

MainLayout::MainLayout() :
    currentContent(nullptr)
{
    this->setComponentID(ComponentIDs::mainLayoutId);
    this->setInterceptsMouseClicks(false, true);
    this->setOpaque(true);
    this->setVisible(false);
    
    this->tooltipContainer = new TooltipContainer();
    this->addChildComponent(this->tooltipContainer);

    this->headline = new Headline();
    this->addAndMakeVisible(this->headline);

    // TODO make it able for user to select a scheme in settings page
    this->hotkeyScheme = HotkeySchemesManager::getInstance().getCurrentScheme();

    this->setPaintingIsUnclipped(true);
    this->setOpaque(false);

    this->setMouseClickGrabsKeyboardFocus(true);
    this->setWantsKeyboardFocus(true);
    this->setFocusContainer(true);

    if (const bool quickStartMode = App::Workspace().isInitialized())
    {
        if (HelioTheme *ht = dynamic_cast<HelioTheme *>(&this->getLookAndFeel()))
        {
            ht->updateBackgroundRenders();
        }

        this->show();
    }
    else
    {
#if HELIO_DESKTOP
        this->initScreen = new InitScreen();
        this->addAndMakeVisible(this->initScreen);
#endif
    }
}

MainLayout::~MainLayout()
{
    this->removeAllChildren();
    this->hotkeyScheme = nullptr;
    this->headline = nullptr;
}

void MainLayout::show()
{
    this->setVisible(true);

    if (this->initScreen != nullptr)
    {
        this->initScreen->toFront(false);
        this->fader.fadeOut(this->initScreen, 500);
        this->initScreen = nullptr;
    }
}

void MainLayout::forceRestoreLastOpenedPage()
{
    App::Workspace().activateSubItemWithId(Config::get(Serialization::Config::lastShownPageId));
}

//===----------------------------------------------------------------------===//
// Pages
//===----------------------------------------------------------------------===//

void hideMarkersRecursive(WeakReference<TreeItem> startFrom)
{
    jassert(startFrom != nullptr);
    startFrom->setMarkerVisible(false);

    for (int i = 0; i < startFrom->getNumSubItems(); ++i)
    {
        if (TreeItem *childOfType = dynamic_cast<TreeItem *>(startFrom->getSubItem(i)))
        {
            hideMarkersRecursive(childOfType);
        }
    }
}

bool MainLayout::isShowingPage(Component *page) const noexcept
{
    jassert(page != nullptr);
    return (this->currentContent == page);
}

void MainLayout::showPage(Component *page, TreeItem *source)
{
    jassert(page != nullptr);

    App::dismissAllModalComponents();

    if (source != nullptr)
    {
        hideMarkersRecursive(App::Workspace().getTreeRoot());
        source->setMarkerVisible(true);
        App::Workspace().getNavigationHistory().addItemIfNeeded(source);
        this->headline->syncWithTree(App::Workspace().getNavigationHistory(), source);
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
    
    Config::set(Serialization::Config::lastShownPageId, source->getItemIdentifierString(), false);
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
// UI
//===----------------------------------------------------------------------===//

void MainLayout::showTooltip(const String &message, int timeOutMs)
{
    this->tooltipContainer->showWithComponent(new GenericTooltip(message), timeOutMs);
}

void MainLayout::showTooltip(Component *newTooltip, int timeOutMs)
{
    this->tooltipContainer->showWithComponent(newTooltip, timeOutMs);
}

void MainLayout::showTooltip(Component *newTooltip, Rectangle<int> callerScreenBounds, int timeOutMs)
{
    this->tooltipContainer->showWithComponent(newTooltip, callerScreenBounds, timeOutMs);
}

void MainLayout::showModalComponentUnowned(Component *targetComponent)
{
    ScopedPointer<Component> ownedTarget(targetComponent);
    this->addChildComponent(ownedTarget);

    const int fadeInTime = 200;
    Desktop::getInstance().getAnimator().animateComponent(ownedTarget,
        ownedTarget->getBounds(),
        1.f, fadeInTime, false, 0.0, 0.0);
    
    ownedTarget->toFront(false);
    ownedTarget->enterModalState(true, nullptr, true);
    ownedTarget.release();
}

void MainLayout::hideModalComponentUnowned()
{
    ScopedPointer<Component> deleter(Component::getCurrentlyModalComponent());
}

// a hack!
Rectangle<int> MainLayout::getPageBounds() const
{
    Rectangle<int> r(this->getLocalBounds());
    r.removeFromLeft(SEQUENCER_SIDEBAR_WIDTH);
    r.removeFromRight(SEQUENCER_SIDEBAR_WIDTH);
    r.removeFromTop(HEADLINE_HEIGHT);
    return r;
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void MainLayout::resized()
{
    Rectangle<int> r(this->getLocalBounds());
    if (r.isEmpty()) { return; }

    this->headline->setBounds(r.removeFromTop(HEADLINE_HEIGHT));

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

    if (key == KeyPress::createFromDescription("command + r"))
    {
        if (HelioTheme *ht = dynamic_cast<HelioTheme *>(&this->getLookAndFeel()))
        {
            auto scheme = ColourSchemesManager::getInstance().getCurrentScheme();
            ht->updateBackgroundRenders(true);
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

static ProjectTreeItem *findParentProjectOfSelectedNode()
{
    if (auto *active = TreeItem::getActiveItem<TreeItem>(App::Workspace().getTreeRoot()))
    {
        if (auto *projectItself = dynamic_cast<ProjectTreeItem *>(active))
        {
            return projectItself;
        }
        else if (auto *projectParent = active->findParentOfType<ProjectTreeItem>())
        {
            return projectParent;
        }
    }

    return nullptr;
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
                project->selectChildOfType<PianoTrackTreeItem>();
            }
            else
            {
                project->getLastShownTrack()->setSelected(true, true);
            }
        }
        break;
    case CommandIDs::SwitchToArrangeMode:
        if (auto *project = findParentProjectOfSelectedNode())
        {
            project->selectChildOfType<PatternEditorTreeItem>();
        }
        break;
    case CommandIDs::SwitchToVersioningMode:
        if (auto *project = findParentProjectOfSelectedNode())
        {
            project->selectChildOfType<VersionControlTreeItem>();
        }
        break;
    case CommandIDs::ShowPreviousPage:
        App::Workspace().navigateBackwardIfPossible();
        break;
    case CommandIDs::ShowNextPage:
        App::Workspace().navigateForwardIfPossible();
        break;
    case CommandIDs::ToggleShowHideConsole:
        //this->toggleShowHideConsole();
        break;
    default:
        break;
    }
}

static void broadcastMessage(Component *root, int commandId)
{
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
    broadcastMessage(this->currentContent.getComponent(), commandId);
}
