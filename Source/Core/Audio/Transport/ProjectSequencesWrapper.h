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

#pragma once

#include "Instrument.h"
#include <float.h>

class MidiSequence;

struct SequenceWrapper : public ReferenceCountedObject
{
    MidiMessageSequence sequence;
    int currentIndex;
    MidiMessageCollector *listener;
    Instrument *instrument;
    const MidiSequence *layer;
    typedef ReferenceCountedObjectPtr<SequenceWrapper> Ptr;
};

struct MessageWrapper : public ReferenceCountedObject
{
    MidiMessage message;
    MidiMessageCollector *listener;
    Instrument *instrument;
    typedef ReferenceCountedObjectPtr<MessageWrapper> Ptr;
};

// TODO: add modifiers like random delays and so forth

class ProjectSequences
{
private:
    
    Array<Instrument *> uniqueInstruments;
    ReferenceCountedArray<SequenceWrapper> sequences;

public:
    
    ProjectSequences() {}
    
    ProjectSequences(const ProjectSequences &other) :
    sequences(other.sequences),
    uniqueInstruments(other.uniqueInstruments) {}
    
    inline Array<Instrument *> getUniqueInstruments() const noexcept
    {
        return this->uniqueInstruments;
    }
    
    SequenceWrapper *addWrapper(SequenceWrapper *const newWrapper) noexcept
    {
        this->uniqueInstruments.addIfNotAlreadyThere(newWrapper->instrument);
        return this->sequences.add(newWrapper);
    }
    
    inline void clear()
    {
        this->uniqueInstruments.clear();
        this->sequences.clear();
    }
    
    inline bool empty() const
    {
        return (this->sequences.size() == 0);
    }
    
    double getSampleRate() const
    {
        if (this->empty())
        { return 0.0; }

        // TODO: something more reasonable?
        return this->sequences[0]->instrument->getProcessorGraph()->getSampleRate();
    }

    int getNumOutputChannels() const
    {
        if (this->empty())
        { return 0; }

        // TODO: something more reasonable?
        return this->sequences[0]->instrument->getProcessorGraph()->getTotalNumOutputChannels();
    }

    int getNumInputChannels() const
    {
        if (this->empty())
        { return 0; }
        
        // TODO: something more reasonable?
        return this->sequences[0]->instrument->getProcessorGraph()->getTotalNumInputChannels();
    }

    ReferenceCountedArray<SequenceWrapper> getAllFor(const MidiSequence *midiLayer)
    {
        ReferenceCountedArray<SequenceWrapper> result;
        
        for (int i = 0; i < this->sequences.size(); ++i)
        {
            SequenceWrapper::Ptr seq(this->sequences[i]);
            
            if (midiLayer == nullptr || midiLayer == seq->layer)
            {
                result.add(seq);
            }
        }
        
        return result;
    }

    void seekToTime(double position)
    {
        for (int i = 0; i < this->sequences.size(); ++i)
        {
            SequenceWrapper *wrapper = this->sequences.getUnchecked(i);
            wrapper->currentIndex = this->getNextIndexAtTime(wrapper->sequence, (position - DBL_MIN));
        }
    }
    
    int getNextIndexAtTime(const MidiMessageSequence &sequence, double timeStamp) const
    {
        int i = 0;
        for (; i < sequence.getNumEvents(); ++i)
        {
            const double eventTs = sequence.getEventPointer(i)->message.getTimeStamp();
            if (eventTs >= timeStamp)
            {
                break;
            }
        }
        
        return i;
    }
    
    void seekToZeroIndexes()
    {
        for (int i = 0; i < this->sequences.size(); ++i)
        {
            this->sequences.getUnchecked(i)->currentIndex = 0;
        }
    }
    
    bool getNextMessage(MessageWrapper &target)
    {
        double minTimeStamp = DBL_MAX;
        int targetSequenceIndex = -1;

        for (int i = 0; i < this->sequences.size(); ++i)
        {
            SequenceWrapper *wrapper = this->sequences.getUnchecked(i);

            if (wrapper->currentIndex < wrapper->sequence.getNumEvents())
            {
                MidiMessage &message = wrapper->sequence.getEventPointer(wrapper->currentIndex)->message;

                if (message.getTimeStamp() < minTimeStamp)
                {
                    minTimeStamp = message.getTimeStamp();
                    targetSequenceIndex = i;
                }
            }
        }

        if (targetSequenceIndex < 0)
        { return false; }

        SequenceWrapper *foundWrapper = this->sequences.getUnchecked(targetSequenceIndex);
        MidiMessage &foundMessage = foundWrapper->sequence.getEventPointer(foundWrapper->currentIndex)->message;
        foundWrapper->currentIndex++;
                
        target.message = foundMessage;
        target.listener = foundWrapper->listener;
        target.instrument = foundWrapper->instrument;

        return true;
    }

    double getLastEventTimestamp() const
    {
        double lastEventTimestamp = 0.f;

        for (int i = 0; i < this->sequences.size(); ++i)
        {
            const SequenceWrapper *wrapper = this->sequences.getUnchecked(i);
            const double &endTime = wrapper->sequence.getEndTime();

            if (lastEventTimestamp < endTime)
            {
                lastEventTimestamp = endTime;
            }
        }

        return lastEventTimestamp;
    }
    
    JUCE_LEAK_DETECTOR(ProjectSequences)
};
