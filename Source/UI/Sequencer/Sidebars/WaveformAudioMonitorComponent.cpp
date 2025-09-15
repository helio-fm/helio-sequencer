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
#include "WaveformAudioMonitorComponent.h"
#include "AudioMonitor.h"

WaveformAudioMonitorComponent::WaveformAudioMonitorComponent(WeakReference<AudioMonitor> targetAnalyzer) :
    audioMonitor(targetAnalyzer)
{
    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);

    if (this->audioMonitor != nullptr)
    {
        this->startTimerHz(30);
    }
}

WaveformAudioMonitorComponent::~WaveformAudioMonitorComponent()
{
    this->stopTimer();
}

void WaveformAudioMonitorComponent::setTargetAnalyzer(WeakReference<AudioMonitor> targetAnalyzer)
{
    if (targetAnalyzer != nullptr)
    {
        this->audioMonitor = targetAnalyzer;
        this->startTimerHz(30);
    }
}

void WaveformAudioMonitorComponent::timerCallback()
{
    constexpr auto lastFrameIndex = WaveformAudioMonitorComponent::bufferSize - 1;

    // Shift buffers:
    for (int i = 0; i < lastFrameIndex; ++i)
    {
        this->peakBufferLeft[i] = this->peakBufferLeft[i + 1];
        this->peakBufferRight[i] = this->peakBufferRight[i + 1];
        this->rmsBufferLeft[i] = this->rmsBufferLeft[i + 1];
        this->rmsBufferRight[i] = this->rmsBufferRight[i + 1];
    }

    // Push next values:
    this->peakBufferLeft[lastFrameIndex] = this->audioMonitor->getPeak(0);
    this->peakBufferRight[lastFrameIndex] = this->audioMonitor->getPeak(1);
    this->rmsBufferLeft[lastFrameIndex] = this->audioMonitor->getRootMeanSquare(0);
    this->rmsBufferRight[lastFrameIndex] = this->audioMonitor->getRootMeanSquare(1);

    if (this->peakBufferLeft[lastFrameIndex] > 0.f ||
        this->peakBufferRight[lastFrameIndex] > 0.f)
    {
        this->emptyFramesCounter = 0;
    }
    else if (this->emptyFramesCounter <= WaveformAudioMonitorComponent::bufferSize)
    {
        this->emptyFramesCounter++;
    }

    if (this->emptyFramesCounter <= WaveformAudioMonitorComponent::bufferSize)
    {
        this->repaint();
    }
}

//===----------------------------------------------------------------------===//
// Helpers
//===----------------------------------------------------------------------===//

static inline float fastLog2(float value) noexcept
{
    auto x = float(bitCast<uint32_t, float>(value));
    x *= 1.1920928955078125e-7f;
    return x - 126.94269504f;
}

static inline float fastLog10(float val) noexcept
{
    return fastLog2(val) / 3.32192809488f;
}

static inline float iecLevel(float db) noexcept
{
    if (db < -70.f) { return 0.f; }
    else if (db < -60.f) { return (db + 70.f) * 0.0025f; }
    else if (db < -50.f) { return (db + 60.f) * 0.005f + 0.025f; }
    else if (db < -40.f) { return (db + 50.f) * 0.0075f + 0.075f; }
    else if (db < -30.f) { return (db + 40.f) * 0.015f + 0.15f; }
    else if (db < -20.f) { return (db + 30.f) * 0.02f + 0.3f; }
    return (db + 20.f) * 0.025f + 0.5f;
}

static inline float waveformIecLevel(float peak) noexcept
{
    // -69 instead of -70 to have that nearly invisible horizontal line:
    constexpr auto minDb = -69.f;
    constexpr auto maxDb = +4.f;
    const auto vauleInDb = jlimit(minDb, maxDb, 20.f * fastLog10(peak));
    return iecLevel(vauleInDb);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void WaveformAudioMonitorComponent::paint(Graphics &g)
{
    if (this->audioMonitor == nullptr)
    {
        return;
    }
    
    const float midH = float(this->getHeight()) / 2.f;
    constexpr int w = WaveformAudioMonitorComponent::bufferSize;
    
#if PLATFORM_DESKTOP
    // the audio monitor is not supposed to be stretched on desktop platforms
    constexpr float peakStretch = 1.f;
#elif PLATFORM_MOBILE
    // the entire sidebar can be stretched to avoid being cut-off by frontal camera,
    // let's try to make the audio monotor look nicer in that case
    const float peakStretch = float(this->getWidth()) / float(w * 2.f);
#endif

    g.setColour(this->peaksColour);

    for (int i = 0; i < w - 1; ++i)
    {
        const float fancyFade = (i == 0 || i == (w - 2)) ? 0.75f : ((i == 1 || i == (w - 3)) ? 0.9f : 1.f);
        const float peakL = waveformIecLevel(this->peakBufferLeft[i]) * midH * fancyFade;
        const float peakR = waveformIecLevel(this->peakBufferRight[i]) * midH * fancyFade;
        g.fillRect(((i * 2.f) * peakStretch) + peakStretch, midH - peakL, 1.f, peakR + peakL);
    }

    g.setColour(this->rmsColour);

    for (int i = 0; i < w; ++i)
    {
        const float fancyFade = (i == 0 || i == (w - 1)) ? 0.85f : ((i == 1 || i == (w - 2)) ? 0.95f : 1.f);
        const float rmsL = waveformIecLevel(this->rmsBufferLeft[i]) * midH * fancyFade;
        const float rmsR = waveformIecLevel(this->rmsBufferRight[i]) * midH * fancyFade;
        g.fillRect((i * 2.f) * peakStretch, midH - rmsL, 1.f, rmsR + rmsL);
    }
}
