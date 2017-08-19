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
#include "SpectralLogo.h"
#include <math.h>

#define MAXDB (+4.0f)
#define MINDB (-70.0f)
#define DECAY_RATE1 (1.0f - 3E-5f)
#define DECAY_RATE2 (1.0f - 3E-7f)
#define CYCLES_BEFORE_PEAK_FALLOFF 20
#define NUM_WAVES 7

#define NUM_BANDS 70
#define NUM_SEGMENTS_TO_SKIP 8

#define M_PI        3.14159265358979323846264338327950288
#define M_PI_2      1.57079632679489661923132169163975144   /* pi/2 */
#define M_2PI       6.28318530717958647692528676655900576   /* 2*pi */

SpectralLogo::SpectralLogo()
    : Thread("Spectral Logo"),
      bandCount(NUM_BANDS),
      skewTime(0),
      pulse(0.f),
      randomnessRange(0),
      lineThickness(0),
      lineStepSize(0),
      lineWidth(0)
{
    for (int band = 0; band < this->bandCount; ++band)
    {
        this->bands.add(new SpectralLogo::Band(this));
    }
    
    this->startThread(5);
}

SpectralLogo::~SpectralLogo()
{ 
    this->stopThread(1000);
}

void SpectralLogo::run()
{
    while (! this->threadShouldExit())
    {
        Thread::sleep(jlimit(10, 100, 70 - this->skewTime));
        const double b = Time::getMillisecondCounterHiRes();
        this->triggerAsyncUpdate();
        const double a = Time::getMillisecondCounterHiRes();
        this->skewTime = int(a - b);
    }
}

void SpectralLogo::handleAsyncUpdate()
{
    this->pulse = fmodf(this->pulse + M_PI / 9.f, M_2PI);
    this->repaint();
}

float SpectralLogo::getRandomnessRange() const noexcept
{
    return this->randomnessRange;
}

float SpectralLogo::getLineThickness() const noexcept
{
    return this->lineThickness;
}

float SpectralLogo::getLineStepSize() const noexcept
{
    return this->lineStepSize;
}

float SpectralLogo::getLineWidth() const noexcept
{
    return this->lineWidth;
}

void SpectralLogo::resized()
{
    this->randomnessRange = float(this->getWidth()) / 8.f;
    this->lineThickness = ceilf(float(this->getWidth()) / 128.f);
    this->lineStepSize = ceilf(this->lineThickness * 1.5f);
    this->lineWidth = (float(this->getWidth()) / float(NUM_BANDS) * 1.15f);
}

void SpectralLogo::paint(Graphics &g)
{
    const int width = this->getWidth();
    const int height = this->getHeight();

    Image img(Image::ARGB, width, height, true);
    const float cx = float(img.getWidth()) / 2.f;
    const float cy = float(img.getHeight()) / 2.f;
    const float bandSize = float(height) / 2.5f;

    //{
    //  g.setColour(Colours::white);
    //  g.fillEllipse(1, 1, img.getWidth() - 2, img.getHeight() - 2);
    //}

    {
        Random r;
        Graphics g1(img);
        Colour c(Colours::white.withAlpha(0.175f));
        g1.setColour(c);

        for (int i = 0; i < this->bandCount; ++i)
        {
            const float waveOffset = -(M_PI_2);
            const float wavePosition = ((M_2PI * NUM_WAVES) / float(this->bandCount)) * i;
            const float multiplier = 0.5f + sinf(waveOffset + wavePosition) / 2.f;

            // Controls rays sharpness:
            const float heptagramShape = 0.5f + ((multiplier * multiplier) / 2.f);
            //const float heptagramShape = 0.5f + ((multiplier * multiplier * multiplier) / 2.f);
            
            const float pulseScale = 1.4f;
            const float pulseMultiplier = pulseScale * (0.5f + (sinf(this->pulse) / 2.f));
            
            const float v =
                (heptagramShape * bandSize) -
                (r.nextFloat() * this->getRandomnessRange()) -
                (pulseMultiplier * pulseMultiplier * this->getRandomnessRange() * (0.5f - heptagramShape) * 0.5f);
            
            this->bands[i]->setValue(v);
            const float radians = (M_2PI / float(this->bandCount)) * i;
            this->bands[i]->drawBand(g1, cx, cy, bandSize, radians, NUM_SEGMENTS_TO_SKIP);
        }

        //static PNGImageFormat png;
        //static int i = 0;

        //File f("test" + String(++i) + ".png");
        //FileOutputStream outStream(f);
        //png.writeImageToStream(img, outStream);
        //outStream.flush();
    }

    g.drawImageAt(img, 0, 0);
}

SpectralLogo::Band::Band(SpectralLogo *parent) :
    meter(parent),
    value(0.0f),
    valueHold(0.0f),
    valueDecay(DECAY_RATE1),
    peak(0.0f),
    peakHold(0.0f),
    peakDecay(DECAY_RATE2)
{
}

void SpectralLogo::Band::setValue(float value)
{
    this->value = value;
}

void SpectralLogo::Band::reset()
{
    this->value = 0.0f;
    this->valueHold = 0.0f;
    this->valueDecay = DECAY_RATE1;
    this->peak = 0.0f;
    this->peakHold = 0.0f;
    this->peakDecay = DECAY_RATE2;
}

inline Path SpectralLogo::Band::buildPath(float cx, float cy,
    float h, float radians, int numSkippedSegments)
{
    float valueInY = this->value;
    
    if (this->valueHold < valueInY)
    {
        this->valueHold = valueInY;
        this->valueDecay = DECAY_RATE1;
    }
    else
    {
        this->valueHold = int(this->valueHold * this->valueDecay);
        
        if (this->valueHold < valueInY)
        {
            this->valueHold = valueInY;
        }
        else
        {
            this->valueDecay = this->valueDecay * this->valueDecay;
            valueInY = this->valueHold;
        }
    }

    if (this->peak < valueInY)
    {
        this->peak = valueInY;
        this->peakHold = 0.0;
        this->peakDecay = DECAY_RATE2;
    }
    else if (++this->peakHold > CYCLES_BEFORE_PEAK_FALLOFF)
    {
        this->peak = this->peak * this->peakDecay;
        this->peakDecay = this->peakDecay * this->peakDecay;
    }
    
    const float lineStepSize = this->meter->getLineStepSize();
    const float lineThickness = this->meter->getLineThickness();
    const float lineWidth = this->meter->getLineWidth();

    this->peak = jmax(this->peak, lineStepSize * (numSkippedSegments + 1));
    
    Path path;
    
    int segmentIndex = 0;
    for (float i = 0.f; i < valueInY; i += lineStepSize)
    {
        if (segmentIndex++ > numSkippedSegments)
        {
            const float w = lineWidth * (i / h);
            path.addLineSegment(Line<float>(-w, i, w, i), lineThickness);
        }
    }
    
    const float alignedPeak = roundf(this->peak / lineStepSize) * lineStepSize;
    const float peakH = jmax(this->peak, alignedPeak);
    const float w = lineWidth * (peakH / h);
    path.addLineSegment(Line<float>(-w, peakH, w, peakH), lineThickness / 4.f);
    
    AffineTransform transform = AffineTransform::translation(cx, cy).rotated(radians, cx, cy);
    path.applyTransform(transform);
    return path;
}

inline void SpectralLogo::Band::drawBand(Graphics &g, 
    float cx, float cy, 
    float h, float radians,
    int numSkippedSegments)
{
    g.fillPath(this->buildPath(cx, cy, h, radians, numSkippedSegments));
}
