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
#include "MenuPanel.h"
#include "AppInfoDto.h"
#include "SerializationKeys.h"
#include "PanelBackgroundA.h"
//[/Headers]

#include "UpdatesInfoComponent.h"

//[MiscUserDefs]
//[/MiscUserDefs]

UpdatesInfoComponent::UpdatesInfoComponent()
{
    this->comboPrimer.reset(new MobileComboBox::Primer());
    this->addAndMakeVisible(comboPrimer.get());

    this->label.reset(new Label(String(),
                                 TRANS("update::proceed")));
    this->addAndMakeVisible(label.get());
    this->label->setFont(Font (17.00f, Font::plain).withTypefaceStyle ("Regular"));
    label->setJustificationType(Justification::centredBottom);
    label->setEditable(false, false, false);


    //[UserPreSize]

    // here we assume that backend response will only contain
    // the updates info for the current platform, so that
    // we don't need to filter them,

    // but
    // the returned latest version can still be the same
    // as current one, so filtering them anyway:

    AppInfoDto appInfo;
    App::Config().load(&appInfo, Serialization::Config::lastUpdatesInfo);

    bool hasNewerStable = false;
    for (const auto &v : appInfo.getVersions())
    {
        if (v.isLaterThanCurrentVersion())
        {
            this->versions.add(v);
            hasNewerStable = hasNewerStable || !v.isDevelopmentBuild();
        }
    }

    if (hasNewerStable)
    {
        //this->label->setText("v" + App::getAppReadableVersion(), dontSendNotification);
        this->label->setAlpha(0.5f);

        MenuPanel::Menu menu;
        for (int i = 0; i < this->versions.size(); ++i)
        {
            const auto &version = this->versions.getUnchecked(i);
            menu.add(MenuItem::item(Icons::helio, CommandIDs::SelectVersion + i,
                version.getHumanReadableDescription()));
        }

        this->comboPrimer->initWith(this->label.get(), menu, { new PanelBackgroundA() });
    }
    else
    {
        this->label->setVisible(false);
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

    comboPrimer = nullptr;
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

    comboPrimer->setBounds(0, 0, getWidth() - 0, getHeight() - 0);
    label->setBounds((getWidth() / 2) - ((getWidth() - 56) / 2), 2, getWidth() - 56, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void UpdatesInfoComponent::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    const int idx = commandId - CommandIDs::SelectVersion;
    if (idx >= 0 && idx < this->versions.size())
    {
        // what TODO here instead of just opening a browser?
        const auto version = this->versions.getReference(idx);
        jassert(version.getLink().isNotEmpty());
        URL(version.getLink()).launchInDefaultBrowser();
    }
    //[/UserCode_handleCommandMessage]
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
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <GENERICCOMPONENT name="" id="524df900a9089845" memberName="comboPrimer" virtualName=""
                    explicitFocusOrder="0" pos="0 0 0M 0M" class="MobileComboBox::Primer"
                    params=""/>
  <LABEL name="" id="94cfd29b32ea20f9" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="0Cc 2 56M 24" labelText="update::proceed"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="17.00000000000000000000" kerning="0.00000000000000000000"
         bold="0" italic="0" justification="20"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
