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

#include "Transport.h"
#include "RenderFormat.h"

class RendererThread final : private Thread
{
public:

    explicit RendererThread(Transport &parentTransport);
    ~RendererThread() override;
    
    float getPercentsComplete() const noexcept;
    const Array<float, CriticalSection> &getWaveformThumbnail() const;

    bool startRendering(const URL &target, RenderFormat format,
        Transport::PlaybackContext::Ptr context,
        int waveformThumbnailResolution);

    void stop();
    bool isRendering() const;

private:

    //===------------------------------------------------------------------===//
    // Thread
    //===------------------------------------------------------------------===//

    void run() override;

private:

    Transport &transport;
    Transport::PlaybackContext::Ptr context;
    RenderFormat format;

    // this needs to be kept alive while rendering (why - because iOS)
    URL renderTarget;

    CriticalSection writerLock;
    UniquePointer<AudioFormatWriter> writer;

    // some platforms (looking at you, Android) don't support seekable file output streams,
    // which some audio formats need to write metadata, so we have to render to memory first;
    // todo in future: optionally normalize all rendered data before writing to file
    MemoryBlock renderBlock;

    Atomic<float> percentsDone = 0.f;

    // the all-channels peaks-only low-resolution waveform preview,
    // simplest to compute, but good enough for the progress bar:
    Array<float, CriticalSection> waveformThumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RendererThread)
};
