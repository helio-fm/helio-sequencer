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

#include "Transport.h"

class RendererThread final : private Thread
{
public:

    explicit RendererThread(Transport &parentTransport);
    ~RendererThread() override;
    
    float getPercentsComplete() const;

    void startRecording(const File &file,
        Transport::PlaybackContext::Ptr context);

    void stop();
    bool isRecording() const;

private:

    //===------------------------------------------------------------------===//
    // Thread
    //===------------------------------------------------------------------===//

    void run() override;

private:

    Transport &transport;
    Transport::PlaybackContext::Ptr context;

    CriticalSection writerLock;
    UniquePointer<AudioFormatWriter> writer;

    ReadWriteLock percentsLock;
    float percentsDone = 0.f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RendererThread)
};
