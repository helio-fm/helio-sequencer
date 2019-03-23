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

#include "HeadlineNavigationPanel.h"

//[MiscUserDefs]
#include "IconComponent.h"
#include "PanelBackgroundB.h"
#include "HeadlineDropdown.h"
#include "ColourIDs.h"
#include "Workspace.h"
//[/MiscUserDefs]

HeadlineNavigationPanel::HeadlineNavigationPanel()
{
    this->navigatePrevious.reset(new IconButton(Icons::findByName(Icons::back, 20), CommandIDs::ShowPreviousPage));
    this->addAndMakeVisible(navigatePrevious.get());

    this->navigateNext.reset(new IconButton(Icons::findByName(Icons::forward, 20), CommandIDs::ShowNextPage));
    this->addAndMakeVisible(navigateNext.get());

    this->component.reset(new HeadlineItemArrow());
    this->addAndMakeVisible(component.get());

    //[UserPreSize]
    this->bgColour = findDefaultColour(ColourIDs::BackgroundA::fill);
    this->setInterceptsMouseClicks(false, true);
    this->setPaintingIsUnclipped(true);
    //[/UserPreSize]

    this->setSize(60, 32);

    //[Constructor]
    //[/Constructor]
}

HeadlineNavigationPanel::~HeadlineNavigationPanel()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    navigatePrevious = nullptr;
    navigateNext = nullptr;
    component = nullptr;

    //[Destructor]
    //[/Destructor]
}

void HeadlineNavigationPanel::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    g.setColour(this->bgColour);
    g.fillRect(0.f, 0.f, float(this->getWidth() - 2), float(this->getHeight() - 2));
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void HeadlineNavigationPanel::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    navigatePrevious->setBounds(0, (getHeight() / 2) + -1 - (32 / 2), 25, 32);
    navigateNext->setBounds(21, (getHeight() / 2) + -1 - (32 / 2), 25, 32);
    component->setBounds(getWidth() - 16, 0, 16, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void HeadlineNavigationPanel::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    switch (commandId)
    {
    case CommandIDs::ShowPreviousPage:
        App::Workspace().navigateBackwardIfPossible();
        break;
    case CommandIDs::ShowNextPage:
        App::Workspace().navigateForwardIfPossible();
        break;
    default:
        break;
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
void HeadlineNavigationPanel::updateState(bool canGoPrevious, bool canGoNext)
{
    this->navigatePrevious->setInterceptsMouseClicks(canGoPrevious, false);
    this->navigatePrevious->setAlpha(canGoPrevious ? 0.45f : 0.2f);
    this->navigateNext->setInterceptsMouseClicks(canGoNext, false);
    this->navigateNext->setAlpha(canGoNext ? 0.45f : 0.2f);
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="HeadlineNavigationPanel"
                 template="../../Template" componentName="" parentClasses="public Component"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="60"
                 initialHeight="32">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="88e1e92c7548ba42" memberName="navigatePrevious" virtualName=""
                    explicitFocusOrder="0" pos="0 -1Cc 25 32" class="IconButton"
                    params="Icons::findByName(Icons::back, 20), CommandIDs::ShowPreviousPage"/>
  <GENERICCOMPONENT name="" id="900658e63c264259" memberName="navigateNext" virtualName=""
                    explicitFocusOrder="0" pos="21 -1Cc 25 32" class="IconButton"
                    params="Icons::findByName(Icons::forward, 20), CommandIDs::ShowNextPage"/>
  <JUCERCOMP name="" id="6845054f3705e31" memberName="component" virtualName=""
             explicitFocusOrder="0" pos="0Rr 0 16 0M" sourceFile="HeadlineItemArrow.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
