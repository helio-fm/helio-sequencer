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
#include "DataEncoder.h"
#include "AudioCore.h"
#include "FileUtils.h"
#include "Config.h"
#include "SerializationKeys.h"

#include "BuiltInSynthFormat.h"

PluginManager::PluginManager() :
Thread("Plugin Scanner Thread"),
working(false),
usingExternalProcess(false)
{
    this->startThread(0);
    Config::load(Serialization::Core::pluginManager, this);
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
                    const File tempFile(FileUtils::getTempSlot(tempFileName.toString()));
                    tempFile.replaceWithText(i);
                    
                    const String myself = File::getSpecialLocation(File::currentExecutableFile).getFullPathName();
                    
                    ChildProcess checkerProcess;
                    String commandLine(myself + " " + tempFileName.toString());
                    checkerProcess.start(commandLine);
                    
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
                                ScopedPointer<XmlElement> xml(DataEncoder::loadObfuscated(tempFile));
                                
                                // todo as Serialization::Core::smartPluginDescription
                                if (xml)
                                {
                                    forEachXmlChildElementWithTagName(*xml, e, "PLUGIN")
                                    {
                                        const ScopedWriteLock lock(this->pluginsListLock);
                                        PluginDescription pluginDescription;
                                        pluginDescription.loadFromXml(*e);
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
                    //const File pluginFile(pluginPath);
                    
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
                    
                    // если мы дошли до сих пор, то все хорошо и плагин нас не обрушил
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

            Config::save(Serialization::Core::pluginManager, this);
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
    ValueTree tree(Serialization::Core::pluginManager);

    for (int i = 0; i < this->pluginsList.getNumTypes(); ++i)
    {
        tree.addChild(this->pluginsList.getType(i)->createXml());
    }

    return tree;
}

void PluginManager::deserialize(const ValueTree &tree)
{
    this->reset();

    const ScopedWriteLock lock(this->pluginsListLock);

    const XmlElement *mainSlot = tree.hasTagName(Serialization::Core::pluginManager) ?
                                 &tree : tree.getChildByName(Serialization::Core::pluginManager);

    if (mainSlot == nullptr) { return; }

    forEachXmlChildElement(*mainSlot, child)
    {
        PluginDescription pluginDescription;
        pluginDescription.loadFromXml(*child);
        this->pluginsList.addType(pluginDescription);
    }

    this->sendChangeMessage();
}

void PluginManager::reset()
{
    const ScopedWriteLock lock(this->pluginsListLock);
    this->pluginsList.clear();
    this->sendChangeMessage();
}
