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
#include "Headline.h"
#include "HeadlineItem.h"
#include "HelioTheme.h"
#include "MainLayout.h"

Headline::Headline()
{
    this->setOpaque(true);
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, true);
    this->setAccessible(false);
    this->setOpaque(App::isUsingNativeTitleBar());

    this->navPanel = make<HeadlineNavigationPanel>();
    this->addAndMakeVisible(this->navPanel.get());

    this->consoleButton = make<IconButton>(Icons::findByName(Icons::console, 20), CommandIDs::CommandPalette);
    this->addChildComponent(this->consoleButton.get()); // invisible by default, e.g. on mobile platforms

#if PLATFORM_DESKTOP
    this->consoleButton->setVisible(App::isUsingNativeTitleBar());
#endif

    App::Config().getUiFlags()->addListener(this);
    this->onSidebarWidthChanged(App::Config().getUiFlags()->getLeftSidebarWidth(), 0);

    this->setSize(100, Globals::UI::headlineHeight);
}

Headline::~Headline()
{
    App::Config().getUiFlags()->removeListener(this);
    this->chain.clearQuick(true);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void Headline::paint(Graphics &g)
{
    if (!App::isUsingNativeTitleBar())
    {
        return;
    }

    const auto &theme = HelioTheme::getCurrentTheme();
    g.setFillType({ theme.getHeadlineBackground(), {} });
    g.fillRect(this->getLocalBounds());

    g.setColour(this->borderLightColour);
    g.fillRect(0, this->getHeight() - 2, this->getWidth(), 1);
    g.setColour(this->borderDarkColour);
    g.fillRect(0, this->getHeight() - 1, this->getWidth(), 1);
}

void Headline::resized()
{
    constexpr auto consoleButtonSize = 45;
    constexpr auto navPanelSize =
        Globals::UI::sidebarWidth + Headline::itemsOverlapOffset;

    this->navPanel->setBounds(this->sidebarOffset, 0, navPanelSize, this->getHeight());

    this->consoleButton->setBounds(this->getWidth() - consoleButtonSize, 1,
        consoleButtonSize, this->getHeight() - 2);
}

void Headline::handleCommandMessage(int commandId)
{
    App::Layout().broadcastCommandMessage(commandId);
}

void Headline::onSidebarWidthChanged(int leftWidth, int rightWidth)
{
    this->sidebarOffset = leftWidth - Globals::UI::sidebarWidth;
    this->resized();
    this->triggerAsyncUpdate();
}

void Headline::handleAsyncUpdate()
{
    int posX = this->sidebarOffset + Headline::itemsOverlapOffset + Headline::rootNodeOffset;
    TreeNode *previousItem = nullptr;

    const bool hasSelectionItem = this->selectionItem != nullptr &&
        !this->selectionItem->getDataSource().wasObjectDeleted();

    for (int i = 0; i < this->chain.size(); ++i)
    {
        HeadlineItem *child = this->chain.getUnchecked(i);

        TreeNode *treeItem = static_cast<TreeNode *>(child->getDataSource().get());
        if (treeItem == nullptr || (previousItem != nullptr && treeItem->getParent() != previousItem))
        {
            // An item inserted or removed, need to re-sync the whole chain:
            TreeNode *lastItem = static_cast<TreeNode *>(this->chain.getLast()->getDataSource().get());
            if (lastItem == nullptr)
            {
                jassertfalse;
                break;
            }

            posX = this->rebuildChain(lastItem);
            if (hasSelectionItem)
            {
                this->animator.cancelAnimation(this->selectionItem.get(), true);
                this->selectionItem->setTopLeftPosition(Headline::rootNodeOffset, this->selectionItem->getY());
            }

            break;
        }

        previousItem = static_cast<TreeNode *>(treeItem);

        this->animator.cancelAnimation(child, true);

        child->updateContent();
        const auto boundsAfter = child->getBounds().withX(posX - Headline::itemsOverlapOffset);
        child->setBounds(boundsAfter);

        posX += boundsAfter.getWidth() - Headline::itemsOverlapOffset;
    }

    if (hasSelectionItem)
    {
        const auto finalPos = this->selectionItem->getBounds().withX(posX - Headline::itemsOverlapOffset);
        this->animator.cancelAnimation(this->selectionItem.get(), false);
        this->animator.animateComponent(this->selectionItem.get(), finalPos, 1.f, Globals::UI::fadeInLong, false, 1.0, 0.0);
        this->selectionItem->toBack();
        this->navPanel->toFront(false);
    }
}

//===----------------------------------------------------------------------===//
// Private
//===----------------------------------------------------------------------===//

Array<TreeNode *> createSortedBranchArray(WeakReference<TreeNode> leaf)
{
    Array<TreeNode *> items;
    TreeNode *item = leaf;
    items.add(item);
    while (item->getParent() != nullptr)
    {
        item = static_cast<TreeNode *>(item->getParent());
        items.add(item);
    }

    Array<TreeNode *> result;
    for (int i = items.size(); i --> 0 ;)
    {
        result.add(items[i]);
    }

    return result;
}

void Headline::syncWithTree(NavigationHistory &navHistory, WeakReference<TreeNode> leaf)
{
    // Removes selection menu item, if any
    this->hideSelectionMenu();
    this->rebuildChain(leaf);
    this->navPanel->updateState(navHistory.canGoBackward(), navHistory.canGoForward());
}

int Headline::rebuildChain(WeakReference<TreeNode> leaf)
{
    const float startingAlpha = this->getAlphaForAnimation();
    Array<TreeNode *> branch = createSortedBranchArray(leaf);

    // Finds the first inconsistency point in the chain
    int firstInvalidUnitIndex = 0;
    int fadePositionX = Headline::itemsOverlapOffset + Headline::rootNodeOffset;
    for (; firstInvalidUnitIndex < this->chain.size(); firstInvalidUnitIndex++)
    {
        if (this->chain[firstInvalidUnitIndex]->getDataSource().wasObjectDeleted() ||
            this->chain[firstInvalidUnitIndex]->getDataSource() != branch[firstInvalidUnitIndex])
        {
            break;
        }

        fadePositionX += (this->chain[firstInvalidUnitIndex]->getWidth() - Headline::itemsOverlapOffset);
    }

    // Removes the rest of the chain
    for (int i = firstInvalidUnitIndex; i < this->chain.size();)
    {
        const auto child = this->chain[i];
        const auto finalPos = child->getBounds().withX(fadePositionX - child->getWidth());
        this->animator.cancelAnimation(child, false);
        this->animator.animateComponent(child, finalPos, startingAlpha, Globals::UI::fadeOutShort, true, 0.0, 1.0);
        this->chain.remove(i, true);
    }

    // Adds the new elements
    int lastPosX = fadePositionX;
    for (int i = firstInvalidUnitIndex; i < branch.size(); i++)
    {
        auto *child = new HeadlineItem(branch[i], *this);
        child->updateContent();
        this->chain.add(child);
        this->addAndMakeVisible(child);
        child->setTopLeftPosition(fadePositionX - child->getWidth(), 0);
        child->setAlpha(startingAlpha);
        child->toBack();
        const auto finalPos = child->getBounds().withX(lastPosX - Headline::itemsOverlapOffset);
        lastPosX += child->getWidth() - Headline::itemsOverlapOffset;
        this->animator.animateComponent(child, finalPos, 1.f, Globals::UI::fadeInLong, false, 1.0, 0.0);
    }

    this->navPanel->toFront(false);

    return lastPosX;
}

void Headline::showNeighbourMenu(WeakReference<HeadlineItemDataSource> origin, int delta)
{
    jassert(delta != 0);

    Array<HeadlineItem *> fullChain;
    for (auto *item : this->chain)
    {
        fullChain.add(item);
    }
    if (this->selectionItem != nullptr)
    {
        fullChain.add(this->selectionItem.get());
    }

    for (int i = 0; i < fullChain.size(); ++i)
    {
        if (fullChain.getUnchecked(i)->getDataSource() == origin)
        {
            int newIndex = jlimit(0, fullChain.size() - 1, i + delta);
            if (!fullChain.getUnchecked(newIndex)->hasMenu())
            {
                newIndex = jlimit(0, fullChain.size() - 1, newIndex + delta);
            }
            //jassert(fullChain.getUnchecked(newIndex)->hasMenu());
            if (newIndex != i)
            {
                fullChain.getUnchecked(newIndex)->showMenuIfAny(true);
            }
            return;
        }
    }
}

void Headline::showSelectionMenu(WeakReference<HeadlineItemDataSource> menuSource)
{
    if (this->chain.size() == 0)
    {
        return;
    }

    const bool upToDate = (this->selectionItem != nullptr &&
        !this->selectionItem->getDataSource().wasObjectDeleted() &&
        this->selectionItem->getDataSource() == menuSource);

    if (upToDate)
    {
        return;
    }

    this->hideSelectionMenu();

    const auto x = this->getChainWidth() + Headline::rootNodeOffset;
    this->selectionItem = make<HeadlineItem>(menuSource, *this);
    this->selectionItem->updateContent();
    this->addAndMakeVisible(this->selectionItem.get());
    this->selectionItem->setTopLeftPosition(x - this->selectionItem->getWidth(), 0);
    this->selectionItem->setAlpha(this->getAlphaForAnimation());
    this->selectionItem->toBack();
    const auto finalPos = this->selectionItem->getBounds().withX(x);
    this->animator.animateComponent(this->selectionItem.get(), finalPos,
        1.f, Globals::UI::fadeInLong, false, 1.0, 0.0);

    this->navPanel->toFront(false);
}

void Headline::hideSelectionMenu()
{
    if (this->selectionItem != nullptr)
    {
        //const auto w = this->selectionItem->getBounds().getWidth();
        //const auto finalPos = this->selectionItem->getBounds().translated(-w, 0);
        const auto finalPos = this->selectionItem->getBounds().withX(Headline::rootNodeOffset);
        this->animator.cancelAnimation(this->selectionItem.get(), false);
        this->animator.animateComponent(this->selectionItem.get(), finalPos,
            this->getAlphaForAnimation(), Globals::UI::fadeOutShort, true, 0.0, 1.0);
        this->selectionItem = nullptr;
    }
}

float Headline::getAlphaForAnimation() const noexcept
{
    return App::isOpenGLRendererEnabled() ? 0.f : 1.f;
}

int Headline::getChainWidth() const noexcept
{
    int w = 0;
    for (const auto &child : this->chain)
    {
        w += child->getWidth() - Headline::itemsOverlapOffset;
    }
    return w;
}

HeadlineItem *Headline::getTailItem() const noexcept
{
    return (this->selectionItem != nullptr) ?
        this->selectionItem.get() : this->chain.getLast();
}
