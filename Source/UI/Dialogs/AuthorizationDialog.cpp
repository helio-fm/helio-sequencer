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

#include "AuthorizationDialog.h"

//[MiscUserDefs]
#include "HelioTheme.h"
#include "ComponentIDs.h"

#include "App.h"
#include "SessionService.h"
#include "ProgressTooltip.h"
#include "SuccessTooltip.h"
#include "FailTooltip.h"
#include "MainLayout.h"
#include "Config.h"
#include "SerializationKeys.h"

class LabelWithPassword : public Label
{
public:

    LabelWithPassword() : passwordCharacter(0) {}

    void setPasswordCharacter (const juce_wchar newPasswordCharacter)
    {
        if (passwordCharacter != newPasswordCharacter)
        {
            passwordCharacter = newPasswordCharacter;
            this->repaint();
        }
    }

    juce_wchar getPasswordCharacter() const noexcept
    {
        return this->passwordCharacter;
    }

    void paint (Graphics &g) override
    {
        HelioTheme &ht = static_cast<HelioTheme &>(getLookAndFeel());
        ht.drawLabel (g, *this, getPasswordCharacter());
    }

protected:

    static void copyColourIfSpecified (Label& l, TextEditor& ed, int colourID, int targetColourID)
    {
        if (l.isColourSpecified (colourID) || l.getLookAndFeel().isColourSpecified (colourID))
            ed.setColour (targetColourID, l.findColour (colourID));
    }

    TextEditor *createEditorComponent() override
    {
        TextEditor* const ed = new TextEditor (getName(), this->passwordCharacter);
        ed->applyFontToAllText (getLookAndFeel().getLabelFont (*this));
        copyAllExplicitColoursTo (*ed);

        copyColourIfSpecified (*this, *ed, textWhenEditingColourId, TextEditor::textColourId);
        copyColourIfSpecified (*this, *ed, backgroundWhenEditingColourId, TextEditor::backgroundColourId);
        copyColourIfSpecified (*this, *ed, outlineWhenEditingColourId, TextEditor::focusedOutlineColourId);

        return ed;
    }

private:

    juce_wchar passwordCharacter;

};
//[/MiscUserDefs]

AuthorizationDialog::AuthorizationDialog()
{
    addAndMakeVisible (background = new DialogPanel());
    addAndMakeVisible (loginButton = new TextButton (String()));
    loginButton->setButtonText (TRANS("dialog::auth::proceed"));
    loginButton->setConnectedEdges (Button::ConnectedOnLeft | Button::ConnectedOnTop);
    loginButton->addListener (this);

    addAndMakeVisible (emailEditor = new Label (String(),
                                                TRANS("...")));
    emailEditor->setFont (Font (Font::getDefaultSerifFontName(), 37.00f, Font::plain).withTypefaceStyle ("Regular"));
    emailEditor->setJustificationType (Justification::centredLeft);
    emailEditor->setEditable (true, true, false);
    emailEditor->addListener (this);

    addAndMakeVisible (emailLabel = new Label (String(),
                                               TRANS("dialog::auth::email")));
    emailLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    emailLabel->setJustificationType (Justification::topRight);
    emailLabel->setEditable (false, false, false);

    addAndMakeVisible (passwordLabel = new Label (String(),
                                                  TRANS("dialog::auth::password")));
    passwordLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    passwordLabel->setJustificationType (Justification::topRight);
    passwordLabel->setEditable (false, false, false);

    addAndMakeVisible (cancelButton = new TextButton (String()));
    cancelButton->setButtonText (TRANS("dialog::auth::cancel"));
    cancelButton->setConnectedEdges (Button::ConnectedOnRight | Button::ConnectedOnTop);
    cancelButton->addListener (this);

    addAndMakeVisible (passwordEditor = new LabelWithPassword());

    addAndMakeVisible (separatorH = new SeparatorHorizontal());
    addAndMakeVisible (separatorV = new SeparatorVertical());

    //[UserPreSize]
    const String lastLogin = Config::get(Serialization::Config::lastUsedLogin);
#if HELIO_DESKTOP
    const String defaultLogin = TRANS("dialog::auth::defaultlogin::desktop");
#elif HELIO_MOBILE
    const String defaultLogin = TRANS("dialog::auth::defaultlogin::mobile");
#endif

    this->emailEditor->setText(lastLogin.isEmpty() ? defaultLogin : lastLogin, sendNotificationSync);
    this->emailEditor->setKeyboardType(TextInputTarget::emailAddressKeyboard);
    //this->messageLabel->setText(message, dontSendNotification);

    this->passwordEditor->setFont (Font (Font::getDefaultSerifFontName(), 37.00f, Font::plain).withTypefaceStyle ("Regular"));
    this->passwordEditor->setJustificationType (Justification::centredLeft);
    this->passwordEditor->setEditable (true, true, false);
    this->passwordEditor->setColour (Label::textColourId, Colours::white);
    this->passwordEditor->setColour (TextEditor::textColourId, Colours::black);
    this->passwordEditor->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    this->passwordEditor->addListener (this);
    this->passwordEditor->setPasswordCharacter(static_cast<juce_wchar>(0x00B7));
    this->passwordEditor->setText("111", sendNotification);

    this->separatorH->setAlphaMultiplier(2.5f);
    //[/UserPreSize]

    setSize (530, 210);

    //[Constructor]
    this->rebound();
    //[/Constructor]
}

AuthorizationDialog::~AuthorizationDialog()
{
    //[Destructor_pre]
    FadingDialog::fadeOut();
    //[/Destructor_pre]

    background = nullptr;
    loginButton = nullptr;
    emailEditor = nullptr;
    emailLabel = nullptr;
    passwordLabel = nullptr;
    cancelButton = nullptr;
    passwordEditor = nullptr;
    separatorH = nullptr;
    separatorV = nullptr;

    //[Destructor]
    //[/Destructor]
}

void AuthorizationDialog::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        float x = 0.0f, y = 0.0f, width = static_cast<float> (getWidth() - 0), height = static_cast<float> (getHeight() - 0);
        Colour fillColour = Colour (0x59000000);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 10.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AuthorizationDialog::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds ((getWidth() / 2) - ((getWidth() - 8) / 2), 4, getWidth() - 8, getHeight() - 8);
    loginButton->setBounds (getWidth() - 4 - 390, getHeight() - 4 - 48, 390, 48);
    emailEditor->setBounds ((getWidth() / 2) + 54 - (369 / 2), 4 + 32, 369, 40);
    emailLabel->setBounds ((getWidth() / 2) + -194 - (111 / 2), 4 + 24, 111, 47);
    passwordLabel->setBounds ((getWidth() / 2) + -194 - (111 / 2), 4 + 84, 111, 51);
    cancelButton->setBounds (4, getHeight() - 4 - 48, 131, 48);
    passwordEditor->setBounds ((getWidth() / 2) + 54 - (369 / 2), 92, 369, 40);
    separatorH->setBounds (4, getHeight() - 52 - 2, getWidth() - 8, 2);
    separatorV->setBounds (134, getHeight() - 4 - 48, 2, 48);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AuthorizationDialog::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == loginButton)
    {
        //[UserButtonCode_loginButton] -- add your button handler code here..
        this->login();
        //[/UserButtonCode_loginButton]
    }
    else if (buttonThatWasClicked == cancelButton)
    {
        //[UserButtonCode_cancelButton] -- add your button handler code here..
        delete this;
        //[/UserButtonCode_cancelButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void AuthorizationDialog::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == emailEditor)
    {
        //[UserLabelCode_emailEditor] -- add your label text handling code here..
        //[/UserLabelCode_emailEditor]
    }

    //[UserlabelTextChanged_Post]
    this->loginButton->setEnabled(this->validateTextFields());
    //[/UserlabelTextChanged_Post]
}

void AuthorizationDialog::parentHierarchyChanged()
{
    //[UserCode_parentHierarchyChanged] -- Add your code here...
    this->rebound();
    //[/UserCode_parentHierarchyChanged]
}

void AuthorizationDialog::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
    this->rebound();
    //[/UserCode_parentSizeChanged]
}

bool AuthorizationDialog::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    if (key.isKeyCode(KeyPress::returnKey))
    {
        if (this->validateTextFields())
        {
            this->login();
        }

        return true;
    }
    else if (key.isKeyCode(KeyPress::tabKey))
    {
        // don't delete self
        return true;
    }

    return false;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
    //[/UserCode_keyPressed]
}

void AuthorizationDialog::inputAttemptWhenModal()
{
    //[UserCode_inputAttemptWhenModal] -- Add your code here...
    delete this;
    //[/UserCode_inputAttemptWhenModal]
}


//[MiscUserCode]

void AuthorizationDialog::editorShown(Label *targetLabel, TextEditor &editor)
{
    //String::repeatedString(String::charToString (passwordCharacter), atomText.length())

//    if (targetLabel == this->passwordEditor)
//    {
//        editor.setPasswordCharacter((juce_wchar)'*');
//    }
}

void AuthorizationDialog::login()
{
    Config::set(Serialization::Config::lastUsedLogin, this->emailEditor->getText());

    const String passwordHash = SHA256(this->passwordEditor->getText().toUTF8()).toHexString();
    const String email = this->emailEditor->getText();

    App::Layout().showModalComponentUnowned(new ProgressTooltip());
    App::Helio().getSessionService()->signIn(email, passwordHash, [this](bool success, const Array<String> &errors)
    {
        if (ProgressTooltip *modalComponent =
            dynamic_cast<ProgressTooltip *>(Component::getCurrentlyModalComponent()))
        {
            delete modalComponent;
        }

        if (success)
        {
            App::Layout().showModalComponentUnowned(new SuccessTooltip());
            delete this;
        }
        else
        {
            App::Layout().showModalComponentUnowned(new FailTooltip());
            if (!errors.isEmpty())
            {
                App::Layout().showTooltip(errors.getFirst());
            }
        }
    });
}

bool AuthorizationDialog::validateTextFields() const
{
    // simple asf
    const bool hasValidEmail = (this->emailEditor->getText().isNotEmpty() &&
                                this->emailEditor->getText().contains("@") &&
                                this->emailEditor->getText().contains("."));

    const bool hasValidPassword = (this->passwordEditor->getText().length() >= 6);

    return hasValidEmail && hasValidPassword;
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AuthorizationDialog" template="../../Template"
                 componentName="" parentClasses="public FadingDialog" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="530" initialHeight="210">
  <METHODS>
    <METHOD name="parentHierarchyChanged()"/>
    <METHOD name="parentSizeChanged()"/>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="inputAttemptWhenModal()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0">
    <ROUNDRECT pos="0 0 0M 0M" cornerSize="10.00000000000000000000" fill="solid: 59000000"
               hasStroke="0"/>
  </BACKGROUND>
  <JUCERCOMP name="" id="e96b77baef792d3a" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0Cc 4 8M 8M" posRelativeH="ac3897c4f32c4354"
             sourceFile="../Themes/DialogPanel.cpp" constructorParams=""/>
  <TEXTBUTTON name="" id="7855caa7c65c5c11" memberName="loginButton" virtualName=""
              explicitFocusOrder="0" pos="4Rr 4Rr 390 48" buttonText="dialog::auth::proceed"
              connectedEdges="5" needsCallback="1" radioGroupId="0"/>
  <LABEL name="" id="9c63b5388edfe183" memberName="emailEditor" virtualName=""
         explicitFocusOrder="0" pos="54.5Cc 32 369 40" posRelativeY="e96b77baef792d3a"
         labelText="..." editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="37.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="cf32360d33639f7f" memberName="emailLabel" virtualName=""
         explicitFocusOrder="0" pos="-193.5Cc 24 111 47" posRelativeY="e96b77baef792d3a"
         labelText="dialog::auth::email" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="21.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="c134a00c2bb2de66" memberName="passwordLabel" virtualName=""
         explicitFocusOrder="0" pos="-193.5Cc 84 111 51" posRelativeY="e96b77baef792d3a"
         labelText="dialog::auth::password" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="21.00000000000000000000"
         kerning="0.00000000000000000000" bold="0" italic="0" justification="10"/>
  <TEXTBUTTON name="" id="27c5d30533a1f7a9" memberName="cancelButton" virtualName=""
              explicitFocusOrder="0" pos="4 4Rr 131 48" buttonText="dialog::auth::cancel"
              connectedEdges="6" needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="" id="ac81a17122003703" memberName="passwordEditor" virtualName=""
                    explicitFocusOrder="0" pos="54.5Cc 92 369 40" class="LabelWithPassword"
                    params=""/>
  <JUCERCOMP name="" id="e39d9e103e2a60e6" memberName="separatorH" virtualName=""
             explicitFocusOrder="0" pos="4 52Rr 8M 2" sourceFile="../Themes/SeparatorHorizontal.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="1fb927654787aaf4" memberName="separatorV" virtualName=""
             explicitFocusOrder="0" pos="134 4Rr 2 48" sourceFile="../Themes/SeparatorVertical.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
