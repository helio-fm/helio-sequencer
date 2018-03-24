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

#include "InstrumentsPage.h"

//[MiscUserDefs]
#include "InstrumentRow.h"
#include "PluginScanner.h"
#include "InstrumentsRootTreeItem.h"
#include "MainLayout.h"
#include "AudioCore.h"
#include "App.h"
#include "CommandItemComponent.h"
#include "Icons.h"
#include "ProgressTooltip.h"
#include "CommandIDs.h"
#include "App.h"
#include "Workspace.h"
#include "ComponentIDs.h"
//[/MiscUserDefs]

InstrumentsPage::InstrumentsPage(PluginScanner &scanner, InstrumentsRootTreeItem &instrumentsTreeItem)
    : pluginManager(scanner),
      instrumentsRoot(instrumentsTreeItem)
{
    addAndMakeVisible (background = new PanelBackgroundB());
    addAndMakeVisible (panel = new FramePanel());
    addAndMakeVisible (pluginsList = new ListBox ("Instruments", this));

    addAndMakeVisible (initButton = new TextButton ("saxophone"));
    initButton->setButtonText (TRANS("instruments::init"));
    initButton->setConnectedEdges (Button::ConnectedOnRight | Button::ConnectedOnTop);
    initButton->addListener (this);

    addAndMakeVisible (removeButton = new TextButton ("minus"));
    removeButton->setButtonText (TRANS("instruments::remove"));
    removeButton->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnTop);
    removeButton->addListener (this);

    addAndMakeVisible (scanButton = new TextButton ("open"));
    scanButton->setButtonText (TRANS("instruments::search"));
    scanButton->setConnectedEdges (Button::ConnectedOnTop);
    scanButton->addListener (this);

    addAndMakeVisible (shadow = new LightShadowDownwards());
    addAndMakeVisible (initialScanButton = new CommandItemComponent (this, nullptr, CommandItem::withParams(Icons::saxophone, CommandIDs::ScanAllPlugins, TRANS("instruments::initialscan"))));

    addAndMakeVisible (separator1 = new SeparatorHorizontalFading());
    addAndMakeVisible (separator2 = new SeparatorHorizontalFading());

    //[UserPreSize]
    this->initialScanButton->setMouseCursor(MouseCursor::PointingHandCursor);

    if (this->pluginManager.getList().getNumTypes() == 0)
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

    this->pluginManager.addChangeListener(this);

    this->setWantsKeyboardFocus(true);
    this->setFocusContainer(true);

    //[/Constructor]
}

InstrumentsPage::~InstrumentsPage()
{
    //[Destructor_pre]
    this->pluginManager.removeChangeListener(this);

    //[/Destructor_pre]

    background = nullptr;
    panel = nullptr;
    pluginsList = nullptr;
    initButton = nullptr;
    removeButton = nullptr;
    scanButton = nullptr;
    shadow = nullptr;
    initialScanButton = nullptr;
    separator1 = nullptr;
    separator2 = nullptr;

    //[Destructor]
    //[/Destructor]
}

void InstrumentsPage::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void InstrumentsPage::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    panel->setBounds (20, 20, getWidth() - 40, getHeight() - 90);
    pluginsList->setBounds (25, 24, getWidth() - 50, getHeight() - 99);
    initButton->setBounds (20 + 5, 20 + (getHeight() - 90), 155, 51);
    removeButton->setBounds ((20 + 5) + 155 - -5, 20 + (getHeight() - 90), 155, 51);
    scanButton->setBounds (20 + (getWidth() - 40) - 5 - 155, 20 + (getHeight() - 90), 155, 51);
    shadow->setBounds (20 + 6, 20 + (getHeight() - 90), (getWidth() - 40) - 12, 16);
    initialScanButton->setBounds ((getWidth() / 2) - (310 / 2), (getHeight() / 2) - (64 / 2), 310, 64);
    separator1->setBounds (((getWidth() / 2) - (310 / 2)) + 310 / 2 - (300 / 2), ((getHeight() / 2) - (64 / 2)) + -16, 300, 3);
    separator2->setBounds (((getWidth() / 2) - (310 / 2)) + 310 / 2 - (300 / 2), ((getHeight() / 2) - (64 / 2)) + 64 - -16, 300, 3);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void InstrumentsPage::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == initButton)
    {
        //[UserButtonCode_initButton] -- add your button handler code here..

        const int selectedRow = this->pluginsList->getSelectedRow();

        if (this->pluginManager.getList().getType(selectedRow) != nullptr)
        {
            const PluginDescription pluginDescription(*this->pluginManager.getList().getType(selectedRow));
            App::Workspace().getAudioCore().addInstrument(pluginDescription, pluginDescription.descriptiveName,
                [this](Instrument *instrument)
            {
                this->instrumentsRoot.addInstrumentTreeItem(instrument);
                this->pluginsList->setSelectedRows(SparseSet<int>());
            });
        }
        else
        {
            App::Layout().showTooltip(TRANS("warnings::noinstrument"));
        }

        //[/UserButtonCode_initButton]
    }
    else if (buttonThatWasClicked == removeButton)
    {
        //[UserButtonCode_removeButton] -- add your button handler code here..

        const int selectedRow = this->pluginsList->getSelectedRow();
        PluginDescription *pluginDescription = this->pluginManager.getList().getType(selectedRow);

        if (pluginDescription != nullptr)
        {
            this->pluginManager.removeListItem(selectedRow);
            this->pluginsList->updateContent();
        }
        else
        {
            App::Layout().showTooltip(TRANS("warnings::noinstrument"));
        }

        //[/UserButtonCode_removeButton]
    }
    else if (buttonThatWasClicked == scanButton)
    {
        //[UserButtonCode_scanButton] -- add your button handler code here..

        //this->instrumentsRoot.getPluginManager().runInitialScan();

#if HELIO_DESKTOP
        FileChooser fc(TRANS("dialog::scanfolder::caption"),
                       File::getCurrentWorkingDirectory(), ("*.*"), true);

        if (fc.browseForDirectory())
        {
            App::Layout().showModalComponentUnowned(new ProgressTooltip());
            this->pluginManager.scanFolderAndAddResults(fc.getResult());
            this->pluginsList->updateContent();
        }
#endif

        //[/UserButtonCode_scanButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void InstrumentsPage::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::ScanAllPlugins)
    {
        App::Layout().showModalComponentUnowned(new ProgressTooltip());
        this->pluginManager.runInitialScan();
        this->hideGreeting();

    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]

void InstrumentsPage::showGreeting()
{
    this->pluginsList->setVisible(false);
    this->panel->setVisible(false);
    this->shadow->setVisible(false);
    this->scanButton->setVisible(false);
    this->initButton->setVisible(false);
    this->removeButton->setVisible(false);

    this->initialScanButton->setVisible(true);
    this->separator1->setVisible(true);
    this->separator2->setVisible(true);
}

void InstrumentsPage::hideGreeting()
{
    this->pluginsList->setVisible(true);
    this->panel->setVisible(true);
    this->shadow->setVisible(true);
    this->scanButton->setVisible(true);
    this->initButton->setVisible(true);
    this->removeButton->setVisible(true);

    this->initialScanButton->setVisible(false);
    this->separator1->setVisible(false);
    this->separator2->setVisible(false);
}


//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *InstrumentsPage::refreshComponentForRow(int rowNumber, bool isRowSelected,
        Component *existingComponentToUpdate)
{
    PluginDescription *pd =
        this->pluginManager.getList().getType(rowNumber);

    if (!pd) { return existingComponentToUpdate; }

    if (existingComponentToUpdate != nullptr)
    {
        if (InstrumentRow *row = dynamic_cast<InstrumentRow *>(existingComponentToUpdate))
        {
            row->setSelected(isRowSelected);
            row->refreshPluginDescription(*pd);
        }
    }
    else
    {
        InstrumentRow *row = new InstrumentRow(*pd);
        row->setSelected(isRowSelected);
        return row;
    }

    return existingComponentToUpdate;
}

String InstrumentsPage::getTooltipForRow(int row)
{
    PluginDescription *pd =
        pluginManager.getList().getType(row);

    if (!pd) { return ""; }

    return pd->fileOrIdentifier;
}


var InstrumentsPage::getDragSourceDescription(const SparseSet<int> &currentlySelectedRows)
{
    PluginDescription *pd =
        this->pluginManager.getList().getType(currentlySelectedRows[0]);

    if (!pd) { return var::null; }

    PluginDescriptionWrapper::Ptr pluginWrapper = new PluginDescriptionWrapper();
    pluginWrapper->pluginDescription = PluginDescription(*pd);
    var pluginVar(pluginWrapper);

    return pluginVar;
}

int InstrumentsPage::getNumRows()
{
    const int numTypes = this->pluginManager.getList().getNumTypes();
    return numTypes;
}

void InstrumentsPage::paintListBoxItem(int rowNumber, Graphics &g, int width,
                                       int height, bool rowIsSelected)
{
}


//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void InstrumentsPage::changeListenerCallback(ChangeBroadcaster *source)
{
    if (PluginScanner *scanner = dynamic_cast<PluginScanner *>(source))
    {
        this->pluginsList->updateContent();
        this->pluginsList->setSelectedRows(SparseSet<int>());

        if (! this->pluginManager.isWorking())
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

<JUCER_COMPONENT documentType="Component" className="InstrumentsPage" template="../../Template"
                 componentName="" parentClasses="public Component, public ListBoxModel, public ChangeListener"
                 constructorParams="PluginManager &amp;scanner, InstrumentsRootTreeItem &amp;instrumentsTreeItem"
                 variableInitialisers="pluginManager(scanner),&#10;instrumentsRoot(instrumentsTreeItem)"
                 snapPixels="4" snapActive="1" snapShown="0" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="9957575039af6ddc" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="d37f5d299f347b6c" memberName="panel" virtualName=""
             explicitFocusOrder="0" pos="20 20 40M 90M" sourceFile="../Themes/FramePanel.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="1b089ba42e39d447" memberName="pluginsList" virtualName=""
                    explicitFocusOrder="0" pos="25 24 50M 99M" class="ListBox" params="&quot;Instruments&quot;, this"/>
  <TEXTBUTTON name="saxophone" id="2ee7fcb67a7b0eaa" memberName="initButton"
              virtualName="" explicitFocusOrder="0" pos="5 0R 155 51" posRelativeX="d37f5d299f347b6c"
              posRelativeY="d37f5d299f347b6c" buttonText="instruments::init"
              connectedEdges="6" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="minus" id="ea7d7d2896f28adb" memberName="removeButton"
              virtualName="" explicitFocusOrder="0" pos="-5R 0R 155 51" posRelativeX="2ee7fcb67a7b0eaa"
              posRelativeY="d37f5d299f347b6c" buttonText="instruments::remove"
              connectedEdges="5" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="open" id="308078ca18c39bc7" memberName="scanButton" virtualName=""
              explicitFocusOrder="0" pos="5Rr 0R 155 51" posRelativeX="d37f5d299f347b6c"
              posRelativeY="d37f5d299f347b6c" buttonText="instruments::search"
              connectedEdges="4" needsCallback="1" radioGroupId="0"/>
  <JUCERCOMP name="" id="e80d8080a2f39d47" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="6 0R 12M 16" posRelativeX="d37f5d299f347b6c"
             posRelativeY="d37f5d299f347b6c" posRelativeW="d37f5d299f347b6c"
             sourceFile="../Themes/LightShadowDownwards.cpp" constructorParams=""/>
  <GENERICCOMPONENT name="" id="62a5bd7c1a3ec2" memberName="initialScanButton" virtualName=""
                    explicitFocusOrder="0" pos="0Cc 0Cc 310 64" class="CommandItemComponent"
                    params="this, nullptr, CommandItem::withParams(Icons::saxophone, CommandIDs::ScanAllPlugins, TRANS(&quot;instruments::initialscan&quot;))"/>
  <JUCERCOMP name="" id="8817b1b124163b2f" memberName="separator1" virtualName=""
             explicitFocusOrder="0" pos="0Cc -16 300 3" posRelativeX="62a5bd7c1a3ec2"
             posRelativeY="62a5bd7c1a3ec2" sourceFile="../Themes/SeparatorHorizontalFading.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="af3fdc94cce3ad8c" memberName="separator2" virtualName=""
             explicitFocusOrder="0" pos="0Cc -16R 300 3" posRelativeX="62a5bd7c1a3ec2"
             posRelativeY="62a5bd7c1a3ec2" sourceFile="../Themes/SeparatorHorizontalFading.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
