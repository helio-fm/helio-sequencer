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

#include "SyncSettings.h"

//[MiscUserDefs]
#include "MainWindow.h"
#include "SyncSettingsItem.h"
#include "HelioApiRoutes.h"
#include "Config.h"

#if HELIO_DESKTOP
#   define SYNC_SETTINGS_ROW_HEIGHT (32)
#elif HELIO_MOBILE
#   define SYNC_SETTINGS_ROW_HEIGHT (48)
#endif
//[/MiscUserDefs]

SyncSettings::SyncSettings()
{
    this->resourcesList.reset(new ListBox());
    this->addAndMakeVisible(resourcesList.get());

    this->shadow.reset(new SeparatorHorizontal());
    this->addAndMakeVisible(shadow.get());

    //[UserPreSize]
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(true);
    this->setOpaque(true);

    this->resourcesList->setModel(this);
    this->resourcesList->setRowHeight(SYNC_SETTINGS_ROW_HEIGHT);
    this->resourcesList->getViewport()->setScrollBarsShown(true, false);
    //[/UserPreSize]

    this->setSize(600, 265);

    //[Constructor]
    //const auto resources(TranslationsManager::getInstance().getAvailableLocales());
    //this->setSize(600, 40 + locales.size() * SYNC_SETTINGS_ROW_HEIGHT);

    // TODO listen all resource managers
    //TranslationsManager::getInstance().addChangeListener(this);
    //[/Constructor]
}

SyncSettings::~SyncSettings()
{
    //[Destructor_pre]
    //TranslationsManager::getInstance().removeChangeListener(this);
    //[/Destructor_pre]

    resourcesList = nullptr;
    shadow = nullptr;

    //[Destructor]
    //[/Destructor]
}

void SyncSettings::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SyncSettings::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    resourcesList->setBounds(0, 0, getWidth() - 0, getHeight() - 0);
    shadow->setBounds(0, -1, getWidth() - 0, 11);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void SyncSettings::changeListenerCallback(ChangeBroadcaster *source)
{
    this->resourcesList->updateContent();
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *SyncSettings::refreshComponentForRow(int rowNumber, bool isRowSelected,
    Component *existingComponentToUpdate)
{
    //if (rowNumber >= resources.size()) { return existingComponentToUpdate; }

    //const bool isSynced = false; // TODO;
    //const bool isLastRow = (rowNumber == resources.size() - 1);
    //const String localeDescription(resources[rowNumber]->getId());

    //if (existingComponentToUpdate != nullptr)
    //{
    //    if (auto *row = dynamic_cast<SyncSettingsItem *>(existingComponentToUpdate))
    //    {
    //        row->updateDescription(isLastRow, isSynced, resources[rowNumber]->getName());
    //    }
    //}
    //else
    //{
    //    auto *row = new SyncSettingsItem(*this->resourcesList);
    //    row->updateDescription(isLastRow, isSynced, resources[rowNumber]->getName());
    //    return row;
    //}

    return existingComponentToUpdate;
}

int SyncSettings::getNumRows()
{
    // TODO
    return 0;
}

int SyncSettings::getRowHeight() const noexcept
{
    return this->resourcesList->getRowHeight();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SyncSettings" template="../../../Template"
                 componentName="" parentClasses="public Component, public ListBoxModel, private ChangeListener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="600"
                 initialHeight="265">
  <BACKGROUND backgroundColour="4d4d4d"/>
  <GENERICCOMPONENT name="" id="5005ba29a3a1bbc6" memberName="resourcesList" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 0M" class="ListBox" params=""/>
  <JUCERCOMP name="" id="34270fb50cf926d8" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0 -1 0M 11" posRelativeX="c5bde2fac9c39997"
             posRelativeY="c5bde2fac9c39997" posRelativeW="c5bde2fac9c39997"
             sourceFile="../../Themes/SeparatorHorizontal.cpp" constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
