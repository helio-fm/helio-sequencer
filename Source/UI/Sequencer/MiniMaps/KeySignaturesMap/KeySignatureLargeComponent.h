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

class NoteNameComponent;

#include "KeySignatureComponent.h"
#include "ColourIDs.h"

class KeySignatureLargeComponent final : public KeySignatureComponent
{
public:

    KeySignatureLargeComponent(KeySignaturesProjectMap &parent,
        const KeySignatureEvent &targetEvent) noexcept;

    ~KeySignatureLargeComponent();

    float getTextWidth() const override;
    void updateContent(const Temperament::Period &keyNames, bool useFixedDo) override;
    void setRealBounds(const Rectangle<float> bounds) override;

    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;

    static constexpr auto keySignatureHeight = 25;

    //===------------------------------------------------------------------===//
    // SelectableComponent
    //===------------------------------------------------------------------===//

    void setSelected(bool selected) override;
    bool isSelected() const noexcept override;
    const String &getSelectionGroupId() const noexcept override;

private:

    ComponentDragger dragger;
    KeySignatureEvent anchor;

    void startDragging();
    bool getDraggingDelta(const MouseEvent &e, float &outDelta);
    KeySignatureEvent continueDragging(float deltaBeat) const noexcept;
    void endDragging();

    static constexpr int labelX = 4;
    static constexpr int labelWidth = 300;

    float textWidth = 0.f;

    bool draggingState = false;
    bool draggingHadCheckpoint = false;
    bool selectedState = false;

    UniquePointer<NoteNameComponent> nameComponent;

    const Colour fillColour = findDefaultColour(ColourIDs::Roll::noteNameFill);
    const Colour borderColour = findDefaultColour(Label::textColourId);

    static constexpr float fillUnfocusedAlpha = 0.2f;
    static constexpr float borderUnfocusedAlpha = 0.65f;
    static constexpr float fillFocusedAlpha = 0.4f;
    static constexpr float borderFocusedAlpha = 0.75f;

    float fillAlpha = fillUnfocusedAlpha;
    float borderAlpha = borderUnfocusedAlpha;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeySignatureLargeComponent)
};
