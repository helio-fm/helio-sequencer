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
#include "InstrumentsListComponent.h"

#include "OrchestraPitNode.h"
#include "InstrumentMenu.h"
#include "HeadlineContextMenuController.h"
#include "Instrument.h"
#include "MainLayout.h"

InstrumentsListComponent::InstrumentsListComponent(PluginScanner &pluginScanner, OrchestraPitNode &instrumentsRoot) :
    pluginScanner(pluginScanner),
    instrumentsRoot(instrumentsRoot)
{
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);

    this->instrumentsList = make<ListBox>(String(), this);
    this->addAndMakeVisible(this->instrumentsList.get());

    this->titleLabel = make<Label>(String(),TRANS(I18n::Page::orchestraInstruments));
    this->addAndMakeVisible(this->titleLabel.get());
    this->titleLabel->setJustificationType(Justification::centred);
    this->titleLabel->setFont({ 21.f });

    this->separator = make<SeparatorHorizontalFadingReversed>();
    this->addAndMakeVisible(this->separator.get());

    this->contextMenuController = make<HeadlineContextMenuController>(*this);

    this->instrumentsList->setMultipleSelectionEnabled(false);
    this->instrumentsList->setRowHeight(InstrumentsListComponent::rowHeight);
}

InstrumentsListComponent::~InstrumentsListComponent() = default;

void InstrumentsListComponent::resized()
{
    constexpr auto titleHeight = 26;
    constexpr auto listPadding = 40;

    this->instrumentsList->setBounds(this->getLocalBounds().withTrimmedTop(listPadding).reduced(2));
    this->titleLabel->setBounds(0, 0, this->getWidth(), titleHeight);
    this->separator->setBounds(0, listPadding, this->getWidth(), 3);
}

void InstrumentsListComponent::parentHierarchyChanged()
{
    if (this->getParentComponent() != nullptr)
    {
        this->updateListContent();
    }
}

void InstrumentsListComponent::clearSelection()
{
    this->instrumentsList->setSelectedRows({}, dontSendNotification);
}

void InstrumentsListComponent::updateListContent()
{
    this->instrumentIcon = Icons::findByName(Icons::instrument,
        int(InstrumentsListComponent::rowHeight * 0.75f));

    this->instruments = this->instrumentsRoot.findChildrenRefsOfType<InstrumentNode>();
    this->instrumentsList->updateContent();
    this->clearSelection();
    this->repaint();
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
    if (instrument == nullptr)
    {
        jassertfalse;
        return;
    }

    g.setFont(h * 0.4f);
    const int margin = h / 12;

    // todo also display "(unavailable)", if not valid?
    const auto alpha = instrument->isValid() ? 1.f : 0.35f;
    g.setColour(findDefaultColour(ListBox::textColourId).withMultipliedAlpha(alpha));
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
#if PLATFORM_DESKTOP
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
#elif PLATFORM_MOBILE
    auto instrumentNode = this->instruments[lastRowSelected];
    if (instrumentNode != nullptr)
    {
        instrumentNode->setSelected();
    }
#endif
}

void InstrumentsListComponent::listBoxItemDoubleClicked(int rowNumber, const MouseEvent &e)
{
#if PLATFORM_DESKTOP
    auto instrumentNode = this->instruments[rowNumber];
    if (instrumentNode != nullptr && instrumentNode->getInstrument()->isValid())
    {
        instrumentNode->setSelected();
    }
#endif
}

//===----------------------------------------------------------------------===//
// HeadlineItemDataSource
//===----------------------------------------------------------------------===//

bool InstrumentsListComponent::hasMenu() const noexcept { return true; }
bool InstrumentsListComponent::canBeSelectedAsMenuItem() const { return false; }

UniquePointer<Component> InstrumentsListComponent::createMenu()
{
    const auto selectedRow = this->instrumentsList->getSelectedRow();
    const auto instrument = this->instruments[selectedRow];
    if (instrument != nullptr)
    {
        return instrument->createMenu();
    }

    return nullptr;
}

Image InstrumentsListComponent::getIcon() const
{
    return Icons::findByName(Icons::instrument, Globals::UI::headlineIconSize);
}

String InstrumentsListComponent::getName() const
{
    const auto selectedRow = this->instrumentsList->getSelectedRow();
    const auto instrument = this->instruments[selectedRow];
    jassert(instrument);
    return instrument->getName();
}

void InstrumentsListComponent::listBoxItemClicked(int row, const MouseEvent &e)
{
    if (e.mods.isRightButtonDown())
    {
        const auto selectedRow = this->instrumentsList->getSelectedRow();
        const auto instrument = this->instruments[selectedRow];
        if (instrument != nullptr)
        {
            this->contextMenuController->showMenu(e);
        }
    }
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
         fontname="Default font" fontsize="21.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="36"/>
  <JUCERCOMP name="" id="a09914d60dab2768" memberName="separator1" virtualName=""
             explicitFocusOrder="0" pos="0.5Cc 40 0M 3" sourceFile="../../Themes/SeparatorHorizontalFadingReversed.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
