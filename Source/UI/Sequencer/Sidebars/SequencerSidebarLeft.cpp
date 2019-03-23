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

#include "SequencerSidebarLeft.h"

//[MiscUserDefs]

#include "MainLayout.h"
#include "TreeNode.h"
#include "SerializationKeys.h"
#include "GenericAudioMonitorComponent.h"
#include "WaveformAudioMonitorComponent.h"
#include "SpectrogramAudioMonitorComponent.h"
#include "ModeIndicatorComponent.h"
#include "MenuItemComponent.h"
#include "ProjectNode.h"
#include "HelioTheme.h"
#include "IconComponent.h"
#include "Icons.h"
#include "AudioCore.h"
#include "ColourIDs.h"
#include "CommandIDs.h"

static inline constexpr int getAudioMonitorHeight()
{
    return MainLayout::getScrollerHeight() - 2;
}

//[/MiscUserDefs]

SequencerSidebarLeft::SequencerSidebarLeft(ProjectNode &project)
    : project(project)
{
    this->shadow.reset(new ShadowUpwards(Light));
    this->addAndMakeVisible(shadow.get());
    this->headLine.reset(new SeparatorHorizontalReversed());
    this->addAndMakeVisible(headLine.get());
    this->headShadow.reset(new ShadowDownwards(Light));
    this->addAndMakeVisible(headShadow.get());
    this->separator.reset(new SeparatorHorizontal());
    this->addAndMakeVisible(separator.get());
    this->modeIndicatorSelector.reset(new ModeIndicatorTrigger());
    this->addAndMakeVisible(modeIndicatorSelector.get());

    this->modeIndicator.reset(new ModeIndicatorComponent(3));
    this->addAndMakeVisible(modeIndicator.get());

    this->switchPatternModeButton.reset(new MenuItemComponent(this, nullptr, MenuItem::item(Icons::patterns, CommandIDs::SwitchBetweenRolls)));
    this->addAndMakeVisible(switchPatternModeButton.get());

    this->switchLinearModeButton.reset(new MenuItemComponent(this, nullptr, MenuItem::item(Icons::piano, CommandIDs::SwitchBetweenRolls)));
    this->addAndMakeVisible(switchLinearModeButton.get());

    this->listBox.reset(new ListBox());
    this->addAndMakeVisible(listBox.get());


    //[UserPreSize]
    this->setOpaque(true);
    this->setPaintingIsUnclipped(true);
    this->setInterceptsMouseClicks(false, true);

    this->recreateMenu();
    this->listBox->setModel(this);
    this->listBox->setMultipleSelectionEnabled(false);
    this->listBox->setRowHeight(SEQUENCER_SIDEBAR_ROW_HEIGHT);

    this->switchLinearModeButton->setVisible(false);
    this->switchPatternModeButton->setVisible(false);

    this->genericMonitor = new GenericAudioMonitorComponent(nullptr);
    this->waveformMonitor = new WaveformAudioMonitorComponent(nullptr);
    this->spectrogramMonitor = new SpectrogramAudioMonitorComponent(nullptr);

    this->addChildComponent(this->genericMonitor);
    this->addChildComponent(this->waveformMonitor);
    this->addChildComponent(this->spectrogramMonitor);

    this->genericMonitor->setVisible(true);
    //[/UserPreSize]

    this->setSize(48, 640);

    //[Constructor]
    //[/Constructor]
}

SequencerSidebarLeft::~SequencerSidebarLeft()
{
    //[Destructor_pre]
    this->spectrogramMonitor = nullptr;
    this->waveformMonitor = nullptr;
    this->genericMonitor = nullptr;

    //tree->setRootItem(nullptr);
    //[/Destructor_pre]

    shadow = nullptr;
    headLine = nullptr;
    headShadow = nullptr;
    separator = nullptr;
    modeIndicatorSelector = nullptr;
    modeIndicator = nullptr;
    switchPatternModeButton = nullptr;
    switchLinearModeButton = nullptr;
    listBox = nullptr;

    //[Destructor]
    //[/Destructor]
}

void SequencerSidebarLeft::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    auto &theme = static_cast<HelioTheme &>(this->getLookAndFeel());
    g.setFillType({ theme.getBgCacheC(), {} });
    g.fillRect(this->getLocalBounds());
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    g.setColour(findDefaultColour(ColourIDs::Common::borderLineLight));
    g.drawVerticalLine(this->getWidth() - 1, 0.f, float(this->getHeight()));
    //[/UserPaint]
}

void SequencerSidebarLeft::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    this->genericMonitor->setSize(this->getWidth(), getAudioMonitorHeight());
    this->waveformMonitor->setSize(this->getWidth(), getAudioMonitorHeight());
    this->spectrogramMonitor->setSize(this->getWidth(), getAudioMonitorHeight());

    this->genericMonitor->setTopLeftPosition(0, this->getHeight() - getAudioMonitorHeight());
    this->waveformMonitor->setTopLeftPosition(0, this->getHeight() - getAudioMonitorHeight());
    this->spectrogramMonitor->setTopLeftPosition(0, this->getHeight() - getAudioMonitorHeight());
    //[/UserPreResize]

    shadow->setBounds(0, getHeight() - 71 - 6, getWidth() - 0, 6);
    headLine->setBounds(0, 39, getWidth() - 0, 2);
    headShadow->setBounds(0, 40, getWidth() - 0, 6);
    separator->setBounds(0, getHeight() - 70 - 2, getWidth() - 0, 2);
    modeIndicatorSelector->setBounds(0, getHeight() - 70, getWidth() - 0, 70);
    modeIndicator->setBounds(0, getHeight() - 4 - 5, getWidth() - 0, 5);
    switchPatternModeButton->setBounds((getWidth() / 2) - ((getWidth() - 0) / 2), 0, getWidth() - 0, 39);
    switchLinearModeButton->setBounds((getWidth() / 2) - ((getWidth() - 0) / 2), 0, getWidth() - 0, 39);
    listBox->setBounds(0, 41, getWidth() - 0, getHeight() - 113);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SequencerSidebarLeft::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
void SequencerSidebarLeft::setAudioMonitor(AudioMonitor *audioMonitor)
{
    this->spectrogramMonitor->setTargetAnalyzer(audioMonitor);
    this->waveformMonitor->setTargetAnalyzer(audioMonitor);
    this->genericMonitor->setTargetAnalyzer(audioMonitor);
}

void SequencerSidebarLeft::handleChangeMode()
{
    switch (this->modeIndicator->scrollToNextMode())
    {
    case 0:
        this->switchMonitorsAnimated(this->spectrogramMonitor, this->genericMonitor);
        break;
    case 1:
        this->switchMonitorsAnimated(this->genericMonitor, this->waveformMonitor);
        break;
    case 2:
        this->switchMonitorsAnimated(this->waveformMonitor, this->spectrogramMonitor);
        break;
    default:
        break;
    }
}

void SequencerSidebarLeft::switchMonitorsAnimated(Component *oldOne, Component *newOne)
{
    const int w = this->getWidth();
    const int y = this->getHeight() - getAudioMonitorHeight();
    this->animator.animateComponent(oldOne, oldOne->getBounds().translated(-w, 0), 0.f, 200, true, 0.f, 1.f);
    oldOne->setVisible(false);
    newOne->setAlpha(0.f);
    newOne->setVisible(true);
    newOne->setTopLeftPosition(w, y);
    this->animator.animateComponent(newOne, newOne->getBounds().translated(-w, 0), 1.f, 200, false, 1.f, 0.f);
}

void SequencerSidebarLeft::setLinearMode()
{
    this->buttonFader.cancelAllAnimations(false);
    this->buttonFader.fadeIn(this->switchPatternModeButton.get(), 200);
    this->buttonFader.fadeOut(this->switchLinearModeButton.get(), 200);
}

void SequencerSidebarLeft::setPatternMode()
{
    this->buttonFader.cancelAllAnimations(false);
    this->buttonFader.fadeIn(this->switchLinearModeButton.get(), 200);
    this->buttonFader.fadeOut(this->switchPatternModeButton.get(), 200);
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

void SequencerSidebarLeft::recreateMenu()
{
    this->menu.clear();
    this->menu.add(MenuItem::item(Icons::zoomOut, CommandIDs::ZoomOut));
    this->menu.add(MenuItem::item(Icons::zoomIn, CommandIDs::ZoomIn));
    this->menu.add(MenuItem::item(Icons::zoomTool, CommandIDs::ZoomEntireClip));

    // Jump to playhead position (or start following playhead when playing)
    //this->menu.add(MenuItem::item(Icons::playhead, CommandIDs::ZoomEntireClip));

    // Jump to the next anchor, i.e. any timeline event
    this->menu.add(MenuItem::item(Icons::mediaRewind, CommandIDs::TimelineJumpPrevious));
    this->menu.add(MenuItem::item(Icons::mediaForward, CommandIDs::TimelineJumpNext));

    // Focus on next/previous track
    //this->menu.add(MenuItem::item(Icons::pageUp, CommandIDs::ZoomEntireClip));
    //this->menu.add(MenuItem::item(Icons::pageDown, CommandIDs::ZoomEntireClip));
}

Component *SequencerSidebarLeft::refreshComponentForRow(int rowNumber,
    bool isRowSelected, Component *existingComponentToUpdate)
{
    if (rowNumber >= this->menu.size())
    {
        return existingComponentToUpdate;
    }

    const MenuItem::Ptr itemDescription = this->menu[rowNumber];

    if (existingComponentToUpdate != nullptr)
    {
        if (auto *row = dynamic_cast<MenuItemComponent *>(existingComponentToUpdate))
        {
            row->setSelected(isRowSelected);
            row->update(itemDescription);
        }
    }
    else
    {
        auto *row = new MenuItemComponent(this, this->listBox->getViewport(), itemDescription);
        row->setSelected(isRowSelected);
        return row;
    }

    return existingComponentToUpdate;
}

int SequencerSidebarLeft::getNumRows()
{
    return this->menu.size();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SequencerSidebarLeft" template="../../../Template"
                 componentName="" parentClasses="public ModeIndicatorOwnerComponent, protected ListBoxModel"
                 constructorParams="ProjectNode &amp;project" variableInitialisers="project(project)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="48" initialHeight="640">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="accf780c6ef7ae9e" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0 71Rr 0M 6" sourceFile="../../Themes/ShadowUpwards.cpp"
             constructorParams="Light"/>
  <JUCERCOMP name="" id="28ce45d9e84b729c" memberName="headLine" virtualName=""
             explicitFocusOrder="0" pos="0 39 0M 2" sourceFile="../../Themes/SeparatorHorizontalReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="1d398dc12e2047bd" memberName="headShadow" virtualName=""
             explicitFocusOrder="0" pos="0 40 0M 6" sourceFile="../../Themes/ShadowDownwards.cpp"
             constructorParams="Light"/>
  <JUCERCOMP name="" id="22d481533ce3ecd3" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="0 70Rr 0M 2" sourceFile="../../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <GENERICCOMPONENT name="" id="9e1622013601218a" memberName="modeIndicatorSelector"
                    virtualName="" explicitFocusOrder="0" pos="0 0Rr 0M 70" class="ModeIndicatorTrigger"
                    params=""/>
  <GENERICCOMPONENT name="" id="4b6240e11495d88b" memberName="modeIndicator" virtualName=""
                    explicitFocusOrder="0" pos="0 4Rr 0M 5" class="ModeIndicatorComponent"
                    params="3"/>
  <GENERICCOMPONENT name="" id="34c972d7b22acf17" memberName="switchPatternModeButton"
                    virtualName="" explicitFocusOrder="0" pos="0Cc 0 0M 39" class="MenuItemComponent"
                    params="this, nullptr, MenuItem::item(Icons::patterns, CommandIDs::SwitchBetweenRolls)"/>
  <GENERICCOMPONENT name="" id="bbe7f83219439c7f" memberName="switchLinearModeButton"
                    virtualName="" explicitFocusOrder="0" pos="0Cc 0 0M 39" class="MenuItemComponent"
                    params="this, nullptr, MenuItem::item(Icons::piano, CommandIDs::SwitchBetweenRolls)"/>
  <GENERICCOMPONENT name="" id="381fa571a3dfc5cd" memberName="listBox" virtualName=""
                    explicitFocusOrder="0" pos="0 41 0M 113M" class="ListBox" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
