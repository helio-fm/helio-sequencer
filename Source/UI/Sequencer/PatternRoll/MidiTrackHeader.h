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
class MidiTrack;
//[/Headers]


class MidiTrackHeader final : public Component,
                              public Button::Listener
{
public:

    MidiTrackHeader(MidiTrack *track);
    ~MidiTrackHeader();

    //[UserMethods]
    void updateContent();
    const MidiTrack *getTrack() const noexcept;
    inline bool isDetached() const noexcept { return this->track == nullptr; }
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;


private:

    //[UserVariables]
    MidiTrack *track;

    Image fillImage;
    Colour borderLightColour;
    Colour borderDarkColour;
    Colour lineColour;
    int textWidth;
    //[/UserVariables]

    ScopedPointer<Label> trackNameLabel;
    ScopedPointer<ImageButton> setNameButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiTrackHeader)
};
