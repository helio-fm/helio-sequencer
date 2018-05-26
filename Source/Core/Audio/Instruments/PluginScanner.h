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

class PluginScanner :
    public Serializable,
    public Thread,
    public WaitableEvent,
    public ChangeBroadcaster
{
public:

    PluginScanner();
    ~PluginScanner() override;
    
    bool isWorking() const;
    void removeListItem(int index);

    bool hasEffects() const;
    bool hasInstruments() const;

    void removeItem(const PluginDescription &description);

    void sortList(KnownPluginList::SortMethod method, bool forwards);
    const KnownPluginList &getList() const noexcept;

    void runInitialScan();
    void scanFolderAndAddResults(const File &dir);

    //===------------------------------------------------------------------===//
    // Thread
    //===------------------------------------------------------------------===//

    void run() override;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

private:

    ReadWriteLock pluginsListLock;
    KnownPluginList pluginsList;

    ReadWriteLock filesListLock;
    StringArray filesToScan;
    
    ReadWriteLock workingFlagLock;
    
    bool working;
    
    StringArray getFilesToScan() const;
    FileSearchPath getTypicalFolders();
    void scanPossibleSubfolders(const StringArray &possibleSubfolders,
                                const File &currentSystemFolder,
                                FileSearchPath &foldersOut);
};
