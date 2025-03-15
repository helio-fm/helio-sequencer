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
#include "InstrumentsListComponent.h"

#include "OrchestraPitNode.h"
#include "InstrumentMenu.h"
#include "HeadlineContextMenuController.h"
#include "Instrument.h"
#include "MainLayout.h"
#include "HelioTheme.h"

InstrumentsListComponent::InstrumentsListComponent(PluginScanner &pluginScanner, OrchestraPitNode &instrumentsRoot) :
    pluginScanner(pluginScanner),
    instrumentsRoot(instrumentsRoot)
{
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);

    this->instrumentsList = make<ListBox>(String(), this);
    this->addAndMakeVisible(this->instrumentsList.get());

    this->titleLabel = make<Label>(String(), TRANS(I18n::Page::orchestraInstruments));
    this->addAndMakeVisible(this->titleLabel.get());
    this->titleLabel->setJustificationType(Justification::centred);
    this->titleLabel->setFont(Globals::UI::Fonts::L);

    this->separator = make<SeparatorHorizontalFadingReversed>();
    this->addAndMakeVisible(this->separator.get());

    this->contextMenuController = make<HeadlineContextMenuController>(*this);

    this->instrumentsList->setMultipleSelectionEnabled(false);
    this->instrumentsList->setRowHeight(InstrumentsListComponent::rowHeight);
}

InstrumentsListComponent::~InstrumentsListComponent() = default;

void InstrumentsListComponent::resized()
{
    constexpr auto headerSize = 40;
    this->titleLabel->setBounds(0, 0, this->getWidth(), headerSize - 4);
    this->instrumentsList->setBounds(this->getLocalBounds().withTrimmedTop(headerSize).reduced(2));
    this->separator->setBounds(0, headerSize, this->getWidth(), 2);
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
        int(InstrumentsListComponent::iconSize));

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
        g.fillAll(findDefaultColour(Label::textColourId).withAlpha(0.08f));
    }
    //else if (rowNumber % 2)
    //{
    //    g.fillAll(Colours::black.withAlpha(0.025f));
    //}

    const auto instrument = instrumentNode->getInstrument();
    if (instrument == nullptr)
    {
        jassertfalse;
        return;
    }

    g.setFont(Globals::UI::Fonts::M);

    // todo also display "(unavailable)", if not valid?
    const auto alpha = instrument->isValid() ? 1.f : 0.35f;
    g.setColour(findDefaultColour(ListBox::textColourId).withMultipliedAlpha(alpha));

    constexpr auto margin = 6;
    HelioTheme::drawText(g,
        instrumentNode->getName(),
        (margin * 2) + int(InstrumentsListComponent::iconSize), margin,
        w, h - (margin * 2), Justification::centredLeft);

    Icons::drawImageRetinaAware(this->instrumentIcon, g, h / 2, h / 2);
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
