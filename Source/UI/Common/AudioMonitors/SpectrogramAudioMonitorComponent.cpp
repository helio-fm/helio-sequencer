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
#include "ColourIDs.h"

#define SPECTROGRAM_METER_MAXDB (+4.0f)
#define SPECTROGRAM_METER_MINDB (-70.0f)

static const float kSpectrumFrequencies[] =
{
    50.f,    56.f,     63.f,    71.f,    80.f,    90.f,
    100.f,   112.f,    125.f,   140.f,   160.f,   180.f,
    200.f,   224.f,    250.f,   280.f,   315.f,   355.f,
    400.f,   450.f,    500.f,   560.f,   630.f,   710.f,
    800.f,   900.f,    1000.f,  1120.f,  1250.f,  1400.f,
    1600.f,  1800.f,   2000.f,  2240.f,  2500.f,  2800.f,
    3150.f,  3550.f,   4000.f,  4500.f,  5000.f,  5600.f,
    6300.f,  7100.f,   8000.f,  9000.f, 10000.f, 11200.f,
    12500.f, 14000.f,  16000.f
};

SpectrogramAudioMonitorComponent::SpectrogramAudioMonitorComponent(WeakReference<AudioMonitor> targetAnalyzer) :
    Thread("Volume Component"),
    colour(findDefaultColour(ColourIDs::AudioMonitor::foreground)),
    audioMonitor(targetAnalyzer),
    head(0),
    skewTime(0)
{
    this->setInterceptsMouseClicks(false, false);
    this->setPaintingIsUnclipped(true);

    if (this->audioMonitor != nullptr)
    {
        this->startThread(6);
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
        this->audioMonitor = targetAnalyzer;
        this->startThread(6);
    }
}

void SpectrogramAudioMonitorComponent::run()
{
    while (! this->threadShouldExit())
    {
        Thread::sleep(jlimit(10, 100, 35 - this->skewTime));
        const double b = Time::getMillisecondCounterHiRes();

        // Move to the next row:
        this->head = (this->head.get() + 1) % SPECTROGRAM_BUFFER_SIZE;

        // Update matrix:
        for (int i = 0; i < SPECTROGRAM_NUM_BANDS; ++i)
        {
            this->spectrum[this->head.get()][i] =
                this->audioMonitor->getInterpolatedSpectrumAtFrequency(kSpectrumFrequencies[i]);
        }

        this->triggerAsyncUpdate();
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
        jlimit(SPECTROGRAM_METER_MINDB, SPECTROGRAM_METER_MAXDB,
            20.0f * AudioCore::fastLog10(peak));

    return AudioCore::iecLevel(vauleInDb);
}

void SpectrogramAudioMonitorComponent::paint(Graphics &g)
{
    if (this->audioMonitor == nullptr)
    {
        return;
    }
    
    const int h = this->getHeight();
    const int start = this->head.get();

    // Head:
    for (int i = start + 1; i < SPECTROGRAM_BUFFER_SIZE; ++i)
    {
        for (int j = SPECTROGRAM_NUM_BANDS; j-- > 0; )
        {
            const float x = float(i - start);
            const float v = jmin(1.f, iecLevel(this->spectrum[i][j].get()) * 2);
            g.setColour(this->colour.withAlpha(v));
            g.drawHorizontalLine(h - j * 4, x * 2.f, x * 2.f + 1.f);
        }
    }

    // Tail:
    for (int i = 0; i <= start; ++i)
    {
        for (int j = SPECTROGRAM_NUM_BANDS; j-- > 0; )
        {
            const float x = float(i + (SPECTROGRAM_BUFFER_SIZE - start));
            const float v = jmin(1.f, iecLevel(this->spectrum[i][j].get()) * 2);
            g.setColour(this->colour.withAlpha(v));
            g.drawHorizontalLine(h - j * 4, x * 2.f, x * 2.f + 1.f);
        }
    }
}
