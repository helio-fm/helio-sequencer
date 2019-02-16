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
#include "ColourIDs.h"

#define MAXDB (+4.0f)
#define MINDB (-70.0f)
#define NUM_WAVES 7
#define PEAK_MAX_ALPHA 0.35f
#define BAND_FADE_MS 1000.f
#define PEAK_FADE_MS 2500.f
#define NUM_BANDS 70
#define NUM_SEGMENTS_TO_SKIP 8

SpectralLogo::SpectralLogo()
    : Thread("Spectral Logo"),
      colour(findDefaultColour(ColourIDs::Logo::fill)),
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
        Thread::sleep(jlimit(10, 100, 35 - this->skewTime));
        const double b = Time::getMillisecondCounterHiRes();
        this->triggerAsyncUpdate();
        const double a = Time::getMillisecondCounterHiRes();
        this->skewTime = int(a - b);
    }
}

void SpectralLogo::handleAsyncUpdate()
{
    this->pulse = fmodf(this->pulse + MathConstants<float>::pi / 18.f, MathConstants<float>::twoPi);
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
    const float cx = float(this->getWidth()) / 2.f;
    const float cy = float(this->getHeight()) / 2.f;
    const float bandSize = float(this->getHeight()) / 2.5f;

    {
        //Image img(Image::ARGB, this->getWidth(), this->getHeight(), true);
        //Graphics g1(img);

        const uint32 timeNow = Time::getMillisecondCounter();

        Random r;
        g.setColour(this->colour);

        for (int i = 0; i < this->bandCount; ++i)
        {
            const float waveOffset = - MathConstants<float>::halfPi;
            const float wavePosition = float(i) * (MathConstants<float>::twoPi * float(NUM_WAVES) / float(this->bandCount));
            const float multiplier = 0.5f + sinf(waveOffset + wavePosition) / 2.f;

            // Rays' sharpness:
            const float heptagramShape = 0.5f + ((multiplier * multiplier) / 2.f);
            //const float heptagramShape = 0.5f + ((multiplier * multiplier * multiplier) / 2.f);
            
            const float pulseScale = 1.4f;
            const float pulseMultiplier = pulseScale * (0.5f + (sinf(this->pulse) / 2.f));
            
            const float v =
                (heptagramShape * bandSize) -
                (r.nextFloat() * this->getRandomnessRange()) -
                (pulseMultiplier * pulseMultiplier * this->getRandomnessRange() * (0.5f - heptagramShape) * 0.5f);
            
            const float radians = float(i) * (MathConstants<float>::twoPi / float(this->bandCount));
            g.fillPath(this->bands[i]->buildPath(v, cx, cy, bandSize, radians, NUM_SEGMENTS_TO_SKIP, timeNow));
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
    parent(parent),
    value(0.f),
    valueDecay(1.f),
    valueDecayStart(0),
    peak(0.f),
    peakDecay(1.f),
    peakDecayColour(1.f),
    peakDecayStart(0)
{
}

void SpectralLogo::Band::reset()
{
    this->value = 0.f;
    this->valueDecay = 1.f;
    this->peak = 0.f;
    this->peakDecay = 1.f;
    this->peakDecayColour = PEAK_MAX_ALPHA;
}

static float timeToDistance(float time, float startSpeed = 0.f, float midSpeed = 2.f, float endSpeed = 0.f) noexcept
{
    return (time < 0.5f) ? time * (startSpeed + time * (midSpeed - startSpeed))
        : 0.5f * (startSpeed + 0.5f * (midSpeed - startSpeed))
        + (time - 0.5f) * (midSpeed + (time - 0.5f) * (endSpeed - midSpeed));
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
        float newProgress = msElapsed / BAND_FADE_MS;
        if (newProgress >= 0.f && newProgress < 1.f)
        {
            newProgress = timeToDistance(newProgress);
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
        float newProgress = msElapsed / PEAK_FADE_MS;
        if (newProgress >= 0.f && newProgress < 1.f)
        {
            newProgress = timeToDistance(newProgress);
            jassert(newProgress >= this->peakDecay);
            const float delta = (newProgress - this->peakDecay) / (1.f - this->peakDecay);
            this->peakDecayColour = (newProgress * newProgress * newProgress) * PEAK_MAX_ALPHA;
            this->peakDecay = newProgress;
            this->peak -= (this->peak * delta);
        }
        else
        {
            this->peakDecayColour = PEAK_MAX_ALPHA;
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
