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

#pragma once

#include "DialogBase.h"
#include "RenderFormat.h"
#include "ColourIDs.h"

class DocumentOwner;
class ProjectNode;
class MenuItemComponent;

class RenderDialog final : public DialogBase
{
public:

    RenderDialog(ProjectNode &parentProject,
        const URL &target, RenderFormat format);
    ~RenderDialog();

    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage(int commandId) override;
    bool keyPressed(const KeyPress& key) override;
    void inputAttemptWhenModal() override;

private:

    static constexpr auto renderProgressTimer = 100;
    void timerCallback(int timerId) override;

    void startTrackingProgress();
    void stopTrackingProgress();

    ProjectNode &project;

    const RenderFormat format;

    URL renderTarget;
    void updateRenderTargetLabels();

    UniquePointer<FileChooser> renderFileChooser;
    void launchFileChooser();

    void startOrAbortRender();
    void stopRender();

private:

    class SimpleWaveformProgressBar final : public Component
    {
    public:

        SimpleWaveformProgressBar();
        void paint(Graphics &g) override;
        int getThumbnailResolution() const noexcept;
        void update(float newProgress, const Array<float, CriticalSection> &newThumbnail);

    private:

        float progress = 0.f;
        Array<float> waveformThumbnail;

        const Colour fillColour;
        const Colour outlineColour;
        const Colour progressColour;
        const Colour waveformColour;

    };

private:

    UniquePointer<Label> captionLabel;
    UniquePointer<Label> pathLabel;
    UniquePointer<MenuItemComponent> browseButton;
    UniquePointer<SimpleWaveformProgressBar> progressBar;
    UniquePointer<TextButton> renderButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RenderDialog)
};
