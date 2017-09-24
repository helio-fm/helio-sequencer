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
#include "IconComponent.h"
//[/MiscUserDefs]

Headline::Headline()
{
    addAndMakeVisible (bg = new PanelBackgroundB());
    addAndMakeVisible (separator = new SeparatorHorizontalReversed());

    //[UserPreSize]
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    //[/UserPreSize]

    setSize (600, 34);

    //[Constructor]
    //[/Constructor]
}

Headline::~Headline()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    bg = nullptr;
    separator = nullptr;

    //[Destructor]
    //[/Destructor]
}

void Headline::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void Headline::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    bg->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    separator->setBounds (0, getHeight() - 2, getWidth() - 0, 2);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

Array<TreeItem *> createSortedBranchArray(WeakReference<TreeItem> leaf)
{
    Array<TreeItem *> items;
    TreeItem *item = leaf;
    items.add(item);
    while (item->getParentItem() != nullptr)
    {
        item = static_cast<TreeItem *>(item->getParentItem());
        items.add(item);
    }

    Array<TreeItem *> result;
    for (int i = items.size(); i --> 0; )
    {
        result.add(items[i]);
    }

    return result;
}

#define HEADLINE_ITEMS_OVERLAP 16

void Headline::syncWithTree(WeakReference<TreeItem> leaf)
{
    Array<TreeItem *> branch = createSortedBranchArray(leaf);

    // Finds the first inconsistency point in the chain
    int firstInvalidUnitIndex = 0;
    int fadePositionX = HEADLINE_ITEMS_OVERLAP - 6;
    for (; firstInvalidUnitIndex < this->chain.size(); firstInvalidUnitIndex++)
    {
        if (this->chain[firstInvalidUnitIndex]->getTreeItem() != branch[firstInvalidUnitIndex])
        { break; }

        fadePositionX += (this->chain[firstInvalidUnitIndex]->getWidth() - HEADLINE_ITEMS_OVERLAP);
    }

    // Removes the rest of the chain
    for (int i = firstInvalidUnitIndex; i < this->chain.size();)
    {
        const auto child = this->chain[i];
        const auto finalPos = child->getBounds().withX(fadePositionX - child->getWidth());
        this->animator.animateComponent(child, finalPos, 0.f, 200, true, 0.f, 1.f);
        this->chain.remove(i, true);
    }

    // Adds the new elements
    for (int i = firstInvalidUnitIndex, lastPosX = fadePositionX; i < branch.size(); i++)
    {
        const auto child = new HeadlineItem(branch[i]);
        this->chain.add(child);
        this->addAndMakeVisible(child);
        child->setTopLeftPosition(fadePositionX - child->getWidth(), 0);
        child->setAlpha(0.f);
        child->toBack();
        const auto finalPos = child->getBounds().withX(lastPosX - HEADLINE_ITEMS_OVERLAP);
        lastPosX += child->getWidth() - HEADLINE_ITEMS_OVERLAP;
        this->animator.animateComponent(child, finalPos, 1.f, 300, false, 1.f, 0.f);
    }

    this->bg->toBack();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Headline" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="600" initialHeight="34">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="e14a947c03465d1b" memberName="bg" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="e5efefc65cac6ba7" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="0 0Rr 0M 2" sourceFile="../Themes/SeparatorHorizontalReversed.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
