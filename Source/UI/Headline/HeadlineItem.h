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
#include "HighlightedComponent.h"
#include "TreeItem.h"

class IconComponent;
class HeadlineDropdown;
//[/Headers]


class HeadlineItem  : public Component,
                      private Timer,
                      private ChangeListener
{
public:

    HeadlineItem (WeakReference<TreeItem> treeItem, AsyncUpdater &parent);

    ~HeadlineItem();

    //[UserMethods]
    WeakReference<TreeItem> getTreeItem() const noexcept;
    void updateContent();
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void mouseEnter (const MouseEvent& e) override;
    void mouseExit (const MouseEvent& e) override;
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;


private:

    //[UserVariables]

    void changeListenerCallback(ChangeBroadcaster* source) override;
    void timerCallback() override;
    void showMenu();

    WeakReference<TreeItem> item;
    ScopedPointer<HeadlineDropdown> dropdown;
    AsyncUpdater &parentHeadline;

    //[/UserVariables]

    ScopedPointer<Label> titleLabel;
    ScopedPointer<IconComponent> icon;
    Path internalPath1;
    Path internalPath2;
    Path internalPath3;
    Path internalPath4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeadlineItem)
};
