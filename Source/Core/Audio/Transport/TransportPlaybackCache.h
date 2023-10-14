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

class TransportPlaybackCache final
{
private:
    
    Array<Instrument *, CriticalSection> uniqueInstruments;
    ReferenceCountedArray<CachedMidiSequence, CriticalSection> sequences;

public:
    
    TransportPlaybackCache() = default;

    TransportPlaybackCache(const TransportPlaybackCache &other) noexcept
    {
        this->sequences.addArray(other.sequences);
        this->uniqueInstruments.addArray(other.uniqueInstruments);
    }

    TransportPlaybackCache(TransportPlaybackCache &&other) noexcept
    {
        this->sequences.swapWith(other.sequences);
        this->uniqueInstruments.swapWith(other.uniqueInstruments);
    }

    TransportPlaybackCache &operator= (TransportPlaybackCache &&other) noexcept
    {
        this->sequences.swapWith(other.sequences);
        this->uniqueInstruments.swapWith(other.uniqueInstruments);
        return *this;
    }

    inline Array<Instrument *, CriticalSection> getUniqueInstruments() const noexcept
    {
        return this->uniqueInstruments;
    }
    
    void addWrapper(CachedMidiSequence::Ptr newWrapper) noexcept
    {
        if (newWrapper->midiMessages.getNumEvents() > 0)
        {
            this->uniqueInstruments.addIfNotAlreadyThere(newWrapper->instrument);
            this->sequences.add(newWrapper);
        }
    }
    
    inline void clear()
    {
        this->uniqueInstruments.clearQuick();
        this->sequences.clearQuick();
    }
    
    inline bool isEmpty() const
    {
        return this->sequences.isEmpty();
    }
    
    double getSampleRate() const
    {
        if (this->isEmpty())
        {
            return 0.0;
        }

        // TODO: something more reasonable?
        return this->sequences[0]->instrument->getProcessorGraph()->getSampleRate();
    }

    int getNumOutputChannels() const
    {
        if (this->isEmpty())
        {
            return 0;
        }

        // TODO: something more reasonable?
        return this->sequences[0]->instrument->getProcessorGraph()->getTotalNumOutputChannels();
    }

    int getNumInputChannels() const
    {
        if (this->isEmpty())
        {
            return 0;
        }

        // TODO: something more reasonable?
        return this->sequences[0]->instrument->getProcessorGraph()->getTotalNumInputChannels();
    }

    ReferenceCountedArray<CachedMidiSequence> getAllFor(const MidiSequence *midiTrack)
    {
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
        for (auto *wrapper : this->sequences)
        {
            wrapper->currentIndex = this->getNextIndexAtTime(wrapper->midiMessages, (position - DBL_MIN));
        }
    }
    
    void seekToStart()
    {
        for (auto *wrapper : this->sequences)
        {
            wrapper->currentIndex = 0;
        }
    }
    
    bool getNextMessage(CachedMidiMessage &target)
    {
        double minTimeStamp = DBL_MAX;
        int targetSequenceIndex = -1;

        // let's lock it manually one time here to avoid 3 separate locks below,
        // CriticalSection is re-entrant, and the profiler shows it's faster:
        const CriticalSection::ScopedLockType lock(this->sequences.getLock());

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
        jassert(foundWrapper->currentIndex < foundWrapper->midiMessages.getNumEvents());

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

    JUCE_LEAK_DETECTOR(TransportPlaybackCache)
};
