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
#include "GenericAudioMonitorComponent.h"
#include "AudioMonitor.h"
#include "AudioCore.h"

#define GENERIC_METER_MAXDB (+4.0f)
#define GENERIC_METER_MINDB (-70.0f)
#define GENERIC_METER_BAND_FADE_MS 700.f
#define GENERIC_METER_PEAK_FADE_MS 1400.f

static const float kSpectrumFrequencies[] =
{
    31.5f,
    50.f, 80.f,
    125.f, 200.f,
    315.f, 500.f,
    800.f, 1250.f,
    2000.f, 3150.f,
    5000.f, 8000.f
};

GenericAudioMonitorComponent::GenericAudioMonitorComponent(WeakReference<AudioMonitor> monitor)
    : Thread("Spectrum Component"),
      audioMonitor(std::move(monitor)),
      skewTime(0)
{
    // (true, false) will enable switching rendering modes on click
    this->setInterceptsMouseClicks(false, false);

    for (int band = 0; band < GENERIC_METER_NUM_BANDS; ++band)
    {
        this->bands.add(new SpectrumBand());
    }
    
    if (this->audioMonitor != nullptr)
    {
        this->startThread(5);
    }
}

void GenericAudioMonitorComponent::setTargetAnalyzer(WeakReference<AudioMonitor> monitor)
{
    if (monitor != nullptr)
    {
        this->audioMonitor = monitor;
        this->startThread(5);
    }
}

GenericAudioMonitorComponent::~GenericAudioMonitorComponent()
{ 
    this->stopThread(1000);
}

void GenericAudioMonitorComponent::run()
{
    while (! this->threadShouldExit())
    {
        Thread::sleep(jlimit(10, 100, 35 - this->skewTime));
        const double b = Time::getMillisecondCounterHiRes();

        for (int i = 0; i < GENERIC_METER_NUM_BANDS; ++i)
        {
            const float v = this->audioMonitor->getInterpolatedSpectrumAtFrequency(kSpectrumFrequencies[i]);
            this->values[i].set(v);
        }

        this->triggerAsyncUpdate();
        const double a = Time::getMillisecondCounterHiRes();
        this->skewTime = int(a - b);
    }
}

void GenericAudioMonitorComponent::handleAsyncUpdate()
{
    this->repaint();
}

void GenericAudioMonitorComponent::resized()
{
    for (int i = 0; i < this->bands.size(); ++i)
    {
        this->bands[i]->reset();
    }
}

void GenericAudioMonitorComponent::paint(Graphics &g)
{
    if (this->audioMonitor == nullptr)
    {
        return;
    }
    
    const float w = float(this->getWidth());
    const float h = float(this->getHeight());
    const float size = w / float(GENERIC_METER_NUM_BANDS);
    const uint32 timeNow = Time::getMillisecondCounter();

    for (int i = 0; i < GENERIC_METER_NUM_BANDS; ++i)
    {
        this->bands[i]->drawBand(g, this->values[i].get(),
            i * size + 1.f, 0.f, size - 2.f, h, timeNow);
    }

    // TODO draw peak levels:

    // Show levels?
    //if (this->altMode)
    //{
    //    const int valueForMinus6dB = AudioCore::iecLevel(-6.f) * this->getHeight();
    //    const int valueForMinus12dB = AudioCore::iecLevel(-12.f) * this->getHeight();
    //    const int valueForMinus24dB = AudioCore::iecLevel(-24.f) * this->getHeight();
    //    
    //    g.setColour(Colours::white.withAlpha(0.075f));
    //    g.drawHorizontalLine(height - valueForMinus6dB, 0.f, this->getWidth());
    //    g.drawHorizontalLine(height - valueForMinus12dB, 0.f, this->getWidth());
    //    g.drawHorizontalLine(height - valueForMinus24dB, 0.f, this->getWidth());
    //}

    //{
    //    this->peakBand.setValue(this->volumeAnalyzer->getPeak(this->channel));
    //    const float left = (this->orientation == Left) ? 0.f : w;
    //    const float right = w - left;
    //    this->peakBand.drawBand(g1, left, right, h);
    //}
}

GenericAudioMonitorComponent::SpectrumBand::SpectrumBand() :
    value(0.f),
    valueDecayProgress(1.f),
    valueDecayStart(0),
    peak(0.f),
    peakDecayProgress(1.f),
    peakDecayStart(0)
{
}

void GenericAudioMonitorComponent::SpectrumBand::reset()
{
    this->value = 0.f;
    this->valueDecayProgress = 1.f;
    this->peak = 0.f;
    this->peakDecayProgress = 1.f;
}

float timeToDistance(float time, float startSpeed = 0.f,
    float midSpeed = 2.f, float endSpeed = 0.f) noexcept
{
    return (time < 0.5f) ? time * (startSpeed + time * (midSpeed - startSpeed))
        : 0.5f * (startSpeed + 0.5f * (midSpeed - startSpeed))
        + (time - 0.5f) * (midSpeed + (time - 0.5f) * (endSpeed - midSpeed));
}

inline void GenericAudioMonitorComponent::SpectrumBand::drawBand(
    Graphics &g, float signal, float x, float y, float w, float h, uint32 timeNow)
{
    const float valueInDb = jlimit(GENERIC_METER_MINDB,
        GENERIC_METER_MAXDB, 20.f * AudioCore::fastLog10(signal));

    float valueInY = float(AudioCore::iecLevel(valueInDb) * h);
    float peakToShow = valueInY;

    if (this->value < valueInY)
    {
        this->value = valueInY;
        this->valueDecayProgress = 0.f;
        this->valueDecayStart = Time::getMillisecondCounter();
    }
    else
    {
        const auto msElapsed = int(timeNow - this->valueDecayStart);
        float newProgress = msElapsed / GENERIC_METER_BAND_FADE_MS;
        if (newProgress >= 0.f && newProgress < 1.f)
        {
            newProgress = timeToDistance(newProgress);
            const float delta = (newProgress - this->valueDecayProgress) / (1.f - this->valueDecayProgress);
            jassert(newProgress >= this->valueDecayProgress);
            this->valueDecayProgress = newProgress;
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
        this->peakDecayProgress = 0.f;
        this->peakDecayStart = Time::getMillisecondCounter();
    }
    else
    {
        const auto msElapsed = int(timeNow - this->peakDecayStart);
        float newProgress = msElapsed / GENERIC_METER_PEAK_FADE_MS;
        if (newProgress >= 0.f && newProgress < 1.f)
        {
            newProgress = timeToDistance(newProgress);
            const float delta = (newProgress - this->peakDecayProgress) / (1.f - this->peakDecayProgress);
            jassert(newProgress >= this->peakDecayProgress);
            this->peakDecayProgress = newProgress;
            this->peak -= (this->peak * delta);
        }
        else
        {
            this->peak = 0.f;
        }
    }

    g.setColour(Colours::white.withAlpha(0.25f));
    for (int i = int(h + 1); i > float(h - this->value); i -= 2)
    {
        g.drawHorizontalLine(i, x, x + w);
    }

    g.setColour(Colours::white.withAlpha(0.375f - (this->peakDecayProgress * this->peakDecayProgress) / 3.f));
    const float peakH = (h - this->peak - 2.f);
    g.drawHorizontalLine(int(peakH), x, x + w);
}
