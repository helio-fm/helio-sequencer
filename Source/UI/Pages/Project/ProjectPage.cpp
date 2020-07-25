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

#include "ProjectPage.h"

//[MiscUserDefs]
#include "VersionControlNode.h"
#include "PlayerThread.h"
#include "ProjectNode.h"
#include "ProjectMetadata.h"
#include "HelioTheme.h"
#include "HelioCallout.h"
#include "ProjectMenu.h"
#include "CommandIDs.h"

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
//[/MiscUserDefs]

ProjectPage::ProjectPage(ProjectNode &parentProject)
    : project(parentProject)
{
    this->backgroundB.reset(new PanelBackgroundB());
    this->addAndMakeVisible(backgroundB.get());
    this->skew.reset(new SeparatorVerticalSkew());
    this->addAndMakeVisible(skew.get());
    this->backgroundA.reset(new PanelBackgroundA());
    this->addAndMakeVisible(backgroundA.get());
    this->projectTitleEditor.reset(new Label(String(),
                                                    String()));
    this->addAndMakeVisible(projectTitleEditor.get());
    this->projectTitleEditor->setFont(Font (37.00f, Font::plain));
    projectTitleEditor->setJustificationType(Justification::centredLeft);
    projectTitleEditor->setEditable(true, true, false);
    this->projectTitleEditor->addListener(this);

    this->projectTitleLabel.reset(new Label(String(),
                                                   String()));
    this->addAndMakeVisible(projectTitleLabel.get());
    this->projectTitleLabel->setFont(Font (21.00f, Font::plain));
    projectTitleLabel->setJustificationType(Justification::centredRight);
    projectTitleLabel->setEditable(false, false, false);

    this->authorEditor.reset(new Label(String(),
                                              String()));
    this->addAndMakeVisible(authorEditor.get());
    this->authorEditor->setFont(Font (37.00f, Font::plain));
    authorEditor->setJustificationType(Justification::centredLeft);
    authorEditor->setEditable(true, true, false);
    this->authorEditor->addListener(this);

    this->authorLabel.reset(new Label(String(),
                                             String()));
    this->addAndMakeVisible(authorLabel.get());
    this->authorLabel->setFont(Font (21.00f, Font::plain));
    authorLabel->setJustificationType(Justification::centredRight);
    authorLabel->setEditable(false, false, false);

    this->descriptionEditor.reset(new Label(String(),
                                                   String()));
    this->addAndMakeVisible(descriptionEditor.get());
    this->descriptionEditor->setFont(Font (37.00f, Font::plain));
    descriptionEditor->setJustificationType(Justification::centredLeft);
    descriptionEditor->setEditable(true, true, false);
    this->descriptionEditor->addListener(this);

    this->descriptionLabel.reset(new Label(String(),
                                                  String()));
    this->addAndMakeVisible(descriptionLabel.get());
    this->descriptionLabel->setFont(Font (21.00f, Font::plain));
    descriptionLabel->setJustificationType(Justification::centredRight);
    descriptionLabel->setEditable(false, false, false);

    this->locationLabel.reset(new Label(String(),
                                               String()));
    this->addAndMakeVisible(locationLabel.get());
    this->locationLabel->setFont(Font (16.00f, Font::plain));
    locationLabel->setJustificationType(Justification::topRight);
    locationLabel->setEditable(false, false, true);

    this->locationText.reset(new Label(String(),
                                              String()));
    this->addAndMakeVisible(locationText.get());
    this->locationText->setFont(Font (16.00f, Font::plain));
    locationText->setJustificationType(Justification::topLeft);
    locationText->setEditable(false, false, true);

    this->contentStatsLabel.reset(new Label(String(),
                                                   String()));
    this->addAndMakeVisible(contentStatsLabel.get());
    this->contentStatsLabel->setFont(Font (16.00f, Font::plain));
    contentStatsLabel->setJustificationType(Justification::topRight);
    contentStatsLabel->setEditable(false, false, true);

    this->contentStatsText.reset(new Label(String(),
                                                  String()));
    this->addAndMakeVisible(contentStatsText.get());
    this->contentStatsText->setFont(Font (16.00f, Font::plain));
    contentStatsText->setJustificationType(Justification::topLeft);
    contentStatsText->setEditable(false, false, true);

    this->vcsStatsLabel.reset(new Label(String(),
                                               String()));
    this->addAndMakeVisible(vcsStatsLabel.get());
    this->vcsStatsLabel->setFont(Font (16.00f, Font::plain));
    vcsStatsLabel->setJustificationType(Justification::topRight);
    vcsStatsLabel->setEditable(false, false, true);

    this->vcsStatsText.reset(new Label(String(),
                                              String()));
    this->addAndMakeVisible(vcsStatsText.get());
    this->vcsStatsText->setFont(Font (16.00f, Font::plain));
    vcsStatsText->setJustificationType(Justification::topLeft);
    vcsStatsText->setEditable(false, false, true);

    this->startTimeLabel.reset(new Label(String(),
                                                String()));
    this->addAndMakeVisible(startTimeLabel.get());
    this->startTimeLabel->setFont(Font (16.00f, Font::plain));
    startTimeLabel->setJustificationType(Justification::topRight);
    startTimeLabel->setEditable(false, false, true);

    this->startTimeText.reset(new Label(String(),
                                               String()));
    this->addAndMakeVisible(startTimeText.get());
    this->startTimeText->setFont(Font (16.00f, Font::plain));
    startTimeText->setJustificationType(Justification::topLeft);
    startTimeText->setEditable(false, false, true);

    this->lengthLabel.reset(new Label(String(),
                                             String()));
    this->addAndMakeVisible(lengthLabel.get());
    this->lengthLabel->setFont(Font (16.00f, Font::plain));
    lengthLabel->setJustificationType(Justification::topRight);
    lengthLabel->setEditable(false, false, true);

    this->lengthText.reset(new Label(String(),
                                            String()));
    this->addAndMakeVisible(lengthText.get());
    this->lengthText->setFont(Font (16.00f, Font::plain));
    lengthText->setJustificationType(Justification::topLeft);
    lengthText->setEditable(false, false, true);

    this->level1.reset(new Component());
    this->addAndMakeVisible(level1.get());
    level1->setName ("level1");

    this->level2.reset(new Component());
    this->addAndMakeVisible(level2.get());
    level2->setName ("level2");

    this->licenseLabel.reset(new Label(String(),
                                              String()));
    this->addAndMakeVisible(licenseLabel.get());
    this->licenseLabel->setFont(Font (21.00f, Font::plain));
    licenseLabel->setJustificationType(Justification::centredRight);
    licenseLabel->setEditable(false, false, true);

    this->licenseEditor.reset(new Label(String(),
                                               String()));
    this->addAndMakeVisible(licenseEditor.get());
    this->licenseEditor->setFont(Font (37.00f, Font::plain));
    licenseEditor->setJustificationType(Justification::centredLeft);
    licenseEditor->setEditable(true, true, false);
    this->licenseEditor->addListener(this);

    this->revealLocationButton.reset(new ImageButton(String()));
    this->addAndMakeVisible(revealLocationButton.get());
    revealLocationButton->addListener(this);

    revealLocationButton->setImages (false, true, false,
                                     Image(), 1.000f, Colour (0x00000000),
                                     Image(), 1.000f, Colour (0x00000000),
                                     Image(), 1.000f, Colour (0x00000000));
    this->separator.reset(new SeparatorHorizontalFadingReversed());
    this->addAndMakeVisible(separator.get());
    this->separator2.reset(new SeparatorHorizontalFadingReversed());
    this->addAndMakeVisible(separator2.get());
    this->separator3.reset(new SeparatorHorizontalFadingReversed());
    this->addAndMakeVisible(separator3.get());

    //[UserPreSize]
    this->licenseLabel->setText(TRANS(I18n::Page::projectLicense), dontSendNotification);
    this->lengthLabel->setText(TRANS(I18n::Page::projectDuration), dontSendNotification);
    this->startTimeLabel->setText(TRANS(I18n::Page::projectStartdate), dontSendNotification);
    this->vcsStatsLabel->setText(TRANS(I18n::Page::projectStatsVcs), dontSendNotification);
    this->contentStatsLabel->setText(TRANS(I18n::Page::projectStatsContent), dontSendNotification);
    this->locationLabel->setText(TRANS(I18n::Page::projectFilelocation), dontSendNotification);
    this->descriptionLabel->setText(TRANS(I18n::Page::projectDescription), dontSendNotification);
    this->authorLabel->setText(TRANS(I18n::Page::projectAuthor), dontSendNotification);
    this->projectTitleLabel->setText(TRANS(I18n::Page::projectTitle), dontSendNotification);

    this->project.addChangeListener(this);
    this->project.getTransport().addTransportListener(this);

    this->setWantsKeyboardFocus(true);
    this->setFocusContainer(true);

    this->revealLocationButton->setMouseCursor(MouseCursor::PointingHandCursor);
    //[/UserPreSize]

    this->setSize(600, 400);

    //[Constructor]
#if HELIO_MOBILE
    // не комильфо на мобильниках показывать расположение файлв
    this->locationLabel->setVisible(false);
    this->locationText->setVisible(false);
#endif
    //[/Constructor]
}

ProjectPage::~ProjectPage()
{
    //[Destructor_pre]
    this->project.getTransport().removeTransportListener(this);
    this->project.removeChangeListener(this);
    //[/Destructor_pre]

    backgroundB = nullptr;
    skew = nullptr;
    backgroundA = nullptr;
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
    revealLocationButton = nullptr;
    separator = nullptr;
    separator2 = nullptr;
    separator3 = nullptr;

    //[Destructor]
    //[/Destructor]
}

void ProjectPage::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ProjectPage::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    backgroundB->setBounds(getWidth() - (getWidth() - 384), 0, getWidth() - 384, getHeight() - 0);
    skew->setBounds(0 + 320, 0, 64, getHeight() - 0);
    backgroundA->setBounds(0, 0, 320, getHeight() - 0);
    projectTitleEditor->setBounds((getWidth() - (getWidth() - 384)) + 0, proportionOfHeight (0.0896f) + 0, getWidth() - 400, 48);
    projectTitleLabel->setBounds(0 + 320 - 202, proportionOfHeight (0.0896f) + 0, 202, 48);
    authorEditor->setBounds((getWidth() - (getWidth() - 384)) + 0, proportionOfHeight (0.0896f) + 70, getWidth() - 400, 48);
    authorLabel->setBounds(0 + 320 - 202, proportionOfHeight (0.0896f) + 70, 202, 48);
    descriptionEditor->setBounds((getWidth() - (getWidth() - 384)) + 0, proportionOfHeight (0.0896f) + 140, getWidth() - 400, 48);
    descriptionLabel->setBounds(0 + 320 - 202, proportionOfHeight (0.0896f) + 140, 202, 48);
    locationLabel->setBounds(0 + 320 - 300, (proportionOfHeight (0.0896f) + 310) + 138, 300, 32);
    locationText->setBounds((getWidth() - (getWidth() - 384)) + 0, (proportionOfHeight (0.0896f) + 310) + 138, getWidth() - 400, 96);
    contentStatsLabel->setBounds(0 + 320 - 300, (proportionOfHeight (0.0896f) + 310) + 106, 300, 32);
    contentStatsText->setBounds((getWidth() - (getWidth() - 384)) + 0, (proportionOfHeight (0.0896f) + 310) + 106, getWidth() - 400, 32);
    vcsStatsLabel->setBounds(0 + 320 - 300, (proportionOfHeight (0.0896f) + 310) + 76, 300, 32);
    vcsStatsText->setBounds((getWidth() - (getWidth() - 384)) + 0, (proportionOfHeight (0.0896f) + 310) + 76, getWidth() - 400, 32);
    startTimeLabel->setBounds(0 + 320 - 300, (proportionOfHeight (0.0896f) + 310) + 46, 300, 32);
    startTimeText->setBounds((getWidth() - (getWidth() - 384)) + 0, (proportionOfHeight (0.0896f) + 310) + 46, getWidth() - 400, 32);
    lengthLabel->setBounds(0 + 320 - 300, (proportionOfHeight (0.0896f) + 310) + 16, 300, 32);
    lengthText->setBounds((getWidth() - (getWidth() - 384)) + 0, (proportionOfHeight (0.0896f) + 310) + 16, getWidth() - 400, 32);
    level1->setBounds(32, proportionOfHeight (0.0896f), 150, 24);
    level2->setBounds(32, proportionOfHeight (0.0896f) + 310, 150, 24);
    licenseLabel->setBounds(0 + 320 - 202, proportionOfHeight (0.0896f) + 210, 202, 48);
    licenseEditor->setBounds((getWidth() - (getWidth() - 384)) + 0, proportionOfHeight (0.0896f) + 210, getWidth() - 400, 48);
    revealLocationButton->setBounds(((getWidth() - (getWidth() - 384)) + 0) + 0, ((proportionOfHeight (0.0896f) + 310) + 138) + 0, (getWidth() - 400) - 0, 24);
    separator->setBounds(214, (proportionOfHeight (0.0896f) + 0) + 48 - -10, 488, 3);
    separator2->setBounds(200, (proportionOfHeight (0.0896f) + 70) + 48 - -10, 488, 3);
    separator3->setBounds(188, (proportionOfHeight (0.0896f) + 140) + 48 - -10, 488, 3);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ProjectPage::labelTextChanged(Label *labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == projectTitleEditor.get())
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
    else if (labelThatHasChanged == authorEditor.get())
    {
        //[UserLabelCode_authorEditor] -- add your label text handling code here..
        this->project.getProjectInfo()->setAuthor(labelThatHasChanged->getText());
        //[/UserLabelCode_authorEditor]
    }
    else if (labelThatHasChanged == descriptionEditor.get())
    {
        //[UserLabelCode_descriptionEditor] -- add your label text handling code here..
        this->project.getProjectInfo()->setDescription(labelThatHasChanged->getText());
        //[/UserLabelCode_descriptionEditor]
    }
    else if (labelThatHasChanged == licenseEditor.get())
    {
        //[UserLabelCode_licenseEditor] -- add your label text handling code here..
        this->project.getProjectInfo()->setLicense(labelThatHasChanged->getText());
        //[/UserLabelCode_licenseEditor]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}

void ProjectPage::buttonClicked(Button *buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == revealLocationButton.get())
    {
        //[UserButtonCode_revealLocationButton] -- add your button handler code here..
        this->project.getDocument()->getFile().revealToUser();
        //[/UserButtonCode_revealLocationButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void ProjectPage::visibilityChanged()
{
    //[UserCode_visibilityChanged] -- Add your code here...
    const RelativeTime totalTime(this->totalTimeMs.get() / 1000.0);
    this->lengthText->setText(getTimeString(totalTime), dontSendNotification);
    //[/UserCode_visibilityChanged]
}


//[MiscUserCode]
void ProjectPage::updateContent()
{
    const String &fullname = this->project.getProjectInfo()->getFullName();
    const String &author = this->project.getProjectInfo()->getAuthor();
    const String &description = this->project.getProjectInfo()->getDescription();
    const String &license = this->project.getProjectInfo()->getLicense();
    const String &startTime = App::getHumanReadableDate(Time(this->project.getProjectInfo()->getStartTimestamp()));

#if HELIO_DESKTOP
    const String &clickToEdit = TRANS(I18n::Page::projectDefaultValueDesktop);
#elif HELIO_MOBILE
    const String &clickToEdit = TRANS(I18n::Page::projectDefaultValueMobile);
#endif

    this->projectTitleEditor->setText(fullname.isEmpty() ? clickToEdit : fullname, dontSendNotification);
    this->authorEditor->setText(author.isEmpty() ? TRANS(I18n::Page::projectDefaultAuthor) : author, dontSendNotification);
    this->descriptionEditor->setText(description.isEmpty() ? clickToEdit : description, dontSendNotification);
    this->licenseEditor->setText(license.isEmpty() ? TRANS(I18n::Page::projectDefaultLicense) : license, dontSendNotification);

    this->startTimeText->setText(startTime, dontSendNotification);
    this->locationText->setText(this->project.getDocument()->getFullPath(), dontSendNotification);
    this->contentStatsText->setText(this->project.getStats(), dontSendNotification);

    if (auto *vcti = this->project.findChildOfType<VersionControlNode>())
    {
        this->vcsStatsText->setText(vcti->getStatsString(), dontSendNotification);
    }
}

void ProjectPage::onSeek(float beatPosition, double currentTimeMs, double totalTimeMs)
{
    this->totalTimeMs = totalTimeMs;
}

void ProjectPage::onTotalTimeChanged(double totalTimeMs) noexcept
{
    this->totalTimeMs = totalTimeMs;
}

void ProjectPage::changeListenerCallback(ChangeBroadcaster *source)
{
    this->updateContent();
}
//[/MiscUserCode]

#if 0
/*
BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ProjectPage" template="../../../Template"
                 componentName="" parentClasses="public Component, protected TransportListener, protected ChangeListener"
                 constructorParams="ProjectNode &amp;parentProject" variableInitialisers="project(parentProject)"
                 snapPixels="4" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="visibilityChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="0"/>
  <JUCERCOMP name="" id="9e61167b79cef28c" memberName="backgroundB" virtualName=""
             explicitFocusOrder="0" pos="0Rr 0 384M 0M" sourceFile="../../Themes/PanelBackgroundB.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="9bde1b4dd587d5fb" memberName="skew" virtualName=""
             explicitFocusOrder="0" pos="0R 0 64 0M" posRelativeX="981ceff5817d7b34"
             sourceFile="../../Themes/SeparatorVerticalSkew.cpp" constructorParams=""/>
  <JUCERCOMP name="" id="981ceff5817d7b34" memberName="backgroundA" virtualName=""
             explicitFocusOrder="0" pos="0 0 320 0M" sourceFile="../../Themes/PanelBackgroundA.cpp"
             constructorParams=""/>
  <LABEL name="" id="a162c9dbc90775e7" memberName="projectTitleEditor"
         virtualName="" explicitFocusOrder="0" pos="0 0 400M 48" posRelativeX="9e61167b79cef28c"
         posRelativeY="b6ea6ccc6b9be1f8" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default font"
         fontsize="37.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="b93b5ef0dc95ee24" memberName="projectTitleLabel"
         virtualName="" explicitFocusOrder="0" pos="0Rr 0 202 48" posRelativeX="981ceff5817d7b34"
         posRelativeY="b6ea6ccc6b9be1f8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="21.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="9c63b5388edfe183" memberName="authorEditor" virtualName=""
         explicitFocusOrder="0" pos="0 70 400M 48" posRelativeX="9e61167b79cef28c"
         posRelativeY="b6ea6ccc6b9be1f8" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default font"
         fontsize="37.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="cf32360d33639f7f" memberName="authorLabel" virtualName=""
         explicitFocusOrder="0" pos="0Rr 70 202 48" posRelativeX="981ceff5817d7b34"
         posRelativeY="b6ea6ccc6b9be1f8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="21.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="5b9fd0ca53fe4337" memberName="descriptionEditor"
         virtualName="" explicitFocusOrder="0" pos="0 140 400M 48" posRelativeX="9e61167b79cef28c"
         posRelativeY="b6ea6ccc6b9be1f8" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default font"
         fontsize="37.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="" id="1a7ebced267e73e8" memberName="descriptionLabel" virtualName=""
         explicitFocusOrder="0" pos="0Rr 140 202 48" posRelativeX="981ceff5817d7b34"
         posRelativeY="b6ea6ccc6b9be1f8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="21.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="cf836ffeded76ad1" memberName="locationLabel" virtualName=""
         explicitFocusOrder="0" pos="0Rr 138 300 32" posRelativeX="981ceff5817d7b34"
         posRelativeY="91994c13c1a34ef8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default font"
         fontsize="16.0" kerning="0.0" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="e68c5a019e000a0b" memberName="locationText" virtualName=""
         explicitFocusOrder="0" pos="0 138 400M 96" posRelativeX="9e61167b79cef28c"
         posRelativeY="91994c13c1a34ef8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default font"
         fontsize="16.0" kerning="0.0" bold="0" italic="0" justification="9"/>
  <LABEL name="" id="e824154c21ea01f1" memberName="contentStatsLabel"
         virtualName="" explicitFocusOrder="0" pos="0Rr 106 300 32" posRelativeX="981ceff5817d7b34"
         posRelativeY="91994c13c1a34ef8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default font"
         fontsize="16.0" kerning="0.0" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="4c13747d72949ab9" memberName="contentStatsText" virtualName=""
         explicitFocusOrder="0" pos="0 106 400M 32" posRelativeX="9e61167b79cef28c"
         posRelativeY="91994c13c1a34ef8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default font"
         fontsize="16.0" kerning="0.0" bold="0" italic="0" justification="9"/>
  <LABEL name="" id="fa70bc89acdb3acf" memberName="vcsStatsLabel" virtualName=""
         explicitFocusOrder="0" pos="0Rr 76 300 32" posRelativeX="981ceff5817d7b34"
         posRelativeY="91994c13c1a34ef8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default font"
         fontsize="16.0" kerning="0.0" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="6e9d6e323ae75809" memberName="vcsStatsText" virtualName=""
         explicitFocusOrder="0" pos="0 76 400M 32" posRelativeX="9e61167b79cef28c"
         posRelativeY="91994c13c1a34ef8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default font"
         fontsize="16.0" kerning="0.0" bold="0" italic="0" justification="9"/>
  <LABEL name="" id="1ca8b26361947f73" memberName="startTimeLabel" virtualName=""
         explicitFocusOrder="0" pos="0Rr 46 300 32" posRelativeX="981ceff5817d7b34"
         posRelativeY="91994c13c1a34ef8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default font"
         fontsize="16.0" kerning="0.0" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="5c9cf4e334ddde90" memberName="startTimeText" virtualName=""
         explicitFocusOrder="0" pos="0 46 400M 32" posRelativeX="9e61167b79cef28c"
         posRelativeY="91994c13c1a34ef8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default font"
         fontsize="16.0" kerning="0.0" bold="0" italic="0" justification="9"/>
  <LABEL name="" id="54f9aec3fdb83582" memberName="lengthLabel" virtualName=""
         explicitFocusOrder="0" pos="0Rr 16 300 32" posRelativeX="981ceff5817d7b34"
         posRelativeY="91994c13c1a34ef8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default font"
         fontsize="16.0" kerning="0.0" bold="0" italic="0" justification="10"/>
  <LABEL name="" id="3849b372a1b522da" memberName="lengthText" virtualName=""
         explicitFocusOrder="0" pos="0 16 400M 32" posRelativeX="9e61167b79cef28c"
         posRelativeY="91994c13c1a34ef8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default font"
         fontsize="16.0" kerning="0.0" bold="0" italic="0" justification="9"/>
  <GENERICCOMPONENT name="level1" id="b6ea6ccc6b9be1f8" memberName="level1" virtualName=""
                    explicitFocusOrder="0" pos="32 8.964% 150 24" class="Component"
                    params=""/>
  <GENERICCOMPONENT name="level2" id="91994c13c1a34ef8" memberName="level2" virtualName=""
                    explicitFocusOrder="0" pos="32 310 150 24" posRelativeY="b6ea6ccc6b9be1f8"
                    class="Component" params=""/>
  <LABEL name="" id="ed8bce664dddb1d1" memberName="licenseLabel" virtualName=""
         explicitFocusOrder="0" pos="0Rr 210 202 48" posRelativeX="981ceff5817d7b34"
         posRelativeY="b6ea6ccc6b9be1f8" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="1" fontname="Default font"
         fontsize="21.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="" id="63b4a599dbfa30da" memberName="licenseEditor" virtualName=""
         explicitFocusOrder="0" pos="0 210 400M 48" posRelativeX="9e61167b79cef28c"
         posRelativeY="b6ea6ccc6b9be1f8" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Default font"
         fontsize="37.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <IMAGEBUTTON name="" id="6ab32d6eda6c96a8" memberName="revealLocationButton"
               virtualName="" explicitFocusOrder="0" pos="0 0 0M 24" posRelativeX="e68c5a019e000a0b"
               posRelativeY="cf836ffeded76ad1" posRelativeW="e68c5a019e000a0b"
               buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
               keepProportions="0" resourceNormal="" opacityNormal="1.0" colourNormal="0"
               resourceOver="" opacityOver="1.0" colourOver="0" resourceDown=""
               opacityDown="1.0" colourDown="0"/>
  <JUCERCOMP name="" id="26985c577d404f94" memberName="separator" virtualName=""
             explicitFocusOrder="0" pos="214 -10R 488 3" posRelativeY="a162c9dbc90775e7"
             sourceFile="../../Themes/SeparatorHorizontalFadingReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="d7802e4fa157d7e9" memberName="separator2" virtualName=""
             explicitFocusOrder="0" pos="200 -10R 488 3" posRelativeY="9c63b5388edfe183"
             sourceFile="../../Themes/SeparatorHorizontalFadingReversed.cpp"
             constructorParams=""/>
  <JUCERCOMP name="" id="3bb2ffc7a0caddd6" memberName="separator3" virtualName=""
             explicitFocusOrder="0" pos="188 -10R 488 3" posRelativeY="5b9fd0ca53fe4337"
             sourceFile="../../Themes/SeparatorHorizontalFadingReversed.cpp"
             constructorParams=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



