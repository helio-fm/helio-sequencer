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
#include "AudioCore.h"

WaveformAudioMonitorComponent::WaveformAudioMonitorComponent(WeakReference<AudioMonitor> targetAnalyzer) :
    Thread("WaveformAudioMonitor"),
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
    constexpr auto bufferLastIndex = WaveformAudioMonitorComponent::bufferSize - 1;

    while (!this->threadShouldExit())
    {
        Thread::sleep(35);

        // Shift buffers:
        for (int i = 0; i < bufferLastIndex; ++i)
        {
            this->lPeakBuffer[i] = this->lPeakBuffer[i + 1].get();
            this->rPeakBuffer[i] = this->rPeakBuffer[i + 1].get();
            this->lRmsBuffer[i] = this->lRmsBuffer[i + 1].get();
            this->rRmsBuffer[i] = this->rRmsBuffer[i + 1].get();
        }

        // Push next values:
        this->lPeakBuffer[bufferLastIndex] = this->audioMonitor->getPeak(0);
        this->rPeakBuffer[bufferLastIndex] = this->audioMonitor->getPeak(1);
        this->lRmsBuffer[bufferLastIndex] = this->audioMonitor->getRootMeanSquare(0);
        this->rRmsBuffer[bufferLastIndex] = this->audioMonitor->getRootMeanSquare(1);

        this->triggerAsyncUpdate();
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
    // -69 instead of -70 to have that nearly invisible horizontal line:
    static constexpr auto maxDb = +4.f;
    static constexpr auto minDb = -69.f;
    const auto vauleInDb = jlimit(minDb, maxDb, 20.f * AudioCore::fastLog10(peak));
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

    g.setColour(this->peaksColour);

    for (int i = 0; i < w - 1; ++i)
    {
        const float fancyFade = (i == 0 || i == (w - 2)) ? 0.75f : ((i == 1 || i == (w - 3)) ? 0.9f : 1.f);
        const float peakL = waveformIecLevel(this->lPeakBuffer[i].get()) * midH * fancyFade;
        const float peakR = waveformIecLevel(this->rPeakBuffer[i].get()) * midH * fancyFade;
        g.fillRect(1.f + (i * 2.f), midH - peakL, 1.f, peakR + peakL);
    }

    g.setColour(this->rmsColour);

    for (int i = 0; i < w; ++i)
    {
        const float fancyFade = (i == 0 || i == (w - 1)) ? 0.85f : ((i == 1 || i == (w - 2)) ? 0.95f : 1.f);
        const float rmsL = waveformIecLevel(this->lRmsBuffer[i].get()) * midH * fancyFade;
        const float rmsR = waveformIecLevel(this->rRmsBuffer[i].get()) * midH * fancyFade;
        g.fillRect(i * 2.f, midH - rmsL, 1.f, rmsR + rmsL);
    }
}
