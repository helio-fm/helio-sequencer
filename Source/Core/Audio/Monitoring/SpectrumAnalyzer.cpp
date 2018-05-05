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

#define FFT_ABS(x)    ((x) < 0  ? -(x) : (x))
#define FFT_MAX(a, b) ((a) > (b) ? (a) : (b))
#define FFT_MIN(a, b) ((a) < (b) ? (a) : (b))

#define FFT_PI     3.14159265358979323846f
#define FFT_PI2   (3.14159265358979323846f * 2.0f)
#define FFT_PI_2  (3.14159265358979323846f / 2.0f)
#define FFT_SQRT2  1.41421356237309504880f
#define FFT_LOG2   0.693147180559945309417f

#if JUCE_WIN32

#define FFT_SIN   (float)sin
#define FFT_SINH  (float)sinh
#define FFT_COS   (float)cos
#define FFT_TAN   (float)tan
#define FFT_SQRT  (float)sqrt
#define FFT_POW   (float)pow
#define FFT_ATAN  (float)atan
#define FFT_EXP   (float)exp
#define FFT_LDEXP (float)ldexp
#define FFT_FABS  fabsf
#define FFT_LOG   (float)log
#define FFT_LOG10 (float)log10
#define FFT_FMOD  (float)fmod

#else

#define FFT_SIN   sinf
#define FFT_SINH  sinhf
#define FFT_COS   cosf
#define FFT_TAN   tanf
#define FFT_SQRT  sqrtf
#define FFT_POW   powf
#define FFT_ATAN  atanf
#define FFT_EXP   expf
#define FFT_LDEXP ldexpf
#define FFT_FABS  fabsf
#define FFT_LOG   logf
#define FFT_LOG10 log10f
#define FFT_EXP2(_val) powf(2.0f, _val)
#define FFT_FMOD  fmodf

#endif

SpectrumFFT::SpectrumFFT ()
{
    for (int count = 0; count < FFT_COSTABSIZE; count++)
    {
        this->costab[count] = FFT_COS(FFT_PI_2 * static_cast<float>(count) / static_cast<float>(FFT_COSTABSIZE));
    }
}

inline const float SpectrumFFT::cosine(float x)
{
    int y;
    
    x *= FFT_TABLERANGE;
    y = static_cast<int>(x);
    if (y < 0)
    {
        y = -y;
    }
    
    y &= FFT_TABLEMASK;
    switch (y >> FFT_COSTABBITS)
    {
        case 0 : return  this->costab[y];
        case 1 : return -this->costab[(FFT_COSTABSIZE - 1) - (y - (FFT_COSTABSIZE * 1))];
        case 2 : return -this->costab[                       (y - (FFT_COSTABSIZE * 2))];
        case 3 : return  this->costab[(FFT_COSTABSIZE - 1) - (y - (FFT_COSTABSIZE * 3))];
    }
    
    return 0.0f;
}

inline const float SpectrumFFT::sine(float x)
{
    return this->cosine(x - 0.25f);
}

inline const unsigned int SpectrumFFT::reverse(unsigned int val, int bits)
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
    int    count, count2, count3;
    unsigned        i1;
    int             i2, i3, i4, y;
    int             fftlen = 1 << bits;
    float           a1, a2, b1, b2, z1, z2;
    float           oneoverN = 1.0f / fftlen;
    
    i1 = fftlen / 2;
    i2 = 1;
    
    for (count = 0; count < bits; count++)
    {
        i3 = 0;
        i4 = i1;
        
        for (count2 = 0; count2 < i2; count2++)
        {
            y  = this->reverse(i3 / static_cast<int>(i1), bits);
            
            z1 =  this->cosine(static_cast<float>(y) * oneoverN);
            z2 =   -this->sine(static_cast<float>(y) * oneoverN);
            
            for (count3 = i3; count3 < i4; count3++)
            {
                a1 = this->buffer[count3].re;
                a2 = this->buffer[count3].im;
                
                b1 = (z1 * this->buffer[count3+i1].re) - (z2 * this->buffer[count3+i1].im);
                b2 = (z2 * this->buffer[count3+i1].re) + (z1 * this->buffer[count3+i1].im);
                
                this->buffer[count3].re = a1 + b1;
                this->buffer[count3].im = a2 + b2;
                
                this->buffer[count3+i1].re = a1 - b1;
                this->buffer[count3+i1].im = a2 - b2;
            }
            
            i3 += (i1 << 1);
            i4 += (i1 << 1);
        }
        
        i1 >>= 1;
        i2 <<= 1;
    }
}

void SpectrumFFT::computeSpectrum(float *pcmbuffer,
                                  unsigned int pcmposition,
                                  unsigned int pcmlength,
                                  Atomic<float> *spectrum,
                                  int length,
                                  int channel,
                                  int numchannels)
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
        float window;
        float percent = static_cast<float>(count) / static_cast<float>(length);
        
        window = 0.5f * (1.0f  - this->cosine(percent));
        
        this->buffer[count].re = pcmbuffer[pcmposition] * window;
        this->buffer[count].re /= static_cast<float>(length);
        this->buffer[count].im = 0.00000001f;
        
        pcmposition++;
        if (pcmposition >= pcmlength)
        {
            pcmposition = 0;
        }
    }

    this->process(bits);

    nyquist = (length / 2);
    for (count = 0; count < (nyquist - 1); ++count)
    {
        float magnitude;
        int n = count;
        
        n = this->reverse(n, bits);
        
        magnitude = 
            FFT_SQRT((this->buffer[n].re * this->buffer[n].re)
                + (this->buffer[n].im * this->buffer[n].im));
        
        magnitude *= 2.5f; 
        
        if (magnitude > 1.0f)
        {
            magnitude = 1.0f;
        }
        
        spectrum[count] = magnitude;
    }
}
