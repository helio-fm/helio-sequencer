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
#include "RenderDialog.h"
#include "MainLayout.h"
#include "ProjectNode.h"
#include "MenuItemComponent.h"
#include "SerializationKeys.h"
#include "DocumentHelpers.h"
#include "HelioTheme.h"
#include "Config.h"

//===----------------------------------------------------------------------===//
// Progress bar
//===----------------------------------------------------------------------===//

RenderDialog::SimpleWaveformProgressBar::SimpleWaveformProgressBar() :
    fillColour(findDefaultColour(ColourIDs::RenderProgressBar::fill)),
    outlineColour(findDefaultColour(ColourIDs::RenderProgressBar::outline)),
    progressColour(findDefaultColour(ColourIDs::RenderProgressBar::progress)),
    waveformColour(findDefaultColour(ColourIDs::RenderProgressBar::waveform)) {}

void RenderDialog::SimpleWaveformProgressBar::paint(Graphics &g)
{
    g.setColour(this->fillColour);
    g.fillRect(this->getLocalBounds().reduced(1, 1));

    g.setColour(this->outlineColour);
    HelioTheme::drawDashedFrame(g, this->getLocalBounds());

    if (this->progress > 0.f)
    {
        g.setColour(this->progressColour);

        const auto progressBarWidth = float(this->getThumbnailResolution());
        g.fillRect(2.f, 2.f, progressBarWidth * this->progress, float(this->getHeight() - 4));

        const auto waveformHeight = float(this->getHeight() - 2);
        const auto lastFrameIndex = int(ceilf(this->waveformThumbnail.size() * this->progress));
        for (int i = 0; i < lastFrameIndex; ++i)
        {
            g.setColour(this->waveformColour.withMultipliedAlpha((i % 2) == 0 ? 0.5f : 1.f));

            // * 1.25f + 1.f just to make it slightly more visible:
            const auto peak = jmin(waveformHeight,
                this->waveformThumbnail[i] * waveformHeight * 1.25f + 1.f);

            g.fillRect(float(i + 2), waveformHeight / 2.f - peak / 2.f + 1.f, 1.f, peak);
        }
    }
}

int RenderDialog::SimpleWaveformProgressBar::getThumbnailResolution() const noexcept
{
    return this->getWidth() - 4;
}

void RenderDialog::SimpleWaveformProgressBar::update(float newProgress,
    const Array<float, CriticalSection> &newThumbnail)
{
    this->progress = newProgress;
    this->waveformThumbnail.clearQuick();
    this->waveformThumbnail.addArray(newThumbnail);
    this->repaint();
}


//===----------------------------------------------------------------------===//
// Render dialog
//===----------------------------------------------------------------------===//

RenderDialog::RenderDialog(ProjectNode &parentProject,
    const URL &target, RenderFormat format) :
    project(parentProject),
    renderTarget(target),
    format(format)
{
    const auto isPhoneLayout = App::isRunningOnPhone();

    this->renderButton = make<TextButton>();
    this->addAndMakeVisible(this->renderButton.get());
    this->renderButton->setButtonText(TRANS(I18n::Dialog::renderProceed));
    this->renderButton->onClick = [this]() {
        this->startOrAbortRender();
    };

    this->captionLabel = make<Label>();
    this->addAndMakeVisible(this->captionLabel.get());
    this->captionLabel->setFont(Globals::UI::Fonts::L);
    this->captionLabel->setJustificationType(Justification::centredLeft);
    this->captionLabel->setInterceptsMouseClicks(false, false);
    this->captionLabel->setText(TRANS(I18n::Dialog::renderCaption), dontSendNotification);

    this->progressBar = make<SimpleWaveformProgressBar>();
    this->addAndMakeVisible(this->progressBar.get());

    this->browseButton = make<MenuItemComponent>(this, nullptr,
        MenuItem::item(Icons::browse, CommandIDs::Browse));

    this->addAndMakeVisible(this->browseButton.get());
    this->browseButton->setMouseCursor(MouseCursor::PointingHandCursor);

    this->pathLabel = make<Label>();
    this->addAndMakeVisible(this->pathLabel.get());
    this->pathLabel->setFont(Globals::UI::Fonts::M);
    this->pathLabel->setJustificationType(Justification::centredLeft);
    this->pathLabel->setInterceptsMouseClicks(false, false);

    // just in case..
    this->project.getTransport().stopPlaybackAndRecording();

    this->setSize(560, isPhoneLayout? 120 : 185);
    this->updatePosition();
    this->updateRenderTargetLabels();
}

RenderDialog::~RenderDialog() = default;

void RenderDialog::resized()
{
    constexpr auto browseButtonSize = 24;

    this->captionLabel->setBounds(this->getCaptionBounds());

    this->browseButton->setBounds(this->getRowBounds(0.2f, browseButtonSize)
        .withWidth(browseButtonSize).translated(2, 0));

    this->pathLabel->setBounds(this->getRowBounds(0.2f, browseButtonSize)
        .withTrimmedLeft(browseButtonSize));

    this->progressBar->setBounds(this->getRowBounds(0.7f, 38).reduced(5, 0));

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
    if (commandId == CommandIDs::DismissDialog)
    {
        if (!this->project.getTransport().isRendering())
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
        [this](URL &url) {
            this->renderTarget = url;
            this->updateRenderTargetLabels();
        });
}

void RenderDialog::updateRenderTargetLabels()
{
    this->pathLabel->setText(this->renderTarget.getLocalFile().getFullPathName(), dontSendNotification);
}

bool RenderDialog::keyPressed(const KeyPress &key)
{
    if (key == KeyPress::escapeKey)
    {
        this->postCommandMessage(CommandIDs::DismissDialog);
        return true;
    }

    return false;
}

void RenderDialog::startOrAbortRender()
{
    auto &transport = this->project.getTransport();
    if (!transport.isRendering())
    {
#if PLATFORM_DESKTOP
        App::Config().setProperty(Serialization::UI::lastRenderPath,
            this->renderTarget.getParentURL().getLocalFile().getFullPathName());
#endif

        if (transport.startRender(this->renderTarget, this->format,
                this->progressBar->getThumbnailResolution()))
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

void RenderDialog::timerCallback()
{
    auto &transport = this->project.getTransport();
    if (transport.isRendering())
    {
        this->progressBar->update(transport.getRenderingPercentsComplete(),
            transport.getRenderingWaveformThumbnail());
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
    this->startTimer(250);

    this->renderButton->setButtonText(TRANS(I18n::Dialog::renderAbort));
    this->browseButton->setMouseCursor(MouseCursor::NormalCursor);
    this->browseButton->setEnabled(false);
}

void RenderDialog::stopTrackingProgress()
{
    this->stopTimer();

    const auto &transport = this->project.getTransport();
    this->progressBar->update(transport.getRenderingPercentsComplete(),
        transport.getRenderingWaveformThumbnail());

    this->renderButton->setButtonText(TRANS(I18n::Dialog::renderProceed));
    this->browseButton->setMouseCursor(MouseCursor::PointingHandCursor);
    this->browseButton->setEnabled(true);
}
