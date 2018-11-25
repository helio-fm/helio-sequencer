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
#include "Config.h"
#include "AppInfoDto.h"
#include "SerializationKeys.h"
//[/Headers]

#include "UpdatesInfoComponent.h"

//[MiscUserDefs]
//[/MiscUserDefs]

UpdatesInfoComponent::UpdatesInfoComponent()
{
    this->label.reset(new Label(String(),
                                 TRANS("Updates info")));
    this->addAndMakeVisible(label.get());
    this->label->setFont(Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType(Justification::centred);
    label->setEditable(false, false, false);


    //[UserPreSize]

    // here we assume that backend response will only contain
    // the updates info for the current platform, so that
    // we don't need to filter it:

    AppInfoDto updatesInfo;
    Config::load(updatesInfo, Serialization::Config::lastUpdatesInfo);
    Array<AppVersionDto> versions = updatesInfo.getVersions();

    for (const auto &version : versions)
    {
        this->label->setText(this->label->getText() + "\n" + version.getHumanReadableDescription(), dontSendNotification);
    }

    //[/UserPreSize]

    this->setSize(256, 128);

    //[Constructor]
    //[/Constructor]
}

UpdatesInfoComponent::~UpdatesInfoComponent()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    label = nullptr;

    //[Destructor]
    //[/Destructor]
}

void UpdatesInfoComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void UpdatesInfoComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    label->setBounds(8, 0, getWidth() - 16, getHeight() - 0);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="UpdatesInfoComponent" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="256" initialHeight="128">
  <BACKGROUND backgroundColour="0"/>
  <LABEL name="" id="94cfd29b32ea20f9" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="8 0 16M 0M" labelText="Updates info"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
