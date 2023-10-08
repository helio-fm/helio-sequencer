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

class RollBase;
class NoteComponent;
class Lasso;

#include "IconComponent.h"
#include "ComponentFader.h"
#include "ColourIDs.h"

class NoteResizerRight final : public Component
{
public:

    explicit NoteResizerRight(RollBase &parentRoll);
    ~NoteResizerRight();

    void updateBounds();
    void updateTopPosition();

    void paint(Graphics &g) override;
    bool hitTest(int x, int y) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

private:

    NoteComponent *findRightMostEvent(const Lasso &);

    RollBase &roll;
    SafePointer<NoteComponent> groupResizerNote;

    ComponentFader fader;
    ComponentDragger dragger;

    UniquePointer<IconComponent> resizeIcon;

    Path draggerShape;
    static constexpr auto draggerSize = 40;
    static constexpr auto lineAlpha = 0.75f;

    const Colour lineColour =
        findDefaultColour(ColourIDs::SelectionComponent::outline)
            .withMultipliedAlpha(lineAlpha);

    const Colour fillColour =
        findDefaultColour(ColourIDs::Backgrounds::sidebarFill);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteResizerRight)
};
