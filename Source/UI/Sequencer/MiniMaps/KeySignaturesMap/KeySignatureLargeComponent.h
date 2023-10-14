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

#include "KeySignatureComponent.h"

class KeySignatureLargeComponent final : public KeySignatureComponent
{
public:

    KeySignatureLargeComponent(KeySignaturesProjectMap &parent, const KeySignatureEvent &targetEvent);
    ~KeySignatureLargeComponent();

    float getTextWidth() const override;
    void updateContent(const StringArray &keyNames) override;
    void setRealBounds(const Rectangle<float> bounds) override;

    void paint(Graphics &g) override;
    void mouseDown(const MouseEvent &e) override;
    void mouseDrag(const MouseEvent &e) override;
    void mouseUp(const MouseEvent &e) override;

private:

    ComponentDragger dragger;
    KeySignatureEvent anchor;

    float textWidth = 0.f;
    String eventName;

    Rectangle<float> boundsOffset;
    Point<int> clickOffset;

    bool draggingState = false;
    bool draggingHadCheckpoint = false;

    // workaround странного поведения juce
    // возможна ситуация, когда mousedown'а не было, а mouseup срабатывает
    bool mouseDownWasTriggered = false;

    UniquePointer<Label> signatureLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeySignatureLargeComponent)
};
