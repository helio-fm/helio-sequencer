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

#include "LabeledSettingsWrapper.h"

//[MiscUserDefs]
#define MARGIN_X 4
#define MARGIN_Y 6
//[/MiscUserDefs]

LabeledSettingsWrapper::LabeledSettingsWrapper(Component *targetComponent, const String &title)
{
    this->panel.reset(new FramePanel());
    this->addAndMakeVisible(panel.get());
    this->titleLabel.reset(new Label(String(),
                                      TRANS("...")));
    this->addAndMakeVisible(titleLabel.get());
    this->titleLabel->setFont(Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    titleLabel->setJustificationType(Justification::centredLeft);
    titleLabel->setEditable(false, false, false);

    titleLabel->setBounds(8, 8, 576, 26);


    //[UserPreSize]
    this->setPaintingIsUnclipped(true);
    //[/UserPreSize]

    this->setSize(600, 400);

    //[Constructor]
    this->setEnabled(targetComponent->isEnabled() && targetComponent->getHeight() > 0);
    this->showNonOwned(targetComponent, title);
    //[/Constructor]
}

LabeledSettingsWrapper::~LabeledSettingsWrapper()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    panel = nullptr;
    titleLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void LabeledSettingsWrapper::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void LabeledSettingsWrapper::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    panel->setBounds(5, 40, getWidth() - 10, getHeight() - 48);
    //[UserResized] Add your own custom resize handling here..
    if (this->target != nullptr)
    {
        this->target->setBounds(this->panel->getBounds().reduced(MARGIN_X, MARGIN_Y));
    }
    //[/UserResized]
}


//[MiscUserCode]
void LabeledSettingsWrapper::showNonOwned(Component *targetComponent, const String &title)
{
    if (this->target != nullptr)
    {
        this->removeChildComponent(this->target);
    }

    this->target = targetComponent;
    this->addAndMakeVisible(this->target);
    this->titleLabel->setText(title, dontSendNotification);
}

void LabeledSettingsWrapper::visibilityChanged()
{
    if (this->isVisible() && this->target != nullptr)
    {
        const int staticSpaceDelta = this->getHeight() - this->panel->getHeight() + MARGIN_Y * 2;
        this->setSize(this->getWidth(), this->target->getHeight() + staticSpaceDelta);
    }
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LabeledSettingsWrapper" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="Component *targetComponent, const String &amp;title"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="563306a3a7769fb" memberName="panel" virtualName=""
             explicitFocusOrder="0" pos="5 40 10M 48M" sourceFile="../../Themes/FramePanel.cpp"
             constructorParams=""/>
  <LABEL name="" id="9f16871b637bd1bd" memberName="titleLabel" virtualName=""
         explicitFocusOrder="0" pos="8 8 576 26" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="21.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
