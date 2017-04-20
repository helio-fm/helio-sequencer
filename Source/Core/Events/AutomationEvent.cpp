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
#include "MidiLayer.h"
#include "Transport.h"
#include "SerializationKeys.h"

#define AUTOEVENT_DEFAULT_CURVATURE (0.5f)
#define MIN_INTERPOLATED_CONTROLLER_DELTA (0.01f)
#define INTERPOLATED_EVENTS_STEP_MS (350)

AutomationEvent::AutomationEvent() : MidiEvent(nullptr, 0.f)
{
    //jassertfalse;
}

AutomationEvent::AutomationEvent(const AutomationEvent &other) :
    MidiEvent(other.layer, other.beat),
    controllerValue(other.controllerValue),
    curvature(other.curvature)
{
    this->id = other.getID();
}

AutomationEvent::AutomationEvent(MidiLayer *owner, float beatVal, float cValue) :
    MidiEvent(owner, beatVal),
    controllerValue(cValue),
    curvature(AUTOEVENT_DEFAULT_CURVATURE)
{

}

AutomationEvent::~AutomationEvent()
{

}

float linearTween(float delta, float factor)
{
    return delta * factor;
};

float easeInExpo(float delta, float factor)
{
    return delta * powf(2.f, 16.f * (factor - 1.f));
};

float easeOutExpo(float delta, float factor)
{
    return delta * (-powf(2.f, -16.f * factor) + 1.f);
};

// easing == 0: ease out
// easing == 1: ease in
float exponentalInterpolation(float y0, float y1, float factor, float easing)
{
    const float delta = y1 - y0;
    
//    const float e = (easing * 2.f) - 1.f;
//===----------------------------------------------------------------------===//
//    if (e > 0)
//    {
//        const float easeIn = easeInExpo(delta, factor) * e;
//        const float linear = linearTween(delta, factor) * (1.f - e);
//        return y0 + (easeIn + linear);
//    }
//    
//    const float easeOut = easeOutExpo(delta, factor) * -e;
//    const float linear = linearTween(delta, factor) * (1.f + e);
//    return y0 + (easeOut + linear);
    
    const float easeIn = easeInExpo(delta, factor) * easing;
    const float easeOut = easeOutExpo(delta, factor) * (1.f - easing);
    return y0 + (easeIn + easeOut);
}


Array<MidiMessage> AutomationEvent::getSequence() const
{
	Array<MidiMessage> result;
    
    // теперь пусть все треки автоматизации ведут себя одинаково
    //if (this->getLayer()->isTempoLayer())
    {
        MidiMessage cc;
        
        if (this->getLayer()->isTempoLayer())
        {
            cc = MidiMessage::tempoMetaEvent(int((1.f - this->controllerValue) * Transport::millisecondsPerBeat * 1000));
        }
        else
        {
            cc = MidiMessage::controllerEvent(this->layer->getChannel(),
                                              this->getLayer()->getControllerNumber(),
                                              int(this->controllerValue * 127));
        
        }

        const float &startTime = this->beat * Transport::millisecondsPerBeat;
        cc.setTimeStamp(startTime);
        result.add(cc);

        
        // добавить интерполированные события, если таковые должны быть
        const int indexOfThis = this->getLayer()->indexOfSorted(this);
        
        if (indexOfThis >= 0 && indexOfThis < (this->getLayer()->size() - 1))
        {
            const AutomationEvent *nextEvent = static_cast<AutomationEvent *>(this->getLayer()->getUnchecked(indexOfThis + 1));
            const float controllerDelta = fabs(this->controllerValue - nextEvent->controllerValue);
            
            if (controllerDelta > MIN_INTERPOLATED_CONTROLLER_DELTA)
            {
                const float nextTime = nextEvent->beat * Transport::millisecondsPerBeat;
                float interpolatedEventTimeStamp = startTime + INTERPOLATED_EVENTS_STEP_MS;
                
                while (interpolatedEventTimeStamp < nextTime)
                {
                    const float lerpFactor = (interpolatedEventTimeStamp - startTime) / (nextTime - startTime);
                    const float c = (this->controllerValue > nextEvent->controllerValue) ? this->curvature : (1.f - this->curvature);
                    
                    const float interpolatedControllerValue = exponentalInterpolation(this->controllerValue,
                                                                                      nextEvent->controllerValue,
                                                                                      lerpFactor,
                                                                                      c);
                    
                    //Logger::writeToLog(String(this->curvature) + " :: " + String((interpolatedControllerValue - this->controllerValue) / (nextEvent->controllerValue - this->controllerValue) - lerpFactor));
                    
                    //Logger::writeToLog(String(this->controllerValue) + " - " + String(interpolatedControllerValue) + " - " + String(nextEvent->controllerValue));
                    
                    if (this->getLayer()->isTempoLayer())
                    {
                        MidiMessage ci(MidiMessage::tempoMetaEvent(int((1.f - interpolatedControllerValue) * Transport::millisecondsPerBeat * 1000)));
                        ci.setTimeStamp(interpolatedEventTimeStamp);
                        result.add(ci);
                    }
                    else
                    {
                        MidiMessage ci(MidiMessage::controllerEvent(this->getLayer()->getChannel(),
                                                                    this->getLayer()->getControllerNumber(),
                                                                    int(this->controllerValue * 127)));
                        ci.setTimeStamp(interpolatedEventTimeStamp);
                        result.add(ci);
                    }
                    
                    interpolatedEventTimeStamp += INTERPOLATED_EVENTS_STEP_MS;
                }
            }
        }
    }

    return result;
}

AutomationEvent AutomationEvent::copyWithNewId() const
{
    AutomationEvent ae(*this);
    ae.id = this->createId();
    return ae;
}

static float roundBeat(float beat)
{
    return roundf(beat * 1000.f) / 1000.f;
}

AutomationEvent AutomationEvent::withBeat(float newBeat) const
{
    AutomationEvent ae(*this);
    //ae.beat = newBeat;
    ae.beat = roundBeat(newBeat);
    return ae;
}

AutomationEvent AutomationEvent::withDeltaBeat(float deltaBeat) const
{
    AutomationEvent ae(*this);
    //ae.beat = this->beat + deltaBeat;
    ae.beat = roundBeat(this->beat + deltaBeat);
    return ae;
}

AutomationEvent AutomationEvent::withInvertedControllerValue() const
{
    AutomationEvent ae(*this);
    ae.controllerValue = (1.f - this->controllerValue);
    return ae;
}

AutomationEvent AutomationEvent::withParameters(float newBeat, float newControllerValue) const
{
    AutomationEvent ae(*this);
    ae.beat = newBeat;
    ae.controllerValue = newControllerValue;
    return ae;
}

AutomationEvent AutomationEvent::withCurvature(float newCurvature) const
{
    AutomationEvent ae(*this);
    ae.curvature = jmin(1.f, jmax(0.f, newCurvature));
    return ae;
}

AutomationEvent AutomationEvent::withParameters(const XmlElement &xml) const
{
    AutomationEvent ae(*this);
    ae.deserialize(xml);
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

AutomationEvent AutomationEvent::pedalUpEvent(MidiLayer *owner, float beatVal)
{
    return AutomationEvent(owner, beatVal, pedalUpCV);
}

AutomationEvent AutomationEvent::pedalDownEvent(MidiLayer *owner, float beatVal)
{
    return AutomationEvent(owner, beatVal, pedalDownCV);
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *AutomationEvent::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::event);
    xml->setAttribute("val", this->controllerValue);
    xml->setAttribute("beat", this->beat);
    xml->setAttribute("curve", this->curvature);
    //xml->setAttribute("id", this->id.toString());
    xml->setAttribute("id", this->id);
    return xml;
}

void AutomationEvent::deserialize(const XmlElement &xml)
{
    this->reset();

    // здесь никаких проверок. посмотри, как быстро будет работать сериализация.
    this->controllerValue = float(xml.getDoubleAttribute("val"));
    this->curvature = float(xml.getDoubleAttribute("curve", AUTOEVENT_DEFAULT_CURVATURE));
    this->beat = float(xml.getDoubleAttribute("beat"));
    this->id = xml.getStringAttribute("id");
}

void AutomationEvent::reset()
{
    // здесь ничего пока не чистим, это не нужно и вообще, проверь скорость работы сериализации.
}


int AutomationEvent::hashCode() const noexcept
{
    //return roundFloatToInt(this->getControllerValue() * 1000) +
    //       roundFloatToInt(this->getBeat() * 1000) +
    //       this->getID().toString().hashCode();
    return roundFloatToInt(this->getControllerValue() * 1000) +
           roundFloatToInt(this->getBeat() * 1000) +
           this->getID().hashCode();
}

AutomationEvent &AutomationEvent::operator=(const AutomationEvent &right)
{
    //if (this == &right) { return *this; }

    //this->layer = right.layer; // never do this
    this->id = right.id;
    this->beat = right.beat;
    this->curvature = right.curvature;
    this->controllerValue = right.controllerValue;

    return *this;
}
