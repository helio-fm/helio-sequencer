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
#include "SyncThread.h"
#include "Client.h"

using namespace VCS;

bool VCS::syncProgressCallback(void *context, int bytesSent, int totalBytes)
{
    SyncThread *syncThread = static_cast<SyncThread *>(context);
    syncThread->setProgress(bytesSent, totalBytes);
    return true; // always continue
}

SyncThread::SyncThread(URL pushUrl,
                       String projectId,
                       MemoryBlock projectKey,
                       const ValueTree &pushContent) :
    Thread("Sync Thread"),
    url(pushUrl),
    localId(std::move(projectId)),
    localKey(std::move(projectKey)),
    localState(pushContent),
    bytesSent(0),
    totalBytes(0) {}

SyncThread::State SyncThread::getState() const
{
    return (SyncThread::State)this->state.get();
}

void SyncThread::setState(SyncThread::State val)
{
    this->state = val;
    this->sendChangeMessage();
}

float SyncThread::getProgress() const
{
    const float percents = (float(this->bytesSent.get()) / float(this->totalBytes.get()));
    return (percents > 100.f) ? 0.f : percents;
}

void SyncThread::setProgress(int sent, int total)
{
    this->bytesSent = sent;
    this->totalBytes = total;
    this->sendChangeMessage();
}
