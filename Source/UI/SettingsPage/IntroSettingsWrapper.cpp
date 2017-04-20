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

#include "IntroSettingsWrapper.h"

//[MiscUserDefs]
//[/MiscUserDefs]

IntroSettingsWrapper::IntroSettingsWrapper()
    : hasPreviousTarget(false)
{
    addAndMakeVisible (targetBounds = new Component());


    //[UserPreSize]
    this->targetBounds->setVisible(false);
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
    //[/Constructor]
}

IntroSettingsWrapper::~IntroSettingsWrapper()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    targetBounds = nullptr;

    //[Destructor]
    //[/Destructor]
}

void IntroSettingsWrapper::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    if (this->target != nullptr)
    {
    //[/UserPrePaint]

    g.setGradientFill (ColourGradient (Colour (0x35000000),
                                       static_cast<float> ((getWidth() / 2)), static_cast<float> (getHeight()),
                                       Colour (0x00000000),
                                       0.0f, static_cast<float> (getHeight()),
                                       true));
    g.fillRect (0, getHeight() - 2 - 1, getWidth() - 0, 1);

    g.setGradientFill (ColourGradient (Colour (0x15ffffff),
                                       static_cast<float> ((getWidth() / 2)), static_cast<float> (getHeight()),
                                       Colour (0x00ffffff),
                                       0.0f, static_cast<float> (getHeight()),
                                       true));
    g.fillRect (0, getHeight() - 1 - 1, getWidth() - 0, 1);

    g.setGradientFill (ColourGradient (Colour (0x35000000),
                                       static_cast<float> ((getWidth() / 2)), 0.0f,
                                       Colour (0x00000000),
                                       0.0f, 0.0f,
                                       true));
    g.fillRect (0, 1, getWidth() - 0, 1);

    g.setGradientFill (ColourGradient (Colour (0x15ffffff),
                                       static_cast<float> ((getWidth() / 2)), 0.0f,
                                       Colour (0x00ffffff),
                                       0.0f, 0.0f,
                                       true));
    g.fillRect (0, 2, getWidth() - 0, 1);

    //[UserPaint] Add your own custom painting code here..
    }
    //[/UserPaint]
}

void IntroSettingsWrapper::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    targetBounds->setBounds (0, 2, getWidth() - 0, getHeight() - 4);
    //[UserResized] Add your own custom resize handling here..
    if (this->target != nullptr)
    {
        this->target->setBounds(this->targetBounds->getBounds());

        if ((this->target->getAlpha() < 1.f) || this->animator.isAnimating(this->target))
        {
            const int animTime = this->hasPreviousTarget ? 300 : 1000;
            this->animator.cancelAnimation(this->target, false);
            this->animator.animateComponent(this->target, this->target->getBounds(), 1.f, animTime, false, 0, 0);
        }
    }
    //[/UserResized]
}


//[MiscUserCode]
void IntroSettingsWrapper::showNonOwned(Component *targetComponent)
{
    this->hasPreviousTarget = (this->target != nullptr);

    if (this->hasPreviousTarget)
    {
        this->animator.cancelAllAnimations(false);
        this->animator.fadeOut(this->target, 300);
        this->removeChildComponent(this->target);
    }

    this->target = targetComponent;
    this->addAndMakeVisible(this->target);
    this->target->setAlpha(0.f);
    this->resized();
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="IntroSettingsWrapper" template="../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="hasPreviousTarget(false)" snapPixels="8"
                 snapActive="1" snapShown="1" overlayOpacity="0.330" fixedSize="1"
                 initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="0">
    <RECT pos="0 2Rr 0M 1" fill=" radial: 0C 0R, 0 0R, 0=35000000, 1=0"
          hasStroke="0"/>
    <RECT pos="0 1Rr 0M 1" fill=" radial: 0C 0R, 0 0R, 0=15ffffff, 1=ffffff"
          hasStroke="0"/>
    <RECT pos="0 1 0M 1" fill=" radial: 0C 0, 0 0, 0=35000000, 1=0" hasStroke="0"/>
    <RECT pos="0 2 0M 1" fill=" radial: 0C 0, 0 0, 0=15ffffff, 1=ffffff"
          hasStroke="0"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="" id="5005ba29a3a1bbc6" memberName="targetBounds" virtualName=""
                    explicitFocusOrder="0" pos="0 2 0M 4M" class="Component" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
