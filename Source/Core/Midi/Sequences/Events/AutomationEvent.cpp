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
#include "AutomationEvent.h"
#include "MidiSequence.h"
#include "Transport.h"
#include "SerializationKeys.h"
#include "MidiTrack.h"

AutomationEvent::AutomationEvent() noexcept :
    MidiEvent(nullptr, Type::Auto, 0.f) {}

AutomationEvent::AutomationEvent(WeakReference<MidiSequence> owner,
    float beatVal, float cValue) noexcept :
    MidiEvent(owner, Type::Auto, beatVal),
    controllerValue(cValue) {}

AutomationEvent::AutomationEvent(WeakReference<MidiSequence> owner,
    const AutomationEvent &parametersToCopy) noexcept :
    MidiEvent(owner, parametersToCopy),
    controllerValue(parametersToCopy.controllerValue),
    curvature(parametersToCopy.curvature) {}

float easeInExpo(float delta, float factor)
{
    return delta * powf(2.f, 8.f * (factor - 1.f));
};

float easeOutExpo(float delta, float factor)
{
    return delta * (-powf(2.f, -8.f * factor) + 1.f);
};

float AutomationEvent::interpolateEvents(float cv1, float cv2, float factor, float curvature)
{
    // easing == 0: ease out
    // easing == 1: ease in
    const float easing = (cv1 > cv2) ? curvature : (1.f - curvature);
    const float delta = cv2 - cv1;
    const float easeIn = easeInExpo(delta, factor) * easing;
    const float easeOut = easeOutExpo(delta, factor) * (1.f - easing);
    return cv1 + (easeIn + easeOut);
}

void AutomationEvent::exportMessages(MidiMessageSequence &outSequence,
    const Clip &clip, const KeyboardMapping &keyMap, double timeFactor) const noexcept
{
    MidiMessage cc;
    const bool isTempoTrack = this->getSequence()->getTrack()->isTempoTrack();

    if (isTempoTrack)
    {
        cc = MidiMessage::tempoMetaEvent(Transport::getTempoByControllerValue(this->controllerValue));
    }
    else
    {
        cc = MidiMessage::controllerEvent(this->getTrackChannel(),
            this->getTrackControllerNumber(), int(this->controllerValue * 127));
    }

    const double startTime = (this->beat + clip.getBeat()) * timeFactor;
    cc.setTimeStamp(startTime);
    outSequence.addEvent(cc);

    // add interpolated events, if needed
    const int indexOfThis = this->getSequence()->indexOfSorted(this);
    const bool isPedalOrSwitchEvent = this->getSequence()->getTrack()->isOnOffAutomationTrack();
    if (!isPedalOrSwitchEvent && indexOfThis >= 0 && indexOfThis < (this->getSequence()->size() - 1))
    {
        const auto *nextEvent = static_cast<AutomationEvent *>(this->getSequence()->getUnchecked(indexOfThis + 1));
        float interpolatedBeat = this->beat + AutomationEvent::curveInterpolationStepBeat;
        float lastAppliedValue = this->controllerValue;

        while (interpolatedBeat < nextEvent->beat)
        {
            const float factor = (interpolatedBeat - this->beat) / (nextEvent->beat - this->beat);

            const float interpolatedValue =
                AutomationEvent::interpolateEvents(this->controllerValue,
                    nextEvent->controllerValue, factor, this->curvature);

            const float controllerDelta = fabsf(interpolatedValue - lastAppliedValue);
            if (controllerDelta > AutomationEvent::curveInterpolationThreshold)
            {
                const double interpolatedTs = (interpolatedBeat + clip.getBeat()) * timeFactor;
                if (isTempoTrack)
                {
                    MidiMessage ci(MidiMessage::tempoMetaEvent(Transport::getTempoByControllerValue(interpolatedValue)));
                    ci.setTimeStamp(interpolatedTs);
                    outSequence.addEvent(ci);
                }
                else
                {
                    MidiMessage ci(MidiMessage::controllerEvent(this->getTrackChannel(),
                        this->getTrackControllerNumber(), int(interpolatedValue * 127)));
                    ci.setTimeStamp(interpolatedTs);
                    outSequence.addEvent(ci);
                }

                lastAppliedValue = interpolatedValue;
            }

            interpolatedBeat += AutomationEvent::curveInterpolationStepBeat;
        }
    }
}

AutomationEvent AutomationEvent::withNewId(WeakReference<MidiSequence> owner) const noexcept
{
    AutomationEvent ae(*this);
    if (owner != nullptr)
    {
        ae.sequence = owner;
    }

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

AutomationEvent AutomationEvent::withControllerValue(float cv) const noexcept
{
    AutomationEvent ae(*this);
    ae.controllerValue = jlimit(0.f, 1.f, cv);
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
    ae.curvature = jlimit(0.f, 1.f, newCurvature);
    return ae;
}

AutomationEvent AutomationEvent::withParameters(const SerializedData &parameters) const noexcept
{
    AutomationEvent ae(*this);
    ae.deserialize(parameters);
    return ae;
}

AutomationEvent AutomationEvent::withTempoBpm(int bpm) const noexcept
{
    AutomationEvent ae(*this);
    const auto secondsPerQuarterNote = 60.0 / double(jmax(1, bpm));
    ae.controllerValue = Transport::getControllerValueByTempo(secondsPerQuarterNote);
    return ae;
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

int AutomationEvent::getControllerValueAsBPM() const noexcept
{
    const int msPerQuarterNote = Transport::getTempoByControllerValue(this->controllerValue) / 1000;
    return 60000 / jmax(1, msPerQuarterNote);
}

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

static constexpr auto pedalUpCV = 0.f;
static constexpr auto pedalDownCV = 1.f;

bool AutomationEvent::isPedalDownEvent() const noexcept
{
    return this->controllerValue > 0.5f;
}

bool AutomationEvent::isPedalUpEvent() const noexcept
{
    return this->controllerValue <= 0.5f;
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

SerializedData AutomationEvent::serialize() const
{
    using namespace Serialization;
    SerializedData tree(Midi::automationEvent);
    tree.setProperty(Midi::id, packId(this->id));
    tree.setProperty(Midi::value, this->controllerValue);
    tree.setProperty(Midi::curve, this->curvature);
    tree.setProperty(Midi::timestamp, int(this->beat * Globals::ticksPerBeat));
    return tree;
}

void AutomationEvent::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization;
    this->controllerValue = float(data.getProperty(Midi::value));
    this->curvature = float(data.getProperty(Midi::curve, Globals::Defaults::automationControllerCurve));
    this->beat = float(data.getProperty(Midi::timestamp)) / Globals::ticksPerBeat;
    this->id = unpackId(data.getProperty(Midi::id));
}

void AutomationEvent::reset() noexcept {}

void AutomationEvent::applyChanges(const AutomationEvent &parameters) noexcept
{
    jassert(this->id == parameters.id);
    this->beat = parameters.beat;
    this->controllerValue = parameters.controllerValue;
    this->curvature = parameters.curvature;
}

int AutomationEvent::compareElements(const AutomationEvent *const first,
    const AutomationEvent *const second) noexcept
{
    if (first == second) { return 0; }

    const float beatDiff = first->beat - second->beat;
    const int beatResult = (beatDiff > 0.f) - (beatDiff < 0.f);
    if (beatResult != 0) { return beatResult; }

    const auto cvDiff = first->controllerValue - second->controllerValue;
    const int cvResult = (cvDiff > 0) - (cvDiff < 0);
    if (cvResult != 0) { return cvResult; }

    return first->getId() - second->getId();
}
