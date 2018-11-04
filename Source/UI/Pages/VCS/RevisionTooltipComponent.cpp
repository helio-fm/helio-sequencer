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

#include "RevisionTooltipComponent.h"

//[MiscUserDefs]
#include "RevisionItemComponent.h"
#include "RevisionItem.h"
#include "Delta.h"

#if HELIO_DESKTOP
#    define REVISION_TOOLTIP_ROWS_ONSCREEN (4.5)
#    define REVISION_TOOLTIP_ROW_HEIGHT (65)
#elif HELIO_MOBILE
#    define REVISION_TOOLTIP_ROWS_ONSCREEN (3.5)
#    define REVISION_TOOLTIP_ROW_HEIGHT (50)
#endif

//[/MiscUserDefs]

RevisionTooltipComponent::RevisionTooltipComponent(const VCS::Revision::Ptr revision)
    : revision(revision),
      revisionItemsOnly()
{
    this->changesList.reset(new ListBox("", this));
    this->addAndMakeVisible(changesList.get());

    this->separator.reset(new SeparatorHorizontal());
    this->addAndMakeVisible(separator.get());

    //[UserPreSize]

    this->revisionItemsOnly.addArray(this->revision->getItems());

    this->changesList->setRowSelectedOnMouseDown(false);
    this->changesList->setMultipleSelectionEnabled(false);
    this->changesList->setClickingTogglesRowSelection(false);

    this->changesList->setRowHeight(REVISION_TOOLTIP_ROW_HEIGHT);
    this->changesList->getViewport()->setScrollBarsShown(true, false);

    //[/UserPreSize]

    this->setSize(320, 220);

    //[Constructor]
    const int maxHeight = int(REVISION_TOOLTIP_ROW_HEIGHT * REVISION_TOOLTIP_ROWS_ONSCREEN);
    const int newHeight = jmin(maxHeight, this->getNumRows() * REVISION_TOOLTIP_ROW_HEIGHT);
    this->setSize(this->getWidth(), newHeight);
    this->changesList->updateContent();
    //[/Constructor]
}

RevisionTooltipComponent::~RevisionTooltipComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    changesList = nullptr;
    separator = nullptr;

    //[Destructor]
    //[/Destructor]
}

void RevisionTooltipComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void RevisionTooltipComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    changesList->setBounds(0, 0, getWidth() - 0, getHeight() - 0);
    separator->setBounds(0, 0 + (getHeight() - 0) - 1, getWidth() - 0, 4);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void RevisionTooltipComponent::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    this->hide();
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

void RevisionTooltipComponent::hide()
{
    this->getParentComponent()->exitModalState(0);
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *RevisionTooltipComponent::refreshComponentForRow(int rowNumber,
        bool isRowSelected, Component *existingComponentToUpdate)
{
    // juce out-of-range fix
    const int numProps = this->getNumRows();
    const bool isLastRow = (rowNumber == (numProps - 1));

    if (rowNumber >= numProps) { return existingComponentToUpdate; }

    const auto revRecord = this->revisionItemsOnly.getUnchecked(rowNumber);
    if (existingComponentToUpdate != nullptr)
    {
        if (auto *row = dynamic_cast<RevisionItemComponent *>(existingComponentToUpdate))
        {
            row->updateItemInfo(revRecord, rowNumber, isLastRow, false);
            return existingComponentToUpdate;
        }
    }
    else
    {
        auto *row = new RevisionItemComponent(*this->changesList);
        row->updateItemInfo(revRecord, rowNumber, isLastRow, false);
        return row;
    }

    return nullptr;
}

int RevisionTooltipComponent::getNumRows()
{
    return this->revisionItemsOnly.size();
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RevisionTooltipComponent"
                 template="../../../Template" componentName="" parentClasses="public Component, public ListBoxModel"
                 constructorParams="const VCS::Revision::Ptr revision" variableInitialisers="revision(revision),&#10;revisionItemsOnly()"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="320" initialHeight="220">
  <METHODS>
    <METHOD name="inputAttemptWhenModal()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="d017e5395434bb4f" memberName="changesList" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 0M" class="ListBox" params="&quot;&quot;, this"/>
  <JUCERCOMP name="" id="a5ee6384bbe01d79" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="0 1R 0M 4" posRelativeX="c5736d336280caba"
             posRelativeY="d017e5395434bb4f" sourceFile="../../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
