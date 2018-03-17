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

#include "AuthSettings.h"

//[MiscUserDefs]

// todo

#include "App.h"
#include "MainWindow.h"
#include "SessionService.h"
#include "ShapeComponent.h"
#include "Config.h"
#include "SerializationKeys.h"
//[/MiscUserDefs]

AuthSettings::AuthSettings()
{
    addAndMakeVisible (emailEditor = new Label ("emailEditor",
                                                TRANS("...")));
    emailEditor->setFont (Font (Font::getDefaultSerifFontName(), 37.00f, Font::plain).withTypefaceStyle ("Regular"));
    emailEditor->setJustificationType (Justification::centredLeft);
    emailEditor->setEditable (true, true, false);
    emailEditor->addListener (this);

    addAndMakeVisible (authorLabel = new Label (String(),
                                                TRANS("dialog::auth::email")));
    authorLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    authorLabel->setJustificationType (Justification::centredRight);
    authorLabel->setEditable (false, false, false);

    addAndMakeVisible (passwordEditor = new Label (String(),
                                                   TRANS("dialog::auth::defaultlogin::mobile")));
    passwordEditor->setFont (Font (Font::getDefaultSerifFontName(), 37.00f, Font::plain).withTypefaceStyle ("Regular"));
    passwordEditor->setJustificationType (Justification::centredLeft);
    passwordEditor->setEditable (true, true, false);
    passwordEditor->addListener (this);

    addAndMakeVisible (passwordLabel = new Label (String(),
                                                  TRANS("dialog::auth::password")));
    passwordLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    passwordLabel->setJustificationType (Justification::centredRight);
    passwordLabel->setEditable (false, false, false);


    //[UserPreSize]
    const String lastLogin = Config::get(Serialization::Config::lastUsedLogin);
#if HELIO_DESKTOP
    const String defaultLogin = TRANS("dialog::auth::defaultlogin::desktop");
#elif HELIO_MOBILE
    const String defaultLogin = TRANS("dialog::auth::defaultlogin::mobile");
#endif

    this->emailEditor->setText(lastLogin.isEmpty() ? defaultLogin : lastLogin, sendNotificationSync);
    this->emailEditor->setKeyboardType(TextInputTarget::emailAddressKeyboard);
    //[/UserPreSize]

    setSize (600, 128);

    //[Constructor]
    //[/Constructor]
}

AuthSettings::~AuthSettings()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    emailEditor = nullptr;
    authorLabel = nullptr;
    passwordEditor = nullptr;
    passwordLabel = nullptr;

    //[Destructor]
    //[/Destructor]
}

void AuthSettings::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AuthSettings::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    emailEditor->setBounds ((getWidth() / 2) + 58 - (317 / 2), (getHeight() / 2) + -28 - (40 / 2), 317, 40);
    authorLabel->setBounds ((getWidth() / 2) + -164 - (113 / 2), (getHeight() / 2) + -37 - (22 / 2), 113, 22);
    passwordEditor->setBounds ((getWidth() / 2) + 58 - (317 / 2), (getHeight() / 2) + 28 - (40 / 2), 317, 40);
    passwordLabel->setBounds ((getWidth() / 2) + -164 - (113 / 2), (getHeight() / 2) + 19 - (22 / 2), 113, 22);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AuthSettings::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == emailEditor)
    {
        //[UserLabelCode_emailEditor] -- add your label text handling code here..
        //[/UserLabelCode_emailEditor]
    }
    else if (labelThatHasChanged == passwordEditor)
    {
        //[UserLabelCode_passwordEditor] -- add your label text handling code here..
        //[/UserLabelCode_passwordEditor]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}

void AuthSettings::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    //[/UserCode_visibilityChanged]
}


//[MiscUserCode]

//void AuthSettings::changeListenerCallback(ChangeBroadcaster *source)
//{
//    AuthorizationManager *authManager = App::Helio()->getAuthManager();
//    authManager->removeChangeListener(this);
//
//    Component *progressIndicator = App::Helio()->getWorkspace()->findChildWithID(ProgressTooltip::componentId);
//
//    if (progressIndicator)
//    {
//        delete progressIndicator;
//
//        if (authManager->getLastRequestState() == AuthorizationManager::RequestSucceed)
//        {
//            App::Helio()->showModalComponent(new SuccessTooltip());
//            delete this;
//        }
//        else if (authManager->getLastRequestState() == AuthorizationManager::RequestFailed)
//        {
//            App::Helio()->showModalComponent(new FailTooltip());
//        }
//        if (authManager->getLastRequestState() == AuthorizationManager::ConnectionFailed)
//        {
//            App::Helio()->showModalComponent(new FailTooltip());
//        }
//    }
//}

//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AuthSettings" template="../../../Template"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="600" initialHeight="128">
  <METHODS>
    <METHOD name="visibilityChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="4d4d4d"/>
  <LABEL name="emailEditor" id="9c63b5388edfe183" memberName="emailEditor"
         virtualName="" explicitFocusOrder="0" pos="58.5Cc -28Cc 317 40"
         posRelativeY="e96b77baef792d3a" labelText="..." editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="37" kerning="0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="cf32360d33639f7f" memberName="authorLabel" virtualName=""
         explicitFocusOrder="0" pos="-163.5Cc -37Cc 113 22" posRelativeY="e96b77baef792d3a"
         labelText="dialog::auth::email" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="21"
         kerning="0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="fd5279a09ac6b052" memberName="passwordEditor" virtualName=""
         explicitFocusOrder="0" pos="58.5Cc 28Cc 317 40" posRelativeY="e96b77baef792d3a"
         labelText="dialog::auth::defaultlogin::mobile" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="37" kerning="0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="c134a00c2bb2de66" memberName="passwordLabel" virtualName=""
         explicitFocusOrder="0" pos="-163.5Cc 19Cc 113 22" posRelativeY="e96b77baef792d3a"
         labelText="dialog::auth::password" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="21"
         kerning="0" bold="0" italic="0" justification="34"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
