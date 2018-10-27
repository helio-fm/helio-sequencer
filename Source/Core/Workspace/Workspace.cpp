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
#include "DocumentHelpers.h"
#include "AudioCore.h"
#include "Instrument.h"
#include "AudioMonitor.h"
#include "RootTreeItem.h"
#include "PluginScanner.h"
#include "SettingsTreeItem.h"
#include "OrchestraPitTreeItem.h"
#include "VersionControlTreeItem.h"
#include "ProjectTreeItem.h"
#include "RootTreeItem.h"
#include "Dashboard.h"

Workspace::~Workspace()
{
    this->shutdown();
}

void Workspace::init()
{
    if (! this->wasInitialized)
    {
        this->audioCore.reset(new AudioCore());
        this->pluginManager.reset(new PluginScanner());
        this->treeRoot.reset(new RootTreeItem("Workspace"));
        
        if (! this->autoload())
        {
            Logger::writeToLog("Workspace autoload failed, creating the empty workspace");
            this->failedDeserializationFallback();
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

void Workspace::shutdown()
{
    if (this->wasInitialized)
    {
        this->autosave();

        // To cleanup properly, remove all projects first (before instruments etc).
        // Tree item destructor will remove the rest.
        while (this->getLoadedProjects().size() > 0)
        {
            delete this->getLoadedProjects().getFirst();
        }

        this->treeRoot = nullptr;
        this->pluginManager = nullptr;
        this->audioCore = nullptr;

        this->wasInitialized = false;
    }
}

//===----------------------------------------------------------------------===//
// Navigation history
//===----------------------------------------------------------------------===//

NavigationHistory &Workspace::getNavigationHistory()
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

PluginScanner &Workspace::getPluginManager()
{
    jassert(this->audioCore);
    jassert(this->pluginManager);
    return *this->pluginManager;
}

RootTreeItem *Workspace::getTreeRoot()
{
    return this->treeRoot.get();
}

UserProfile &Workspace::getUserProfile()
{
    return this->userProfile;
}

//===----------------------------------------------------------------------===//
// Project management
//===----------------------------------------------------------------------===//

void Workspace::createEmptyProject()
{
    const String newProjectName = TRANS("defaults::newproject::name");
#if HELIO_DESKTOP
    const String fileName = newProjectName + ".helio";
    FileChooser fc(TRANS("dialog::workspace::createproject::caption"),
                   DocumentHelpers::getDocumentSlot(fileName), "*.helio", true);
    
    if (fc.browseForFileToSave(true))
    {
        this->treeRoot->addDefaultProject(fc.getResult());
    }
#else
    this->treeRoot->addDefaultProject(newProjectName);
#endif
}

bool Workspace::loadRecentProject(RecentProjectInfo::Ptr projectInfo)
{
    const File file(projectInfo->getLocalFile());
    if (file.existsAsFile())
    {
        const auto *project = this->treeRoot->openProject(file);
        return (project != nullptr);
    }
    //else if (projectInfo->hasRemoteCopy) // and not present locally
    //{
    //    // TODO checkout and open
    //    this->treeRoot->checkoutProject(fileDescription->projectId);
    //}

    return true;
}

void Workspace::unloadProject(const String &targetProjectId)
{
    const auto projects = this->treeRoot->findChildrenOfType<ProjectTreeItem>();
    TreeItem *currentShowingItem = this->getActiveTreeItem();
    ProjectTreeItem *projectToDelete = nullptr;
    ProjectTreeItem *projectToSwitchTo = nullptr;
    
    for (auto *project : projects)
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
    
    if (projectToDelete != nullptr)
    {
        childrenToDelete = projectToDelete->findChildrenOfType<TreeItem>();
        isShowingAnyProjectToDelete = (currentShowingItem == projectToDelete);
    }
    
    for (auto *treeItem : childrenToDelete)
    {
        if (currentShowingItem == treeItem)
        {
            isShowingAnyOfDeletedChildren = true;
            break;
        }
    }
    
    const bool shouldSwitchToOtherPage = isShowingAnyProjectToDelete || isShowingAnyOfDeletedChildren;
    
    if (projectToDelete != nullptr)
    {
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

void Workspace::deleteProject(ProjectTreeItem &project)
{
    if (auto *vcsTreeItem = project.findChildOfType<VersionControlTreeItem>())
    {
        //vcsTreeItem->deletePermanentlyFromRemoteRepo(); // FIXME
        const File localProjectFile(project.getDocument()->getFullPath());
        this->unloadProject(project.getId());
        this->getUserProfile().onProjectDeleted(project.getId());
        localProjectFile.deleteFile();
    }
}

Array<ProjectTreeItem *> Workspace::getLoadedProjects() const
{
    return this->treeRoot->findChildrenOfType<ProjectTreeItem>();
}

void Workspace::stopPlaybackForAllProjects()
{
    for (auto *project : this->getLoadedProjects())
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
    
    Config::save(this, Serialization::Config::activeWorkspace);
}

bool Workspace::autoload()
{
    if (Config::contains(Serialization::Config::activeWorkspace))
    {
        Config::load(this, Serialization::Config::activeWorkspace);
        return true;
    }
    else
    {
        // Try loading a legacy workspace file, if found one:
        const auto legacyFile(DocumentHelpers::getDocumentSlot("Workspace.helio"));
        if (legacyFile.existsAsFile())
        {
            const auto legacyState = DocumentHelpers::load(legacyFile);
            this->deserialize(legacyState);
            return true;
        }
    }

    return false;
}

void Workspace::failedDeserializationFallback()
{
    this->getAudioCore().autodetectDeviceSetup();
    this->getAudioCore().initDefaultInstrument();

    TreeItem *settings = new SettingsTreeItem();
    this->treeRoot->addChildTreeItem(settings);
    
    //TreeItem *scripts = new ScriptsRootTreeItem(*this);
    //this->treeRoot->addChildTreeItem(scripts);
    
    TreeItem *instruments = new OrchestraPitTreeItem();
    this->treeRoot->addChildTreeItem(instruments);
    
    ProjectTreeItem *project = this->treeRoot->addDefaultProject(TRANS("defaults::newproject::name"));
    project->setSelected(true, false);
    project->showPage();
    
    this->wasInitialized = true;
    this->autosave();
}

void Workspace::importProject(const String &filePattern)
{
#if HELIO_DESKTOP
    FileChooser fc(TRANS("dialog::document::import"),
        File::getCurrentWorkingDirectory(), filePattern, true);

    if (fc.browseForFileToOpen())
    {
        const File file(fc.getResult());
        const String &extension = file.getFileExtension();
        if (extension == ".mid" || extension == ".midi" || extension == ".smf")
        {
            this->treeRoot->importMidi(file);
            this->autosave();
        }
        else
        {
            this->treeRoot->openProject(file);
            this->autosave();
        }
    }
#endif
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

static void addAllActiveItemIds(TreeViewItem *item, ValueTree &parent)
{
    if (TreeItem *treeItem = dynamic_cast<TreeItem *>(item))
    {
        if (treeItem->isPrimarySelection())
        {
            ValueTree child(Serialization::Core::selectedTreeItem);
            child.setProperty(Serialization::Core::treeItemId, item->getItemIdentifierString(), nullptr);
            parent.appendChild(child, nullptr);
        }
        
        for (int i = 0; i < item->getNumSubItems(); ++i)
        {
            addAllActiveItemIds(item->getSubItem(i), parent);
        }
    }
}

static TreeItem *selectActiveSubItemWithId(TreeViewItem *item, const String &id)
{
    if (auto *treeItem = dynamic_cast<TreeItem *>(item))
    {
        if (treeItem->getItemIdentifierString() == id)
        {
            treeItem->setPrimarySelection(true);
            treeItem->setSelected(true, true);
            treeItem->showPage();
            return treeItem;
        }
        
        for (int i = 0; i < item->getNumSubItems(); ++i)
        {
            if (auto *subItem = selectActiveSubItemWithId(item->getSubItem(i), id))
            {
                return subItem;
            }
        }
    }

    return nullptr;
}

void Workspace::activateSubItemWithId(const String &id)
{
    selectActiveSubItemWithId(this->treeRoot.get(), id);
}

ValueTree Workspace::serialize() const
{
    using namespace Serialization;
    ValueTree tree(Core::workspace);

    // TODO serialize window size and position

    tree.appendChild(this->userProfile.serialize(), nullptr);
    tree.appendChild(this->audioCore->serialize(), nullptr);
    tree.appendChild(this->pluginManager->serialize(), nullptr);

    ValueTree treeRootNode(Core::treeRoot);
    treeRootNode.appendChild(this->treeRoot->serialize(), nullptr);
    tree.appendChild(treeRootNode, nullptr);
    
    // TODO serialize tree openness state?
    ValueTree treeStateNode(Core::treeState);
    addAllActiveItemIds(this->treeRoot.get(), treeStateNode);
    tree.appendChild(treeStateNode, nullptr);
    
    return tree;
}

void Workspace::deserialize(const ValueTree &tree)
{
    this->reset();
    using namespace Serialization;
    
    auto root = tree.hasType(Core::workspace) ?
        tree : tree.getChildWithName(Core::workspace);
    
    if (!root.isValid())
    {
        // Always fallback to default workspace
        this->failedDeserializationFallback();
        return;
    }

    this->userProfile.deserialize(root);
    this->audioCore->deserialize(root);
    this->pluginManager->deserialize(root);

    auto treeRootNodeLegacy = root.getChildWithName(Core::treeItem);
    auto treeRootNode = root.getChildWithName(Core::treeRoot);
    this->treeRoot->deserialize(treeRootNode.isValid() ? treeRootNode : treeRootNodeLegacy);
    
    bool foundActiveNode = false;
    const auto treeStateNode = root.getChildWithName(Core::treeState);
    if (treeStateNode.isValid())
    {
        forEachValueTreeChildWithType(treeStateNode, e, Core::selectedTreeItem)
        {
            const String id = e.getProperty(Core::treeItemId);
            foundActiveNode = (nullptr != selectActiveSubItemWithId(this->treeRoot.get(), id));
        }
    }
    
    // TODO pass all opened projects to user profile?

    // If no instruments root item is found for whatever reason
    // (i.e. malformed tree), make sure to add one:
    if (nullptr == this->treeRoot->findChildOfType<OrchestraPitTreeItem>())
    { this->treeRoot->addChildTreeItem(new OrchestraPitTreeItem(), 0); }
    
    // The same hack for settings root:
    if (nullptr == this->treeRoot->findChildOfType<SettingsTreeItem>())
    { this->treeRoot->addChildTreeItem(new SettingsTreeItem(), 0); }

    if (! foundActiveNode)
    {
        // Fallback to the main page
        selectActiveSubItemWithId(this->treeRoot.get(), this->treeRoot->getItemIdentifierString());
    }
}

void Workspace::reset()
{
    this->userProfile.reset();
    this->audioCore->reset();
    this->treeRoot->reset();
}
