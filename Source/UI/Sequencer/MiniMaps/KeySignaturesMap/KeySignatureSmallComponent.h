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

class KeySignatureSmallComponent final : public KeySignatureComponent
{
public:

    KeySignatureSmallComponent(KeySignaturesProjectMap &parent, const KeySignatureEvent &targetEvent);
    ~KeySignatureSmallComponent();

    float getTextWidth() const override;
    void updateContent(const Temperament::Period &keyNames, bool useFixedDo) override;
    void setRealBounds(const Rectangle<float> bounds) override;

    void parentHierarchyChanged() override;

private:

    float textWidth = 0.f;

    Rectangle<float> boundsOffset;

    UniquePointer<Label> signatureLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KeySignatureSmallComponent)
};
