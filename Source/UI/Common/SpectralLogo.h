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

#include "ColourIDs.h"

class SpectralLogo final : public Component, private Timer
{
public:

    SpectralLogo();
    ~SpectralLogo() override;
    
    void paint(Graphics &g) override;
    void resized() override;
    void parentHierarchyChanged() override;

    float getLineThickness() const noexcept;
    float getLineStepSize() const noexcept;
    float getLineWidth() const noexcept;

private:
    
    class Band final
    {
    public:
        
        explicit Band(SpectralLogo *parent);
        void reset();

        inline Path buildPath(float v, float cx, float cy, float h,
            float radians, int numSkippedSegments, uint32 timeNow);

    private:
        
        SpectralLogo *parent;

        float value = 0.f;
        float valueDecay = 1.f;
        uint32 valueDecayStart = 0;

        float peak = 0.f;
        float peakDecay = 1.f;
        float peakDecayColour = 1.f;
        uint32 peakDecayStart = 0;

        static constexpr auto peakMaxAlpha = 0.75f;
        static constexpr auto bandFadeTimeMs = 2000.f;
        static constexpr auto peakFadeTimeMs = 7000.f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Band)
    };
    
private:
    
    void timerCallback() override;
    
    static float timeToDistance(float time, float startSpeed = 0.f,
        float midSpeed = 2.f, float endSpeed = 0.f) noexcept
    {
        return (time < 0.5f) ?
            time * (startSpeed + time * (midSpeed - startSpeed)) :
            0.5f * (startSpeed + 0.5f * (midSpeed - startSpeed)) +
                (time - 0.5f) * (midSpeed + (time - 0.5f) * (endSpeed - midSpeed));
    }

    const Colour colour = findDefaultColour(ColourIDs::Logo::fill);

    OwnedArray<SpectralLogo::Band> bands;
    Path wave;
    
    static constexpr auto bandCount = 70;

    float pulse = 0.f;
    
    float randomnessRange = 0;
    float lineThickness = 0;
    float lineStepSize = 0;
    float lineWidth = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectralLogo)
};
