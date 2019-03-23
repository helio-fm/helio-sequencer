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

#include "TranslationSettings.h"

//[MiscUserDefs]
#include "BackendRequest.h"
#include "TranslationSettingsItem.h"
#include "Network.h"
#include "Config.h"

#if HELIO_DESKTOP
#   define TRANSLATION_SETTINGS_ROW_HEIGHT (32)
#elif HELIO_MOBILE
#   define TRANSLATION_SETTINGS_ROW_HEIGHT (48)
#endif
//[/MiscUserDefs]

TranslationSettings::TranslationSettings()
{
    this->translationsList.reset(new ListBox());
    this->addAndMakeVisible(translationsList.get());

    this->helpButton.reset(new TextButton(String()));
    this->addAndMakeVisible(helpButton.get());
    helpButton->setButtonText(TRANS("settings::language::help"));
    helpButton->addListener(this);

    this->shadow.reset(new SeparatorHorizontal());
    this->addAndMakeVisible(shadow.get());

    //[UserPreSize]
    this->setOpaque(true);
    this->setFocusContainer(false);
    this->setWantsKeyboardFocus(false);
    this->setPaintingIsUnclipped(true);

    this->availableTranslations = App::Config().getTranslations()->getAll();
    this->currentTranslation = App::Config().getTranslations()->getCurrent();
    //[/UserPreSize]

    this->setSize(600, 265);

    //[Constructor]
    this->setSize(600, 40 + this->availableTranslations.size() * TRANSLATION_SETTINGS_ROW_HEIGHT);

    this->translationsList->setModel(this);
    this->translationsList->setRowHeight(TRANSLATION_SETTINGS_ROW_HEIGHT);
    this->translationsList->getViewport()->setScrollBarsShown(true, false);
    this->scrollToSelectedLocale();

    App::Config().getTranslations()->addChangeListener(this);
    //[/Constructor]
}

TranslationSettings::~TranslationSettings()
{
    //[Destructor_pre]
    App::Config().getTranslations()->removeChangeListener(this);
    //[/Destructor_pre]

    translationsList = nullptr;
    helpButton = nullptr;
    shadow = nullptr;

    //[Destructor]
    //[/Destructor]
}

void TranslationSettings::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void TranslationSettings::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    translationsList->setBounds(0, 0, getWidth() - 0, getHeight() - 40);
    helpButton->setBounds(48, getHeight() - 2 - 32, getWidth() - 96, 32);
    shadow->setBounds(48 + 0, (getHeight() - 2 - 32) + -1, (getWidth() - 96) - 0, 11);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void TranslationSettings::buttonClicked(Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == helpButton.get())
    {
        //[UserButtonCode_helpButton] -- add your button handler code here..
        URL updateUrl(Routes::Web::baseURL + Routes::Web::translationsURL);
        updateUrl.launchInDefaultBrowser();
        //[/UserButtonCode_helpButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}


//[MiscUserCode]

void TranslationSettings::scrollToSelectedLocale()
{
    int selectedRow = 0;
    for (int i = 0; i < this->availableTranslations.size(); ++i)
    {
        if (this->availableTranslations[i] == this->currentTranslation)
        {
            selectedRow = i;
            break;
        }
    }

    this->translationsList->scrollToEnsureRowIsOnscreen(selectedRow);
}

//===----------------------------------------------------------------------===//
// ChangeListener
//===----------------------------------------------------------------------===//

void TranslationSettings::changeListenerCallback(ChangeBroadcaster *source)
{
    this->availableTranslations = App::Config().getTranslations()->getAll();
    this->currentTranslation = App::Config().getTranslations()->getCurrent();
    this->translationsList->updateContent();
    this->scrollToSelectedLocale();
}

//===----------------------------------------------------------------------===//
// ListBoxModel
//===----------------------------------------------------------------------===//

Component *TranslationSettings::refreshComponentForRow(int rowNumber,
    bool isRowSelected, Component *existingComponentToUpdate)
{
    if (rowNumber >= this->availableTranslations.size())
    {
        return existingComponentToUpdate;
    }

    const bool isLastRow = (rowNumber == this->availableTranslations.size() - 1);
    const String localeName(this->availableTranslations[rowNumber]->getName());
    const String localeId(this->availableTranslations[rowNumber]->getId());
    const bool isCurrentLocale = this->availableTranslations[rowNumber] == this->currentTranslation;

    if (existingComponentToUpdate != nullptr)
    {
        if (auto *row = dynamic_cast<TranslationSettingsItem *>(existingComponentToUpdate))
        {
            row->updateDescription(isLastRow, isCurrentLocale, localeName, localeId);
        }
    }
    else
    {
        auto *row = new TranslationSettingsItem(*this->translationsList);
        row->updateDescription(isLastRow, isCurrentLocale, localeName, localeId);
        return row;
    }

    return existingComponentToUpdate;
}

int TranslationSettings::getNumRows()
{
    return this->availableTranslations.size();
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TranslationSettings" template="../../../Template"
                 componentName="" parentClasses="public Component, public ListBoxModel, private ChangeListener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="600"
                 initialHeight="265">
  <BACKGROUND backgroundColour="4d4d4d"/>
  <GENERICCOMPONENT name="" id="5005ba29a3a1bbc6" memberName="translationsList" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 40M" class="ListBox" params=""/>
  <TEXTBUTTON name="" id="c5bde2fac9c39997" memberName="helpButton" virtualName=""
              explicitFocusOrder="0" pos="48 2Rr 96M 32" buttonText="settings::language::help"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <JUCERCOMP name="" id="34270fb50cf926d8" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0 -1 0M 11" posRelativeX="c5bde2fac9c39997"
             posRelativeY="c5bde2fac9c39997" posRelativeW="c5bde2fac9c39997"
             sourceFile="../../Themes/SeparatorHorizontal.cpp" constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
