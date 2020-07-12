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

class SpectralLogo final : public Component,
    private Thread, private AsyncUpdater
{
public:

    SpectralLogo();

    ~SpectralLogo() override;
    
    void paint(Graphics &g) override;
    void resized() override;

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

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Band);
    };
    
private:
    
    void run() override;
    void handleAsyncUpdate() override;
    
    Colour colour;
    OwnedArray<SpectralLogo::Band> bands;
    Path wave;
    
    static constexpr int bandCount = 70;
    
    int skewTime = 0;
    float pulse = 0.f;
    
    float randomnessRange = 0;
    float lineThickness = 0;
    float lineStepSize = 0;
    float lineWidth = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectralLogo);
};
