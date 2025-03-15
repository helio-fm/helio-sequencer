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
#include "AudioPluginsListComponent.h"
#include "BuiltInSynthsPluginFormat.h"
#include "MenuItemComponent.h"
#include "OrchestraPit.h"
#include "OrchestraPitPage.h"
#include "OrchestraPitNode.h"
#include "AudioPluginSelectionMenu.h"
#include "HeadlineContextMenuController.h"
#include "PluginScanner.h"
#include "MainLayout.h"
#include "HelioTheme.h"
#include "Config.h"

AudioPluginsListComponent::AudioPluginsListComponent(PluginScanner &pluginScanner, OrchestraPitNode &instrumentsRoot) :
    pluginScanner(pluginScanner),
    instrumentsRoot(instrumentsRoot),
    pluginDescriptions(pluginScanner.getPlugins()),
    filteredDescriptions(pluginScanner.getPlugins())
{
    this->setFocusContainerType(Component::FocusContainerType::none);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);

    this->searchTextEditor = HelioTheme::makeSingleLineTextEditor(true);
    this->addAndMakeVisible(this->searchTextEditor.get());

    this->searchTextEditor->onEscapeKey = [this]()
    {
        this->searchTextEditor->setText({}, true);
    };

    this->searchTextEditor->onTextChange = [this]()
    {
        this->updateFilteredDescriptions();
        this->pluginsList->updateContent();
    };

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
    this->showScanButtonIf(this->filteredDescriptions.isEmpty());

#elif PLATFORM_MOBILE

    // on mobile, there's no such thing as `scan folder`:
    this->initialScanButton = make<MenuItemComponent>(this, nullptr,
        MenuItem::item(Icons::instrument,
            CommandIDs::ScanAllPlugins,
            TRANS(I18n::Menu::instrumentsReload)));

    this->addAndMakeVisible(this->initialScanButton.get());
    this->showScanButtonIf(this->filteredDescriptions.isEmpty());

#endif

    this->pluginsList->setRowHeight(AudioPluginsListComponent::rowHeight);
    this->pluginsList->setHeaderHeight(AudioPluginsListComponent::tableHeaderHeight);
    this->pluginsList->getViewport()->setScrollBarsShown(true, false);

#if PLATFORM_DESKTOP
    this->pluginsList->getViewport()->setScrollBarThickness(2);
#elif PLATFORM_MOBILE
    this->pluginsList->getViewport()->setScrollBarThickness(36);
#endif

    const auto uiFlags = App::Config().getUiFlags();
    const auto pluginSorting = uiFlags->getPluginSorting();
    const auto pluginSortingForwards = uiFlags->isPluginSortingForwards();

    const auto columnFlags =
        TableHeaderComponent::visible |
        TableHeaderComponent::appearsOnColumnMenu |
        TableHeaderComponent::sortable;

    this->pluginsList->getHeader().addColumn(TRANS(I18n::Page::orchestraCategory),
        ColumnIds::category, 50, 50, -1,
            (pluginSorting != KnownPluginList::SortMethod::sortByCategory) ? columnFlags :
                columnFlags | (pluginSortingForwards ?
                    TableHeaderComponent::sortedForwards : TableHeaderComponent::sortedBackwards));

    this->pluginsList->getHeader().addColumn(TRANS(I18n::Page::orchestraVendorandname),
        ColumnIds::vendorAndName, 50, 50, -1,
            (pluginSorting != KnownPluginList::SortMethod::sortAlphabetically) ? columnFlags :
                columnFlags | (pluginSortingForwards ?
                    TableHeaderComponent::sortedForwards : TableHeaderComponent::sortedBackwards));

    this->pluginsList->getHeader().addColumn(TRANS(I18n::Page::orchestraFormat),
        ColumnIds::format, 50, 50, -1,
            (pluginSorting != KnownPluginList::SortMethod::sortByFormat) ? columnFlags :
                columnFlags | (pluginSortingForwards ?
                    TableHeaderComponent::sortedForwards : TableHeaderComponent::sortedBackwards));

    this->pluginsList->setMultipleSelectionEnabled(false);
}

AudioPluginsListComponent::~AudioPluginsListComponent() = default;

void AudioPluginsListComponent::resized()
{
    constexpr auto headerSize = 40;
    this->titleLabel->setBounds(0, 0, this->getWidth(), headerSize - 4);
    this->titleSeparator->setBounds(0, headerSize, this->getWidth(), 2);

    auto pluginsListArea = this->getLocalBounds().withTrimmedTop(headerSize).reduced(2);
    pluginsListArea.removeFromTop(6);
    this->searchTextEditor->setBounds(pluginsListArea.removeFromTop(Globals::UI::textEditorHeight).reduced(6, 0));
    pluginsListArea.removeFromTop(2);
    this->pluginsList->setBounds(pluginsListArea);

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
    this->searchTextEditor->setVisible(!hasNoPlugins);
    this->initialScanButton->setVisible(hasNoPlugins);
}

void AudioPluginsListComponent::clearSelection()
{
    this->pluginsList->setSelectedRows({}, dontSendNotification);
}

void AudioPluginsListComponent::updateListContent()
{
    auto newPlugins = this->pluginScanner.getPlugins();
    const auto shouldResetFilter = newPlugins.size() != this->pluginDescriptions.size();
    this->clearSelection();
    this->pluginDescriptions.swapWith(newPlugins);
    if (shouldResetFilter && !this->searchTextEditor->isEmpty())
    {
        this->searchTextEditor->setText({}, false);
    }
    this->showScanButtonIf(this->pluginDescriptions.isEmpty());
    this->updateFilteredDescriptions();
    this->pluginsList->updateContent();
}

void AudioPluginsListComponent::updateFilteredDescriptions()
{
    const auto searchText = this->searchTextEditor->getText();
    if (searchText.isEmpty())
    {
        this->filteredDescriptions = this->pluginDescriptions;
        return;
    }

    this->filteredDescriptions.clearQuick();
    for (const auto &pd : this->pluginDescriptions)
    {
        if (pd.descriptiveName.containsIgnoreCase(searchText) ||
            pd.pluginFormatName.containsIgnoreCase(searchText) ||
            pd.manufacturerName.containsIgnoreCase(searchText))
        {
            this->filteredDescriptions.add(pd);
        }
    }
}

//===----------------------------------------------------------------------===//
// TableListBoxModel
//===----------------------------------------------------------------------===//

void AudioPluginsListComponent::paintRowBackground(Graphics &g, int rowNumber, int, int, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(findDefaultColour(Label::textColourId).withAlpha(0.08f));
    }
    else if (rowNumber % 2)
    {
        g.fillAll(Colours::black.withAlpha(0.025f));
    }
}

void AudioPluginsListComponent::paintCell(Graphics &g,
    int rowNumber, int columnId,
    int w, int h, bool rowIsSelected)
{
    const auto pd = this->filteredDescriptions[rowNumber];

    constexpr int margin = 5;
    const auto colour = findDefaultColour(Label::textColourId);

#if PLATFORM_MOBILE
    constexpr auto useSmallFonts = true;
#elif PLATFORM_DESKTOP
    constexpr auto useSmallFonts = false;
#endif

    switch (columnId)
    {
    case ColumnIds::vendorAndName:
    {
        const String inputChannelsString = TRANS_PLURAL("{x} input channels", pd.numInputChannels);
        const String outputChannelsString = TRANS_PLURAL("{x} output channels", pd.numOutputChannels);

        g.setFont(useSmallFonts ? Globals::UI::Fonts::S : Globals::UI::Fonts::M);
        g.setColour(colour);
        HelioTheme::drawText(g,
            pd.descriptiveName, margin, margin,
            w - margin * 2, h - margin * 2, Justification::topLeft);

        g.setFont(Globals::UI::Fonts::S);
        g.setColour(colour.withMultipliedAlpha(0.75f));
        HelioTheme::drawText(g,
            pd.manufacturerName, margin, margin,
            w - margin * 2, h - margin * 2, Justification::centredLeft);

        String details = pd.version;
        if (inputChannelsString.isNotEmpty())
        {
            if (details.isNotEmpty())
            {
                details << ", ";
            }

            details << inputChannelsString;
        }

        if (outputChannelsString.isNotEmpty())
        {
            if (details.isNotEmpty())
            {
                details << ", ";
            }

            details << outputChannelsString;
        }

        g.setColour(colour.withMultipliedAlpha(0.5f));
        HelioTheme::drawText(g,
            details, margin, margin,
            w - margin * 2, h - margin * 2 - 1, Justification::bottomLeft);

        break;
    }
    case ColumnIds::category:
    {
        g.setFont(useSmallFonts ? Globals::UI::Fonts::XS : Globals::UI::Fonts::S);
        g.setColour(colour.withMultipliedAlpha(0.5f));
        HelioTheme::drawText(g,
            pd.category, margin, margin,
            w - margin * 2, h - margin * 2, Justification::topRight);
        break;
    }
    case ColumnIds::format:
    {
        if (pd.pluginFormatName != BuiltInSynthsPluginFormat::formatName)
        {
            g.setFont(useSmallFonts ? Globals::UI::Fonts::XS : Globals::UI::Fonts::S);
            g.setColour(colour.withMultipliedAlpha(0.75f));
            HelioTheme::drawText(g,
                pd.pluginFormatName, margin, margin,
                w - margin * 2, h - margin * 2, Justification::topLeft);
        }
        break;
    }
    default:
        break;
    }
}

void AudioPluginsListComponent::sortOrderChanged(int newSortColumnId, bool isForwards)
{
    KnownPluginList::SortMethod sortMethod = KnownPluginList::SortMethod::defaultOrder;
    switch (newSortColumnId)
    {
    case ColumnIds::vendorAndName:
        sortMethod = KnownPluginList::SortMethod::sortAlphabetically;
        break;
    case ColumnIds::category:
        sortMethod = KnownPluginList::SortMethod::sortByCategory;
        break;
    case ColumnIds::format:
        sortMethod = KnownPluginList::SortMethod::sortByFormat;
        break;
    default:
        break;
    }

    this->pluginScanner.sortList(sortMethod, isForwards);
    App::Config().getUiFlags()->setPluginSorting(sortMethod, isForwards);
}

int AudioPluginsListComponent::getNumRows()
{
    return this->filteredDescriptions.size();
}

int AudioPluginsListComponent::getColumnAutoSizeWidth(int columnId)
{
    const auto smallScreenMode = App::isRunningOnPhone();
    const auto formatColumnWidth = smallScreenMode ? 48 : 96;
    const auto categoryColumnWidth = smallScreenMode ? 64 : 112;

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

void AudioPluginsListComponent::selectedRowsChanged(int lastRowSelected)
{
    if (this->pluginsList->getNumSelectedRows() > 0)
    {
        // re-init the existing menu because the selection will be different:
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
    return make<AudioPluginSelectionMenu>(this->filteredDescriptions[selectedRow],
        this->instrumentsRoot, this->pluginScanner);
}

Image AudioPluginsListComponent::getIcon() const
{
    return Icons::findByName(Icons::audioPlugin, Globals::UI::headlineIconSize);
}

String AudioPluginsListComponent::getName() const
{
    const auto selectedRow = this->pluginsList->getSelectedRow();
    return this->filteredDescriptions[selectedRow].descriptiveName;
}

void AudioPluginsListComponent::cellClicked(int rowNumber, int columnId, const MouseEvent &e)
{
    if (e.mods.isRightButtonDown())
    {
        this->contextMenuController->showMenu(e);
    }
}
