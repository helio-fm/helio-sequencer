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

class IconComponent;

#include "ColourIDs.h"
#include "CachedLabelImage.h"

// This uses a mix of labels and svg icons to display note names,
// because even the Noto typeface lacks double-sharp and double-flat signs
class NoteNameComponent final : public Component
{
public:
    
    explicit NoteNameComponent(bool isCentered = false,
        float fontSize = Globals::UI::Fonts::S);

    ~NoteNameComponent();

    void resized() override;

    void setNoteName(const String &noteName,
        Optional<String> detailsText,
        bool useFixedDoNotation) noexcept;

    int getRequiredWidth() const noexcept;
    float getContentWidthFloat() const noexcept;

    // a fallback string which can be used as a label
    const String &getText() const noexcept;
    using CachedNoteImage = CachedLabelImage<NoteNameComponent>;

private:

    const bool isCentered;

    String noteName;
    float textWidth = 0.f;

    Optional<String> detailsText;
    float detailsWidth = 0.f;

    String fallbackLabelText;

    UniquePointer<Label> nameLabel;
    UniquePointer<Label> detailsLabel;

    UniquePointer<IconComponent> prefix; // ^/v, the up/down signs
    UniquePointer<IconComponent> suffix; // (double)flat or (double)sharp

    Rectangle<float> prefixBounds;
    Rectangle<float> suffixBounds;

    const int iconSize = 11;
    const Colour textColour = findDefaultColour(Label::textColourId);

    static constexpr int iconMargin = 1;
    static constexpr int detailsMargin = 3;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteNameComponent)
};
