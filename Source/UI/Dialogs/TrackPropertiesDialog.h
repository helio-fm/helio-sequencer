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
#include "ColourButton.h"
#include "ColourSwatches.h"
#include "MidiTrack.h"

class ProjectNode;
class IconComponent;

class TrackPropertiesDialog final : public DialogBase, public ColourButton::Listener
{
public:

    TrackPropertiesDialog(ProjectNode &project,
        Array<WeakReference<MidiTrack>> tracks);

    TrackPropertiesDialog(ProjectNode &project,
        WeakReference<MidiTrack> track,
        const String &title = "", const String &confirmation = "");

    ~TrackPropertiesDialog();

    Function<void()> onOk;
    Function<void()> onCancel;

    void onColourButtonClicked(ColourButton *button) override;

    void resized() override;
    void parentHierarchyChanged() override;
    void parentSizeChanged() override;
    void handleCommandMessage(int commandId) override;

private:

    void init(const String &title = "", const String &confirmation = "");

    ProjectNode &project;
    Array<WeakReference<MidiTrack>> tracks;

    String originalName;
    String newName;

    Colour originalColour;
    Colour newColour;

    bool hasMadeChanges = false;

    static constexpr auto colourSwatchesMargin = 6;

    Component *getPrimaryFocusTarget() override;

    void updateControls();
    void applyChangesIfAny();
    void cancelChangesIfAny();
    bool hasChanges() const;

    void doCancel();
    void doOk();

    UniquePointer<Label> messageLabel;
    UniquePointer<TextButton> cancelButton;
    UniquePointer<TextButton> okButton;
    UniquePointer<ColourSwatches> colourSwatches;
    UniquePointer<TextEditor> textEditor;
    UniquePointer<IconComponent> multipleNamesIcon;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackPropertiesDialog)
};
