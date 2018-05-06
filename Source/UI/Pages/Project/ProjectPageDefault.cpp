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

#include "ProjectPageDefault.h"

//[MiscUserDefs]
#include "VersionControlTreeItem.h"
#include "PlayerThread.h"
#include "ProjectTreeItem.h"
#include "ProjectInfo.h"
#include "HelioTheme.h"
#include "HelioCallout.h"
#include "App.h"
#include "ProjectMenu.h"
#include "CommandIDs.h"
//[/MiscUserDefs]

ProjectPageDefault::ProjectPageDefault(ProjectTreeItem &parentProject)
    : ProjectPage(parentProject)
{
    addAndMakeVisible (background = new PanelBackgroundB());
    addAndMakeVisible (projectTitleEditor = new Label (String(),
                                                       TRANS("...")));
    projectTitleEditor->setFont (Font (Font::getDefaultSerifFontName(), 37.00f, Font::plain).withTypefaceStyle ("Regular"));
    projectTitleEditor->setJustificationType (Justification::topLeft);
    projectTitleEditor->setEditable (true, true, false);
    projectTitleEditor->addListener (this);

    addAndMakeVisible (projectTitleLabel = new Label (String(),
                                                      TRANS("page::project::title")));
    projectTitleLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    projectTitleLabel->setJustificationType (Justification::topRight);
    projectTitleLabel->setEditable (false, false, false);

    addAndMakeVisible (authorEditor = new Label (String(),
                                                 TRANS("...")));
    authorEditor->setFont (Font (Font::getDefaultSerifFontName(), 37.00f, Font::plain).withTypefaceStyle ("Regular"));
    authorEditor->setJustificationType (Justification::topLeft);
    authorEditor->setEditable (true, true, false);
    authorEditor->addListener (this);

    addAndMakeVisible (authorLabel = new Label (String(),
                                                TRANS("page::project::author")));
    authorLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    authorLabel->setJustificationType (Justification::topRight);
    authorLabel->setEditable (false, false, false);

    addAndMakeVisible (descriptionEditor = new Label (String(),
                                                      TRANS("...")));
    descriptionEditor->setFont (Font (Font::getDefaultSerifFontName(), 37.00f, Font::plain).withTypefaceStyle ("Regular"));
    descriptionEditor->setJustificationType (Justification::topLeft);
    descriptionEditor->setEditable (true, true, false);
    descriptionEditor->addListener (this);

    addAndMakeVisible (descriptionLabel = new Label (String(),
                                                     TRANS("page::project::description")));
    descriptionLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    descriptionLabel->setJustificationType (Justification::topRight);
    descriptionLabel->setEditable (false, false, false);

    addAndMakeVisible (locationLabel = new Label (String(),
                                                  TRANS("page::project::filelocation")));
    locationLabel->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    locationLabel->setJustificationType (Justification::topRight);
    locationLabel->setEditable (false, false, true);

    addAndMakeVisible (locationText = new Label (String(),
                                                 TRANS("...")));
    locationText->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    locationText->setJustificationType (Justification::topLeft);
    locationText->setEditable (false, false, true);

    addAndMakeVisible (contentStatsLabel = new Label (String(),
                                                      TRANS("page::project::stats::content")));
    contentStatsLabel->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    contentStatsLabel->setJustificationType (Justification::topRight);
    contentStatsLabel->setEditable (false, false, true);

    addAndMakeVisible (contentStatsText = new Label (String(),
                                                     TRANS("...")));
    contentStatsText->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    contentStatsText->setJustificationType (Justification::topLeft);
    contentStatsText->setEditable (false, false, true);

    addAndMakeVisible (vcsStatsLabel = new Label (String(),
                                                  TRANS("page::project::stats::vcs")));
    vcsStatsLabel->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    vcsStatsLabel->setJustificationType (Justification::topRight);
    vcsStatsLabel->setEditable (false, false, true);

    addAndMakeVisible (vcsStatsText = new Label (String(),
                                                 TRANS("...")));
    vcsStatsText->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    vcsStatsText->setJustificationType (Justification::topLeft);
    vcsStatsText->setEditable (false, false, true);

    addAndMakeVisible (startTimeLabel = new Label (String(),
                                                   TRANS("page::project::startdate")));
    startTimeLabel->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    startTimeLabel->setJustificationType (Justification::topRight);
    startTimeLabel->setEditable (false, false, true);

    addAndMakeVisible (startTimeText = new Label (String(),
                                                  TRANS("...")));
    startTimeText->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    startTimeText->setJustificationType (Justification::topLeft);
    startTimeText->setEditable (false, false, true);

    addAndMakeVisible (lengthLabel = new Label (String(),
                                                TRANS("page::project::duration")));
    lengthLabel->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    lengthLabel->setJustificationType (Justification::topRight);
    lengthLabel->setEditable (false, false, true);

    addAndMakeVisible (lengthText = new Label (String(),
                                               TRANS("...")));
    lengthText->setFont (Font (Font::getDefaultSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    lengthText->setJustificationType (Justification::topLeft);
    lengthText->setEditable (false, false, true);

    addAndMakeVisible (level1 = new Component());
    level1->setName ("level1");

    addAndMakeVisible (level2 = new Component());
    level2->setName ("level2");

    addAndMakeVisible (licenseLabel = new Label (String(),
                                                 TRANS("page::project::license")));
    licenseLabel->setFont (Font (Font::getDefaultSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
    licenseLabel->setJustificationType (Justification::topRight);
    licenseLabel->setEditable (false, false, true);

    addAndMakeVisible (licenseEditor = new Label (String(),
                                                  TRANS("...")));
    licenseEditor->setFont (Font (Font::getDefaultSerifFontName(), 37.00f, Font::plain).withTypefaceStyle ("Regular"));
    licenseEditor->setJustificationType (Justification::topLeft);
    licenseEditor->setEditable (true, true, false);
    licenseEditor->addListener (this);

    addAndMakeVisible (menuButton = new MenuButton());
    addAndMakeVisible (shadow = new LightShadowRightwards());
    addAndMakeVisible (revealLocationButton = new ImageButton (String()));
    revealLocationButton->addListener (this);

    revealLocationButton->setImages (false, true, false,
                                     Image(), 1.000f, Colour (0x00000000),
                                     Image(), 1.000f, Colour (0x00000000),
                                     Image(), 1.000f, Colour (0x00000000));

    //[UserPreSize]
    this->revealLocationButton->setMouseCursor(MouseCursor::PointingHandCursor);

    // TODO remove this button?
    this->menuButton->setVisible(false);
    //[/UserPreSize]

    setSize (600, 400);

    //[Constructor]
#if HELIO_MOBILE
    // не комильфо на мобильниках показывать расположение файлв
    this->locationLabel->setVisible(false);
    this->locationText->setVisible(false);
#endif
    //[/Constructor]
}

ProjectPageDefault::~ProjectPageDefault()
{
    //[Destructor_pre]
    //[/Destructor_pre]

    background = nullptr;
    projectTitleEditor = nullptr;
    projectTitleLabel = nullptr;
    authorEditor = nullptr;
    authorLabel = nullptr;
    descriptionEditor = nullptr;
    descriptionLabel = nullptr;
    locationLabel = nullptr;
    locationText = nullptr;
    contentStatsLabel = nullptr;
    contentStatsText = nullptr;
    vcsStatsLabel = nullptr;
    vcsStatsText = nullptr;
    startTimeLabel = nullptr;
    startTimeText = nullptr;
    lengthLabel = nullptr;
    lengthText = nullptr;
    level1 = nullptr;
    level2 = nullptr;
    licenseLabel = nullptr;
    licenseEditor = nullptr;
    menuButton = nullptr;
    shadow = nullptr;
    revealLocationButton = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ProjectPageDefault::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ProjectPageDefault::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    background->setBounds (0, 0, getWidth() - 0, getHeight() - 0);
    projectTitleEditor->setBounds ((getWidth() / 2) + -100, proportionOfHeight (0.0755f) + 20, 440, 48);
    projectTitleLabel->setBounds ((getWidth() / 2) + -104 - 202, proportionOfHeight (0.0755f) + 0, 202, 48);
    authorEditor->setBounds ((getWidth() / 2) + -100, proportionOfHeight (0.0755f) + 90, 440, 48);
    authorLabel->setBounds ((getWidth() / 2) + -104 - 202, proportionOfHeight (0.0755f) + 70, 202, 48);
    descriptionEditor->setBounds ((getWidth() / 2) + -100, proportionOfHeight (0.0755f) + 160, 440, 48);
    descriptionLabel->setBounds ((getWidth() / 2) + -104 - 202, proportionOfHeight (0.0755f) + 140, 202, 48);
    locationLabel->setBounds ((getWidth() / 2) + -104 - 300, (proportionOfHeight (0.0755f) + 310) + 138, 300, 32);
    locationText->setBounds ((getWidth() / 2) + -100, (proportionOfHeight (0.0755f) + 310) + 138, 400, 96);
    contentStatsLabel->setBounds ((getWidth() / 2) + -104 - 300, (proportionOfHeight (0.0755f) + 310) + 106, 300, 32);
    contentStatsText->setBounds ((getWidth() / 2) + -100, (proportionOfHeight (0.0755f) + 310) + 106, 400, 32);
    vcsStatsLabel->setBounds ((getWidth() / 2) + -104 - 300, (proportionOfHeight (0.0755f) + 310) + 76, 300, 32);
    vcsStatsText->setBounds ((getWidth() / 2) + -100, (proportionOfHeight (0.0755f) + 310) + 76, 400, 32);
    startTimeLabel->setBounds ((getWidth() / 2) + -104 - 300, (proportionOfHeight (0.0755f) + 310) + 46, 300, 32);
    startTimeText->setBounds ((getWidth() / 2) + -100, (proportionOfHeight (0.0755f) + 310) + 46, 400, 32);
    lengthLabel->setBounds ((getWidth() / 2) + -104 - 300, (proportionOfHeight (0.0755f) + 310) + 16, 300, 32);
    lengthText->setBounds ((getWidth() / 2) + -100, (proportionOfHeight (0.0755f) + 310) + 16, 400, 32);
    level1->setBounds (32, proportionOfHeight (0.0755f), 150, 24);
    level2->setBounds (32, proportionOfHeight (0.0755f) + 310, 150, 24);
    licenseLabel->setBounds ((getWidth() / 2) + -104 - 202, proportionOfHeight (0.0755f) + 210, 202, 48);
    licenseEditor->setBounds ((getWidth() / 2) + -100, proportionOfHeight (0.0755f) + 230, 440, 48);
    menuButton->setBounds ((getWidth() / 2) - (128 / 2), getHeight() - -16 - (128 / 2), 128, 128);
    shadow->setBounds (0, 0, 5, getHeight() - 0);
    revealLocationButton->setBounds (((getWidth() / 2) + -100) + -150, ((proportionOfHeight (0.0755f) + 310) + 138) + 0, 400 - -150, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ProjectPageDefault::labelTextChanged (Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == projectTitleEditor)
    {
        //[UserLabelCode_projectTitleEditor] -- add your label text handling code here..
        if (labelThatHasChanged->getText().isNotEmpty())
        {
            this->project.getProjectInfo()->setFullName(labelThatHasChanged->getText());
        }
        else
        {
            const String &fullname = this->project.getProjectInfo()->getFullName();
            labelThatHasChanged->setText(fullname, sendNotification);
        }
        //[/UserLabelCode_projectTitleEditor]
    }
    else if (labelThatHasChanged == authorEditor)
    {
        //[UserLabelCode_authorEditor] -- add your label text handling code here..
        this->project.getProjectInfo()->setAuthor(labelThatHasChanged->getText());
        //[/UserLabelCode_authorEditor]
    }
    else if (labelThatHasChanged == descriptionEditor)
    {
        //[UserLabelCode_descriptionEditor] -- add your label text handling code here..
        this->project.getProjectInfo()->setDescription(labelThatHasChanged->getText());
        //[/UserLabelCode_descriptionEditor]
    }
    else if (labelThatHasChanged == licenseEditor)
    {
        //[UserLabelCode_licenseEditor] -- add your label text handling code here..
        this->project.getProjectInfo()->setLicense(labelThatHasChanged->getText());
        //[/UserLabelCode_licenseEditor]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}

void ProjectPageDefault::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == revealLocationButton)
    {
        //[UserButtonCode_revealLocationButton] -- add your button handler code here..
        this->project.getDocument()->getFile().revealToUser();
        //[/UserButtonCode_revealLocationButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void ProjectPageDefault::handleCommandMessage (int commandId)
{
    //[UserCode_handleCommandMessage] -- Add your code here...
    if (commandId == CommandIDs::MenuButtonPressed)
    {
        MenuPanel *panel = new ProjectMenu(this->project, MenuPanel::SlideUp);
        HelioCallout::emit(panel, this->menuButton);
    }
    //[/UserCode_handleCommandMessage]
}


//[MiscUserCode]
void ProjectPageDefault::updateContent()
{
    const String &fullname = this->project.getProjectInfo()->getFullName();
    const String &author = this->project.getProjectInfo()->getAuthor();
    const String &description = this->project.getProjectInfo()->getDescription();
    const String &license = this->project.getProjectInfo()->getLicense();
    const String &startTime = App::getSqlFormattedTime(Time(this->project.getProjectInfo()->getStartTimestamp()));

#if HELIO_DESKTOP
    const String &clickToEdit = TRANS("page::project::default::value::desktop");
#elif HELIO_MOBILE
    const String &clickToEdit = TRANS("page::project::default::value::mobile");
#endif

    this->projectTitleEditor->setText(fullname.isEmpty() ? clickToEdit : fullname, dontSendNotification);
    this->authorEditor->setText(author.isEmpty() ? TRANS("page::project::default::author") : author, dontSendNotification);
    this->descriptionEditor->setText(description.isEmpty() ? clickToEdit : description, dontSendNotification);
    this->licenseEditor->setText(license.isEmpty() ? TRANS("page::project::default::license") : license, dontSendNotification);

    this->startTimeText->setText(startTime, dontSendNotification);
    this->locationText->setText(this->project.getDocument()->getFullPath(), dontSendNotification);
    this->contentStatsText->setText(this->project.getStats(), dontSendNotification);

    if (VersionControlTreeItem *vcti = this->project.findChildOfType<VersionControlTreeItem>())
    {
        this->vcsStatsText->setText(vcti->getStatsString(), dontSendNotification);
    }
}

static String getTimeString(const RelativeTime &time)
{
    String res;

    int n = std::abs(int(time.inMinutes()));

    if (n > 0)
    {
        res = res + TRANS_PLURAL("{x} minutes", n) + " ";
    }

    n = std::abs(int(time.inSeconds())) % 60;
    res = res + TRANS_PLURAL("{x} seconds", n) + " ";

    return res;
}

void ProjectPageDefault::onSeek(double absolutePosition,
    double currentTimeMs, double totalTimeMs)
{
    const RelativeTime totalTime(totalTimeMs / 1000.0);
    this->lengthText->setText(getTimeString(totalTime), dontSendNotification);
}

void ProjectPageDefault::onTotalTimeChanged(double timeMs)
{
    const RelativeTime totalTime(timeMs / 1000.0);
    this->lengthText->setText(getTimeString(totalTime), dontSendNotification);
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ProjectPageDefault" template="../../../Template"
                 componentName="" parentClasses="public ProjectPage" constructorParams="ProjectTreeItem &amp;parentProject"
                 variableInitialisers="ProjectPage(parentProject)" snapPixels="4"
                 snapActive="1" snapShown="1" overlayOpacity="0.330" fixedSize="0"
                 initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="handleCommandMessage (int commandId)"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="e130bb0b9ed67f09" memberName="background" virtualName=""
             explicitFocusOrder="0" pos="0 0 0M 0M" sourceFile="../../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <LABEL name="" id="a162c9dbc90775e7" memberName="projectTitleEditor"
         virtualName="" explicitFocusOrder="0" pos="-100C 20 440 48" posRelativeY="b6ea6ccc6b9be1f8"
         labelText="..." editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="37"
         kerning="0" bold="0" italic="0" justification="9"/>
  <LABEL name="" id="b93b5ef0dc95ee24" memberName="projectTitleLabel"
         virtualName="" explicitFocusOrder="0" pos="-104Cr 0 202 48" posRelativeY="b6ea6ccc6b9be1f8"
         labelText="page::project::title" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="21"
         kerning="0" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="9c63b5388edfe183" memberName="authorEditor" virtualName=""
         explicitFocusOrder="0" pos="-100C 90 440 48" posRelativeY="b6ea6ccc6b9be1f8"
         labelText="..." editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="37"
         kerning="0" bold="0" italic="0" justification="9"/>
  <LABEL name="" id="cf32360d33639f7f" memberName="authorLabel" virtualName=""
         explicitFocusOrder="0" pos="-104Cr 70 202 48" posRelativeY="b6ea6ccc6b9be1f8"
         labelText="page::project::author" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="21"
         kerning="0" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="5b9fd0ca53fe4337" memberName="descriptionEditor"
         virtualName="" explicitFocusOrder="0" pos="-100C 160 440 48"
         posRelativeY="b6ea6ccc6b9be1f8" labelText="..." editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="37" kerning="0" bold="0" italic="0" justification="9"/>
  <LABEL name="" id="1a7ebced267e73e8" memberName="descriptionLabel" virtualName=""
         explicitFocusOrder="0" pos="-104Cr 140 202 48" posRelativeY="b6ea6ccc6b9be1f8"
         labelText="page::project::description" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default serif font"
         fontsize="21" kerning="0" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="cf836ffeded76ad1" memberName="locationLabel" virtualName=""
         explicitFocusOrder="0" pos="-104Cr 138 300 32" posRelativeY="91994c13c1a34ef8"
         labelText="page::project::filelocation" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default serif font"
         fontsize="16" kerning="0" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="e68c5a019e000a0b" memberName="locationText" virtualName=""
         explicitFocusOrder="0" pos="-100C 138 400 96" posRelativeY="91994c13c1a34ef8"
         labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="1" fontname="Default serif font" fontsize="16"
         kerning="0" bold="0" italic="0" justification="9"/>
  <LABEL name="" id="e824154c21ea01f1" memberName="contentStatsLabel"
         virtualName="" explicitFocusOrder="0" pos="-104Cr 106 300 32"
         posRelativeY="91994c13c1a34ef8" labelText="page::project::stats::content"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="1"
         fontname="Default serif font" fontsize="16" kerning="0" bold="0"
         italic="0" justification="10"/>
  <LABEL name="" id="4c13747d72949ab9" memberName="contentStatsText" virtualName=""
         explicitFocusOrder="0" pos="-100C 106 400 32" posRelativeY="91994c13c1a34ef8"
         labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="1" fontname="Default serif font" fontsize="16"
         kerning="0" bold="0" italic="0" justification="9"/>
  <LABEL name="" id="fa70bc89acdb3acf" memberName="vcsStatsLabel" virtualName=""
         explicitFocusOrder="0" pos="-104Cr 76 300 32" posRelativeY="91994c13c1a34ef8"
         labelText="page::project::stats::vcs" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default serif font"
         fontsize="16" kerning="0" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="6e9d6e323ae75809" memberName="vcsStatsText" virtualName=""
         explicitFocusOrder="0" pos="-100C 76 400 32" posRelativeY="91994c13c1a34ef8"
         labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="1" fontname="Default serif font" fontsize="16"
         kerning="0" bold="0" italic="0" justification="9"/>
  <LABEL name="" id="1ca8b26361947f73" memberName="startTimeLabel" virtualName=""
         explicitFocusOrder="0" pos="-104Cr 46 300 32" posRelativeY="91994c13c1a34ef8"
         labelText="page::project::startdate" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default serif font"
         fontsize="16" kerning="0" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="5c9cf4e334ddde90" memberName="startTimeText" virtualName=""
         explicitFocusOrder="0" pos="-100C 46 400 32" posRelativeY="91994c13c1a34ef8"
         labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="1" fontname="Default serif font" fontsize="16"
         kerning="0" bold="0" italic="0" justification="9"/>
  <LABEL name="" id="54f9aec3fdb83582" memberName="lengthLabel" virtualName=""
         explicitFocusOrder="0" pos="-104Cr 16 300 32" posRelativeY="91994c13c1a34ef8"
         labelText="page::project::duration" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="1" fontname="Default serif font" fontsize="16"
         kerning="0" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="3849b372a1b522da" memberName="lengthText" virtualName=""
         explicitFocusOrder="0" pos="-100C 16 400 32" posRelativeY="91994c13c1a34ef8"
         labelText="..." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="1" fontname="Default serif font" fontsize="16"
         kerning="0" bold="0" italic="0" justification="9"/>
  <GENERICCOMPONENT name="level1" id="b6ea6ccc6b9be1f8" memberName="level1" virtualName=""
                    explicitFocusOrder="0" pos="32 7.552% 150 24" class="Component"
                    params=""/>
  <GENERICCOMPONENT name="level2" id="91994c13c1a34ef8" memberName="level2" virtualName=""
                    explicitFocusOrder="0" pos="32 310 150 24" posRelativeY="b6ea6ccc6b9be1f8"
                    class="Component" params=""/>
  <LABEL name="" id="ed8bce664dddb1d1" memberName="licenseLabel" virtualName=""
         explicitFocusOrder="0" pos="-104Cr 210 202 48" posRelativeY="b6ea6ccc6b9be1f8"
         labelText="page::project::license" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="1" fontname="Default serif font" fontsize="21"
         kerning="0" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="63b4a599dbfa30da" memberName="licenseEditor" virtualName=""
         explicitFocusOrder="0" pos="-100C 230 440 48" posRelativeY="b6ea6ccc6b9be1f8"
         labelText="..." editableSingleClick="1" editableDoubleClick="1"
         focusDiscardsChanges="0" fontname="Default serif font" fontsize="37"
         kerning="0" bold="0" italic="0" justification="9"/>
  <JUCERCOMP name="" id="2ce00deefdf277e6" memberName="menuButton" virtualName=""
             explicitFocusOrder="0" pos="0Cc -16Rc 128 128" sourceFile="../../Common/MenuButton.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="accf780c6ef7ae9e" memberName="shadow" virtualName=""
             explicitFocusOrder="0" pos="0 0 5 0M" sourceFile="../../Themes/LightShadowRightwards.cpp"
             constructorParams=""/>
  <IMAGEBUTTON name="" id="6ab32d6eda6c96a8" memberName="revealLocationButton"
               virtualName="" explicitFocusOrder="0" pos="-150 0 -150M 24" posRelativeX="e68c5a019e000a0b"
               posRelativeY="cf836ffeded76ad1" posRelativeW="e68c5a019e000a0b"
               buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
               keepProportions="0" resourceNormal="" opacityNormal="1" colourNormal="0"
               resourceOver="" opacityOver="1" colourOver="0" resourceDown=""
               opacityDown="1" colourDown="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif
