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

#include "InstrumentsListComponent.h"

//[MiscUserDefs]
#include "OrchestraPitNode.h"
#include "OrchestraPitPage.h"
#include "InstrumentMenu.h"
#include "Instrument.h"
#include "MainLayout.h"
#include "Icons.h"
//[/MiscUserDefs]

InstrumentsListComponent::InstrumentsListComponent(PluginScanner &pluginScanner, OrchestraPitNode &instrumentsRoot)
    : pluginScanner(pluginScanner),
      instrumentsRoot(instrumentsRoot)
{
    this->instrumentsList.reset(new ListBox("Instruments", this));
    this->addAndMakeVisible(instrumentsList.get());

    this->titleLabel.reset(new Label(String(),
                                      TRANS("page::orchestra::instruments")));
    this->addAndMakeVisible(titleLabel.get());
    this->titleLabel->setFont(Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType(Justification::centred);
    titleLabel->setEditable(false, false, false);

    this->separator1.reset(new SeparatorHorizontalFadingReversed());
    this->addAndMakeVisible(separator1.get());

    //[UserPreSize]
    this->instrumentsList->setMultipleSelectionEnabled(false);
    this->instrumentsList->setRowHeight(INSTRUMENTSLIST_ROW_HEIGHT);
    //[/UserPreSize]

    this->setSize(600, 400);

    //[Constructor]
    //[/Constructor]
}

InstrumentsListComponent::~InstrumentsListComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    instrumentsList = nullptr;
    titleLabel = nullptr;
    separator1 = nullptr;

    //[Destructor]
    //[/Destructor]
}

void InstrumentsListComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void InstrumentsListComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    instrumentsList->setBounds(1, 42, getWidth() - 2, getHeight() - 43);
    titleLabel->setBounds(0, 0, getWidth() - 0, 26);
    separator1->setBounds((getWidth() / 2) - ((getWidth() - 0) / 2), 40, getWidth() - 0, 3);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void InstrumentsListComponent::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    if (this->getParentComponent() != nullptr)
    {
        this->updateListContent();
    }
    //[/UserCode_parentHierarchyChanged]
}


//[MiscUserCode]

void InstrumentsListComponent::clearSelection()
{
    this->instrumentsList->setSelectedRows({}, dontSendNotification);
}

void InstrumentsListComponent::updateListContent()
{
    this->instrumentIcon = Icons::findByName(Icons::instrument, int(INSTRUMENTSLIST_ROW_HEIGHT * 0.75f));
    this->instruments = this->instrumentsRoot.findChildrenRefsOfType<InstrumentNode>();
    this->instrumentsList->updateContent();
    this->clearSelection();
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

int InstrumentsListComponent::getNumRows()
{
    return this->instruments.size();
}

void InstrumentsListComponent::paintListBoxItem(int rowNumber, Graphics &g, int w, int h, bool rowIsSelected)
{
    const auto instrumentNode = this->instruments[rowNumber];
    if (instrumentNode == nullptr) { return; }

    if (rowIsSelected)
    {
        g.fillAll(Colours::white.withAlpha(0.075f));
    }
    else if (rowNumber % 2)
    {
        g.fillAll(Colours::black.withAlpha(0.05f));
    }

    const auto instrument = instrumentNode->getInstrument();
    if (instrument == nullptr) { return; }

    g.setFont(h * 0.4f);
    const int margin = h / 12;

    g.setColour(findDefaultColour(ListBox::textColourId));
    g.drawText(instrumentNode->getName(), (margin * 2) + this->instrumentIcon.getWidth(), margin,
        w, h - (margin * 2), Justification::centredLeft, false);

    const auto placement = RectanglePlacement::yMid | RectanglePlacement::xLeft | RectanglePlacement::doNotResize;
    g.drawImageWithin(this->instrumentIcon, margin, 0, w, h, placement);
}

// Desktop:
// On click - shows menu, if there's anything selected
// On double click - shows instrument page
// Mobile:
// On click - shows instrument page

// Using selectedRowsChanged instead of listBoxItemClicked
// to handle only newly selected rows
void InstrumentsListComponent::selectedRowsChanged(int lastRowSelected)
{
#if HELIO_DESKTOP
    if (this->instrumentsList->getNumSelectedRows() > 0)
    {
        // Hide existing because selection will be always different:
        App::Layout().hideSelectionMenu();
        App::Layout().showSelectionMenu(this);
    }
    else
    {
        App::Layout().hideSelectionMenu();
    }

    if (auto *parent = dynamic_cast<OrchestraPitPage *>(this->getParentComponent()))
    {
        parent->onStageSelectionChanged();
    }
#elif HELIO_MOBILE
    auto instrumentNode = this->instruments[lastRowSelected];
    if (instrumentNode == nullptr) { return; }
    instrumentNode->setSelected();
#endif
}

void InstrumentsListComponent::listBoxItemDoubleClicked(int rowNumber, const MouseEvent &e)
{
#if HELIO_DESKTOP
    auto instrumentNode = this->instruments[rowNumber];
    if (instrumentNode == nullptr) { return; }
    instrumentNode->setSelected();
#endif
}

//===----------------------------------------------------------------------===//
// HeadlineItemDataSource
//===----------------------------------------------------------------------===//

bool InstrumentsListComponent::hasMenu() const noexcept { return true; }
bool InstrumentsListComponent::canBeSelectedAsMenuItem() const { return false; }

ScopedPointer<Component> InstrumentsListComponent::createMenu()
{
    const auto selectedRow = this->instrumentsList->getSelectedRow();
    const auto instrument = this->instruments[selectedRow];
    jassert(instrument);
    return { new InstrumentMenu(*instrument, this->pluginScanner) };
}

Image InstrumentsListComponent::getIcon() const
{
    return Icons::findByName(Icons::instrument, HEADLINE_ICON_SIZE);
}

String InstrumentsListComponent::getName() const
{
    const auto selectedRow = this->instrumentsList->getSelectedRow();
    const auto instrument = this->instruments[selectedRow];
    jassert(instrument);
    return instrument->getName();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="InstrumentsListComponent"
                 template="../../../Template" componentName="" parentClasses="public Component, public ListBoxModel, public HeadlineItemDataSource"
                 constructorParams="PluginScanner &amp;pluginScanner, OrchestraPitNode &amp;instrumentsRoot"
                 variableInitialisers="pluginScanner(pluginScanner),&#10;instrumentsRoot(instrumentsRoot)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="1b089ba42e39d447" memberName="instrumentsList" virtualName=""
                    explicitFocusOrder="0" pos="1 42 2M 43M" class="ListBox" params="&quot;Instruments&quot;, this"/>
  <LABEL name="" id="660583b19bbfaa6b" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="0 0 0M 26" labelText="page::orchestra::instruments"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="21.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="36"/>
  <JUCERCOMP name="" id="a09914d60dab2768" memberName="separator1" virtualName=""
             explicitFocusOrder="0" pos="0Cc 40 0M 3" sourceFile="../../Themes/SeparatorHorizontalFadingReversed.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
