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
#include "Workspace.h"
#include "Config.h"
#include "SerializationKeys.h"
#include "FileUtils.h"
#include "DataEncoder.h"
#include "AudioCore.h"
#include "RootTreeItem.h"
#include "PluginManager.h"
#include "SettingsTreeItem.h"
#include "InstrumentsRootTreeItem.h"
#include "ProjectTreeItem.h"
#include "RootTreeItem.h"

Workspace::Workspace() :
    DocumentOwner(*this, "Workspace", "helio"),
    wasInitialized(false)
{
    this->recentFilesList = new RecentFilesList();
    this->recentFilesList->addChangeListener(this);
}

Workspace::~Workspace()
{
    this->autosave();

    this->treeRoot = nullptr;
    
    this->recentFilesList->removeChangeListener(this);
    this->recentFilesList = nullptr;
    
    this->pluginManager = nullptr;
    this->audioCore = nullptr;
}

void Workspace::init()
{
    if (! this->wasInitialized)
    {
        this->audioCore = new AudioCore();
        this->pluginManager = new PluginManager();
        this->treeRoot = new RootTreeItem("Workspace One");
        
        if (! this->autoload())
        {
            // если что-то пошло не так, создаем воркспейс по дефолту
            Logger::writeToLog("workspace autoload failed, creating an empty one");
            this->createEmptyWorkspace();
            // и тут же сохраняем
            this->wasInitialized = true;
            this->autosave();
        }
        else
        {
            this->wasInitialized = true;
        }
    }
}

bool Workspace::isInitialized() const noexcept
{
    return this->wasInitialized;
}

//===----------------------------------------------------------------------===//
// Navigation history
//===----------------------------------------------------------------------===//

TreeNavigationHistory &Workspace::getNavigationHistory()
{
    return this->navigationHistory;
}

WeakReference<TreeItem> Workspace::getActiveTreeItem() const
{
    return this->navigationHistory.getCurrentItem();
}

void Workspace::navigateBackwardIfPossible()
{
    TreeItem *treeItem = this->navigationHistory.goBack();

    if (treeItem != nullptr)
    {
        const auto scopedHistoryLock(this->navigationHistory.lock());
        treeItem->setSelected(true, true);
    }
}

void Workspace::navigateForwardIfPossible()
{
    TreeItem *treeItem = this->navigationHistory.goForward();

    if (treeItem != nullptr)
    {
        const auto scopedHistoryLock(this->navigationHistory.lock());
        treeItem->setSelected(true, true);
    }
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

AudioCore &Workspace::getAudioCore()
{
    jassert(this->audioCore);
    return *this->audioCore;
}

PluginManager &Workspace::getPluginManager()
{
    jassert(this->audioCore);
    jassert(this->pluginManager);
    return *this->pluginManager;
}

RootTreeItem *Workspace::getTreeRoot() const
{
    return this->treeRoot;
}

//===----------------------------------------------------------------------===//
// RecentFilesListOwner
//===----------------------------------------------------------------------===//

RecentFilesList &Workspace::getRecentFilesList() const
{
    jassert(this->recentFilesList);
    return *this->recentFilesList;
}

bool Workspace::onClickedLoadRecentFile(RecentFileDescription::Ptr fileDescription)
{
    if (fileDescription->hasLocalCopy && fileDescription->path.isNotEmpty())
    {
        File absFile(fileDescription->path);
        ProjectTreeItem *project = this->treeRoot->openProject(absFile);
        
        if (project == nullptr)
        {
            // вот здесь файла может не быть тупо по той причине, что путь записан как абсолютный,
            // а в айос он будет постоянно меняться
            // поэтому, если файла нет, надо проверить в текущей папке
            
            File localFile(this->getDocument()->getFile().getParentDirectory().getChildFile(absFile.getFileName()));
            project = this->treeRoot->openProject(localFile);
            return (project != nullptr);
        }
        
        return true;
    }

    if (fileDescription->hasRemoteCopy)
    {
        this->treeRoot->checkoutProject(fileDescription->title,
                                        fileDescription->projectId,
                                        fileDescription->projectKey);
    }
    
    return true;
}

void Workspace::onClickedUnloadRecentFile(RecentFileDescription::Ptr fileDescription)
{
    this->unloadProjectById(fileDescription->projectId);
}

void Workspace::changeListenerCallback(ChangeBroadcaster *source)
{
    DocumentOwner::sendChangeMessage();
}

//===----------------------------------------------------------------------===//
// Project management
//===----------------------------------------------------------------------===//

void Workspace::createEmptyProject()
{
    const String newProjectName = TRANS("defaults::newproject::name");
#if HELIO_DESKTOP
    const String fileName = newProjectName + ".hp";
    FileChooser fc(TRANS("dialog::workspace::createproject::caption"),
                   FileUtils::getDocumentSlot(fileName), "*.hp", true);
    
    if (fc.browseForFileToSave(true))
    {
        this->treeRoot->addDefaultProject(fc.getResult());
    }
#else
    this->treeRoot->addDefaultProject(newProjectName);
#endif
}

void Workspace::unloadProjectById(const String &targetProjectId)
{
    Array<ProjectTreeItem *> projects =
    this->treeRoot->findChildrenOfType<ProjectTreeItem>();
    
    TreeItem *currentShowingItem = this->getActiveTreeItem();
    ProjectTreeItem *projectToDelete = nullptr;
    ProjectTreeItem *projectToSwitchTo = nullptr;
    
    for (auto project : projects)
    {
        if (project->getId() == targetProjectId)
        {
            projectToDelete = project;
        }
        else
        {
            projectToSwitchTo = project;
        }
    }
    
    bool isShowingAnyOfDeletedChildren = false;
    bool isShowingAnyProjectToDelete = false;
    Array<TreeItem *> childrenToDelete;
    
    if (projectToDelete)
    {
        childrenToDelete = projectToDelete->findChildrenOfType<TreeItem>();
        isShowingAnyProjectToDelete = (currentShowingItem == projectToDelete);
    }
    
    for (auto i : childrenToDelete)
    {
        if (currentShowingItem == i)
        {
            isShowingAnyOfDeletedChildren = true;
            break;
        }
    }
    
    const bool shouldSwitchToOtherPage = isShowingAnyProjectToDelete || isShowingAnyOfDeletedChildren;
    
    if (projectToDelete)
    {
        //TreeItem::deleteItem(projectToDelete);
        delete projectToDelete;
    }
    
    if (shouldSwitchToOtherPage)
    {
        if (projectToSwitchTo)
        {
            projectToSwitchTo->showPage();
        }
        else
        {
            this->treeRoot->showPage();
        }
    }
}

Array<ProjectTreeItem *> Workspace::getLoadedProjects() const
{
    return this->treeRoot->findChildrenOfType<ProjectTreeItem>();
}

void Workspace::stopPlaybackForAllProjects()
{
    Array<ProjectTreeItem *> projects = this->getLoadedProjects();
    
    for (auto project : projects)
    {
        project->getTransport().stopPlayback();
    }
}

//===----------------------------------------------------------------------===//
// Save/Load/Init
//===----------------------------------------------------------------------===//

void Workspace::autosave()
{
    if (! this->wasInitialized)
    {
        return;
    }
    
    this->getDocument()->save();
    Config::set(Serialization::Core::lastWorkspace, this->getDocument()->getFullPath());
    //Config::set(Serialization::Core::lastPage, ...);
    //Config::set(Serialization::Core::treeSize, ...);
    
    Logger::writeToLog("autosaved at " + this->getDocument()->getFullPath());
}

bool Workspace::autoload()
{
    const String lastSavedName = Config::get(Serialization::Core::lastWorkspace);
    File lastSavedFile(lastSavedName);
    
    // пытаемся найти файл по относительному пути
    if (! lastSavedFile.existsAsFile())
    {
        lastSavedFile =
        FileUtils::getDocumentSlot(lastSavedFile.getFileName());
    }
    
    Logger::writeToLog("Workspace::autoload - " + lastSavedFile.getFullPathName());
    
    if (lastSavedFile.existsAsFile())
    {
        return this->getDocument()->load(lastSavedFile.getFullPathName());
    }
    
    return false;
}

void Workspace::createEmptyWorkspace()
{
    // для того, чтоб не делать это несколько раз, делаем это здесь.
    this->getAudioCore().initDefaultInstrument();
    
    TreeItem *settings = new SettingsTreeItem();
    this->treeRoot->addChildTreeItem(settings);
    
    //TreeItem *scripts = new ScriptsRootTreeItem(*this);
    //this->treeRoot->addChildTreeItem(scripts);
    
    TreeItem *instruments = new InstrumentsRootTreeItem();
    this->treeRoot->addChildTreeItem(instruments);
    
    ProjectTreeItem *project = this->treeRoot->addDefaultProject(TRANS("defaults::newproject::name"));
    project->setSelected(true, false);
    project->showPage();
    
    //instruments->setSelected(true, false);
    //instruments->showPage();
    
    this->sendChangeMessage(); // to be saved ok
}

//===----------------------------------------------------------------------===//
// DocumentOwner
//===----------------------------------------------------------------------===//

bool Workspace::onDocumentLoad(File &file)
{
    ScopedPointer<XmlElement> xml(DataEncoder::loadObfuscated(file));
    
    if (xml)
    {
        this->deserialize(*xml);
        return true;
    }
    
    // если что-то пошло не так, создаем воркспейс по дефолту
    this->createEmptyWorkspace();
    return false;
}

bool Workspace::onDocumentSave(File &file)
{
    ScopedPointer<XmlElement> xml(this->serialize());
    return DataEncoder::saveObfuscated(file, xml);
}

void Workspace::onDocumentImport(File &file)
{
    const String &extension = file.getFileExtension();
    
    if (extension == ".mid" || extension == ".midi" || extension == ".smf")
    {
        this->treeRoot->importMidi(file);
    }
    else if (extension == ".hp")
    {
        this->treeRoot->openProject(file);
    }
}

bool Workspace::onDocumentExport(File &file)
{
    return false;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

static void addAllActiveItemIds(TreeViewItem *item, XmlElement &parent)
{
    if (TreeItem *treeItem = dynamic_cast<TreeItem *>(item))
    {
        if (treeItem->isMarkerVisible())
        {
            parent.createNewChildElement(Serialization::Core::selectedTreeItem)->
                setAttribute(Serialization::Core::treeItemId, item->getItemIdentifierString());
        }
        
        for (int i = 0; i < item->getNumSubItems(); ++i)
        {
            addAllActiveItemIds(item->getSubItem(i), parent);
        }
    }
}

static TreeItem *selectActiveSubItemWithId(TreeViewItem *item, const String &id)
{
    if (TreeItem *treeItem = dynamic_cast<TreeItem *>(item))
    {
        if (treeItem->getItemIdentifierString() == id)
        {
            treeItem->setMarkerVisible(true);
            treeItem->setSelected(true, true);
            treeItem->showPage();
            return treeItem;
        }
        
        for (int i = 0; i < item->getNumSubItems(); ++i)
        {
            if (TreeItem *treeItem = selectActiveSubItemWithId(item->getSubItem(i), id))
            {
                return treeItem;
            }
        }
    }

    return nullptr;
}

void Workspace::activateSubItemWithId(const String &id)
{
    selectActiveSubItemWithId(this->treeRoot, id);
}

XmlElement *Workspace::serialize() const
{
    auto xml = new XmlElement(Serialization::Core::workspace);
    
    // TODO serialize window size and position
    
    xml->addChildElement(this->audioCore->serialize());
    xml->addChildElement(this->pluginManager->serialize());
    xml->addChildElement(this->treeRoot->serialize());
    xml->addChildElement(this->recentFilesList->serialize());
    
    // TODO serialize tree openness state?
    auto treeStateNode = new XmlElement(Serialization::Core::treeState);
    addAllActiveItemIds(this->treeRoot, *treeStateNode);
    xml->addChildElement(treeStateNode);
    
    return xml;
}

void Workspace::deserialize(const XmlElement &xml)
{
    this->reset();
    
    const XmlElement *root = xml.hasTagName(Serialization::Core::workspace) ?
    &xml : xml.getChildByName(Serialization::Core::workspace);
    
    if (root == nullptr)
    {
        // Since we are supposed to be the root element, let's attempt to deserialize anyway
        root = xml.getFirstChildElement();
    }
    
    this->recentFilesList->deserialize(*root);
    this->audioCore->deserialize(*root);
    this->pluginManager->deserialize(*root);
    this->treeRoot->deserialize(*root);
    
    bool foundActiveNode = false;
    
    if (XmlElement *treeStateNode = root->getChildByName(Serialization::Core::treeState))
    {
        forEachXmlChildElementWithTagName(*treeStateNode, e, Serialization::Core::selectedTreeItem)
        {
            const String id = e->getStringAttribute(Serialization::Core::treeItemId);
            foundActiveNode = (nullptr != selectActiveSubItemWithId(this->treeRoot, id));
        }
    }
    
    if (! foundActiveNode)
    {
        // Fallback to the main page
        selectActiveSubItemWithId(this->treeRoot, this->treeRoot->getItemIdentifierString());
    }
}

void Workspace::reset()
{
    this->recentFilesList->reset();
    this->audioCore->reset();
    this->treeRoot->reset();
}
