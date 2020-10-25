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

#include "Instrument.h"

class HeadlineContextMenuController;

class KeyboardMappingPage final :
    public Component,
    public ChangeListener
{
public:

    explicit KeyboardMappingPage(WeakReference<Instrument> instrument);
    ~KeyboardMappingPage() override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void resized() override;
    void mouseDown(const MouseEvent &e) override;
    void handleCommandMessage(int commandId) override;

private:

    //===------------------------------------------------------------------===//
    // ChangeListener
    //===------------------------------------------------------------------===//

    void changeListenerCallback(ChangeBroadcaster *source) override;

private:

    WeakReference<Instrument> instrument;

    void syncWithRange(int base);
    int currentPageBase = 0;

    void onKeyPreview(int i);
    void onKeyMappingUpdated(int i);

    bool canShowPreviousPage() const noexcept;
    bool canShowNextPage() const noexcept;

    void loadScalaMappings() const;

    UniquePointer<HeadlineContextMenuController> contextMenuController;

    UniquePointer<Component> background;

    UniquePointer<Component> leftArrow;
    UniquePointer<Component> rightArrow;
    UniquePointer<Label> rangeLabel;

    OwnedArray<Button> keyButtons;
    OwnedArray<Label> keyLabels;
    OwnedArray<Label> mappingLabels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardMappingPage)
};
