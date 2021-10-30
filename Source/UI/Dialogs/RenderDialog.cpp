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

#include "Common.h"
#include "RenderDialog.h"
#include "MainLayout.h"
#include "ProjectNode.h"
#include "ProgressIndicator.h"
#include "MenuItemComponent.h"
#include "SerializationKeys.h"
#include "DocumentHelpers.h"
#include "Config.h"

RenderDialog::RenderDialog(ProjectNode &parentProject,
    const URL &target, RenderFormat format) :
    project(parentProject),
    renderTarget(target),
    format(format)
{
    jassert(target.isLocalFile());

    this->renderButton = make<TextButton>();
    this->addAndMakeVisible(this->renderButton.get());
    this->renderButton->setButtonText(TRANS(I18n::Dialog::renderProceed));
    this->renderButton->onClick = [this]()
    {
        this->startOrAbortRender();
    };

    this->filenameEditor = make<Label>();
    this->addAndMakeVisible(this->filenameEditor.get());
    this->filenameEditor->setFont(Globals::UI::Fonts::XL);
    this->filenameEditor->setJustificationType(Justification::topLeft);

#if PLATFORM_DESKTOP
    this->filenameEditor->setEditable(true, true, false);
#elif PLATFORM_MOBILE
    this->filenameEditor->setEditable(false);
#endif

    this->captionLabel = make<Label>();
    this->addAndMakeVisible(this->captionLabel.get());
    this->captionLabel->setFont(Globals::UI::Fonts::L);
    this->captionLabel->setJustificationType(Justification::centredLeft);
    this->captionLabel->setInterceptsMouseClicks(false, false);
    this->captionLabel->setText(TRANS(I18n::Dialog::renderCaption), dontSendNotification);

    this->slider = make<Slider>();
    this->addAndMakeVisible(this->slider.get());
    this->slider->setRange(0, 1000, 0);
    this->slider->setSliderStyle(Slider::LinearBar);
    this->slider->setTextBoxStyle(Slider::NoTextBox, true, 80, 20);
    this->slider->setEnabled(false);
    this->slider->setRange(0.0, 1.0, 0.01);

    this->indicator = make<ProgressIndicator>();
    this->addChildComponent(this->indicator.get());

    this->browseButton = make<MenuItemComponent>(this, nullptr, MenuItem::item(Icons::browse, CommandIDs::Browse));
    this->addAndMakeVisible(this->browseButton.get());
    this->browseButton->setMouseCursor(MouseCursor::PointingHandCursor);

    this->pathLabel = make<Label>();
    this->addAndMakeVisible(this->pathLabel.get());
    this->pathLabel->setFont(Globals::UI::Fonts::S);
    this->pathLabel->setJustificationType(Justification::centredLeft);
    this->pathLabel->setInterceptsMouseClicks(false, false);

    this->separator = make<SeparatorHorizontalFading>();
    this->addAndMakeVisible(this->separator.get());

    // just in case..
    this->project.getTransport().stopPlaybackAndRecording();

    this->setSize(520, 224);
    this->updatePosition();
    this->updateRenderTargetLabels();
}

RenderDialog::~RenderDialog() = default;

void RenderDialog::resized()
{
    constexpr auto browseButtonWidth = 56;

    this->captionLabel->setBounds(this->getCaptionBounds().withTrimmedLeft(browseButtonWidth));

    this->pathLabel->setBounds(this->getRowBounds(0.1f, 24).withTrimmedLeft(browseButtonWidth));
    this->filenameEditor->setBounds(this->getRowBounds(0.35f, 32).withTrimmedLeft(browseButtonWidth));
    this->browseButton->setBounds(this->getRowBounds(0.35f, 48).withWidth(browseButtonWidth));

    this->separator->setBounds(this->getRowBounds(0.65f, 8));
    this->slider->setBounds(this->getRowBounds(0.85f, 12).withTrimmedLeft(browseButtonWidth).reduced(6, 0));
    this->indicator->setBounds(this->getRowBounds(0.84f, 24).withWidth(browseButtonWidth));

    this->renderButton->setBounds(this->getButtonsBounds());
}

void RenderDialog::parentHierarchyChanged()
{
    this->updatePosition();
}

void RenderDialog::parentSizeChanged()
{
    this->updatePosition();
}

void RenderDialog::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::HideDialog)
    {
        Transport &transport = this->project.getTransport();
        if (! transport.isRendering())
        {
            this->dismiss();
        }
    }
    else if (commandId == CommandIDs::Browse)
    {
        this->launchFileChooser();
    }
}

void RenderDialog::launchFileChooser()
{
    const auto extension = getExtensionForRenderFormat(this->format);

    this->renderFileChooser = make<FileChooser>(TRANS(I18n::Dialog::renderCaption),
        this->renderTarget.getLocalFile(), "*." + extension, true);

    DocumentHelpers::showFileChooser(this->renderFileChooser,
        Globals::UI::FileChooser::forFileToSave,
        [this](URL &url)
    {
        // todo someday: test rendering to any stream, not only local files
        if (url.isLocalFile())
        {
            this->renderTarget = url;
            this->updateRenderTargetLabels();
        }
    });
}

void RenderDialog::updateRenderTargetLabels()
{
    jassert(this->renderTarget.isLocalFile());
    const auto file = this->renderTarget.getLocalFile();
    this->pathLabel->setText(file.getParentDirectory().getFullPathName(), dontSendNotification);
    this->filenameEditor->setText(file.getFileName(), dontSendNotification);
}

bool RenderDialog::keyPressed(const KeyPress &key)
{
    return false;
}

void RenderDialog::inputAttemptWhenModal()
{
    this->postCommandMessage(CommandIDs::HideDialog);
}

void RenderDialog::startOrAbortRender()
{
    auto &transport = this->project.getTransport();
    if (! transport.isRendering())
    {
#if PLATFORM_DESKTOP
        App::Config().setProperty(Serialization::UI::lastRenderPath,
            this->renderTarget.getParentURL().getLocalFile().getFullPathName());
#endif

        if (transport.startRender(this->renderTarget, this->format))
        {
            this->startTrackingProgress();
        }
        else
        {
            App::Layout().showTooltip({}, MainLayout::TooltipIcon::Failure);
        }
    }
    else
    {
        transport.stopRender();
        this->stopTrackingProgress();
        App::Layout().showTooltip({}, MainLayout::TooltipIcon::Failure);
    }
}

void RenderDialog::stopRender()
{
    auto &transport = this->project.getTransport();
    if (transport.isRendering())
    {
        transport.stopRender();
        this->stopTrackingProgress();
    }
}

void RenderDialog::timerCallback(int timerId)
{
    if (timerId != RenderDialog::renderProgressTimer)
    {
        return;
    }

    auto &transport = this->project.getTransport();
    if (transport.isRendering())
    {
        const float percentsDone = transport.getRenderingPercentsComplete();
        this->slider->setValue(percentsDone, dontSendNotification);
    }
    else
    {
        this->stopTrackingProgress();
        transport.stopRender();
        App::Layout().showTooltip({}, MainLayout::TooltipIcon::Success);
    }
}

void RenderDialog::startTrackingProgress()
{
    this->startTimer(RenderDialog::renderProgressTimer, 17);
    this->indicator->startAnimating();
    this->animator.fadeIn(this->indicator.get(), Globals::UI::fadeInLong);
    this->renderButton->setButtonText(TRANS(I18n::Dialog::renderAbort));
}

void RenderDialog::stopTrackingProgress()
{
    this->stopTimer(RenderDialog::renderProgressTimer);

    auto &transport = this->project.getTransport();
    const auto percentsDone = transport.getRenderingPercentsComplete();
    this->slider->setValue(percentsDone, dontSendNotification);
    this->indicator->stopAnimating();
    this->animator.fadeOut(this->indicator.get(), Globals::UI::fadeOutShort);
    this->renderButton->setButtonText(TRANS(I18n::Dialog::renderProceed));
}
