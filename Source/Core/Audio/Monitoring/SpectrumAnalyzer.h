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

const int FFT_COSTABBITS = 13;
const int FFT_COSTABSIZE = (1 << FFT_COSTABBITS);
const int FFT_TABLERANGE = (FFT_COSTABSIZE * 4);
const int FFT_TABLEMASK  = (FFT_TABLERANGE - 1);

// Make sure no consumer ever asks for a spectrum larger than that:
#define FFT_MAX_SPECTRUM_SIZE (512)

class SpectrumFFT final
{
public:
    
    SpectrumFFT();
    
    void computeSpectrum(float *pcmbuffer,
        unsigned int pcmposition,
        unsigned int pcmlength,
        Atomic<float> *spectrum,
        int length,
        int channel,
        int numchannels);
    
private:
    
    typedef struct
    {
        float re;
        float im;
    } FftComplex;
    
    FftComplex buffer[FFT_MAX_SPECTRUM_SIZE];
    float costab[FFT_COSTABSIZE];
    
    inline const float cosine(float x);
    inline const float sine(float x);
    inline const unsigned int reverse(unsigned int val, int bits);
    inline void process(int bits);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumFFT);
};
