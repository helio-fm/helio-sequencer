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

#include "TranslationSettingsItem.h"

//[MiscUserDefs]
#include "Icons.h"
#include "SettingsListItemHighlighter.h"
#include "SettingsListItemSelection.h"
#include "Config.h"
//[/MiscUserDefs]

TranslationSettingsItem::TranslationSettingsItem(ListBox &parentListBox)
    : DraggingListBoxComponent(parentListBox.getViewport())
{
    this->localeLabel.reset(new Label(String(),
                                       TRANS("...")));
    this->addAndMakeVisible(localeLabel.get());
    this->localeLabel->setFont(Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    localeLabel->setJustificationType(Justification::centredLeft);
    localeLabel->setEditable(false, false, false);

    this->idLabel.reset(new Label(String(),
                                   TRANS("...")));
    this->addAndMakeVisible(idLabel.get());
    this->idLabel->setFont(Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    idLabel->setJustificationType(Justification::centredRight);
    idLabel->setEditable(false, false, false);

    this->separator.reset(new SeparatorHorizontal());
    this->addAndMakeVisible(separator.get());

    //[UserPreSize]
    this->selectionComponent = new SettingsListItemSelection();
    this->addChildComponent(this->selectionComponent);
    //[/UserPreSize]

    this->setSize(350, 32);

    //[Constructor]
    //[/Constructor]
}

TranslationSettingsItem::~TranslationSettingsItem()
{
    //[Destructor_pre]
    //this->selectionComponent = nullptr;
    //[/Destructor_pre]

    localeLabel = nullptr;
    idLabel = nullptr;
    separator = nullptr;

    //[Destructor]
    //[/Destructor]
}

void TranslationSettingsItem::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void TranslationSettingsItem::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    localeLabel->setBounds(48, 0, proportionOfWidth (0.3800f), getHeight() - 2);
    idLabel->setBounds(getWidth() - 12 - proportionOfWidth (0.4771f), 0, proportionOfWidth (0.4771f), getHeight() - 2);
    separator->setBounds(55, getHeight() - 2, getWidth() - 65, 2);
    //[UserResized] Add your own custom resize handling here..
    this->selectionComponent->setBounds(this->getLocalBounds());
    //[/UserResized]
}


//[MiscUserCode]

void TranslationSettingsItem::setSelected(bool shouldBeSelected)
{
    if (shouldBeSelected)
    {
        App::Config().getTranslations()->loadLocaleWithId(this->idLabel->getText());
        App::recreateLayout();
    }
}

void TranslationSettingsItem::updateDescription(bool isLastRowInList, bool isCurrentLocale,
    const String &localeName, const String &localeId)
{
    this->separator->setVisible(!isLastRowInList);
    this->localeLabel->setText(localeName, dontSendNotification);
    this->idLabel->setText(localeId, dontSendNotification);

    if (isCurrentLocale)
    {
        this->selectionAnimator.fadeIn(this->selectionComponent, 150);
    }
    else
    {
        this->selectionAnimator.fadeOut(this->selectionComponent, 50);
    }
}

Component *TranslationSettingsItem::createHighlighterComponent()
{
    return new SettingsListItemHighlighter();
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TranslationSettingsItem"
                 template="../../../Template" componentName="" parentClasses="public DraggingListBoxComponent"
                 constructorParams="ListBox &amp;parentListBox" variableInitialisers="DraggingListBoxComponent(parentListBox.getViewport())"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="350" initialHeight="32">
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="c261305e2de1ebf2" memberName="localeLabel" virtualName=""
         explicitFocusOrder="0" pos="48 0 38% 2M" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="21.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
  <LABEL name="" id="a7e8c6a3ddd9ea22" memberName="idLabel" virtualName=""
         explicitFocusOrder="0" pos="12Rr 0 47.714% 2M" labelText="..."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default serif font" fontsize="21.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="34"/>
  <JUCERCOMP name="" id="6f5a73e394d91c2a" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="55 0Rr 65M 2" sourceFile="../../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
