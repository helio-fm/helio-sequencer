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
#include "OrchestraPitTreeItem.h"
#include "PluginScanner.h"
#include "Icons.h"
//[/MiscUserDefs]

InstrumentsListComponent::InstrumentsListComponent(PluginScanner &pluginScanner, OrchestraPitTreeItem &instrumentsRoot)
    : pluginScanner(pluginScanner),
      instrumentsRoot(instrumentsRoot)
{
    addAndMakeVisible (panel = new FramePanel());
    addAndMakeVisible (instrumentsList = new ListBox ("Instruments", this));

    addAndMakeVisible (titleLabel = new Label (String(),
                                               TRANS("page::orchestra::instruments")));
    titleLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType (Justification::centred);
    titleLabel->setEditable (false, false, false);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
    //[/Constructor]
}

InstrumentsListComponent::~InstrumentsListComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    panel = nullptr;
    instrumentsList = nullptr;
    titleLabel = nullptr;

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

    panel->setBounds (0, 35, getWidth() - 0, getHeight() - 35);
    instrumentsList->setBounds (1, 36, getWidth() - 2, getHeight() - 37);
    titleLabel->setBounds (0, 0, getWidth() - 0, 26);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

int InstrumentsListComponent::getNumRows()
{
    // this->instrumentsRoot.findChildrenOfType<InstrumentTreeItem>()
    return 0; // TODO
}

Component *InstrumentsListComponent::refreshComponentForRow(int rowNumber,
    bool isRowSelected, Component *existingComponent)
{
    return existingComponent; // TODO
}

void InstrumentsListComponent::listBoxItemClicked(int rowNumber, const MouseEvent &e)
{
    // Desktop:
    // On click - shows menu,
    // On double click - shows instrument page

    // Mobile:
    // On click - shows instrument page
}

//===----------------------------------------------------------------------===//
// HeadlineItemDataSource
//===----------------------------------------------------------------------===//

bool InstrumentsListComponent::hasMenu() const noexcept { return false; }
bool InstrumentsListComponent::canBeSelectedAsMenuItem() const { return false; }

ScopedPointer<Component> InstrumentsListComponent::createMenu()
{
    return nullptr; // { new InstrumentMenu(this->changesList->getSelectedRows(), this->vcs) };
}

Image InstrumentsListComponent::getIcon() const
{
    return Icons::findByName(Icons::selection, HEADLINE_ICON_SIZE);
}

String InstrumentsListComponent::getName() const
{
    return TRANS("menu::selection::instrument");
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="InstrumentsListComponent"
                 template="../../../Template" componentName="" parentClasses="public Component, public ListBoxModel, public HeadlineItemDataSource"
                 constructorParams="PluginScanner &amp;pluginScanner, OrchestraPitTreeItem &amp;instrumentsRoot"
                 variableInitialisers="pluginScanner(pluginScanner),&#10;instrumentsRoot(instrumentsRoot)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="d37f5d299f347b6c" memberName="panel" virtualName=""
             explicitFocusOrder="0" pos="0 35 0M 35M" sourceFile="../../Themes/FramePanel.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="1b089ba42e39d447" memberName="instrumentsList" virtualName=""
                    explicitFocusOrder="0" pos="1 36 2M 37M" class="ListBox" params="&quot;Instruments&quot;, this"/>
  <LABEL name="" id="660583b19bbfaa6b" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="0 0 0M 26" labelText="page::orchestra::instruments"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="21.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
