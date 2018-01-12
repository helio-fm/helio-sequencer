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

namespace VCS
{
    bool syncProgressCallback(void *context, int bytesSent, int totalBytes);

    class SyncThread :
        public ChangeBroadcaster,
        public Thread
    {
    public:

        SyncThread(URL pushUrl,
                   String projectId,
                   MemoryBlock projectKey,
                   ScopedPointer<XmlElement> pushContent);
        
        enum State
        {
            readyToRock,
            connect,
            connectError,
            fetchHistory,
            fetchHistoryError,
            merge,
            mergeError,
            sync,
            syncError,
            unauthorizedError,
            forbiddenError,

            notFoundError,
            upToDate,
            allDone
        };

        SyncThread::State getState() const;
        void setState(SyncThread::State val);

        float getProgress() const;
        void setProgress(int sent, int total);

    protected:

        URL url;

        String localId;
        MemoryBlock localKey;
        ScopedPointer<XmlElement> localXml;

        Atomic<int> bytesSent;
        Atomic<int> totalBytes;
        Atomic<int> state;

    };
}  // namespace VCS
