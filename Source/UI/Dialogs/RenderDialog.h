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
#include "SeparatorHorizontalFading.h"
#include "RenderFormat.h"

class DocumentOwner;
class ProjectNode;
class ProgressIndicator;
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

    ComponentAnimator animator;
    ProjectNode &project;

    const RenderFormat format;

    URL renderTarget;
    void updateRenderTargetLabels();

    UniquePointer<FileChooser> renderFileChooser;
    void launchFileChooser();

    void startOrAbortRender();
    void stopRender();

    UniquePointer<TextButton> renderButton;
    UniquePointer<Label> filenameEditor;
    UniquePointer<Label> filenameLabel;
    UniquePointer<Slider> slider;
    UniquePointer<ProgressIndicator> indicator;
    UniquePointer<MenuItemComponent> browseButton;
    UniquePointer<Label> pathEditor;
    UniquePointer<SeparatorHorizontalFading> separator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RenderDialog)
};
