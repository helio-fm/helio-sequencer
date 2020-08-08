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

#include "DocumentOwner.h"
#include "MainLayout.h"
#include "ProjectNode.h"
#include "PlayerThread.h"
#include "ProgressIndicator.h"
#include "MenuItemComponent.h"
#include "CommandIDs.h"

RenderDialog::RenderDialog(ProjectNode &parentProject,
    const File &renderTo, const String &formatExtension) :
    project(parentProject),
    extension(formatExtension.toLowerCase())
{
    this->renderButton = make<TextButton>();
    this->addAndMakeVisible(this->renderButton.get());
    this->renderButton->setButtonText(TRANS(I18n::Dialog::renderProceed));
    this->renderButton->onClick = [this]()
    {
        this->startOrAbortRender();
    };

    this->filenameEditor = make<Label>();
    this->addAndMakeVisible(this->filenameEditor.get());
    this->filenameEditor->setFont({ 28.f });
    this->filenameEditor->setJustificationType(Justification::topLeft);
    this->filenameEditor->setText(renderTo.getFileName(), dontSendNotification);
#if JUCE_MAC
    this->filenameEditor->setEditable(false);
#else
    this->filenameEditor->setEditable(true, true, false);
#endif

    this->filenameLabel = make<Label>();
    this->addAndMakeVisible(this->filenameLabel.get());
    this->filenameLabel->setFont({ 21.f });
    this->filenameLabel->setJustificationType(Justification::centredLeft);
    this->filenameLabel->setText(TRANS(I18n::Dialog::renderCaption), dontSendNotification);

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

    this->pathEditor = make<Label>();
    this->addAndMakeVisible(this->pathEditor.get());
    this->pathEditor->setFont({ 16.f });
    this->pathEditor->setJustificationType(Justification::centredLeft);
    this->pathEditor->setText(renderTo.getParentDirectory().getFullPathName(), dontSendNotification);

    this->component3 = make<SeparatorHorizontalFading>();
    this->addAndMakeVisible(this->component3.get());
    this->component3->setBounds(32, 121, 456, 8);

    // just in case..
    this->project.getTransport().stopPlaybackAndRecording();

    this->setSize(520, 224);
    this->updatePosition();
}

RenderDialog::~RenderDialog() {}

void RenderDialog::resized()
{
    // todo: refactor this abomination:
    filenameEditor->setBounds((getWidth() / 2) + 25 - (406 / 2), 71, 406, 32);
    filenameLabel->setBounds((getWidth() / 2) + 29 - (414 / 2), 16, 414, 22);
    slider->setBounds((getWidth() / 2) + 24 - (392 / 2), 139, 392, 12);
    indicator->setBounds((getWidth() / 2) + -212 - (32 / 2), 139 + 12 / 2 + -2 - (32 / 2), 32, 32);
    browseButton->setBounds(getWidth() - 448 - 48, 59, 48, 48);
    pathEditor->setBounds((getWidth() / 2) + 25 - (406 / 2), 48, 406, 24);

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
#if HELIO_DESKTOP
        FileChooser fc(TRANS(I18n::Dialog::renderSelectFile),
                       File(this->getFileName()), ("*." + this->extension), true);

        if (fc.browseForFileToSave(true))
        {
            this->pathEditor->setText(fc.getResult().getParentDirectory().getFullPathName(), dontSendNotification);
            this->filenameEditor->setText(fc.getResult().getFileName(), dontSendNotification);
        }
#endif
    }
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
    Transport &transport = this->project.getTransport();

    if (! transport.isRendering())
    {
        transport.startRender(this->getFileName());
        this->startTrackingProgress();
    }
    else
    {
        transport.stopRender();
        this->stopTrackingProgress();
        App::Layout().showTooltip({}, MainLayout::TooltipType::Failure);
    }
}

void RenderDialog::stopRender()
{
    Transport &transport = this->project.getTransport();

    if (transport.isRendering())
    {
        transport.stopRender();
        this->stopTrackingProgress();
    }
}

String RenderDialog::getFileName() const
{
    const String safeRenderName = File::createLegalFileName(this->filenameEditor->getText(true));
    const String absolutePath = this->pathEditor->getText();
    return File(absolutePath).getChildFile(safeRenderName).getFullPathName();
}

void RenderDialog::timerCallback(int timerId)
{
    if (timerId != RenderDialog::renderProgressTimer)
    {
        return;
    }

    Transport &transport = this->project.getTransport();

    if (transport.isRendering())
    {
        const float percentsDone = transport.getRenderingPercentsComplete();
        this->slider->setValue(percentsDone, dontSendNotification);
    }
    else
    {
        this->stopTrackingProgress();
        transport.stopRender();
        App::Layout().showTooltip({}, MainLayout::TooltipType::Success);
    }
}

void RenderDialog::startTrackingProgress()
{
    this->startTimer(RenderDialog::renderProgressTimer, 17);
    this->indicator->startAnimating();
    this->animator.fadeIn(this->indicator.get(), 250);
    this->renderButton->setButtonText(TRANS(I18n::Dialog::renderAbort));
}

void RenderDialog::stopTrackingProgress()
{
    this->stopTimer(RenderDialog::renderProgressTimer);

    Transport &transport = this->project.getTransport();
    const float percentsDone = transport.getRenderingPercentsComplete();
    this->slider->setValue(percentsDone, dontSendNotification);

    this->animator.fadeOut(this->indicator.get(), 250);
    this->indicator->stopAnimating();
    this->renderButton->setButtonText(TRANS(I18n::Dialog::renderProceed));
}
