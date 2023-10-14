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

class SpectrumFFT final
{
public:
    
    SpectrumFFT();
    
    void computeSpectrum(float *pcmBuffer,
        unsigned int pcmPosition, unsigned int pcmLength,
        Atomic<float> *spectrum, int length,
        int channel, int numchannels);

    // please make sure no consumer ever asks for a spectrum larger than that:
    static constexpr auto maxSpectrumSize = 512;

private:

    static constexpr auto cosTableBits = 13;
    static constexpr auto cosTableSize = 1 << cosTableBits;
    static constexpr auto tableRange = cosTableSize * 4;
    static constexpr auto tableMask = tableRange - 1;

    struct FftComplex final
    {
        float re = 0.f;
        float im = 0.f;
    };
    
    FftComplex buffer[maxSpectrumSize];
    float costab[cosTableSize];
    
    inline float cosine(float x) const noexcept;
    inline float sine(float x) const noexcept;
    inline unsigned int reverse(unsigned int val, int bits) const noexcept;
    inline void process(int bits);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumFFT)
};
