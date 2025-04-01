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

#include "HeadlineItemArrow.h"
#include "HeadlineItemDataSource.h"
#include "ComponentFader.h"
#include "ColourIDs.h"

class HeadlineContextMenuMarker;
class IconComponent;

class HeadlineItem final : public Component,
    private ChangeListener
{
public:

    HeadlineItem(WeakReference<HeadlineItemDataSource> dataSource, AsyncUpdater &parent);
    ~HeadlineItem();

    WeakReference<HeadlineItemDataSource> getDataSource() const noexcept;
    void updateContent();

    void showContextMenuMarker();
    void hideContextMenuMarker();

    void paint(Graphics &g) override;
    void resized() override;
    bool hitTest(int x, int y) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;

private:

    static constexpr auto maxWidth = 256;

    void changeListenerCallback(ChangeBroadcaster *source) override;
    void showMenuIfAny();

    ComponentFader animator;
    UniquePointer<HeadlineContextMenuMarker> menuMarker;

    WeakReference<HeadlineItemDataSource> dataSource;
    AsyncUpdater &parentHeadline;

    const Colour bgColour = findDefaultColour(ColourIDs::Breadcrumbs::fill);

    UniquePointer<Label> titleLabel;
    UniquePointer<IconComponent> icon;
    UniquePointer<HeadlineItemArrow> arrow;
    Path backgroundShape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeadlineItem)
};
