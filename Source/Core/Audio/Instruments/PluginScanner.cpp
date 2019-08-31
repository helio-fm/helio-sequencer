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
#include "PluginScanner.h"
#include "AudioCore.h"
#include "DocumentHelpers.h"
#include "XmlSerializer.h"
#include "Config.h"
#include "SerializationKeys.h"
#include "BuiltInSynthFormat.h"
#include "SerializablePluginDescription.h"

#if HELIO_DESKTOP
#   define SAFE_SCAN 1
#else
#   define SAFE_SCAN 0
#endif

PluginScanner::PluginScanner() :
    Thread("Plugin Scanner")
{
    this->startThread(0);
}

PluginScanner::~PluginScanner()
{
    this->signalThreadShouldExit();
    this->signal();
    this->waitForThreadToExit(500);
}

bool PluginScanner::hasEffects() const
{
    for (const auto &description : this->getPlugins())
    {
        if (! description.isInstrument)
        {
            return true;
        }
    }

    return false;
}

bool PluginScanner::hasInstruments() const
{
    for (const auto &description : this->getPlugins())
    {
        if (description.isInstrument)
        {
            return true;
        }
    }

    return false;
}

void PluginScanner::removePlugin(const PluginDescription &description)
{
    this->pluginsList.removeType(description);
    this->sendChangeMessage();
}

void PluginScanner::sortList(KnownPluginList::SortMethod fieldToSortBy, bool forwards)
{
    this->pluginsList.sort(fieldToSortBy, forwards);
}

StringArray PluginScanner::getFilesToScan() const
{
    const ScopedReadLock lock(this->filesListLock);
    return this->filesToScan;
}

bool PluginScanner::isWorking() const
{
    return this->working.get();
}

void PluginScanner::cancelRunningScan()
{
    if (!this->isWorking())
    {
        jassertfalse;
        return;
    }

    this->cancelled = true;
}

void PluginScanner::runInitialScan()
{
    if (this->isWorking())
    {
        DBG("PluginScanner scan thread is already running!");
        return;
    }
    
    FileSearchPath pathToScan = this->getTypicalFolders();

    {
        const ScopedWriteLock filesLock(this->filesListLock);
        this->filesToScan.addIfNotAlreadyThere(BuiltInSynth::pianoId); // add built-in synths

        for (const auto &it : this->getPlugins())
        {
            this->filesToScan.addIfNotAlreadyThere(it.fileOrIdentifier);
        }

        this->pluginsList.clear();

        AudioPluginFormatManager formatManager;
        AudioCore::initAudioFormats(formatManager);

        for (int i = 0; i < formatManager.getNumFormats(); ++i)
        {
            AudioPluginFormat *format = formatManager.getFormat(i);
            FileSearchPath defaultLocations = format->getDefaultLocationsToSearch();

            for (int j = 0; j < defaultLocations.getNumPaths(); ++j)
            {
                pathToScan.addIfNotAlreadyThere(defaultLocations[j]);
            }

            StringArray foundPlugins = format->searchPathsForPlugins(pathToScan, true, true);
            this->filesToScan.addArray(foundPlugins);
        }
    }

    this->signal();
}

void PluginScanner::scanFolderAndAddResults(const File &dir)
{
    if (this->isWorking())
    {
        DBG("PluginScanner scan thread is already running!");
        return;
    }

    FileSearchPath pathToScan = dir.getFullPathName();

    Array<File> subPaths;
    pathToScan.findChildFiles(subPaths, File::findDirectories, false);

    for (auto &subPath : subPaths)
    {
        pathToScan.addIfNotAlreadyThere(subPath);
    }

    {
        const ScopedWriteLock lock(this->filesListLock);

        AudioPluginFormatManager formatManager;
        AudioCore::initAudioFormats(formatManager);

        for (int i = 0; i < formatManager.getNumFormats(); ++i)
        {
            AudioPluginFormat *format = formatManager.getFormat(i);
            StringArray foundPlugins = format->searchPathsForPlugins(pathToScan, true);
            this->filesToScan.addArray(foundPlugins);
        }
    }

    this->signal();
}

//===----------------------------------------------------------------------===//
// Thread
//===----------------------------------------------------------------------===//

void PluginScanner::run()
{
    WaitableEvent::wait();
    
    AudioPluginFormatManager formatManager;
    AudioCore::initAudioFormats(formatManager);
    
    while (!this->threadShouldExit())
    {
        this->working = true;

        // list might have changed while waiting:
        this->sendChangeMessage();

        StringArray uncheckedList = this->getFilesToScan();
        const auto myPath(File::getSpecialLocation(File::currentExecutableFile).getFullPathName());

        try
        {
            for (const auto &pluginPath : uncheckedList)
            {
                if (this->cancelled.get())
                {
                    DBG("Plugin scanning canceled");
                    break;
                }

#if SAFE_SCAN
                DBG("Safe scanning: " + pluginPath);

                const Uuid tempFileName;
                const File tempFile(DocumentHelpers::getTempSlot(tempFileName.toString()));
                tempFile.appendText(pluginPath, false, false);

                Thread::sleep(50);

                ChildProcess checkerProcess;
                const String commandLine(myPath + " " + tempFileName.toString());
                checkerProcess.start(commandLine);
                    
                // FIXME! (#60): skips some valid plugins sometimes
                if (!checkerProcess.waitForProcessToFinish(5000))
                {
                    checkerProcess.kill();
                }
                else
                {
                    Thread::sleep(50);

                    if (tempFile.existsAsFile())
                    {
                        try
                        {
                            const auto tree(DocumentHelpers::load<XmlSerializer>(tempFile));
                            if (tree.isValid())
                            {
                                forEachValueTreeChildWithType(tree, e, Serialization::Audio::plugin)
                                {
                                    SerializablePluginDescription pluginDescription;
                                    pluginDescription.deserialize(e);
                                    this->pluginsList.addType(pluginDescription);
                                }
                                    
                                this->sendChangeMessage();
                            }
                        }
                        catch (...) {}
                    }
                }

                tempFile.deleteFile();
#else
                DBG("Unsafe scanning: " + pluginPath);

                KnownPluginList knownPluginList;
                OwnedArray<PluginDescription> typesFound;
                    
                try
                {
                    for (int j = 0; j < formatManager.getNumFormats(); ++j)
                    {
                        AudioPluginFormat *format = formatManager.getFormat(j);
                        knownPluginList.scanAndAddFile(pluginPath, false, typesFound, *format);
                    }
                }
                catch (...) {}
                    
                // at this point we are still alive and plugin haven't crashed the app
                if (typesFound.size() != 0)
                {
                    for (auto *type : typesFound)
                    {
                        this->pluginsList.addType(*type);
                    }
                }
                    
                this->sendChangeMessage();
                Thread::sleep(150);
#endif
            }
        }
        catch (...) {}

        {
            this->cancelled = false;
            this->working = false;
            
            DBG("Done scanning for audio plugins");
            this->sendChangeMessage();
        }
        
        WaitableEvent::wait();
    }
}

FileSearchPath PluginScanner::getTypicalFolders()
{
    FileSearchPath folders;

    StringArray possibleSubfolders;
    possibleSubfolders.add("Audio");
    possibleSubfolders.add("Steinberg");
    possibleSubfolders.add("VST Plugins");
    possibleSubfolders.add("VSTPlugins");
    possibleSubfolders.add("VST");
    possibleSubfolders.add("VST2");
    possibleSubfolders.add("VST 2");
    possibleSubfolders.add("VST3");
    possibleSubfolders.add("VST 3");
    possibleSubfolders.add("ladspa");
    possibleSubfolders.add(".ladspa");

    Array<File> systemFolders;

    systemFolders.add(File::getCurrentWorkingDirectory());
    systemFolders.add(File::getSpecialLocation(File::currentExecutableFile).getParentDirectory());
    systemFolders.add(File::getSpecialLocation(File::currentApplicationFile).getParentDirectory());
    systemFolders.add(File::getSpecialLocation(File::userHomeDirectory));
    systemFolders.add(File::getSpecialLocation(File::userDocumentsDirectory));
    systemFolders.add(File::getSpecialLocation(File::userDesktopDirectory));
    systemFolders.add(File::getSpecialLocation(File::userApplicationDataDirectory));
    systemFolders.add(File::getSpecialLocation(File::commonApplicationDataDirectory));
    systemFolders.add(File::getSpecialLocation(File::globalApplicationsDirectory));
    systemFolders.add(File::getSpecialLocation(File::userMusicDirectory));

    // здесь создаем все комбинации системных папок с возможными подпапками
    for (auto && systemFolder : systemFolders)
    {
        // здесь проходим по первому уровню системных папок и ищем еще и в них
        Array<File> subPaths;
        systemFolder.findChildFiles(subPaths, File::findDirectories, false);

        for (auto &subPath : subPaths)
        {
            this->scanPossibleSubfolders(possibleSubfolders, subPath, folders);
        }

        this->scanPossibleSubfolders(possibleSubfolders, systemFolder, folders);
    }

    return folders;
}

void PluginScanner::scanPossibleSubfolders(const StringArray &possibleSubfolders,
        const File &currentSystemFolder, FileSearchPath &foldersOut)
{
    for (const auto & possibleSubfolder : possibleSubfolders)
    {
        File f(currentSystemFolder.getChildFile(possibleSubfolder));

        if (f.exists())
        {
            foldersOut.add(f);
        }
    }
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree PluginScanner::serialize() const
{
    ValueTree tree(Serialization::Audio::pluginsList);

    for (const auto &type : this->getPlugins())
    {
        const SerializablePluginDescription pd(type);
        tree.appendChild(pd.serialize(), nullptr);
    }

    return tree;
}

void PluginScanner::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root = tree.hasType(Serialization::Audio::pluginsList) ?
        tree : tree.getChildWithName(Serialization::Audio::pluginsList);

    if (!root.isValid()) { return; }
    
    for (const auto &child : root)
    {
        SerializablePluginDescription pluginDescription;
        pluginDescription.deserialize(child);
        if (pluginDescription.isValid())
        {
            this->pluginsList.addType(pluginDescription);
        }
    }

    this->sendChangeMessage();
}

void PluginScanner::reset()
{
    this->pluginsList.clear();
    this->sendChangeMessage();
}
