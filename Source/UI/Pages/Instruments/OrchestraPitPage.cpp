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
//[/MiscUserDefs]

OrchestraPitPage::OrchestraPitPage(PluginScanner &pluginScanner, OrchestraPitTreeItem &instrumentsRoot)
    : pluginScanner(pluginScanner),
      instrumentsRoot(instrumentsRoot)
{
    addAndMakeVisible (background = new PanelBackgroundB());
    addAndMakeVisible (pluginsList = new AudioPluginsListComponent (pluginScanner, instrumentsRoot));
    addAndMakeVisible (anchor = new Component());

    addAndMakeVisible (instrumentsList = new InstrumentsListComponent (pluginScanner, instrumentsRoot));

    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
    this->pluginScanner.addChangeListener(this);
    //[/Constructor]
}

OrchestraPitPage::~OrchestraPitPage()
{
    //[Destructor_pre]
    this->pluginScanner.removeChangeListener(this);
    //[/Destructor_pre]

    background = nullptr;
    pluginsList = nullptr;
    anchor = nullptr;
    instrumentsList = nullptr;

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
    pluginsList->setBounds (15, 10, proportionOfWidth (0.5006f) - 15, getHeight() - 20);
    anchor->setBounds (0, 0, proportionOfWidth (0.5006f), 8);
    instrumentsList->setBounds (0 + proportionOfWidth (0.5006f) - -15, 10, proportionOfWidth (0.5006f) - 30, getHeight() - 20);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void OrchestraPitPage::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::ScanAllPlugins)
    {
        App::Layout().showModalComponentUnowned(new ProgressTooltip());
        this->pluginScanner.runInitialScan();
        this->pluginsList->showScanButtonIf(false);
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

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void OrchestraPitPage::changeListenerCallback(ChangeBroadcaster *source)
{
    if (auto scanner = dynamic_cast<PluginScanner *>(source))
    {
        this->pluginsList->showScanButtonIf(scanner->getList().getNumTypes() == 0);
        this->pluginsList->updateListContent();

        if (!scanner->isWorking())
        {
            if (auto spinner = App::Layout().findChildWithID(ComponentIDs::progressTooltipId))
            {
                // Nasty hack -_-
                delete spinner;
            }
        }
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="OrchestraPitPage" template="../../../Template"
                 componentName="" parentClasses="public Component, public ChangeListener"
                 constructorParams="PluginScanner &amp;pluginScanner, OrchestraPitTreeItem &amp;instrumentsRoot"
                 variableInitialisers="pluginScanner(pluginScanner),&#10;instrumentsRoot(instrumentsRoot)"
                 snapPixels="4" snapActive="1" snapShown="0" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="9957575039af6ddc" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="d37f5d299f347b6c" memberName="pluginsList" virtualName=""
             explicitFocusOrder="0" pos="15 10 15M 20M" posRelativeW="4ac6bf71d1e1d84f"
             sourceFile="AudioPluginsListComponent.cpp" constructorParams="pluginScanner, instrumentsRoot"/>
  <GENERICCOMPONENT name="" id="4ac6bf71d1e1d84f" memberName="anchor" virtualName=""
                    explicitFocusOrder="0" pos="0 0 50.065% 8" class="Component"
                    params=""/>
  <JUCERCOMP name="" id="23f9d3ba9d40a668" memberName="instrumentsList" virtualName=""
             explicitFocusOrder="0" pos="-15R 10 30M 20M" posRelativeX="4ac6bf71d1e1d84f"
             posRelativeW="4ac6bf71d1e1d84f" sourceFile="InstrumentsListComponent.cpp"
             constructorParams="pluginScanner, instrumentsRoot"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
