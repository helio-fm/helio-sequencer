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

#include "Headline.h"

//[MiscUserDefs]
#include "HeadlineItem.h"
#include "IconButton.h"
#include "HelioTheme.h"
#include "Icons.h"
#include "ColourIDs.h"
#include "SequencerLayout.h"
#include "MainLayout.h"

//[/MiscUserDefs]

Headline::Headline()
{
    this->navPanel.reset(new HeadlineNavigationPanel());
    this->addAndMakeVisible(navPanel.get());
    this->consoleButton.reset(new IconButton(Icons::findByName(Icons::console, 20), CommandIDs::CommandPalette));
    this->addAndMakeVisible(consoleButton.get());


    //[UserPreSize]
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);
    this->setOpaque(App::isUsingNativeTitleBar());
    this->consoleButton->setVisible(App::isUsingNativeTitleBar());
    //[/UserPreSize]

    this->setSize(600, 34);

    //[Constructor]
    //[/Constructor]
}

Headline::~Headline()
{
    //[Destructor_pre]
    this->chain.clearQuick(true);
    //[/Destructor_pre]

    navPanel = nullptr;
    consoleButton = nullptr;

    //[Destructor]
    //[/Destructor]
}

void Headline::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    if (App::isUsingNativeTitleBar())
    {
        const auto &theme = HelioTheme::getCurrentTheme();
        g.setFillType({ theme.getBgCacheA(), {} });
        g.fillRect(this->getLocalBounds());

        g.setColour(findDefaultColour(ColourIDs::Common::borderLineLight));
        g.fillRect(0, this->getHeight() - 2, this->getWidth(), 1);
        g.setColour(findDefaultColour(ColourIDs::Common::borderLineDark));
        g.fillRect(0, this->getHeight() - 1, this->getWidth(), 1);
    }
    //[/UserPaint]
}

void Headline::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    navPanel->setBounds(0, 0, 60, getHeight() - 0);
    consoleButton->setBounds(getWidth() - 45, (getHeight() / 2) - ((getHeight() - 2) / 2), 45, getHeight() - 2);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void Headline::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    App::Layout().broadcastCommandMessage(commandId);
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]

void Headline::handleAsyncUpdate()
{
    int posX = Headline::itemsOverlapOffset + Headline::rootNodeOffset;
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
                this->animator.cancelAnimation(this->selectionItem.get(), false);
                this->selectionItem->setTopLeftPosition(Headline::rootNodeOffset, this->selectionItem->getY());
            }

            break;
        }

        previousItem = static_cast<TreeNode *>(treeItem);

        const auto boundsBefore = child->getBounds();
        child->updateContent();
        const auto boundsAfter = child->getBounds().withX(posX - Headline::itemsOverlapOffset);
        this->animator.cancelAnimation(child, false);
        if (boundsBefore != boundsAfter)
        {
            child->setBounds(boundsBefore);
            this->animator.animateComponent(child, boundsAfter, 1.f, 250, false, 1.f, 0.f);
        }

        posX += boundsAfter.getWidth() - Headline::itemsOverlapOffset;
    }

    if (hasSelectionItem)
    {
        const auto finalPos = this->selectionItem->getBounds().withX(posX - Headline::itemsOverlapOffset);
        this->animator.cancelAnimation(this->selectionItem.get(), false);
        this->animator.animateComponent(this->selectionItem.get(), finalPos, 1.f, 250, false, 1.f, 0.f);
        this->selectionItem->toBack();
        this->navPanel->toFront(false);
    }
}

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
    for (int i = items.size(); i --> 0; )
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
        this->animator.animateComponent(child, finalPos, startingAlpha, 100, true, 0.f, 1.f);
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
        this->animator.animateComponent(child, finalPos, 1.f, 150, false, 1.f, 0.f);
    }

    this->navPanel->toFront(false);

    return lastPosX;
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
    this->animator.animateComponent(this->selectionItem.get(), finalPos, 1.f, 150, false, 1.f, 0.f);

    this->navPanel->toFront(false);
}

void Headline::hideSelectionMenu()
{
    if (this->selectionItem != nullptr)
    {
        // TODO test animations for all-chain updates
        //const auto w = this->selectionItem->getBounds().getWidth();
        //const auto finalPos = this->selectionItem->getBounds().translated(-w, 0);
        const auto finalPos = this->selectionItem->getBounds().withX(Headline::rootNodeOffset);
        this->animator.cancelAnimation(this->selectionItem.get(), false);
        this->animator.animateComponent(this->selectionItem.get(), finalPos, this->getAlphaForAnimation(), 150, true, 0.f, 1.f);
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

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Headline" template="../../Template"
                 componentName="" parentClasses="public Component, public AsyncUpdater"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="600"
                 initialHeight="34">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="666c39451424e53c" memberName="navPanel" virtualName=""
             explicitFocusOrder="0" pos="0 0 60 0M" sourceFile="HeadlineNavigationPanel.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="292b9635259e430" memberName="consoleButton" virtualName=""
                    explicitFocusOrder="0" pos="0Rr 0Cc 45 2M" class="IconButton"
                    params="Icons::findByName(Icons::console, 20), CommandIDs::CommandPalette"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



