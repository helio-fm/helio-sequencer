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

class MidiSequence;

struct CachedMidiSequence final : public ReferenceCountedObject
{
    MidiMessageSequence midiMessages;
    int currentIndex;
    MidiMessageCollector *listener;
    Instrument *instrument;
    const MidiSequence *track;

    using Ptr = ReferenceCountedObjectPtr<CachedMidiSequence>;

    static Ptr createFrom(Instrument *instrument, const MidiSequence *track = nullptr)
    {
        jassert(instrument != nullptr);
        CachedMidiSequence::Ptr wrapper(new CachedMidiSequence());
        wrapper->track = track;
        wrapper->currentIndex = 0;
        wrapper->instrument = instrument;
        wrapper->listener = &instrument->getProcessorPlayer().getMidiMessageCollector();
        return wrapper;
    }
};

struct CachedMidiMessage final : public ReferenceCountedObject
{
    MidiMessage message;
    MidiMessageCollector *listener;
    Instrument *instrument;
    using Ptr = ReferenceCountedObjectPtr<CachedMidiMessage>;
};

class ProjectSequences final
{
private:
    
    Array<Instrument *> uniqueInstruments;
    ReferenceCountedArray<CachedMidiSequence> sequences;

public:
    
    ProjectSequences() = default;
    
    inline Array<Instrument *> getUniqueInstruments() const noexcept
    {
        const SpinLock::ScopedLockType lock(this->instrumentsLock);
        return this->uniqueInstruments;
    }
    
    void addWrapper(CachedMidiSequence::Ptr newWrapper) noexcept
    {
        const SpinLock::ScopedLockType lock(this->sequencesLock);
        if (newWrapper->midiMessages.getNumEvents() > 0)
        {
            this->uniqueInstruments.addIfNotAlreadyThere(newWrapper->instrument);
            this->sequences.add(newWrapper);
        }
    }
    
    inline void clear()
    {
        const SpinLock::ScopedLockType lock(this->sequencesLock);
        this->uniqueInstruments.clearQuick();
        this->sequences.clearQuick();
    }
    
    inline bool isEmpty() const
    {
        const SpinLock::ScopedLockType lock(this->sequencesLock);
        return this->sequences.isEmpty();
    }
    
    double getSampleRate() const
    {
        if (this->isEmpty())
        {
            return 0.0;
        }

        const SpinLock::ScopedLockType lock(this->sequencesLock);

        // TODO: something more reasonable?
        return this->sequences[0]->instrument->getProcessorGraph()->getSampleRate();
    }

    int getNumOutputChannels() const
    {
        if (this->isEmpty())
        {
            return 0;
        }

        const SpinLock::ScopedLockType lock(this->sequencesLock);

        // TODO: something more reasonable?
        return this->sequences[0]->instrument->getProcessorGraph()->getTotalNumOutputChannels();
    }

    int getNumInputChannels() const
    {
        if (this->isEmpty())
        {
            return 0;
        }

        const SpinLock::ScopedLockType lock(this->sequencesLock);

        // TODO: something more reasonable?
        return this->sequences[0]->instrument->getProcessorGraph()->getTotalNumInputChannels();
    }

    ReferenceCountedArray<CachedMidiSequence> getAllFor(const MidiSequence *midiTrack)
    {
        const SpinLock::ScopedLockType lock(this->sequencesLock);

        ReferenceCountedArray<CachedMidiSequence> result;
        for (int i = 0; i < this->sequences.size(); ++i)
        {
            CachedMidiSequence::Ptr seq(this->sequences[i]);
            if (midiTrack == nullptr || midiTrack == seq->track)
            {
                result.add(seq);
            }
        }
        
        return result;
    }

    void seekToTime(double position)
    {
        const SpinLock::ScopedLockType lock(this->sequencesLock);
        
        for (auto *wrapper : this->sequences)
        {
            wrapper->currentIndex = this->getNextIndexAtTime(wrapper->midiMessages, (position - DBL_MIN));
        }
    }
    
    void seekToZeroIndexes()
    {
        const SpinLock::ScopedLockType lock(this->sequencesLock);

        for (auto *wrapper : this->sequences)
        {
            wrapper->currentIndex = 0;
        }
    }
    
    bool getNextMessage(CachedMidiMessage &target)
    {
        const SpinLock::ScopedLockType lock(this->sequencesLock);

        double minTimeStamp = DBL_MAX;
        int targetSequenceIndex = -1;

        for (int i = 0; i < this->sequences.size(); ++i)
        {
            const auto *wrapper = this->sequences.getObjectPointer(i);
            if (wrapper->currentIndex < wrapper->midiMessages.getNumEvents())
            {
                const auto &message = wrapper->midiMessages.getEventPointer(wrapper->currentIndex)->message;

                if (message.getTimeStamp() < minTimeStamp)
                {
                    minTimeStamp = message.getTimeStamp();
                    targetSequenceIndex = i;
                }
            }
        }

        if (targetSequenceIndex < 0)
        {
            return false;
        }

        auto *foundWrapper = this->sequences.getObjectPointer(targetSequenceIndex);
        auto &foundMessage = foundWrapper->midiMessages.getEventPointer(foundWrapper->currentIndex)->message;
        foundWrapper->currentIndex++;

        target.message = foundMessage;
        target.listener = foundWrapper->listener;
        target.instrument = foundWrapper->instrument;

        return true;
    }
    
private:
    
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

    SpinLock instrumentsLock;
    SpinLock sequencesLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectSequences)
};
