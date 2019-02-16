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

#include "AudioPluginsListComponent.h"

//[MiscUserDefs]

#include "MenuItemComponent.h"
#include "OrchestraPit.h"
#include "OrchestraPitPage.h"
#include "OrchestraPitNode.h"
#include "AudioPluginSelectionMenu.h"
#include "PluginScanner.h"
#include "MainLayout.h"
#include "CommandIDs.h"
#include "Icons.h"

#define CATEGORY_COLUMN_WIDTH (100)
#define FORMAT_COLUMN_WIDTH (75)

enum ColumnIds
{
    vendorAndName = 1, // TableListBox needs any number apart from 0
    category = 2,
    format = 3
};

//[/MiscUserDefs]

AudioPluginsListComponent::AudioPluginsListComponent(PluginScanner &pluginScanner, OrchestraPitNode &instrumentsRoot)
    : pluginScanner(pluginScanner),
      instrumentsRoot(instrumentsRoot)
{
    this->pluginsList.reset(new TableListBox("Instruments", this));
    this->addAndMakeVisible(pluginsList.get());

    this->initialScanButton.reset(new MenuItemComponent(this, nullptr, MenuItem::item(Icons::instrument, CommandIDs::ScanAllPlugins, TRANS("instruments::initialscan"))));
    this->addAndMakeVisible(initialScanButton.get());

    this->separator1.reset(new SeparatorHorizontalFading());
    this->addAndMakeVisible(separator1.get());
    this->separator2.reset(new SeparatorHorizontalFading());
    this->addAndMakeVisible(separator2.get());
    this->titleLabel.reset(new Label(String(),
                                      TRANS("page::orchestra::plugins")));
    this->addAndMakeVisible(titleLabel.get());
    this->titleLabel->setFont(Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType(Justification::centred);
    titleLabel->setEditable(false, false, false);

    this->separator3.reset(new SeparatorHorizontalFadingReversed());
    this->addAndMakeVisible(separator3.get());

    //[UserPreSize]
    this->initialScanButton->setMouseCursor(MouseCursor::PointingHandCursor);
    this->showScanButtonIf(this->pluginScanner.getList().getNumTypes() == 0);
    //[/UserPreSize]

    this->setSize(600, 400);

    //[Constructor]

    this->pluginsList->setRowHeight(PLUGINSLIST_ROW_HEIGHT);
    this->pluginsList->setHeaderHeight(PLUGINSLIST_HEADER_HEIGHT);
    this->pluginsList->getViewport()->setScrollBarsShown(true, false);

    const auto columnFlags =
        TableHeaderComponent::visible |
        TableHeaderComponent::appearsOnColumnMenu |
        TableHeaderComponent::sortable;

    this->pluginsList->getHeader().addColumn(TRANS("page::orchestra::category"),
        category, 50, 50, -1, columnFlags);

    this->pluginsList->getHeader().addColumn(TRANS("page::orchestra::vendorandname"),
        vendorAndName, 50, 50, -1, columnFlags);

    this->pluginsList->getHeader().addColumn(TRANS("page::orchestra::format"),
        format, 50, 50, -1, columnFlags);

    this->pluginsList->getHeader().setSortColumnId(vendorAndName, true);
    this->pluginsList->setMultipleSelectionEnabled(false);

    this->setWantsKeyboardFocus(true);
    this->setFocusContainer(true);
    //[/Constructor]
}

AudioPluginsListComponent::~AudioPluginsListComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    pluginsList = nullptr;
    initialScanButton = nullptr;
    separator1 = nullptr;
    separator2 = nullptr;
    titleLabel = nullptr;
    separator3 = nullptr;

    //[Destructor]
    //[/Destructor]
}

void AudioPluginsListComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AudioPluginsListComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    pluginsList->setBounds(1, 42, getWidth() - 2, getHeight() - 43);
    initialScanButton->setBounds((getWidth() / 2) - (310 / 2), (getHeight() / 2) - (64 / 2), 310, 64);
    separator1->setBounds(((getWidth() / 2) - (310 / 2)) + 310 / 2 - (300 / 2), ((getHeight() / 2) - (64 / 2)) + -18, 300, 3);
    separator2->setBounds(((getWidth() / 2) - (310 / 2)) + 310 / 2 - (300 / 2), ((getHeight() / 2) - (64 / 2)) + 64 - -14, 300, 3);
    titleLabel->setBounds(0, 0, getWidth() - 0, 26);
    separator3->setBounds((getWidth() / 2) - ((getWidth() - 0) / 2), 40, getWidth() - 0, 3);
    //[UserResized] Add your own custom resize handling here..
    this->pluginsList->autoSizeAllColumns();
    //[/UserResized]
}

void AudioPluginsListComponent::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    if (this->getParentComponent() != nullptr)
    {
        this->updateListContent();
    }
    //[/UserCode_parentHierarchyChanged]
}


//[MiscUserCode]

void AudioPluginsListComponent::showScanButtonIf(bool hasNoPlugins)
{
    this->pluginsList->setVisible(!hasNoPlugins);
    this->initialScanButton->setVisible(hasNoPlugins);
    this->separator1->setVisible(hasNoPlugins);
    this->separator2->setVisible(hasNoPlugins);
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
    auto pd = this->pluginScanner.getList().getType(currentlySelectedRows[0]);
    if (pd == nullptr) { return {}; }

    PluginDescriptionDragnDropWrapper::Ptr pluginWrapper = new PluginDescriptionDragnDropWrapper();
    pluginWrapper->pluginDescription = PluginDescription(*pd);
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
    g.setFont(h * 0.27f);
    const auto *pd = this->pluginScanner.getList().getType(rowNumber);
    if (pd == nullptr)
    {
        // Plugin scanner must be re-building a list now
        return;
    }

    const int margin = h / 12;
    const Colour colour(findDefaultColour(Label::textColourId));

    switch (columnId)
    {
    case ColumnIds::vendorAndName:
    {
        const String inputChannelsString = TRANS_PLURAL("{x} input channels", pd->numInputChannels);
        const String outputChannelsString = TRANS_PLURAL("{x} output channels", pd->numOutputChannels);

        g.setColour(colour);
        g.drawText(pd->descriptiveName, margin, margin, w, h, Justification::topLeft, false);

        g.setColour(colour.withAlpha(0.7f));
        g.drawText(pd->manufacturerName, margin, 0, w, h, Justification::centredLeft, false);

        g.setColour(colour.withAlpha(0.5f));
        g.drawText(pd->version + ", " + inputChannelsString + ", " + outputChannelsString,
            margin, -margin, w, h, Justification::bottomLeft, false);

        break;
    }
    case ColumnIds::category:
    {
        g.setColour(colour.withAlpha(0.5f));
        g.drawText(pd->category, 0, margin, w - int(margin * 1.5f), h, Justification::topRight, false);
        break;
    }
    case ColumnIds::format:
    {
        g.setColour(colour.withAlpha(0.7f));
        g.drawText(pd->pluginFormatName, margin, margin, w, h, Justification::topLeft, false);
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
    return this->pluginScanner.getList().getNumTypes();
}

int AudioPluginsListComponent::getColumnAutoSizeWidth(int columnId)
{
    switch (columnId)
    {
    case ColumnIds::vendorAndName:
        return this->pluginsList->getVisibleContentWidth() - CATEGORY_COLUMN_WIDTH - FORMAT_COLUMN_WIDTH;
    case ColumnIds::category:
        return CATEGORY_COLUMN_WIDTH;
    case ColumnIds::format:
        return FORMAT_COLUMN_WIDTH;
    default:
        return 0;
    }
}

String AudioPluginsListComponent::getCellTooltip(int rowNumber, int columnId)
{
    auto description = pluginScanner.getList().getType(rowNumber);
    if (description == nullptr) { return {}; }
    return description->fileOrIdentifier;
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

ScopedPointer<Component> AudioPluginsListComponent::createMenu()
{
    const auto selectedRow = this->pluginsList->getSelectedRow();
    const auto description = pluginScanner.getList().getType(selectedRow);
    jassert(description);
    return { new AudioPluginSelectionMenu(*description, this->instrumentsRoot, this->pluginScanner) };
}

Image AudioPluginsListComponent::getIcon() const
{
    return Icons::findByName(Icons::audioPlugin, HEADLINE_ICON_SIZE);
}

String AudioPluginsListComponent::getName() const
{
    const auto selectedRow = this->pluginsList->getSelectedRow();
    auto description = pluginScanner.getList().getType(selectedRow);
    jassert(description);
    return description->descriptiveName;
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AudioPluginsListComponent"
                 template="../../../Template" componentName="" parentClasses="public Component, public TableListBoxModel, public HeadlineItemDataSource"
                 constructorParams="PluginScanner &amp;pluginScanner, OrchestraPitNode &amp;instrumentsRoot"
                 variableInitialisers="pluginScanner(pluginScanner),&#10;instrumentsRoot(instrumentsRoot)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="1b089ba42e39d447" memberName="pluginsList" virtualName=""
                    explicitFocusOrder="0" pos="1 42 2M 43M" class="TableListBox"
                    params="&quot;Instruments&quot;, this"/>
  <GENERICCOMPONENT name="" id="62a5bd7c1a3ec2" memberName="initialScanButton" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 0Cc 310 64" class="MenuItemComponent"
                    params="this, nullptr, MenuItem::item(Icons::instrument, CommandIDs::ScanAllPlugins, TRANS(&quot;instruments::initialscan&quot;))"/>
  <JUCERCOMP name="" id="8817b1b124163b2f" memberName="separator1" virtualName=""
             explicitFocusOrder="0" pos="0Cc -18 300 3" posRelativeX="62a5bd7c1a3ec2"
             posRelativeY="62a5bd7c1a3ec2" sourceFile="../../Themes/SeparatorHorizontalFading.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="af3fdc94cce3ad8c" memberName="separator2" virtualName=""
             explicitFocusOrder="0" pos="0Cc -14R 300 3" posRelativeX="62a5bd7c1a3ec2"
             posRelativeY="62a5bd7c1a3ec2" sourceFile="../../Themes/SeparatorHorizontalFading.cpp"
             constructorParams=""/>
  <LABEL name="" id="660583b19bbfaa6b" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="0 0 0M 26" labelText="page::orchestra::plugins"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="21.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="36"/>
  <JUCERCOMP name="" id="a09914d60dab2768" memberName="separator3" virtualName=""
             explicitFocusOrder="0" pos="0Cc 40 0M 3" sourceFile="../../Themes/SeparatorHorizontalFadingReversed.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
