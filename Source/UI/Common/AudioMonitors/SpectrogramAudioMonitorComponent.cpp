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
#include "SpectrogramAudioMonitorComponent.h"
#include "AudioMonitor.h"
#include "AudioCore.h"

#define HQ_METER_MAXDB (+4.0f)
// -69 instead of -70 to have that nearly invisible horizontal line
#define HQ_METER_MINDB (-69.0f)
#define HQ_METER_DECAY_RATE1 (1.0f - 3E-5f)
#define HQ_METER_DECAY_RATE2 (1.0f - 3E-7f)
#define HQ_METER_CYCLES_BEFORE_PEAK_FALLOFF 100

SpectrogramAudioMonitorComponent::SpectrogramAudioMonitorComponent(WeakReference<AudioMonitor> targetAnalyzer) :
    Thread("Volume Component"),
    volumeAnalyzer(std::move(targetAnalyzer)),
    skewTime(0)
{
    this->setInterceptsMouseClicks(false, false);
    
    if (this->volumeAnalyzer != nullptr)
    {
        this->startThread(5);
    }
}

SpectrogramAudioMonitorComponent::~SpectrogramAudioMonitorComponent()
{
    this->stopThread(1000);
}

void SpectrogramAudioMonitorComponent::setTargetAnalyzer(WeakReference<AudioMonitor> targetAnalyzer)
{
    if (targetAnalyzer != nullptr)
    {
        this->volumeAnalyzer = targetAnalyzer;
        this->startThread(5);
    }
}

void SpectrogramAudioMonitorComponent::run()
{
    while (! this->threadShouldExit())
    {
        Thread::sleep(jlimit(10, 100, 35 - this->skewTime));
        const double b = Time::getMillisecondCounterHiRes();
        this->triggerAsyncUpdate();

        // Shift buffers:
        for (int i = 0; i < SPECTROGRAM_BUFFER_SIZE - 1; ++i)
        {
            this->lPeakBuffer[i] = this->lPeakBuffer[i + 1].get();
            this->rPeakBuffer[i] = this->rPeakBuffer[i + 1].get();
            this->lRmsBuffer[i] = this->lRmsBuffer[i + 1].get();
            this->rRmsBuffer[i] = this->rRmsBuffer[i + 1].get();
        }

        const int i = SPECTROGRAM_BUFFER_SIZE - 1;

        // Push next values:
        this->lPeakBuffer[i] = this->volumeAnalyzer->getPeak(0);
        this->rPeakBuffer[i] = this->volumeAnalyzer->getPeak(1);
        this->lRmsBuffer[i] = this->volumeAnalyzer->getRootMeanSquare(0);
        this->rRmsBuffer[i] = this->volumeAnalyzer->getRootMeanSquare(1);

        const double a = Time::getMillisecondCounterHiRes();
        this->skewTime = int(a - b);
    }
}

void SpectrogramAudioMonitorComponent::handleAsyncUpdate()
{
    this->repaint();
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

inline static float iecLevel(float peak)
{
    const float vauleInDb =
        jlimit(HQ_METER_MINDB, HQ_METER_MAXDB,
            20.0f * AudioCore::fastLog10(peak));

    return AudioCore::iecLevel(vauleInDb);
}

void SpectrogramAudioMonitorComponent::paint(Graphics &g)
{
    if (this->volumeAnalyzer == nullptr)
    {
        return;
    }
    
    const int w = float(this->getWidth());
    const int h = float(this->getHeight());
    
    const float midH = float(this->getHeight()) / 2.f;

    g.setColour(Colours::white.withAlpha(0.05f));

    for (int i = 0; i < SPECTROGRAM_BUFFER_SIZE; ++i)
    {
        const float peakL = iecLevel(this->lPeakBuffer[i].get()) * midH;
        const float peakR = iecLevel(this->rPeakBuffer[i].get()) * midH;
        g.drawVerticalLine(i, midH - peakL, midH);
        g.drawVerticalLine(i, midH, midH + peakR);
    }

    g.setColour(Colours::white.withAlpha(0.1f));

    for (int i = 0; i < SPECTROGRAM_BUFFER_SIZE; ++i)
    {
        const float rmsL = iecLevel(this->lRmsBuffer[i].get()) * midH;
        const float rmsR = iecLevel(this->rRmsBuffer[i].get()) * midH;
        g.drawVerticalLine(i, midH - rmsL, midH);
        g.drawVerticalLine(i, midH, midH + rmsR);
    }
}
