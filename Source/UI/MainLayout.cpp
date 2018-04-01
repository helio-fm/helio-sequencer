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
#include "InitScreen.h"
#include "NavigationSidebar.h"
#include "ToolsSidebar.h"
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

void hideMarkersRecursive(TreeItem *startFrom)
{
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
    return (this->currentContent == page);
}

void MainLayout::showPage(Component *page, TreeItem *source)
{
    App::dismissAllModalComponents();
    
#if HAS_FADING_PAGECHANGE
    this->pageFader.cancelAllAnimations(true);
#endif
    
    if (source != nullptr)
    {
        hideMarkersRecursive(App::Workspace().getTreeRoot());
        source->setMarkerVisible(true);
        App::Workspace().getNavigationHistory().addItemIfNeeded(source);
        this->headline->syncWithTree(App::Workspace().getNavigationHistory(), source);
    }

    if (this->currentContent != nullptr)
    {
        // баг здесь. currentContent может быть композитным компонентом типа оригами, в котором уже уничтожен ключевой кусок
#if HAS_FADING_PAGECHANGE
        //this->currentContent->toFront(false);
        //this->pageFader.animateComponent(this->currentContent, this->currentContent->getBounds(), 0.f, 200, true, 0.0, 0.0);
#endif

        this->currentContent->setVisible(false);
        this->removeChildComponent(this->currentContent);
    }
    
    this->currentContent = page;    

    this->addAndMakeVisible(this->currentContent);
    this->resized();

    this->currentContent->setExplicitFocusOrder(1);

#if HAS_FADING_PAGECHANGE
    {
        this->currentContent->toFront(false);

#if JUCE_WINDOWS
        if (MainWindow::isOpenGLRendererEnabled())
        {
            this->currentContent->setAlpha(0.f);
            this->pageFader.animateComponent(this->currentContent, this->currentContent->getBounds(), 1.f, 200, false, 0.0, 0.0);
        }
#else
        this->currentContent->setAlpha(0.f);
        this->pageFader.animateComponent(this->currentContent, this->currentContent->getBounds(), 1.f, 200, false, 0.0, 0.0);
#endif

    }
#endif

    this->currentContent->toFront(false);
    
    Config::set(Serialization::Config::lastShownPageId, source->getItemIdentifierString());

    //const Component *focused = Component::getCurrentlyFocusedComponent();
    //Logger::outputDebugString(focused ? focused->getName() : "null");
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

// a hack!
Rectangle<int> MainLayout::getPageBounds() const
{
    Rectangle<int> r(this->getLocalBounds());
    r.removeFromLeft(NAVIGATION_SIDEBAR_WIDTH);
    r.removeFromRight(TOOLS_SIDEBAR_WIDTH);
    return r;
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void MainLayout::resized()
{
    Rectangle<int> r(this->getLocalBounds());
    if (r.isEmpty()) { return; }

    this->headline->setBounds(r.removeFromTop(this->headline->getHeight()));

    if (this->currentContent)
    {
        this->currentContent->setBounds(r);
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
#endif

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

void MainLayout::handleCommandMessage(int commandId)
{
    switch (commandId)
    {
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
