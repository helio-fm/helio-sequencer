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

    KeySignatureLargeComponent(KeySignaturesProjectMap &parent, const KeySignatureEvent &targetEvent);
    ~KeySignatureLargeComponent();

    float getTextWidth() const override;
    void updateContent(const Temperament::Period &keyNames, bool useFixedDo) override;
    void setRealBounds(const Rectangle<float> bounds) override;

    void paint(Graphics &g) override;
    void resized() override;

    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;
    void mouseEnter(const MouseEvent &e) override;
    void mouseExit(const MouseEvent &e) override;

    static constexpr auto keySignatureHeight = 25;

private:

    ComponentDragger dragger;
    KeySignatureEvent anchor;

    float textWidth = 0.f;

    Rectangle<float> boundsOffset;
    Point<int> clickOffset;

    bool draggingState = false;
    bool draggingHadCheckpoint = false;

    Path internalPath;

    UniquePointer<NoteNameComponent> nameComponent;

    const Colour fillColour = findDefaultColour(ColourIDs::Roll::noteNameFill);
    const Colour borderColour = findDefaultColour(Label::textColourId);

    static constexpr float fillUnfocusedAlpha = 0.2f;
    static constexpr float borderUnfocusedAlpha = 0.7f;
    static constexpr float fillFocusedAlpha = 0.5f;
    static constexpr float borderFocusedAlpha = 0.8f;

    float fillAlpha = fillUnfocusedAlpha;
    float borderAlpha = borderUnfocusedAlpha;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeySignatureLargeComponent)
};
