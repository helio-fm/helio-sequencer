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
#include "WaveformAudioMonitorComponent.h"
#include "AudioMonitor.h"
#include "AudioCore.h"
#include "ColourIDs.h"

WaveformAudioMonitorComponent::WaveformAudioMonitorComponent(WeakReference<AudioMonitor> targetAnalyzer) :
    Thread("WaveformAudioMonitor"),
    colour(findDefaultColour(ColourIDs::AudioMonitor::foreground)),
    audioMonitor(targetAnalyzer)
{
    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);

    if (this->audioMonitor != nullptr)
    {
        this->startThread(6);
    }
}

WaveformAudioMonitorComponent::~WaveformAudioMonitorComponent()
{
    this->stopThread(1000);
}

void WaveformAudioMonitorComponent::setTargetAnalyzer(WeakReference<AudioMonitor> targetAnalyzer)
{
    if (targetAnalyzer != nullptr)
    {
        this->audioMonitor = targetAnalyzer;
        this->startThread(6);
    }
}

void WaveformAudioMonitorComponent::run()
{
    while (! this->threadShouldExit())
    {
        Thread::sleep(jlimit(10, 100, 35 - this->skewTime));
        const double b = Time::getMillisecondCounterHiRes();

        // Shift buffers:
        for (int i = 0; i < WaveformAudioMonitorComponent::bufferSize - 1; ++i)
        {
            this->lPeakBuffer[i] = this->lPeakBuffer[i + 1].get();
            this->rPeakBuffer[i] = this->rPeakBuffer[i + 1].get();
            this->lRmsBuffer[i] = this->lRmsBuffer[i + 1].get();
            this->rRmsBuffer[i] = this->rRmsBuffer[i + 1].get();
        }

        const int i = WaveformAudioMonitorComponent::bufferSize - 1;

        // Push next values:
        this->lPeakBuffer[i] = this->audioMonitor->getPeak(0);
        this->rPeakBuffer[i] = this->audioMonitor->getPeak(1);
        this->lRmsBuffer[i] = this->audioMonitor->getRootMeanSquare(0);
        this->rRmsBuffer[i] = this->audioMonitor->getRootMeanSquare(1);

        if (this->isVisible())
        {
            this->triggerAsyncUpdate();
        }

        const double a = Time::getMillisecondCounterHiRes();
        this->skewTime = int(a - b);
    }
}

void WaveformAudioMonitorComponent::handleAsyncUpdate()
{
    this->repaint();
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

inline static float waveformIecLevel(float peak)
{
    static constexpr auto maxDb = +4.0f;
    static constexpr auto minDb = -69.0f;
    // -69 instead of -70 to have that nearly invisible horizontal line

    const float vauleInDb = jlimit(minDb, maxDb,
        20.0f * AudioCore::fastLog10(peak));

    return AudioCore::iecLevel(vauleInDb);
}

void WaveformAudioMonitorComponent::paint(Graphics &g)
{
    if (this->audioMonitor == nullptr)
    {
        return;
    }
    
    const float midH = float(this->getHeight()) / 2.f;
    constexpr int w = WaveformAudioMonitorComponent::bufferSize;

    g.setColour(this->colour.withAlpha(0.2f));

    for (int i = 0; i < w - 1; ++i)
    {
        const float fancyFade = (i == 0 || i == (w - 2)) ? 0.75f : ((i == 1 || i == (w - 3)) ? 0.9f : 1.f);
        const float peakL = waveformIecLevel(this->lPeakBuffer[i].get()) * midH * fancyFade;
        const float peakR = waveformIecLevel(this->rPeakBuffer[i].get()) * midH * fancyFade;
        g.fillRect(1.f + (i * 2.f), midH - peakL, 1.f, peakR + peakL);
    }

    g.setColour(this->colour.withAlpha(0.25f));

    for (int i = 0; i < w; ++i)
    {
        const float fancyFade = (i == 0 || i == (w - 1)) ? 0.85f : ((i == 1 || i == (w - 2)) ? 0.95f : 1.f);
        const float rmsL = waveformIecLevel(this->lRmsBuffer[i].get()) * midH * fancyFade;
        const float rmsR = waveformIecLevel(this->rRmsBuffer[i].get()) * midH * fancyFade;
        g.fillRect(i * 2.f, midH - rmsL, 1.f, rmsR + rmsL);
    }
}
