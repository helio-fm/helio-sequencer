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

#define WAVEFORM_METER_MAXDB (+4.0f)
// -69 instead of -70 to have that nearly invisible horizontal line
#define WAVEFORM_METER_MINDB (-69.0f)

WaveformAudioMonitorComponent::WaveformAudioMonitorComponent(WeakReference<AudioMonitor> targetAnalyzer) :
    Thread("Volume Component"),
    colour(findDefaultColour(ColourIDs::AudioMonitor::foreground)),
    audioMonitor(targetAnalyzer),
    skewTime(0)
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
        for (int i = 0; i < WAVEFORM_METER_BUFFER_SIZE - 1; ++i)
        {
            this->lPeakBuffer[i] = this->lPeakBuffer[i + 1].get();
            this->rPeakBuffer[i] = this->rPeakBuffer[i + 1].get();
            this->lRmsBuffer[i] = this->lRmsBuffer[i + 1].get();
            this->rRmsBuffer[i] = this->rRmsBuffer[i + 1].get();
        }

        const int i = WAVEFORM_METER_BUFFER_SIZE - 1;

        // Push next values:
        this->lPeakBuffer[i] = this->audioMonitor->getPeak(0);
        this->rPeakBuffer[i] = this->audioMonitor->getPeak(1);
        this->lRmsBuffer[i] = this->audioMonitor->getRootMeanSquare(0);
        this->rRmsBuffer[i] = this->audioMonitor->getRootMeanSquare(1);

        this->triggerAsyncUpdate();

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

inline static float iecLevel(float peak)
{
    const float vauleInDb =
        jlimit(WAVEFORM_METER_MINDB, WAVEFORM_METER_MAXDB,
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

    g.setColour(this->colour.withAlpha(0.2f));

    for (int i = 0; i < WAVEFORM_METER_BUFFER_SIZE; ++i)
    {
        const float peakL = iecLevel(this->lPeakBuffer[i].get()) * midH;
        const float peakR = iecLevel(this->rPeakBuffer[i].get()) * midH;
        g.drawVerticalLine(1 + i * 2, midH - peakL, midH + peakR);
    }

    g.setColour(this->colour.withAlpha(0.25f));

    for (int i = 0; i < WAVEFORM_METER_BUFFER_SIZE; ++i)
    {
        const float rmsL = iecLevel(this->lRmsBuffer[i].get()) * midH;
        const float rmsR = iecLevel(this->rRmsBuffer[i].get()) * midH;
        g.drawVerticalLine(i * 2, midH - rmsL, midH + rmsR);
    }
}
