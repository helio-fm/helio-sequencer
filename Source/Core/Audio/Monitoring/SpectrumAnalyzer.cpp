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

#include "Common.h"
#include "SpectrumAnalyzer.h"

SpectrumFFT::SpectrumFFT()
{
    for (int count = 0; count < SpectrumFFT::cosTableSize; count++)
    {
        this->costab[count] = cosf(MathConstants<float>::halfPi *
            static_cast<float>(count) / static_cast<float>(SpectrumFFT::cosTableSize));
    }
}

inline float SpectrumFFT::cosine(float x) const noexcept
{
    int y;
    
    x *= SpectrumFFT::tableRange;
    y = static_cast<int>(x);
    if (y < 0)
    {
        y = -y;
    }

    y &= SpectrumFFT::tableMask;
    switch (y >> SpectrumFFT::cosTableBits)
    {
        case 0 : return  this->costab[y];
        case 1 : return -this->costab[(SpectrumFFT::cosTableSize - 1) - (y - (SpectrumFFT::cosTableSize * 1))];
        case 2 : return -this->costab[                                  (y - (SpectrumFFT::cosTableSize * 2))];
        case 3 : return  this->costab[(SpectrumFFT::cosTableSize - 1) - (y - (SpectrumFFT::cosTableSize * 3))];
    }
    
    return 0.f;
}

inline float SpectrumFFT::sine(float x) const noexcept
{
    return this->cosine(x - 0.25f);
}

inline unsigned int SpectrumFFT::reverse(unsigned int val, int bits) const noexcept
{
    unsigned int retn = 0;
    
    while (bits--)
    {
        retn <<= 1;
        retn |= (val & 1);
        val >>= 1;
    }
    
    return (retn);
}

inline void SpectrumFFT::process(int bits)
{
    int count, count2, count3;
    int fftlen = 1 << bits;
    unsigned i1 = fftlen / 2;
    int i2 = 1, i3, i4, y;
    float a1, a2, b1, b2, z1, z2;
    float oneOverN = 1.0f / fftlen;
    
    for (count = 0; count < bits; count++)
    {
        i3 = 0;
        i4 = i1;
        
        for (count2 = 0; count2 < i2; count2++)
        {
            y  = this->reverse(i3 / static_cast<int>(i1), bits);
            z1 = this->cosine(static_cast<float>(y) * oneOverN);
            z2 = -this->sine(static_cast<float>(y) * oneOverN);

            for (count3 = i3; count3 < i4; count3++)
            {
                a1 = this->buffer[count3].re;
                a2 = this->buffer[count3].im;
                
                b1 = (z1 * this->buffer[count3 + i1].re) - (z2 * this->buffer[count3 + i1].im);
                b2 = (z2 * this->buffer[count3 + i1].re) + (z1 * this->buffer[count3 + i1].im);
                
                this->buffer[count3].re = a1 + b1;
                this->buffer[count3].im = a2 + b2;
                
                this->buffer[count3 + i1].re = a1 - b1;
                this->buffer[count3 + i1].im = a2 - b2;
            }
            
            i3 += (i1 << 1);
            i4 += (i1 << 1);
        }
        
        i1 >>= 1;
        i2 <<= 1;
    }
}

void SpectrumFFT::computeSpectrum(float *pcmBuffer,
    unsigned int pcmPosition, unsigned int pcmLength,
    Atomic<float> *spectrum, int length,
    int channel, int numchannels)
{
    int count, bits, bitslength, nyquist;
    
    bitslength = length;
    bits = 0;
    while (bitslength > 1)
    {
        bitslength >>= 1;
        bits++;
    }
    
    // Apply Hanning window
    for (count = 0; count < length; count++)
    {
        const float percent = static_cast<float>(count) / static_cast<float>(length);
        const float window = 0.5f * (1.0f  - this->cosine(percent));
        
        this->buffer[count].re = pcmBuffer[pcmPosition] * window;
        this->buffer[count].re /= static_cast<float>(length);
        this->buffer[count].im = 0.00000001f;
        
        pcmPosition++;
        if (pcmPosition >= pcmLength)
        {
            pcmPosition = 0;
        }
    }

    this->process(bits);

    nyquist = (length / 2);
    for (count = 0; count < (nyquist - 1); ++count)
    {
        const auto n = this->reverse(count, bits);
        spectrum[count] = jmin(1.0f, 2.5f *
            (sqrtf((this->buffer[n].re * this->buffer[n].re)
                + (this->buffer[n].im * this->buffer[n].im))));
    }
}
