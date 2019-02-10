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

//[Headers]
#include "FadingDialog.h"

class DocumentOwner;
class ProjectNode;
class ProgressIndicator;
class MenuItemComponent;
//[/Headers]

#include "../Themes/DialogPanel.h"
#include "../Themes/SeparatorHorizontalFading.h"
#include "../Themes/SeparatorHorizontal.h"

class RenderDialog final : public FadingDialog,
                           private Timer,
                           public Button::Listener,
                           public Label::Listener,
                           public Slider::Listener
{
public:

    RenderDialog(ProjectNode &parentProject, const File &renderTo, const String &formatExtension);
    ~RenderDialog();

    //[UserMethods]
    void showImport(DocumentOwner *owner);
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void labelTextChanged (Label* labelThatHasChanged) override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage (int commandId) override;
    bool keyPressed (const KeyPress& key) override;
    void inputAttemptWhenModal() override;


private:

    //[UserVariables]

    String getFileName() const;
    void timerCallback() override;

    void startTrackingProgress();
    void stopTrackingProgress();

    ComponentAnimator animator;
    ProjectNode &project;

    String extension;
    bool shouldRenderAfterDialogCompletes;

    void startOrAbortRender();
    void stopRender();

    //[/UserVariables]

    ScopedPointer<DialogPanel> background;
    ScopedPointer<TextButton> renderButton;
    ScopedPointer<Label> filenameEditor;
    ScopedPointer<Label> filenameLabel;
    ScopedPointer<TextButton> cancelButton;
    ScopedPointer<Slider> slider;
    ScopedPointer<ProgressIndicator> indicator;
    ScopedPointer<MenuItemComponent> browseButton;
    ScopedPointer<Label> pathEditor;
    ScopedPointer<SeparatorHorizontalFading> component3;
    ScopedPointer<SeparatorHorizontal> separatorH;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RenderDialog)
};
