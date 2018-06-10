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
#include "AutomationEvent.h"
#include "MidiSequence.h"
#include "Transport.h"
#include "SerializationKeys.h"
#include "MidiTrack.h"

#define AUTOEVENT_DEFAULT_CURVATURE (0.5f)

AutomationEvent::AutomationEvent() noexcept : MidiEvent(nullptr, MidiEvent::Auto, 0.f)
{
    //jassertfalse;
}

AutomationEvent::AutomationEvent(const AutomationEvent &other) noexcept :
    MidiEvent(other),
    controllerValue(other.controllerValue),
    curvature(other.curvature) {}

AutomationEvent::AutomationEvent(WeakReference<MidiSequence> owner, float beatVal, float cValue) noexcept :
    MidiEvent(owner, MidiEvent::Auto, beatVal),
    controllerValue(cValue),
    curvature(AUTOEVENT_DEFAULT_CURVATURE) {}

AutomationEvent::AutomationEvent(WeakReference<MidiSequence> owner,
    const AutomationEvent &parametersToCopy) noexcept :
    MidiEvent(owner, parametersToCopy),
    controllerValue(parametersToCopy.controllerValue),
    curvature(parametersToCopy.curvature) {}

float linearTween(float delta, float factor)
{
    return delta * factor;
};

float easeInExpo(float delta, float factor)
{
    return delta * powf(2.f, 8.f * (factor - 1.f));
};

float easeOutExpo(float delta, float factor)
{
    return delta * (-powf(2.f, -8.f * factor) + 1.f);
};

// easing == 0: ease out
// easing == 1: ease in
float AutomationEvent::interpolateEvents(float cv1, float cv2, float factor, float curvature)
{
    const float delta = cv2 - cv1;
    const float easing = (cv1 > cv2) ? curvature : (1.f - curvature);

    //const float e = (easing * 2.f) - 1.f;

    //if (e > 0)
    //{
    //    const float easeIn = easeInExpo(delta, factor) * e;
    //    const float linear = linearTween(delta, factor) * (1.f - e);
    //    return cv1 + (easeIn + linear);
    //}

    //const float easeOut = easeOutExpo(delta, factor) * -e;
    //const float linear = linearTween(delta, factor) * (1.f + e);
    //return cv1 + (easeOut + linear);

    const float easeIn = easeInExpo(delta, factor) * easing;
    const float easeOut = easeOutExpo(delta, factor) * (1.f - easing);
    return cv1 + (easeIn + easeOut);
}

Array<MidiMessage> AutomationEvent::toMidiMessages() const
{
    Array<MidiMessage> result;

    MidiMessage cc;
    const bool isTempoTrack = this->getSequence()->getTrack()->isTempoTrack();

    if (isTempoTrack)
    {
        cc = MidiMessage::tempoMetaEvent(int((1.f - this->controllerValue) * MS_PER_BEAT * 1000));
    }
    else
    {
        cc = MidiMessage::controllerEvent(this->getTrackChannel(),
            this->getTrackControllerNumber(),int(this->controllerValue * 127));
    }

    const double startTime = round(this->beat * MS_PER_BEAT);
    cc.setTimeStamp(startTime);
    result.add(cc);

    // add interpolated events, if needed
    const int indexOfThis = this->getSequence()->indexOfSorted(this);
    const bool isOnOff = this->getSequence()->getTrack()->isOnOffAutomationTrack();
    if (!isOnOff && indexOfThis >= 0 && indexOfThis < (this->getSequence()->size() - 1))
    {
        const auto *nextEvent = static_cast<AutomationEvent *>(this->getSequence()->getUnchecked(indexOfThis + 1));
        const float controllerDelta = fabs(this->controllerValue - nextEvent->controllerValue);

        if (controllerDelta > CURVE_INTERPOLATION_THRESHOLD)
        {
            const double nextTime = nextEvent->beat * MS_PER_BEAT;
            float interpolatedBeat = this->beat + CURVE_INTERPOLATION_STEP_BEAT;

            while (interpolatedBeat < nextEvent->beat)
            {
                const double interpolatedEventTs = round(interpolatedBeat * MS_PER_BEAT);
                const float factor = (interpolatedBeat - this->beat) / (nextEvent->beat - this->beat);

                const float interpolatedValue =
                    AutomationEvent::interpolateEvents(this->controllerValue,
                        nextEvent->controllerValue, factor, this->curvature);

                if (isTempoTrack)
                {
                    const auto tempo = int((1.f - interpolatedValue) * MS_PER_BEAT * 1000);
                    MidiMessage ci(MidiMessage::tempoMetaEvent(tempo));
                    ci.setTimeStamp(interpolatedEventTs);
                    result.add(ci);
                }
                else
                {
                    MidiMessage ci(MidiMessage::controllerEvent(this->getTrackChannel(),
                        this->getTrackControllerNumber(), int(this->controllerValue * 127)));
                    ci.setTimeStamp(interpolatedEventTs);
                    result.add(ci);
                }

                interpolatedBeat += CURVE_INTERPOLATION_STEP_BEAT;
            }
        }
    }

    return result;
}

AutomationEvent AutomationEvent::copyWithNewId() const noexcept
{
    AutomationEvent ae(*this);
    ae.id = ae.createId();
    return ae;
}

AutomationEvent AutomationEvent::withBeat(float newBeat) const noexcept
{
    AutomationEvent ae(*this);
    ae.beat = roundBeat(newBeat);
    return ae;
}

AutomationEvent AutomationEvent::withDeltaBeat(float deltaBeat) const noexcept
{
    AutomationEvent ae(*this);
    ae.beat = roundBeat(this->beat + deltaBeat);
    return ae;
}

AutomationEvent AutomationEvent::withInvertedControllerValue() const noexcept
{
    AutomationEvent ae(*this);
    ae.controllerValue = (1.f - this->controllerValue);
    return ae;
}

AutomationEvent AutomationEvent::withParameters(float newBeat, float newControllerValue) const noexcept
{
    AutomationEvent ae(*this);
    ae.beat = newBeat;
    ae.controllerValue = newControllerValue;
    return ae;
}

AutomationEvent AutomationEvent::withCurvature(float newCurvature) const noexcept
{
    AutomationEvent ae(*this);
    ae.curvature = jmin(1.f, jmax(0.f, newCurvature));
    return ae;
}

AutomationEvent AutomationEvent::withParameters(const ValueTree &parameters) const noexcept
{
    AutomationEvent ae(*this);
    ae.deserialize(parameters);
    return ae;
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

float AutomationEvent::getControllerValue() const noexcept
{
    return this->controllerValue;
}

float AutomationEvent::getCurvature() const noexcept
{
    return this->curvature;
}

//===----------------------------------------------------------------------===//
// Pedal helpers
//===----------------------------------------------------------------------===//

static const float pedalUpCV = 0.f;
static const float pedalDownCV = 1.f;

bool AutomationEvent::isPedalDownEvent() const noexcept
{
    return (this->controllerValue > 0.5f);
}

bool AutomationEvent::isPedalUpEvent() const noexcept
{
    return (this->controllerValue <= 0.5f);
}

AutomationEvent AutomationEvent::pedalUpEvent(MidiSequence *owner, float beatVal)
{
    return AutomationEvent(owner, beatVal, pedalUpCV);
}

AutomationEvent AutomationEvent::pedalDownEvent(MidiSequence *owner, float beatVal)
{
    return AutomationEvent(owner, beatVal, pedalDownCV);
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree AutomationEvent::serialize() const noexcept
{
    using namespace Serialization;
    ValueTree tree(Midi::automationEvent);
    tree.setProperty(Midi::id, this->id, nullptr);
    tree.setProperty(Midi::value, this->controllerValue, nullptr);
    tree.setProperty(Midi::curve, this->curvature, nullptr);
    tree.setProperty(Midi::timestamp, roundToInt(this->beat * TICKS_PER_BEAT), nullptr);
    return tree;
}

void AutomationEvent::deserialize(const ValueTree &tree) noexcept
{
    this->reset();
    using namespace Serialization;
    this->controllerValue = float(tree.getProperty(Midi::value));
    this->curvature = float(tree.getProperty(Midi::curve, AUTOEVENT_DEFAULT_CURVATURE));
    this->beat = float(tree.getProperty(Midi::timestamp)) / TICKS_PER_BEAT;
    this->id = tree.getProperty(Midi::id);
}

void AutomationEvent::reset() noexcept {}

void AutomationEvent::applyChanges(const AutomationEvent &parameters) noexcept
{
    jassert(this->id == parameters.id);
    this->beat = parameters.beat;
    this->controllerValue = parameters.controllerValue;
    this->curvature = parameters.curvature;
}
