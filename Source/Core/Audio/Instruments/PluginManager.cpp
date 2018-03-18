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
#include "PluginManager.h"
#include "AudioCore.h"
#include "DocumentHelpers.h"
#include "XmlSerializer.h"
#include "Config.h"
#include "SerializationKeys.h"
#include "BuiltInSynthFormat.h"
#include "PluginSmartDescription.h"

PluginManager::PluginManager() :
    Thread("Plugin Scanner Thread"),
    working(false),
    usingExternalProcess(false)
{
    this->startThread(0);
}

PluginManager::~PluginManager()
{
    this->signalThreadShouldExit();
    this->signal();
    this->waitForThreadToExit(500);
}

void PluginManager::removeListItem(int index)
{
    const ScopedWriteLock lock(this->pluginsListLock);
    return this->pluginsList.removeType(index);
}

const KnownPluginList &PluginManager::getList()
{
    const ScopedReadLock lock(this->pluginsListLock);
    return this->pluginsList;
}

void PluginManager::sortList(KnownPluginList::SortMethod sortMethod, bool forward)
{
    const ScopedReadLock lock(this->pluginsListLock);
    this->pluginsList.sort(sortMethod, forward);
}

StringArray PluginManager::getFilesToScan() const
{
    const ScopedReadLock lock(this->filesListLock);
    return this->filesToScan;
}

bool PluginManager::isWorking() const
{
    const ScopedReadLock lock(this->workingFlagLock);
    return this->working;
}

void PluginManager::runInitialScan()
{
    if (this->isWorking())
    {
        Logger::writeToLog("PluginManager scan thread is already running!");
        return;
    }
    
#if HELIO_DESKTOP
    this->usingExternalProcess = true;
#endif
    
    FileSearchPath pathToScan = this->getTypicalFolders();

    {
        const ScopedWriteLock filesLock(this->filesListLock);
        //this->filesToScan.addIfNotAlreadyThere(BuiltInSynth::sineId); // add built-in synths
        this->filesToScan.addIfNotAlreadyThere(BuiltInSynth::pianoId); // add built-in synths

        // проверить на валидность все имеющиеся плагины
        for (auto & it : this->getList())
        {
            this->filesToScan.addIfNotAlreadyThere(it->fileOrIdentifier);
        }

        {
            const ScopedWriteLock pluginsLock(this->pluginsListLock);
            this->pluginsList.clear();
        }

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

void PluginManager::scanFolderAndAddResults(const File &dir)
{
    if (this->isWorking())
    {
        Logger::writeToLog("PluginManager scan thread is already running!");
        return;
    }
    
    this->usingExternalProcess = false;
    
    FileSearchPath pathToScan = dir.getFullPathName();

    Array<File> subPaths;
    pathToScan.findChildFiles(subPaths, File::findDirectories, false);

    for (auto && subPath : subPaths)
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

void PluginManager::run()
{
    WaitableEvent::wait();
    
    AudioPluginFormatManager formatManager;
    AudioCore::initAudioFormats(formatManager);
    
    while (!this->threadShouldExit())
    {
        {
            const ScopedWriteLock lock(this->workingFlagLock);
            this->working = true;
        }
        
        StringArray uncheckedList = this->getFilesToScan();

        try
        {
            for (const auto & i : uncheckedList)
            {
                Logger::writeToLog(i);
                if (this->usingExternalProcess)
                {
                    const Uuid tempFileName;
                    const File tempFile(DocumentHelpers::getTempSlot(tempFileName.toString()));
                    tempFile.replaceWithText(i);
                    
                    const String myself = File::getSpecialLocation(File::currentExecutableFile).getFullPathName();
                    
                    ChildProcess checkerProcess;
                    String commandLine(myself + " " + tempFileName.toString());
                    checkerProcess.start(commandLine);
                    
                    // FIXME! (#60): skips some valid plugins sometimes
                    if (!checkerProcess.waitForProcessToFinish(3000))
                    {
                        checkerProcess.kill();
                    }
                    else
                    {
                        if (tempFile.existsAsFile())
                        {
                            try
                            {
                                const auto tree(DocumentHelpers::load<XmlSerializer>(tempFile));
                                if (tree.isValid())
                                {
                                    forEachValueTreeChildWithType(tree, e, Serialization::Audio::plugin)
                                    {
                                        const ScopedWriteLock lock(this->pluginsListLock);
                                        PluginSmartDescription pluginDescription;
                                        pluginDescription.deserialize(e);
                                        this->pluginsList.addType(pluginDescription);
                                    }
                                    
                                    this->sendChangeMessage();
                                }
                            }
                            catch (...)
                            { }
                        }
                    }
                    
                    tempFile.deleteFile();
                }
                else
                {
                    const String pluginPath(i);
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
                        for (auto type : typesFound)
                        {
                            const ScopedWriteLock lock(this->pluginsListLock);
                            this->pluginsList.addType(*type);
                        }
                    }
                    
                    this->sendChangeMessage();
                    Thread::sleep(150);
                }
            }
        }
        catch (...) { }

        {
            const ScopedWriteLock lock(this->workingFlagLock);
            this->working = false;
            
            Logger::writeToLog("Done scanning for audio plugins");
            this->sendChangeMessage();
        }
        
        WaitableEvent::wait();
    }
}

FileSearchPath PluginManager::getTypicalFolders()
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

        for (auto && subPath : subPaths)
        {
            this->scanPossibleSubfolders(possibleSubfolders, subPath, folders);
        }

        this->scanPossibleSubfolders(possibleSubfolders, systemFolder, folders);
    }

    return folders;
}

void PluginManager::scanPossibleSubfolders(const StringArray &possibleSubfolders,
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

ValueTree PluginManager::serialize() const
{
    const ScopedReadLock lock(this->pluginsListLock);
    ValueTree tree(Serialization::Audio::pluginsList);

    for (int i = 0; i < this->pluginsList.getNumTypes(); ++i)
    {
        PluginSmartDescription pd(this->pluginsList.getType(i));
        tree.appendChild(pd.serialize(), nullptr);
    }

    return tree;
}

void PluginManager::deserialize(const ValueTree &tree)
{
    this->reset();

    const ScopedWriteLock lock(this->pluginsListLock);

    const auto root = tree.hasType(Serialization::Audio::pluginsList) ?
        tree : tree.getChildWithName(Serialization::Audio::pluginsList);

    if (!root.isValid()) { return; }
    
    for (const auto &child : root)
    {
        PluginSmartDescription pluginDescription;
        pluginDescription.deserialize(child);
        if (pluginDescription.isValid())
        {
            this->pluginsList.addType(pluginDescription);
        }
    }

    this->sendChangeMessage();
}

void PluginManager::reset()
{
    const ScopedWriteLock lock(this->pluginsListLock);
    this->pluginsList.clear();
    this->sendChangeMessage();
}
