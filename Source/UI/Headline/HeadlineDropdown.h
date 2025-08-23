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

#pragma once

#include "TreeNode.h"
#include "HighlightedComponent.h"
#include "HotkeyScheme.h"
#include "ColourIDs.h"

class HeadlineItemDataSource;
class HeadlineDropdownHeader;

class HeadlineDropdown final : public Component, private Timer
{
public:

    HeadlineDropdown(WeakReference<HeadlineItemDataSource> targetItem,
        const Point<int> &position, bool shouldShowCursor);
    ~HeadlineDropdown();

    void childBoundsChanged(Component *) override;

    void paint(Graphics &g) override;
    void resized() override;
    void mouseDown(const MouseEvent &e) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;
    void handleCommandMessage(int commandId) override;
    bool keyPressed(const KeyPress &key) override;
    bool keyStateChanged(bool isKeyDown) override;
    void inputAttemptWhenModal() override;

private:

    static HotkeyScheme::Ptr getHotkeyScheme();

    static constexpr auto padding = 7;

    bool shouldDismissOnMouseExit = true;

    WeakReference<HeadlineItemDataSource> item;

    void timerCallback() override;
    void syncWidthWithContent();
    void dismiss();
    void showCursor();

    Path backgroundShape;

    const Colour fillColour =
        findDefaultColour(ColourIDs::Breadcrumbs::fill).brighter(0.02f);
    const Colour borderLightColour =
        findDefaultColour(ColourIDs::Common::borderLineLight);
    const Colour borderDarkColour =
        findDefaultColour(ColourIDs::Common::borderLineDark);

    UniquePointer<Component> content;
    UniquePointer<Component> cursor;
    UniquePointer<HeadlineDropdownHeader> header;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeadlineDropdown)
};
