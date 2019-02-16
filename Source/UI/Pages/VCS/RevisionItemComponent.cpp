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

#include "RevisionItemComponent.h"

//[MiscUserDefs]

#include "Icons.h"
#include "HelioTheme.h"
#include "ColourIDs.h"

class RevisionItemHighlighter final : public Component
{
public:

    RevisionItemHighlighter()
    {
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        g.setColour(findDefaultColour(ColourIDs::VersionControl::highlight));
        g.fillRoundedRectangle (5.0f, 2.0f, float(this->getWidth() - 10), float(this->getHeight() - 8), 2.f);
    }
};

class RevisionItemSelectionComponent final : public Component
{
public:

    RevisionItemSelectionComponent()
    {
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        const float height = float(this->getHeight());
        const auto imgOk(Icons::findByName(Icons::apply, int(height * 0.7f)));

        const auto colour1(Colours::black.withAlpha(0.07f));
        const auto colour2(Colours::black.withAlpha(0.1f));
        g.setGradientFill(ColourGradient(colour1, 0.f, 0.f, colour2, 0.f, float(this->getHeight()), false));
        g.fillRoundedRectangle (5.0f, 2.0f, float(this->getWidth() - 10), float(this->getHeight() - 8), 2.f);

        g.setOpacity(0.35f);
        const int rightTextBorder = this->getWidth() - this->getHeight() / 2 - 5;
        Icons::drawImageRetinaAware(imgOk, g, rightTextBorder, this->getHeight() / 2 - 2);
    }
};

//[/MiscUserDefs]

RevisionItemComponent::RevisionItemComponent(ListBox &parentListBox)
    : DraggingListBoxComponent(parentListBox.getViewport()),
      list(parentListBox),
      row(0)
{
    this->itemLabel.reset(new Label(String(),
                                     TRANS("...")));
    this->addAndMakeVisible(itemLabel.get());
    this->itemLabel->setFont(Font (Font::getDefaultSerifFontName(), 18.00f, Font::plain).withTypefaceStyle ("Regular"));
    itemLabel->setJustificationType(Justification::centredLeft);
    itemLabel->setEditable(false, false, false);

    this->deltasLabel.reset(new Label(String(),
                                       TRANS("...")));
    this->addAndMakeVisible(deltasLabel.get());
    this->deltasLabel->setFont(Font (Font::getDefaultSansSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    deltasLabel->setJustificationType(Justification::topLeft);
    deltasLabel->setEditable(false, false, false);

    this->separator.reset(new SeparatorHorizontalFading());
    this->addAndMakeVisible(separator.get());

    //[UserPreSize]
    this->selectionComponent = new RevisionItemSelectionComponent();
    this->addChildComponent(this->selectionComponent);

    // Gray out details a bit:
    this->deltasLabel->setColour(Label::textColourId,
        findDefaultColour(Label::textColourId).withMultipliedAlpha(0.75f));
    //[/UserPreSize]

    this->setSize(500, 70);

    //[Constructor]
    this->itemLabel->setInterceptsMouseClicks(false, false);
    this->deltasLabel->setInterceptsMouseClicks(false, false);
    //[/Constructor]
}

RevisionItemComponent::~RevisionItemComponent()
{
    //[Destructor_pre]
    this->selectionComponent = nullptr;
    //[/Destructor_pre]

    itemLabel = nullptr;
    deltasLabel = nullptr;
    separator = nullptr;

    //[Destructor]
    //[/Destructor]
}

void RevisionItemComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    DraggingListBoxComponent::paint(g);
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void RevisionItemComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    itemLabel->setBounds(5, 3, getWidth() - 10, 24);
    deltasLabel->setBounds(10, 24, getWidth() - 90, 38);
    separator->setBounds(10, getHeight() - 1 - 2, getWidth() - 20, 2);
    //[UserResized] Add your own custom resize handling here..

    if (this->isEnabled())
    {
        this->selectionComponent->setBounds(this->getLocalBounds());
    }
    else
    {
        deltasLabel->setBounds(10, 25, getWidth() - 30, getHeight() - 32);
    }

    //[/UserResized]
}


//[MiscUserCode]

void RevisionItemComponent::updateItemInfo(VCS::RevisionItem::Ptr revisionItemInfo,
    int rowNumber, bool isLastRow, bool isSelectable)
{
    this->row = rowNumber;
    this->revisionItem = revisionItemInfo;
    this->setEnabled(isSelectable);

    this->separator->setVisible(! isLastRow);

    const auto itemType = this->revisionItem->getType();
    const String itemTypeStr = this->revisionItem->getTypeAsString();
    const String itemDescription = TRANS(this->revisionItem->getVCSName());

    String itemDeltas;
    bool needsComma = false;
    for (int i = 0; i < this->revisionItem->getNumDeltas(); ++i)
    {
        const VCS::Delta *delta = this->revisionItem->getDelta(i);
        const String &description = delta->getHumanReadableText();

        if (description.isNotEmpty())
        {
            itemDeltas += needsComma ? ", " : "";
            itemDeltas = itemDeltas + description;
            needsComma = true;
        }
    }

    if (itemType == VCS::RevisionItem::Added ||
        itemType == VCS::RevisionItem::Removed)
    {
        this->itemLabel->setText(itemTypeStr + " " + itemDescription, dontSendNotification);
    }
    else
    {
        this->itemLabel->setText(itemDescription, dontSendNotification);

    }

    this->deltasLabel->setText(itemDeltas, dontSendNotification);

    if (!this->selectionAnimator.isAnimating())
    {
        this->selectionComponent->setVisible(this->isSelected());
        this->selectionComponent->setAlpha(this->isSelected() ? 1.f : 0.f);
    }

    this->resized();
}

void RevisionItemComponent::select() const
{
    if (!this->isSelected()) { this->invertSelection(); }
}

void RevisionItemComponent::deselect() const
{
    if (this->isSelected()) { this->invertSelection(); }
}

VCS::RevisionItem::Ptr RevisionItemComponent::getRevisionItem() const noexcept
{
    return this->revisionItem;
}

void RevisionItemComponent::setSelected(bool shouldBeSelected)
{
    this->invertSelection();
}

void RevisionItemComponent::invertSelection() const
{
    const bool rowWillBeSelected = !this->list.isRowSelected(this->row);

    if (this->selectionComponent->isVisible() && !rowWillBeSelected)
    {
        this->selectionAnimator.fadeOut(this->selectionComponent, 150);
    }
    else if (!this->selectionComponent->isVisible() && rowWillBeSelected)
    {
        this->selectionAnimator.fadeIn(this->selectionComponent, 75);
    }

    this->list.flipRowSelection(this->row);
}

bool RevisionItemComponent::isSelected() const
{
    return this->list.isRowSelected(this->row);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="RevisionItemComponent" template="../../../Template"
                 componentName="" parentClasses="public DraggingListBoxComponent"
                 constructorParams="ListBox &amp;parentListBox" variableInitialisers="DraggingListBoxComponent(parentListBox.getViewport()),&#10;list(parentListBox),&#10;row(0)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="500" initialHeight="70">
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="c261305e2de1ebf2" memberName="itemLabel" virtualName=""
         explicitFocusOrder="0" pos="5 3 10M 24" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="18.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <LABEL name="" id="12427a53408d61ee" memberName="deltasLabel" virtualName=""
         explicitFocusOrder="0" pos="10 24 90M 38" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="16.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="9"/>
  <JUCERCOMP name="" id="2e5e217f3d476ef8" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="10 1Rr 20M 2" sourceFile="../../Themes/SeparatorHorizontalFading.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
