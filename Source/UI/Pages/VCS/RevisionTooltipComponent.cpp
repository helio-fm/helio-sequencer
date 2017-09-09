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

RevisionTooltipComponent::RevisionTooltipComponent(VersionControl &owner, const VCS::Revision target)
    : vcs(owner),
      revision(target)
{
    addAndMakeVisible (background = new PanelBackgroundC());
    addAndMakeVisible (panel = new PanelA());
    addAndMakeVisible (changesList = new ListBox ("", this));

    addAndMakeVisible (checkoutRevisionButton = new TextButton (String()));
    checkoutRevisionButton->setButtonText (TRANS("vcs::history::checkout"));
    checkoutRevisionButton->setConnectedEdges (Button::ConnectedOnTop);
    checkoutRevisionButton->addListener (this);

    addAndMakeVisible (shadow = new ShadowDownwards());

    //[UserPreSize]

    //this->revisionDescription->setText(this->revision.getProperty(Serialization::VCS::commitMessage), dontSendNotification);
    //this->revisionId->setText(this->revision.getProperty(Serialization::VCS::commitId), dontSendNotification);

    //this->revisionId->setInterceptsMouseClicks(false, false);
    //this->revisionDescription->setInterceptsMouseClicks(false, false);


    for (int i = 0; i < this->revision.getNumProperties(); ++i)
    {
        Identifier id = this->revision.getPropertyName(i);
        const var property = this->revision.getProperty(id);

        if (VCS::RevisionItem *item = dynamic_cast<VCS::RevisionItem *>(property.getObject()))
        {
            this->revisionItemsOnly.setProperty(id, property, nullptr);
        }
    }

    this->changesList->setMultipleSelectionEnabled(true);
    this->changesList->setColour(ListBox::backgroundColourId, Colours::transparentBlack);
    this->changesList->setRowHeight(REVISION_TOOLTIP_ROW_HEIGHT);

    //[/UserPreSize]

    setSize (320, 240);

    //[Constructor]
    const int topBottomMargins = 86;
    const int maxHeight = int(REVISION_TOOLTIP_ROW_HEIGHT * REVISION_TOOLTIP_ROWS_ONSCREEN);

    //const int numItems = this->getNumRows();
    //const int listHeight = (numItems * REVISION_TOOLTIP_ROW_HEIGHT);
    //const int newHeight = jmin(maxHeight, listHeight) + topBottomMargins;
    const int newHeight = maxHeight + topBottomMargins;

    this->setSize(this->getWidth(), newHeight);
    this->changesList->updateContent();
    //[/Constructor]
}

RevisionTooltipComponent::~RevisionTooltipComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    background = nullptr;
    panel = nullptr;
    changesList = nullptr;
    checkoutRevisionButton = nullptr;
    shadow = nullptr;

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

    background->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    panel->setBounds (0, 0, getWidth() - 0, getHeight() - 60);
    changesList->setBounds (0, 0, getWidth() - 0, getHeight() - 60);
    checkoutRevisionButton->setBounds (0 + 5, 0 + (getHeight() - 60), getWidth() - 10, 55);
    shadow->setBounds (0 + 5, 0 + (getHeight() - 60) - 3, getWidth() - 10, 26);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void RevisionTooltipComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == checkoutRevisionButton)
    {
        //[UserButtonCode_checkoutRevisionButton] -- add your button handler code here..
        this->vcs.checkout(this->revision);
        this->hide();
        //[/UserButtonCode_checkoutRevisionButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
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
    this->getParentComponent()->exitModalState(0); // calloutbox?
    //Desktop::getInstance().getAnimator().fadeOut(this, 200);
    //delete this;
}


//===----------------------------------------------------------------------===//
// ListBoxModel
//

Component *RevisionTooltipComponent::refreshComponentForRow(int rowNumber,
        bool isRowSelected, Component *existingComponentToUpdate)
{
    // juce out-of-range fix
    const int numProps = this->getNumRows();
    const bool isLastRow = (rowNumber == (numProps - 1));

    if (rowNumber >= numProps) { return existingComponentToUpdate; }

    const Identifier id = this->revisionItemsOnly.getPropertyName(rowNumber);
    const var property = this->revisionItemsOnly.getProperty(id);

    if (VCS::RevisionItem *revRecord = dynamic_cast<VCS::RevisionItem *>(property.getObject()))
    {
        if (existingComponentToUpdate != nullptr)
        {
            if (RevisionItemComponent *row = dynamic_cast<RevisionItemComponent *>(existingComponentToUpdate))
            {
                row->updateItemInfo(rowNumber, isLastRow, revRecord);
                return existingComponentToUpdate;
            }
        }
        else
        {
            auto row = new RevisionItemComponent(*this->changesList, this->vcs.getHead());
            row->updateItemInfo(rowNumber, isLastRow, revRecord);
            return row;
        }
    }

    return nullptr;
}

void RevisionTooltipComponent::listBoxItemClicked(int row, const MouseEvent &e)
{
}

void RevisionTooltipComponent::listBoxItemDoubleClicked(int row, const MouseEvent &e)
{
}

int RevisionTooltipComponent::getNumRows()
{
    const int numProps = this->revisionItemsOnly.getNumProperties();
    return numProps;
}

void RevisionTooltipComponent::paintListBoxItem(int rowNumber, Graphics &g,
                                      int width, int height, bool rowIsSelected)
{
}


//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RevisionTooltipComponent"
                 template="../../Template" componentName="" parentClasses="public Component, public ListBoxModel"
                 constructorParams="VersionControl &amp;owner, const VCS::Revision target"
                 variableInitialisers="vcs(owner),&#10;revision(target)" snapPixels="8"
                 snapActive="1" snapShown="1" overlayOpacity="0.330" fixedSize="1"
                 initialWidth="320" initialHeight="240">
  <METHODS>
    <METHOD name="inputAttemptWhenModal()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="e29f8df9016fa10d" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundC.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="c5736d336280caba" memberName="panel" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 60M" sourceFile="../Themes/PanelA.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="d017e5395434bb4f" memberName="changesList" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 60M" class="ListBox" params="&quot;&quot;, this"/>
  <TEXTBUTTON name="" id="d22a0ea951756643" memberName="checkoutRevisionButton"
              virtualName="" explicitFocusOrder="0" pos="5 0R 10M 55" posRelativeX="c5736d336280caba"
              posRelativeY="c5736d336280caba" buttonText="vcs::history::checkout"
              connectedEdges="4" needsCallback="1" radioGroupId="0"/>
  <JUCERCOMP name="" id="34270fb50cf926d8" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="5 3R 10M 26" posRelativeX="c5736d336280caba"
             posRelativeY="c5736d336280caba" sourceFile="../Themes/ShadowDownwards.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
