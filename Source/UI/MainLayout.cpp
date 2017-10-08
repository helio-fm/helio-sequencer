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
#include "NavigationSidebar.h"
#include "InitScreen.h"
#include "ToolsSidebar.h"
#include "ColourSchemeManager.h"


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
    this->setVisible(false);
    
    this->tooltipContainer = new TooltipContainer();
    this->addChildComponent(this->tooltipContainer);

    this->headline = new Headline();
    this->addAndMakeVisible(this->headline);

    this->navSidebar = new NavigationSidebar();
    
    this->addAndMakeVisible(this->navSidebar);
    
    this->sidebarBorder = new ResizableEdgeComponent(this->navSidebar,
        nullptr, ResizableEdgeComponent::rightEdge);
    this->addAndMakeVisible(this->sidebarBorder);
    this->sidebarBorder->setInterceptsMouseClicks(false, false);
    this->sidebarBorder->toFront(false);

    this->setWantsKeyboardFocus(false);
    this->setFocusContainer(false);

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
    this->sidebarBorder = nullptr;
    this->navSidebar = nullptr;
    this->headline = nullptr;
}

void MainLayout::init()
{
    if (HelioTheme *ht = dynamic_cast<HelioTheme *>(&this->getLookAndFeel()))
    {
        ht->updateBackgroundRenders();
    }
    
    this->navSidebar->setSize(NAVIGATION_SIDEBAR_WIDTH, this->getParentHeight());
    this->navSidebar->setAudioMonitor(App::Workspace().getAudioCore().getMonitor());
    
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

void MainLayout::hideConsole()
{
    // todo
}

void MainLayout::showConsole(bool alsoShowLog)
{
    // todo
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
// Pages stack
//===----------------------------------------------------------------------===//

LastShownTreeItems &MainLayout::getLastShownItems()
{
    return this->lastShownItems;
}

WeakReference<TreeItem> MainLayout::getActiveTreeItem() const
{
    return this->lastShownItems.getCurrentItem();
}

void MainLayout::showPrevPageIfAny()
{
    TreeItem *treeItem = this->lastShownItems.goBack();

    if (treeItem != nullptr)
    {
        this->lastShownItems.setLocked(true);
        treeItem->showPage();
        this->lastShownItems.setLocked(false);
    }
}

void MainLayout::showNextPageIfAny()
{
    TreeItem *treeItem = this->lastShownItems.goForward();
    
    if (treeItem != nullptr)
    {
        this->lastShownItems.setLocked(true);
        treeItem->showPage();
        this->lastShownItems.setLocked(false);
    }
}

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
    jassert(source != nullptr);
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
    this->navSidebar->repaint();
    this->headline->syncWithTree(item);
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
        this->navSidebar->repaint();
        this->headline->syncWithTree(source);
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
    
    this->lastShownItems.addItemIfNeeded(source);
    this->currentContent = page;    

    this->addAndMakeVisible(this->currentContent);
    this->resized();

    this->currentContent->setExplicitFocusOrder(1);
    this->navSidebar->setExplicitFocusOrder(10);

#if HAS_FADING_PAGECHANGE
    {
        this->currentContent->toFront(true);

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

    this->currentContent->toFront(true);
    this->currentContent->grabKeyboardFocus();
    
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

    const int fadeInTime = 250;
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
        { this->setInterceptsMouseClicks(true, true); }
        
        void paint(Graphics &g) override
        { g.fillAll(Colours::white.withAlpha(0.05f)); }
        
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

Rectangle<int> MainLayout::getPageBounds() const
{
    Rectangle<int> r(this->getLocalBounds());
    r.removeFromLeft(this->navSidebar->getWidth());
    r.removeFromRight(TOOLS_SIDEBAR_WIDTH); // a hack!
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
    this->navSidebar->setBounds(r.removeFromLeft(this->navSidebar->getWidth()));
    this->sidebarBorder->setBounds(this->navSidebar->getBounds().
        withWidth(2).translated(this->navSidebar->getWidth() - 1, 0));

    if (this->currentContent)
    {
        this->currentContent->setBounds(r);
    }
}

void MainLayout::lookAndFeelChanged()
{
    this->repaint();
}

void MainLayout::childBoundsChanged(Component *child)
{
    if (child == this->navSidebar)
    {
        this->resized();
    }
}


bool MainLayout::keyPressed(const KeyPress &key)
{
    // under Android, it sends here the ';' key, when you type a capital letter or a char like @, !, ?
    // a piece of juce.

#if HELIO_MOBILE
    return false;
#endif

    //Logger::writeToLog("MainLayout::keyPressed " + key.getTextDescription());
    
#if HELIO_DESKTOP

    if (key == KeyPress::createFromDescription("command + ctrl + cursor left"))
    {
        this->showPrevPageIfAny();
        return true;
    }
    if (key == KeyPress::createFromDescription("command + ctrl + cursor right"))
    {
        this->showNextPageIfAny();
        return true;
    }
    else if (key == KeyPress::createFromDescription("F2"))
    {
        if (MidiTrackTreeItem *primaryItem = dynamic_cast<MidiTrackTreeItem *>(this->getActiveTreeItem().get()))
        {
            // TODO rename track
        }
    }
    else if (key == KeyPress::createFromDescription("Tab") &&
             ! key.getModifiers().isAnyModifierKeyDown())
    {
        // TODO switch between pattern roll and piano roll

        //Array <Component*> comps;
        //KeyboardFocusDebugger::findAllFocusableComponents(this, comps);

        //for (const auto &comp : comps)
        //{
        //    Logger::writeToLog(comp->getName());
        //}
    }
    
#if JUCE_ENABLE_LIVE_CONSTANT_EDITOR

    else if (key == KeyPress::createFromDescription("command + r"))
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
#endif

    if (this->currentContent)
    {
        this->currentContent->toFront(true);
    }

    return false;
}
