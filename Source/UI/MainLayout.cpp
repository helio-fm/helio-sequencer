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
#include "TransientTreeItems.h"
#include "MidiTrackTreeItem.h"
#include "Supervisor.h"
#include "InitScreen.h"
#include "NavigationSidebar.h"
#include "ToolsSidebar.h"
#include "ColourSchemeManager.h"
#include "HotkeyScheme.h"
#include "ComponentIDs.h"
#include "CommandIDs.h"
#include "Workspace.h"
#include "App.h"

namespace KeyboardFocusDebugger
{
    // This will sort a set of components, so that they are ordered in terms of
    // left-to-right and then top-to-bottom.
    struct ScreenPositionComparator
    {
        static int compareElements(const Component* const first, const Component* const second)
        {
            const int explicitOrder1 = getOrder(first);
            const int explicitOrder2 = getOrder(second);

            if (explicitOrder1 != explicitOrder2)
                return explicitOrder1 - explicitOrder2;

            const int yDiff = first->getY() - second->getY();

            return yDiff == 0 ? first->getX() - second->getX()
                : yDiff;
        }

        static int getOrder(const Component* const c)
        {
            const int order = c->getExplicitFocusOrder();
            return order > 0 ? order : (std::numeric_limits<int>::max() / 2);
        }
    };

    static void findAllFocusableComponents(Component* const parent, Array <Component*>& comps)
    {
        if (parent->getNumChildComponents() > 0)
        {
            Array <Component*> localComps;
            ScreenPositionComparator comparator;

            for (int i = parent->getNumChildComponents(); --i >= 0;)
            {
                Component* const c = parent->getChildComponent(i);

                if (c->isVisible() && c->isEnabled())
                    localComps.addSorted(comparator, c);
            }

            for (int i = 0; i < localComps.size(); ++i)
            {
                Component* const c = localComps.getUnchecked(i);

                if (c->getWantsKeyboardFocus())
                    comps.add(c);

                if (!c->isFocusContainer())
                    findAllFocusableComponents(c, comps);
            }
        }
    }

    static Component* findFocusContainer(Component* c)
    {
        c = c->getParentComponent();

        if (c != nullptr)
            while (c->getParentComponent() != nullptr && !c->isFocusContainer())
                c = c->getParentComponent();

        return c;
    }
}


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

    this->hotkeyScheme = new HotkeyScheme();
    {
        // TODO hot-keys manager
        const String hotkeysXmlString =
            String(CharPointer_UTF8(BinaryData::DefaultHotkeys_xml),
                BinaryData::DefaultHotkeys_xmlSize);
        const ScopedPointer<XmlElement> hotkeysXml(XmlDocument::parse(hotkeysXmlString));
        if (XmlElement *defaultScheme = hotkeysXml->getFirstChildElement())
        {
            this->hotkeyScheme->deserialize(*defaultScheme);
        }
    }

    this->setMouseClickGrabsKeyboardFocus(true);
    this->setWantsKeyboardFocus(true);
    this->setFocusContainer(true);

    if (const bool quickStartMode = App::Workspace().isInitialized())
    {
        this->init();
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

void MainLayout::init()
{
    if (HelioTheme *ht = dynamic_cast<HelioTheme *>(&this->getLookAndFeel()))
    {
        ht->updateBackgroundRenders();
    }
    
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
    App::Workspace().activateSubItemWithId(Config::get(Serialization::UI::lastShownPageId));
}

void MainLayout::toggleShowHideConsole()
{
    // TODO
}

int MainLayout::getScrollerHeight()
{
    if (App::isRunningOnPhone())
    {
        return TRACK_SCROLLER_HEIGHT_PHONE;
    }
    
    return TRACK_SCROLLER_HEIGHT_DEFAULT;
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

void MainLayout::showTransientItem(
    ScopedPointer<TransientTreeItem> newItem, TreeItem *parent)
{
    jassert(parent != nullptr);
    const auto treeRoot = App::Workspace().getTreeRoot();
    hideMarkersRecursive(treeRoot);

    // Cleanup: delete all existing transient items
    OwnedArray<TreeItem> transients;
    transients.addArray(treeRoot->findChildrenOfType<TransientTreeItem>());
    transients.clear(true);

    // Attach new item to the tree
    TreeItem *item = newItem.release();
    parent->addChildTreeItem(item);
    item->setSelected(true, true, dontSendNotification);
    item->setMarkerVisible(true);
    // TODO
    //App::Workspace().getNavigationHistory().addItemIfNeeded
    this->headline->syncWithTree(App::Workspace().getNavigationHistory(), item);
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
    
    Config::set(Serialization::UI::lastShownPageId, source->getItemIdentifierString());

    //const Component *focused = Component::getCurrentlyFocusedComponent();
    //Logger::outputDebugString(focused ? focused->getName() : "null");
}


//===----------------------------------------------------------------------===//
// UI
//===----------------------------------------------------------------------===//

void MainLayout::setStatus(const String &text)
{

}

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

void MainLayout::showModalNonOwnedDialog(Component *targetComponent)
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

void MainLayout::showBlockingNonModalDialog(Component *targetComponent)
{
    class Blocker : public Component
    {
    public:
        
        Blocker()
        { 
            this->setInterceptsMouseClicks(true, true);
            this->setMouseClickGrabsKeyboardFocus(false);
            this->setPaintingIsUnclipped(true);
        }
        
        void paint(Graphics &g) override
        {
            g.setColour(Colours::white.withAlpha(0.05f));
            g.fillRect(this->getLocalBounds());
        }
        
        void parentHierarchyChanged() override
        { this->rebound(); }
        
        void parentSizeChanged() override
        { this->rebound(); }
        
    private:
        
        void rebound()
        {
            if (! this->getParentComponent())
            {
                delete this;
                return;
            }

            this->setBounds(this->getParentComponent()->getLocalBounds());
        }
    };
    
    auto blocker = new Blocker();
    this->addChildComponent(blocker);
    this->addChildComponent(targetComponent);
    
    const int fadeInTime = 250;
    Desktop::getInstance().getAnimator().animateComponent(blocker, blocker->getBounds(), 1.f, fadeInTime, false, 0.0, 0.0);
    Desktop::getInstance().getAnimator().animateComponent(targetComponent, targetComponent->getBounds(), 1.f, fadeInTime, false, 0.0, 0.0);
    
    targetComponent->setAlwaysOnTop(true);
    targetComponent->toFront(false);
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
            auto scheme = ColourSchemeManager::getInstance().getCurrentScheme();
            ht->updateBackgroundRenders(true);
            ht->initColours(scheme);
            this->repaint();

            scheme.exportColourChanges();
            const ScopedPointer<XmlElement> xml(scheme.serialize());
            const String xmlString(xml->createDocument("", true, false, "UTF-8", 512));
            SystemClipboard::copyTextToClipboard(xmlString);
            return true;
        }
    }

#endif
    
    return false;
}

bool MainLayout::keyStateChanged(bool isKeyDown)
{
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
        this->toggleShowHideConsole();
        break;
    default:
        break;
    }
}