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

#include "OrchestraPitPage.h"

//[MiscUserDefs]
#include "PluginScanner.h"
#include "OrchestraPitTreeItem.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "App.h"
#include "MenuItemComponent.h"
#include "Icons.h"
#include "ProgressTooltip.h"
#include "CommandIDs.h"
#include "App.h"
#include "Workspace.h"
#include "ComponentIDs.h"

#define CATEGORY_COLUMN_WIDTH (100)
#define FORMAT_COLUMN_WIDTH (75)

enum ColumnIds
{
    vendorAndName = 1, // TableListBox needs any number apart from 0
    category = 2,
    format = 3
};

//[/MiscUserDefs]

OrchestraPitPage::OrchestraPitPage(PluginScanner &scanner, OrchestraPitTreeItem &instrumentsTreeItem)
    : pluginScanner(scanner),
      instrumentsRoot(instrumentsTreeItem)
{
    addAndMakeVisible (background = new PanelBackgroundB());
    addAndMakeVisible (panel = new FramePanel());
    addAndMakeVisible (pluginsList = new TableListBox ("Instruments", this));

    addAndMakeVisible (initialScanButton = new MenuItemComponent (this, nullptr, MenuItem::item(Icons::saxophone, CommandIDs::ScanAllPlugins, TRANS("instruments::initialscan"))));

    addAndMakeVisible (separator1 = new SeparatorHorizontalFading());
    addAndMakeVisible (separator2 = new SeparatorHorizontalFading());

    //[UserPreSize]
    this->initialScanButton->setMouseCursor(MouseCursor::PointingHandCursor);

    if (this->pluginScanner.getList().getNumTypes() == 0)
    {
        this->showGreeting();
    }
    else
    {
        this->hideGreeting();
    }
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]

    this->pluginsList->setColour(ListBox::backgroundColourId, Colours::transparentBlack);
    this->pluginsList->setRowHeight(PLUGINSLIST_ROW_HEIGHT);
    this->pluginsList->setHeaderHeight(PLUGINSLIST_HEADER_HEIGHT);

    const auto columnFlags =
        TableHeaderComponent::visible |
        TableHeaderComponent::appearsOnColumnMenu |
        TableHeaderComponent::sortable;

    this->pluginsList->getHeader().addColumn(TRANS("page::instruments::category"),
        category, 50, 50, -1, columnFlags);

    this->pluginsList->getHeader().addColumn(TRANS("page::instruments::vendorandname"),
        vendorAndName, 50, 50, -1, columnFlags);

    this->pluginsList->getHeader().addColumn(TRANS("page::instruments::format"),
        format, 50, 50, -1, columnFlags);

    this->pluginsList->getHeader().setSortColumnId(vendorAndName, true);
    this->pluginsList->setMultipleSelectionEnabled(false);

    this->pluginScanner.addChangeListener(this);

    this->setWantsKeyboardFocus(true);
    this->setFocusContainer(true);

    //[/Constructor]
}

OrchestraPitPage::~OrchestraPitPage()
{
    //[Destructor_pre]
    this->pluginScanner.removeChangeListener(this);

    //[/Destructor_pre]

    background = nullptr;
    panel = nullptr;
    pluginsList = nullptr;
    initialScanButton = nullptr;
    separator1 = nullptr;
    separator2 = nullptr;

    //[Destructor]
    //[/Destructor]
}

void OrchestraPitPage::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void OrchestraPitPage::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    panel->setBounds (20, 20, getWidth() - 40, getHeight() - 40);
    pluginsList->setBounds (25, 25, getWidth() - 50, getHeight() - 50);
    initialScanButton->setBounds ((getWidth() / 2) - (310 / 2), (getHeight() / 2) - (64 / 2), 310, 64);
    separator1->setBounds (((getWidth() / 2) - (310 / 2)) + 310 / 2 - (300 / 2), ((getHeight() / 2) - (64 / 2)) + -16, 300, 3);
    separator2->setBounds (((getWidth() / 2) - (310 / 2)) + 310 / 2 - (300 / 2), ((getHeight() / 2) - (64 / 2)) + 64 - -16, 300, 3);
    //[UserResized] Add your own custom resize handling here..
    this->pluginsList->autoSizeAllColumns();
    //[/UserResized]
}

void OrchestraPitPage::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::ScanAllPlugins)
    {
        App::Layout().showModalComponentUnowned(new ProgressTooltip());
        this->pluginScanner.runInitialScan();
        this->hideGreeting();
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]

/*

    if (buttonThatWasClicked == initButton)
    {
        //[UserButtonCode_initButton] -- add your button handler code here..
        const int selectedRow = this->pluginsList->getSelectedRow();
        if (this->pluginScanner.getList().getType(selectedRow) != nullptr)
        {
            const PluginDescription pluginDescription(*this->pluginScanner.getList().getType(selectedRow));
            App::Workspace().getAudioCore().addInstrument(pluginDescription, pluginDescription.descriptiveName,
                [this](Instrument *instrument)
            {
                this->instrumentsRoot.addInstrumentTreeItem(instrument);
                this->pluginsList->setSelectedRows(SparseSet<int>());
            });
        }
        //[/UserButtonCode_initButton]
    }
    else if (buttonThatWasClicked == removeButton)
    {
        //[UserButtonCode_removeButton] -- add your button handler code here..
        const int selectedRow = this->pluginsList->getSelectedRow();
        PluginDescription *pluginDescription = this->pluginScanner.getList().getType(selectedRow);

        if (pluginDescription != nullptr)
        {
            this->pluginScanner.removeListItem(selectedRow);
            this->pluginsList->updateContent();
        }
        //[/UserButtonCode_removeButton]
    }
    else if (buttonThatWasClicked == scanButton)
    {
        //[UserButtonCode_scanButton] -- add your button handler code here..
#if HELIO_DESKTOP
        FileChooser fc(TRANS("dialog::scanfolder::caption"),
                       File::getCurrentWorkingDirectory(), ("*.*"), true);

        if (fc.browseForDirectory())
        {
            App::Layout().showModalComponentUnowned(new ProgressTooltip());
            this->pluginScanner.scanFolderAndAddResults(fc.getResult());
            this->pluginsList->updateContent();
        }
#endif
        //[/UserButtonCode_scanButton]
    }

*/

void OrchestraPitPage::showGreeting()
{
    this->pluginsList->setVisible(false);
    this->panel->setVisible(false);

    this->initialScanButton->setVisible(true);
    this->separator1->setVisible(true);
    this->separator2->setVisible(true);
}

void OrchestraPitPage::hideGreeting()
{
    this->pluginsList->setVisible(true);
    this->panel->setVisible(true);

    this->initialScanButton->setVisible(false);
    this->separator1->setVisible(false);
    this->separator2->setVisible(false);
}

//===----------------------------------------------------------------------===//
// TableListBoxModel
//===----------------------------------------------------------------------===//

var OrchestraPitPage::getDragSourceDescription(const SparseSet<int> &currentlySelectedRows)
{
    auto pd = this->pluginScanner.getList().getType(currentlySelectedRows[0]);
    if (pd == nullptr) { return var::null; }

    PluginDescriptionDragnDropWrapper::Ptr pluginWrapper = new PluginDescriptionDragnDropWrapper();
    pluginWrapper->pluginDescription = PluginDescription(*pd);
    var pluginVar(pluginWrapper);

    return pluginVar;
}

void OrchestraPitPage::paintRowBackground(Graphics &g, int rowNumber, int, int, bool rowIsSelected)
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

void OrchestraPitPage::paintCell(Graphics& g, int rowNumber, int columnId,
    int w, int h, bool rowIsSelected)
{
    g.setFont(Font(Font::getDefaultSansSerifFontName(), h * 0.27f, Font::plain));
    const auto pd = this->pluginScanner.getList().getType(rowNumber);
    const int margin = h / 12;

    switch (columnId)
    {
    case ColumnIds::vendorAndName:
    {
        const String inputChannelsString = TRANS_PLURAL("{x} input channels", pd->numInputChannels);
        const String outputChannelsString = TRANS_PLURAL("{x} output channels", pd->numOutputChannels);

        g.setColour(Colours::white);
        g.drawText(pd->descriptiveName, margin, margin, w, h, Justification::topLeft, false);

        g.setColour(Colours::white.withAlpha(0.7f));
        g.drawText(pd->manufacturerName, margin, 0, w, h, Justification::centredLeft, false);

        g.setColour(Colours::white.withAlpha(0.5f));
        g.drawText(pd->version + ", " + inputChannelsString + ", " + outputChannelsString,
            margin, -margin, w, h, Justification::bottomLeft, false);

        break;
    }
    case ColumnIds::category:
    {
        g.setColour(Colours::white.withAlpha(0.5f));
        g.drawText(pd->category, 0, margin, w - int(margin * 1.5f), h, Justification::topRight, false);
        break;
    }
    case ColumnIds::format:
    {
        g.setColour(Colours::white.withAlpha(0.7f));
        g.drawText(pd->pluginFormatName, margin, margin, w, h, Justification::topLeft, false);
        break;
    }
    default:
        break;
    }
}

void OrchestraPitPage::sortOrderChanged(int newSortColumnId, bool isForwards)
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

int OrchestraPitPage::getNumRows()
{
    return this->pluginScanner.getList().getNumTypes();
}

int OrchestraPitPage::getColumnAutoSizeWidth(int columnId)
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

String OrchestraPitPage::getCellTooltip(int rowNumber, int columnId)
{
    auto description = pluginScanner.getList().getType(rowNumber);
    if (description == nullptr) { return {}; }
    return description->fileOrIdentifier;
}

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void OrchestraPitPage::changeListenerCallback(ChangeBroadcaster *source)
{
    if (PluginScanner *scanner = dynamic_cast<PluginScanner *>(source))
    {
        this->pluginsList->updateContent();
        this->pluginsList->setSelectedRows(SparseSet<int>());

        if (! this->pluginScanner.isWorking())
        {
            if (Component *progressIndicator =
                App::Layout().findChildWithID(ComponentIDs::progressTooltipId))
            {
                // Nasty hack -_-
                delete progressIndicator;
            }
        }
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="OrchestraPitPage" template="../../../Template"
                 componentName="" parentClasses="public Component, public TableListBoxModel, public ChangeListener"
                 constructorParams="PluginScanner &amp;scanner, OrchestraPitTreeItem &amp;instrumentsTreeItem"
                 variableInitialisers="pluginScanner(scanner),&#10;instrumentsRoot(instrumentsTreeItem)"
                 snapPixels="4" snapActive="1" snapShown="0" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="9957575039af6ddc" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="d37f5d299f347b6c" memberName="panel" virtualName=""
             explicitFocusOrder="0" pos="20 20 40M 40M" sourceFile="../../Themes/FramePanel.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="1b089ba42e39d447" memberName="pluginsList" virtualName=""
                    explicitFocusOrder="0" pos="25 25 50M 50M" class="TableListBox"
                    params="&quot;Instruments&quot;, this"/>
  <GENERICCOMPONENT name="" id="62a5bd7c1a3ec2" memberName="initialScanButton" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 0Cc 310 64" class="MenuItemComponent"
                    params="this, nullptr, MenuItem::item(Icons::saxophone, CommandIDs::ScanAllPlugins, TRANS(&quot;instruments::initialscan&quot;))"/>
  <JUCERCOMP name="" id="8817b1b124163b2f" memberName="separator1" virtualName=""
             explicitFocusOrder="0" pos="0Cc -16 300 3" posRelativeX="62a5bd7c1a3ec2"
             posRelativeY="62a5bd7c1a3ec2" sourceFile="../../Themes/SeparatorHorizontalFading.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="af3fdc94cce3ad8c" memberName="separator2" virtualName=""
             explicitFocusOrder="0" pos="0Cc -16R 300 3" posRelativeX="62a5bd7c1a3ec2"
             posRelativeY="62a5bd7c1a3ec2" sourceFile="../../Themes/SeparatorHorizontalFading.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
