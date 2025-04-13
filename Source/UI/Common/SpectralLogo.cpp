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

#include "Common.h"
#include "SpectralLogo.h"
#include "ColourIDs.h"

SpectralLogo::SpectralLogo()
{
    for (int i = 0; i < SpectralLogo::bandCount; ++i)
    {
        this->bands.add(new SpectralLogo::Band(this));
    }
}

SpectralLogo::~SpectralLogo()
{ 
    this->stopTimer();
}

void SpectralLogo::timerCallback()
{
    static constexpr auto pulseSpeed = 35.f;
    this->pulse = fmodf(this->pulse +
        MathConstants<float>::pi / pulseSpeed,
        MathConstants<float>::twoPi);

    this->repaint();
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

void SpectralLogo::parentHierarchyChanged()
{
    if (this->isShowing())
    {
        this->startTimer(70);
    }
    else
    {
        this->stopTimer();
    }
}

void SpectralLogo::resized()
{
    this->randomnessRange = float(this->getWidth()) / 8.f;
    this->lineThickness = ceilf(float(this->getWidth()) / 96.f);
    this->lineStepSize = ceilf(this->lineThickness * 1.5f);
    this->lineWidth = (float(this->getWidth()) / float(SpectralLogo::bandCount)); //* 1.1f)
}

void SpectralLogo::paint(Graphics &g)
{
    const float cx = float(this->getWidth()) / 2.f;
    const float cy = float(this->getHeight()) / 2.f;
    const float bandSize = float(this->getHeight()) / 2.5f;

    {
        //Image img(Image::ARGB, this->getWidth(), this->getHeight(), true);
        //Graphics g1(img);

        const auto timeNow = Time::getMillisecondCounter();

        Random r;
        g.setColour(this->colour);

        static constexpr auto numWaves = 7.f;
        static constexpr auto coreCircleSize = 6;

        for (int i = 0; i < SpectralLogo::bandCount; ++i)
        {
            const float waveOffset = -MathConstants<float>::halfPi;
            const float wavePosition = float(i) * (MathConstants<float>::twoPi * numWaves / float(SpectralLogo::bandCount));
            const float multiplier = 0.5f + sinf(waveOffset + wavePosition) / 2.f;

            // Rays' sharpness:
            const float heptagramShape = 0.5f + ((multiplier * multiplier) / 2.f);
            //const float heptagramShape = 0.5f + ((multiplier * multiplier * multiplier) / 2.f);
            
            const float pulseScale = 1.4f;
            const float pulseMultiplier = pulseScale * (0.5f + (sinf(this->pulse) / 2.f));
            
            const float v =
                (heptagramShape * bandSize) -
                (r.nextFloat() * this->randomnessRange) -
                (pulseMultiplier * pulseMultiplier * this->randomnessRange * (0.5f - heptagramShape) * 0.5f);
            
            const float radians = float(i) * (MathConstants<float>::twoPi / float(SpectralLogo::bandCount));
            g.fillPath(this->bands[i]->buildPath(v, cx, cy, bandSize, radians, coreCircleSize, timeNow));
        }

        //static PNGImageFormat png;
        //static int i = 0;

        //File f("test" + String(++i) + ".png");
        //FileOutputStream outStream(f);
        //png.writeImageToStream(img, outStream);
        //outStream.flush();
    }
}

SpectralLogo::Band::Band(SpectralLogo *parent) :
    parent(parent) {}

void SpectralLogo::Band::reset()
{
    this->value = 0.f;
    this->valueDecay = 1.f;
    this->peak = 0.f;
    this->peakDecay = 1.f;
    this->peakDecayColour = Band::peakMaxAlpha;
}

inline Path SpectralLogo::Band::buildPath(float valueInY,
    float cx, float cy, float h, float radians,
    int numSkippedSegments, uint32 timeNow)
{
    if (this->value < valueInY)
    {
        this->value = valueInY;
        this->valueDecay = 0.f;
        this->valueDecayStart = Time::getMillisecondCounter();
    }
    else
    {
        const auto msElapsed = int(timeNow - this->valueDecayStart);
        float newProgress = msElapsed / Band::bandFadeTimeMs;
        if (newProgress >= 0.f && newProgress < 1.f)
        {
            newProgress = SpectralLogo::timeToDistance(newProgress);
            jassert(newProgress >= this->valueDecay);
            const float delta = (newProgress - this->valueDecay) / (1.f - this->valueDecay);
            this->valueDecay = newProgress;
            this->value -= (this->value * delta);
        }
        else
        {
            this->value = 0.f;
        }
    }

    if (this->peak < this->value)
    {
        this->peak = this->value;
        this->peakDecay = 0.f;
        this->peakDecayStart = Time::getMillisecondCounter();
    }
    else
    {
        const auto msElapsed = int(timeNow - this->peakDecayStart);
        float newProgress = msElapsed / Band::peakFadeTimeMs;
        if (newProgress >= 0.f && newProgress < 1.f)
        {
            newProgress = SpectralLogo::timeToDistance(newProgress);
            jassert(newProgress >= this->peakDecay);
            const float delta = (newProgress - this->peakDecay) / (1.f - this->peakDecay);
            this->peakDecayColour = (newProgress * newProgress * newProgress) * Band::peakMaxAlpha;
            this->peakDecay = newProgress;
            this->peak -= (this->peak * delta);
        }
        else
        {
            this->peakDecayColour = Band::peakMaxAlpha;
            this->peak = 0.f;
        }
    }
    
    const float lineStepSize = this->parent->getLineStepSize();
    const float lineThickness = this->parent->getLineThickness();
    const float lineWidth = this->parent->getLineWidth();

    this->peak = jmax(this->peak, lineStepSize * (numSkippedSegments + 1));
    
    Path path;
    
    int segmentIndex = 0;
    for (float i = 0.f; i < this->value; i += lineStepSize)
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
