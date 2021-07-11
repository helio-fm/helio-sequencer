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
#include "AudioPluginsListComponent.h"

#include "MenuItemComponent.h"
#include "OrchestraPit.h"
#include "OrchestraPitPage.h"
#include "OrchestraPitNode.h"
#include "AudioPluginSelectionMenu.h"
#include "HeadlineContextMenuController.h"
#include "PluginScanner.h"
#include "MainLayout.h"

AudioPluginsListComponent::AudioPluginsListComponent(PluginScanner &pluginScanner, OrchestraPitNode &instrumentsRoot) :
    pluginScanner(pluginScanner),
    instrumentsRoot(instrumentsRoot)
{
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);

    this->pluginsList = make<TableListBox>(String(), this);
    this->addAndMakeVisible(this->pluginsList.get());

    this->titleLabel = make<Label>(String(), TRANS(I18n::Page::orchestraPlugins));
    this->addAndMakeVisible(this->titleLabel.get());
    this->titleLabel->setJustificationType(Justification::centred);
    this->titleLabel->setFont(Globals::UI::Fonts::L);

    this->titleSeparator = make<SeparatorHorizontalFadingReversed>();
    this->addAndMakeVisible(this->titleSeparator.get());

    this->contextMenuController = make<HeadlineContextMenuController>(*this);

#if PLATFORM_DESKTOP

    this->initialScanButton = make<MenuItemComponent>(this,
        nullptr, MenuItem::item(Icons::empty,
            CommandIDs::ScanPluginsFolder, TRANS(I18n::Menu::instrumentsScanFolder)));
    this->addAndMakeVisible(this->initialScanButton.get());
    this->initialScanButton->setMouseCursor(MouseCursor::PointingHandCursor);
    this->showScanButtonIf(this->pluginScanner.getNumPlugins() == 0);

#elif PLATFORM_MOBILE

    // on mobile, there's no such thing as `scan folder`:
    this->initialScanButton = make<MenuItemComponent>(this, nullptr,
        MenuItem::item(Icons::instrument,
            CommandIDs::ScanAllPlugins,
            TRANS(I18n::Menu::instrumentsReload)));

    this->addAndMakeVisible(this->initialScanButton.get());
    this->showScanButtonIf(this->pluginScanner.getNumPlugins() == 0);

#endif

    this->pluginsList->setRowHeight(AudioPluginsListComponent::rowHeight);
    this->pluginsList->setHeaderHeight(AudioPluginsListComponent::tableHeaderHeight);
    this->pluginsList->getViewport()->setScrollBarsShown(true, false);

    const auto columnFlags =
        TableHeaderComponent::visible |
        TableHeaderComponent::appearsOnColumnMenu |
        TableHeaderComponent::sortable;

    this->pluginsList->getHeader().addColumn(TRANS(I18n::Page::orchestraCategory),
        ColumnIds::category, 50, 50, -1, columnFlags);

    this->pluginsList->getHeader().addColumn(TRANS(I18n::Page::orchestraVendorandname),
        ColumnIds::vendorAndName, 50, 50, -1, columnFlags);

    this->pluginsList->getHeader().addColumn(TRANS(I18n::Page::orchestraFormat),
        ColumnIds::format, 50, 50, -1, columnFlags);

    this->pluginsList->getHeader().setSortColumnId(ColumnIds::vendorAndName, true);
    this->pluginsList->setMultipleSelectionEnabled(false);
}

AudioPluginsListComponent::~AudioPluginsListComponent() = default;

void AudioPluginsListComponent::resized()
{
    constexpr auto titleHeight = 26;
    constexpr auto listPadding = 40;

    this->pluginsList->setBounds(this->getLocalBounds().withTrimmedTop(listPadding).reduced(2));
    this->titleLabel->setBounds(0, 0, this->getWidth(), titleHeight);
    this->titleSeparator->setBounds(0, listPadding, this->getWidth(), 3);

    constexpr auto scanButtonWidth = 150;
    constexpr auto scanButtonHeight = 96;

    const auto scanButtonBounds = this->getLocalBounds().withSizeKeepingCentre(scanButtonWidth, scanButtonHeight);
    this->initialScanButton->setBounds(scanButtonBounds);

    this->pluginsList->autoSizeAllColumns();
}

void AudioPluginsListComponent::parentHierarchyChanged()
{
    if (this->getParentComponent() != nullptr)
    {
        this->updateListContent();
    }
}

void AudioPluginsListComponent::showScanButtonIf(bool hasNoPlugins)
{
    this->pluginsList->setVisible(!hasNoPlugins);
    this->initialScanButton->setVisible(hasNoPlugins);
}

void AudioPluginsListComponent::clearSelection()
{
    this->pluginsList->setSelectedRows({}, dontSendNotification);
}

void AudioPluginsListComponent::updateListContent()
{
    this->pluginsList->updateContent();
    this->clearSelection();
}

//===----------------------------------------------------------------------===//
// TableListBoxModel
//===----------------------------------------------------------------------===//

var AudioPluginsListComponent::getDragSourceDescription(const SparseSet<int> &currentlySelectedRows)
{
    const auto pd = this->pluginScanner.getPlugins()[currentlySelectedRows[0]];

    PluginDescriptionDragnDropWrapper::Ptr pluginWrapper = new PluginDescriptionDragnDropWrapper();
    pluginWrapper->pluginDescription = pd;
    var pluginVar(pluginWrapper.get());

    return pluginVar;
}

void AudioPluginsListComponent::paintRowBackground(Graphics &g, int rowNumber, int, int, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(Colours::white.withAlpha(0.075f));
    }
    else if (rowNumber % 2)
    {
        g.fillAll(Colours::black.withAlpha(0.05f));
    }
}

void AudioPluginsListComponent::paintCell(Graphics &g,
    int rowNumber, int columnId,
    int w, int h, bool rowIsSelected)
{
    const auto pd = this->pluginScanner.getPlugins()[rowNumber];

    const int margin = h / 12;
    const Colour colour(findDefaultColour(Label::textColourId));

    switch (columnId)
    {
    case ColumnIds::vendorAndName:
    {
        const String inputChannelsString = TRANS_PLURAL("{x} input channels", pd.numInputChannels);
        const String outputChannelsString = TRANS_PLURAL("{x} output channels", pd.numOutputChannels);

        g.setFont(Globals::UI::Fonts::M);
        g.setColour(colour);
        g.drawText(pd.descriptiveName, margin, margin, w, h, Justification::topLeft, false);

        g.setFont(Globals::UI::Fonts::S);
        g.setColour(colour.withAlpha(0.7f));
        g.drawText(pd.manufacturerName, margin, 0, w, h, Justification::centredLeft, false);

        g.setColour(colour.withAlpha(0.5f));

        String details = pd.version;
        if (inputChannelsString.isNotEmpty())
        {
            details << ", " << inputChannelsString;
        }

        if (outputChannelsString.isNotEmpty())
        {
            details << ", " << outputChannelsString;
 
        }

        g.drawText(details, margin, -margin, w, h, Justification::bottomLeft, false);
        break;
    }
    case ColumnIds::category:
    {
        g.setFont(Globals::UI::Fonts::S);
        g.setColour(colour.withAlpha(0.5f));
        g.drawText(pd.category, 0, margin, w - int(margin * 1.5f), h, Justification::topRight, false);
        break;
    }
    case ColumnIds::format:
    {
        g.setFont(Globals::UI::Fonts::S);
        g.setColour(colour.withAlpha(0.7f));
        g.drawText(pd.pluginFormatName, margin, margin, w, h, Justification::topLeft, false);
        break;
    }
    default:
        break;
    }
}

void AudioPluginsListComponent::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    switch (newSortColumnId)
    {
    case ColumnIds::vendorAndName:
        this->pluginScanner.sortList(KnownPluginList::SortMethod::sortByManufacturer, isForwards);
    case ColumnIds::category:
        this->pluginScanner.sortList(KnownPluginList::SortMethod::sortByCategory, isForwards);
    case ColumnIds::format:
        this->pluginScanner.sortList(KnownPluginList::SortMethod::sortByFormat, isForwards);
    default:
        break;
    }

    this->pluginsList->updateContent();
}

int AudioPluginsListComponent::getNumRows()
{
    return this->pluginScanner.getNumPlugins();
}

int AudioPluginsListComponent::getColumnAutoSizeWidth(int columnId)
{
    constexpr auto formatColumnWidth = 96;
    constexpr auto categoryColumnWidth = 112;

    switch (columnId)
    {
    case ColumnIds::vendorAndName:
        return this->pluginsList->getVisibleContentWidth() - categoryColumnWidth - formatColumnWidth;
    case ColumnIds::category:
        return categoryColumnWidth;
    case ColumnIds::format:
        return formatColumnWidth;
    default:
        return 0;
    }
}

String AudioPluginsListComponent::getCellTooltip(int rowNumber, int columnId)
{
    const auto description = pluginScanner.getPlugins()[rowNumber];
    return description.fileOrIdentifier;
}

void AudioPluginsListComponent::selectedRowsChanged(int lastRowSelected)
{
    if (this->pluginsList->getNumSelectedRows() > 0)
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
        parent->onPluginsSelectionChanged();
    }
}

//===----------------------------------------------------------------------===//
// HeadlineItemDataSource
//===----------------------------------------------------------------------===//

bool AudioPluginsListComponent::hasMenu() const noexcept { return true; }
bool AudioPluginsListComponent::canBeSelectedAsMenuItem() const { return false; }

UniquePointer<Component> AudioPluginsListComponent::createMenu()
{
    const auto selectedRow = this->pluginsList->getSelectedRow();
    const auto description = pluginScanner.getPlugins()[selectedRow];
    return make<AudioPluginSelectionMenu>(description, this->instrumentsRoot, this->pluginScanner);
}

Image AudioPluginsListComponent::getIcon() const
{
    return Icons::findByName(Icons::audioPlugin, Globals::UI::headlineIconSize);
}

String AudioPluginsListComponent::getName() const
{
    const auto selectedRow = this->pluginsList->getSelectedRow();
    const auto description = pluginScanner.getPlugins()[selectedRow];
    return description.descriptiveName;
}

void AudioPluginsListComponent::cellClicked(int rowNumber, int columnId, const MouseEvent &e)
{
    if (e.mods.isRightButtonDown())
    {
        this->contextMenuController->showMenu(e);
    }
}
