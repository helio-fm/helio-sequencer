/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "ProjectPage.h"

#include "VersionControlNode.h"
#include "PlayerThread.h"
#include "ProjectNode.h"
#include "ProjectMetadata.h"
#include "Config.h"

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

ProjectPage::ProjectPage(ProjectNode &parentProject) :
    project(parentProject)
{
    this->setFocusContainerType(Component::FocusContainerType::keyboardFocusContainer);
    this->setWantsKeyboardFocus(true);
    this->setPaintingIsUnclipped(true);

    this->backgroundB = make<PageBackgroundB>();
    this->addAndMakeVisible(this->backgroundB.get());

    this->skew = make<SeparatorVerticalSkew>();
    this->addAndMakeVisible(this->skew.get());

    this->backgroundA = make<PageBackgroundA>();
    this->addAndMakeVisible(this->backgroundA.get());

    this->projectTitleEditor = make<Label>();
    this->addAndMakeVisible(this->projectTitleEditor.get());
    this->projectTitleEditor->setEditable(true, true, false);
    this->projectTitleEditor->onTextChange = [this]()
    {
        if (this->projectTitleEditor->getText().isNotEmpty())
        {
            this->project.getProjectInfo()->setFullName(this->projectTitleEditor->getText());
        }
        else
        {
            const String &fullname = this->project.getProjectInfo()->getFullName();
            this->projectTitleEditor->setText(fullname, sendNotification);
        }
    };

    this->projectTitleLabel = make<Label>();
    this->addAndMakeVisible(this->projectTitleLabel.get());

    this->authorEditor = make<Label>();
    this->addAndMakeVisible(this->authorEditor.get());
    this->authorEditor->setEditable(true, true, false);
    this->authorEditor->onTextChange = [this]()
    {
        this->project.getProjectInfo()->setAuthor(this->authorEditor->getText());
    };

    this->authorLabel = make<Label>();
    this->addAndMakeVisible(this->authorLabel.get());

    this->descriptionEditor = make<Label>();
    this->addAndMakeVisible(this->descriptionEditor.get());
    this->descriptionEditor->setEditable(true, true, false);
    this->descriptionEditor->onTextChange = [this]()
    {
        this->project.getProjectInfo()->setDescription(this->descriptionEditor->getText());
    };

    this->descriptionLabel = make<Label>();
    this->addAndMakeVisible(this->descriptionLabel.get());

    this->locationLabel = make<Label>();
    this->addAndMakeVisible(this->locationLabel.get());

    this->locationText = make<Label>();
    this->addAndMakeVisible(this->locationText.get());

    this->contentStatsLabel = make<Label>();
    this->addAndMakeVisible(this->contentStatsLabel.get());

    this->contentStatsText = make<Label>();
    this->addAndMakeVisible(this->contentStatsText.get());

    this->vcsStatsLabel = make<Label>();
    this->addAndMakeVisible(this->vcsStatsLabel.get());

    this->vcsStatsText = make<Label>();
    this->addAndMakeVisible(this->vcsStatsText.get());

    this->startTimeLabel = make<Label>();
    this->addAndMakeVisible(this->startTimeLabel.get());

    this->startTimeText = make<Label>();
    this->addAndMakeVisible(this->startTimeText.get());

    this->lengthLabel = make<Label>();
    this->addAndMakeVisible(this->lengthLabel.get());

    this->lengthText = make<Label>();
    this->addAndMakeVisible(this->lengthText.get());

    this->licenseLabel = make<Label>();
    this->addAndMakeVisible(this->licenseLabel.get());

    this->licenseEditor = make<Label>();
    this->addAndMakeVisible(this->licenseEditor.get());
    this->licenseEditor->setEditable(true, true, false);
    this->licenseEditor->onTextChange = [this]()
    {
        this->project.getProjectInfo()->setLicense(this->licenseEditor->getText());
    };

    this->revealLocationButton = make<ImageButton>();
    this->addAndMakeVisible(this->revealLocationButton.get());
    this->revealLocationButton->onClick = [this]()
    {
        this->project.getDocument()->revealToUser();
    };

    this->temperamentLabel = make<Label>();
    this->addAndMakeVisible(this->temperamentLabel.get());

    this->temperamentText = make<Label>();
    this->addAndMakeVisible(this->temperamentText.get());

    this->licenseLabel->setText(TRANS(I18n::Page::projectLicense), dontSendNotification);
    this->lengthLabel->setText(TRANS(I18n::Page::projectDuration), dontSendNotification);
    this->startTimeLabel->setText(TRANS(I18n::Page::projectStartdate), dontSendNotification);
    this->vcsStatsLabel->setText(TRANS(I18n::Page::projectStatsVcs), dontSendNotification);
    this->contentStatsLabel->setText(TRANS(I18n::Page::projectStatsContent), dontSendNotification);
    this->locationLabel->setText(TRANS(I18n::Page::projectFilelocation), dontSendNotification);
    this->descriptionLabel->setText(TRANS(I18n::Page::projectDescription), dontSendNotification);
    this->authorLabel->setText(TRANS(I18n::Page::projectAuthor), dontSendNotification);
    this->projectTitleLabel->setText(TRANS(I18n::Page::projectTitle), dontSendNotification);
    this->temperamentLabel->setText(TRANS(I18n::Page::projectTemperament), dontSendNotification);

    this->revealLocationButton->setMouseCursor(MouseCursor::PointingHandCursor);

    this->metadataCaptions.add(this->projectTitleLabel.get(), this->authorLabel.get(),
        this->descriptionLabel.get(), this->licenseLabel.get());

    this->metadataEditors.add(this->projectTitleEditor.get(), this->authorEditor.get(),
        this->descriptionEditor.get(), this->licenseEditor.get());

    this->statisticsCaptions.add(this->startTimeLabel.get(), this->lengthLabel.get(),
        this->temperamentLabel.get(), this->vcsStatsLabel.get(),
        this->contentStatsLabel.get(), this->locationLabel.get());

    this->statisticsLabels.add(this->startTimeText.get(), this->lengthText.get(),
        this->temperamentText.get(), this->vcsStatsText.get(),
        this->contentStatsText.get(), this->locationText.get());

    const auto phoneScreenMode = App::isRunningOnPhone();

    for (auto *metadataCaption : this->metadataCaptions)
    {
        metadataCaption->setFont(phoneScreenMode ? Globals::UI::Fonts::L : Globals::UI::Fonts::XL);
        metadataCaption->setJustificationType(Justification::centredRight);
    }

    for (auto *metadataEditor : this->metadataEditors)
    {
        metadataEditor->setFont(phoneScreenMode ? Globals::UI::Fonts::XL : Globals::UI::Fonts::XXL);
        metadataEditor->setJustificationType(Justification::centredLeft);
    }

    for (auto *statisticsCaption : this->statisticsCaptions)
    {
        statisticsCaption->setFont(Globals::UI::Fonts::S);
        statisticsCaption->setJustificationType(Justification::centredRight);
        if (phoneScreenMode) // no screen space for all that on the phones
        {
            statisticsCaption->setVisible(false);
        }
    }

    for (auto *statisticsLabel : this->statisticsLabels)
    {
        statisticsLabel->setFont(Globals::UI::Fonts::S);
        statisticsLabel->setJustificationType(Justification::centredLeft);
        if (phoneScreenMode)
        {
            statisticsLabel->setVisible(false);
        }
    }

    this->project.addChangeListener(this);
    this->project.getTransport().addTransportListener(this);

#if PLATFORM_MOBILE
    this->locationLabel->setVisible(false);
    this->locationText->setVisible(false);
#endif
}

ProjectPage::~ProjectPage()
{
    this->project.getTransport().removeTransportListener(this);
    this->project.removeChangeListener(this);
}

void ProjectPage::resized()
{
    const auto smallScreenMode = App::isRunningOnPhone();
    const auto uiScaleFactor = App::Config().getUiFlags()->getUiScaleFactor();

    const auto skewWidth = smallScreenMode ? int(32 / uiScaleFactor) : int(64 / uiScaleFactor);
    const auto labelsSectionWidth = smallScreenMode ? int(240 / uiScaleFactor) : int(320 / uiScaleFactor);

    this->skew->setBounds(labelsSectionWidth, 0, skewWidth, this->getHeight());
    this->backgroundA->setBounds(0, 0, labelsSectionWidth, this->getHeight());
    this->backgroundB->setBounds(labelsSectionWidth + skewWidth, 0,
        this->getWidth() - labelsSectionWidth - skewWidth, this->getHeight());

    const auto metadataY = this->proportionOfHeight(0.125f);
    const auto statisticsY = this->proportionOfHeight(0.55f);

    static constexpr auto padding = 12;
    const auto metadataLineHeight = smallScreenMode ? int(48 / uiScaleFactor) : int(64 / uiScaleFactor);
    const auto statsLineHeight = int(34 / uiScaleFactor);

    const auto getSkewX = [this, skewWidth](int y)
    {
        return int((1.f - float(y) / float(this->getHeight())) * skewWidth);
    };

    const auto layoutLeftSide = [this, &getSkewX, labelsSectionWidth]
    (Array<Label *> &target, int yOffset, int lineHeight)
    {
        for (int i = 0; i < target.size(); ++i)
        {
            auto *captionComponent = target.getUnchecked(i);
            const auto y = yOffset + i * lineHeight;
            const auto w = labelsSectionWidth + getSkewX(y) - padding;
            captionComponent->setBounds(0, y, w, lineHeight);
        }
    };

    const auto layoutRightSide = [this, &getSkewX, labelsSectionWidth]
    (Array<Label *> &target, int yOffset, int lineHeight)
    {
        for (int i = 0; i < target.size(); ++i)
        {
            auto *textComponent = target.getUnchecked(i);
            const auto y = yOffset + i * lineHeight;
            const auto x = labelsSectionWidth + getSkewX(y) + padding;
            const auto w = this->getWidth() - x - padding;
            const auto h = int(textComponent->getFont().getHeight()) + 2;
            textComponent->setBounds(x, y + (lineHeight / 2 - h / 2), w, h);
        }
    };

    layoutLeftSide(this->metadataCaptions, metadataY, metadataLineHeight);
    layoutLeftSide(this->statisticsCaptions, statisticsY, statsLineHeight);

    layoutRightSide(this->metadataEditors, metadataY, metadataLineHeight);
    layoutRightSide(this->statisticsLabels, statisticsY, statsLineHeight);

    this->revealLocationButton->setBounds(this->locationText->getBounds());
}

void ProjectPage::visibilityChanged()
{
    const RelativeTime totalTime(this->totalTimeMs.get() / 1000.0);
    this->lengthText->setText(getTimeString(totalTime), dontSendNotification);
}

void ProjectPage::updateContent()
{
    const auto fullname = this->project.getProjectInfo()->getFullName();
    const auto author = this->project.getProjectInfo()->getAuthor();
    const auto description = this->project.getProjectInfo()->getDescription();
    const auto license = this->project.getProjectInfo()->getLicense();
    const auto startTime = App::getHumanReadableDate(Time(this->project.getProjectInfo()->getStartTimestamp()));
    const auto temperamentName = this->project.getProjectInfo()->getTemperament()->getName();

    this->projectTitleEditor->setText(fullname.isEmpty() ? "..." : fullname, dontSendNotification);
    this->authorEditor->setText(author.isEmpty() ? "..." : author, dontSendNotification);
    this->descriptionEditor->setText(description.isEmpty() ? "..." : description, dontSendNotification);
    this->licenseEditor->setText(license.isEmpty() ? TRANS(I18n::Page::projectDefaultLicense) : license, dontSendNotification);

    this->startTimeText->setText(startTime, dontSendNotification);
    this->locationText->setText(this->project.getDocument()->getFullPath(), dontSendNotification);
    this->contentStatsText->setText(this->project.getStats(), dontSendNotification);
    this->temperamentText->setText(temperamentName, dontSendNotification);

    if (auto *vcs = this->project.findChildOfType<VersionControlNode>())
    {
        this->vcsStatsText->setText(vcs->getStatsString(), dontSendNotification);
    }
}

void ProjectPage::onTotalTimeChanged(double totalTimeMs) noexcept
{
    this->totalTimeMs = totalTimeMs;
}

void ProjectPage::changeListenerCallback(ChangeBroadcaster *source)
{
    this->updateContent();
}
