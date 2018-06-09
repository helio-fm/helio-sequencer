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

#include "SignInRow.h"

//[MiscUserDefs]

#include "App.h"
#include "SessionService.h"
#include "Icons.h"
#include "IconComponent.h"
#include "DashboardMenu.h"
#include "CommandIDs.h"

class SignInHighlighter : public Component
{
public:

    SignInHighlighter()
    {
        this->setInterceptsMouseClicks(false, false);
    }

    void paint(Graphics &g) override
    {
        g.setColour(Colours::white.withAlpha(0.025f));
        g.fillRoundedRectangle (5.0f, 6.0f, static_cast<float> (getWidth() - 10), static_cast<float> (getHeight() - 5), 2.000f);
    }
};

//[/MiscUserDefs]

SignInRow::SignInRow(Component &parentComponent, ListBox &parentListBox)
    : DraggingListBoxComponent(parentListBox.getViewport()),
      parent(parentComponent)
{
    addAndMakeVisible (actionLabel = new Label (String(),
                                                TRANS("...")));
    actionLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    actionLabel->setJustificationType (Justification::centredLeft);
    actionLabel->setEditable (false, false, false);

    addAndMakeVisible (descriptionLabel = new Label (String(),
                                                     TRANS("...")));
    descriptionLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    descriptionLabel->setJustificationType (Justification::topLeft);
    descriptionLabel->setEditable (false, false, false);

    addAndMakeVisible (loginImage = new IconComponent (Icons::login));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (350, 56);

    //[Constructor]
    this->updateContent();
    //[/Constructor]
}

SignInRow::~SignInRow()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    actionLabel = nullptr;
    descriptionLabel = nullptr;
    loginImage = nullptr;

    //[Destructor]
    //[/Destructor]
}

void SignInRow::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    if (0)
    {
    //[/UserPrePaint]

    {
        float x = 10.0f, y = 6.0f, width = static_cast<float> (getWidth() - 20), height = static_cast<float> (getHeight() - 5);
        Colour strokeColour = Colour (0x30000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 10.000f, 2.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    }
    //[/UserPaint]
}

void SignInRow::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    actionLabel->setBounds (54, 8, 274, 24);
    descriptionLabel->setBounds (54 + 0, 8 + 24, getWidth() - 76, 18);
    loginImage->setBounds (16, (getHeight() / 2) + 2 - (24 / 2), 24, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


//[MiscUserCode]

Component *SignInRow::createHighlighterComponent()
{
    return new SignInHighlighter();
}

void SignInRow::setSelected(bool shouldBeSelected)
{
    this->parent.postCommandMessage(CommandIDs::LoginLogout);
}

void SignInRow::updateContent()
{
    const SessionService *authManager = App::Helio().getSessionService();
    const bool isLoggedIn = SessionService::isLoggedIn();
    this->actionLabel->setText(isLoggedIn ? TRANS("menu::workspace::logout") : TRANS("menu::workspace::login"), dontSendNotification);
    this->descriptionLabel->setText(isLoggedIn ? authManager->getUserProfile().getName() : TRANS("menu::workspace::login::hint"), dontSendNotification);
}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SignInRow" template="../../../../Template"
                 componentName="" parentClasses="public DraggingListBoxComponent"
                 constructorParams="Component &amp;parentComponent, ListBox &amp;parentListBox"
                 variableInitialisers="DraggingListBoxComponent(parentListBox.getViewport()),&#10;parent(parentComponent)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="350" initialHeight="56">
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="10 6 20M 5M" cornerSize="10" fill="solid: 0" hasStroke="1"
               stroke="2, mitered, butt" strokeColour="solid: 30000000"/>
  </BACKGROUND>
  <LABEL name="" id="c261305e2de1ebf2" memberName="actionLabel" virtualName=""
         explicitFocusOrder="0" pos="54 8 274 24" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="21" kerning="0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="12427a53408d61ee" memberName="descriptionLabel" virtualName=""
         explicitFocusOrder="0" pos="0 0R 76M 18" posRelativeX="c261305e2de1ebf2"
         posRelativeY="c261305e2de1ebf2" labelText="..." editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="16" kerning="0" bold="0" italic="0" justification="9"/>
  <GENERICCOMPONENT name="" id="42f3ed34561654ef" memberName="loginImage" virtualName=""
                    explicitFocusOrder="0" pos="16 2Cc 24 24" class="IconComponent"
                    params="Icons::login"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
