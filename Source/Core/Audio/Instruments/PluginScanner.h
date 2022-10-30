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

class PluginScanner final :
    public Serializable,
    private Thread,
    private WaitableEvent,
    public ChangeBroadcaster
{
public:

    PluginScanner();
    ~PluginScanner() override;
    
    bool isWorking() const;
    bool hasEffects() const;
    bool hasInstruments() const;

    void removePlugin(const PluginDescription &description);
    void sortList(KnownPluginList::SortMethod method, bool forwards);

    inline const Array<PluginDescription> getPlugins() const noexcept
    {
        return this->pluginsList.getTypes();
    }

    inline const int getNumPlugins() const noexcept
    {
        return this->pluginsList.getNumTypes();
    }

    void runInitialScan();
    void scanFolderAndAddResults(const File &dir);
    void cancelRunningScan();

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

private:

    //===------------------------------------------------------------------===//
    // Thread
    //===------------------------------------------------------------------===//

    void run() override;

    KnownPluginList pluginsList;
    
    Atomic<bool> working = false;
    Atomic<bool> cancelled = false;

    // paths and files to be checked in a search thread:
    FileSearchPath searchPath;
    StringArray filesToScan;

    FileSearchPath getCommonFolders();
    void scanPossibleSubfolders(const StringArray &possibleSubfolders,
        const File &currentSystemFolder, FileSearchPath &foldersOut);
};
