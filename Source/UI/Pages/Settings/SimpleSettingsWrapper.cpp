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

#include "SimpleSettingsWrapper.h"

//[MiscUserDefs]
#define MARGIN_X 4
#define MARGIN_Y 6
//[/MiscUserDefs]

SimpleSettingsWrapper::SimpleSettingsWrapper(Component *targetComponent)
{
    this->panel.reset(new FramePanel());
    this->addAndMakeVisible(panel.get());

    //[UserPreSize]
    this->setPaintingIsUnclipped(true);
    //[/UserPreSize]

    this->setSize(600, 400);

    //[Constructor]
    this->setEnabled(targetComponent->isEnabled() && targetComponent->getHeight() > 0);
    this->showNonOwned(targetComponent);
    //[/Constructor]
}

SimpleSettingsWrapper::~SimpleSettingsWrapper()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    panel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void SimpleSettingsWrapper::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SimpleSettingsWrapper::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    panel->setBounds(5, 6, getWidth() - 10, getHeight() - 12);
    //[UserResized] Add your own custom resize handling here..
    if (this->target != nullptr)
    {
        this->target->setBounds(this->panel->getBounds().reduced(MARGIN_X, MARGIN_Y));
    }
    //[/UserResized]
}


//[MiscUserCode]
void SimpleSettingsWrapper::showNonOwned(Component *targetComponent)
{
    this->target = targetComponent;
    this->addAndMakeVisible(this->target);
}

void SimpleSettingsWrapper::visibilityChanged()
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

<JUCER_COMPONENT documentType="Component" className="SimpleSettingsWrapper" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams="Component *targetComponent"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="563306a3a7769fb" memberName="panel" virtualName=""
             explicitFocusOrder="0" pos="5 6 10M 12M" sourceFile="../../Themes/FramePanel.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
