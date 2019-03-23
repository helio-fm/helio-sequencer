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
#include "RootNode.h"
#include "PluginScanner.h"
#include "SettingsNode.h"
#include "OrchestraPitNode.h"
#include "VersionControlNode.h"
#include "MidiTrackNode.h"
#include "ProjectNode.h"
#include "RootNode.h"
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
        this->treeRoot.reset(new RootNode("Workspace"));
        
        if (! this->autoload())
        {
            DBG("Workspace autoload failed, creating the empty workspace");
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

WeakReference<TreeNode> Workspace::getActiveTreeItem() const
{
    return this->navigationHistory.getCurrentItem();
}

void Workspace::navigateBackwardIfPossible()
{
    TreeNode *treeItem = this->navigationHistory.goBack();

    if (treeItem != nullptr)
    {
        const auto scopedHistoryLock(this->navigationHistory.lock());
        treeItem->setSelected();
    }
}

void Workspace::navigateForwardIfPossible()
{
    TreeNode *treeItem = this->navigationHistory.goForward();

    if (treeItem != nullptr)
    {
        const auto scopedHistoryLock(this->navigationHistory.lock());
        treeItem->setSelected();
    }
}

//===----------------------------------------------------------------------===//
// Accessors
//===----------------------------------------------------------------------===//

AudioCore &Workspace::getAudioCore() noexcept
{
    jassert(this->audioCore);
    return *this->audioCore;
}

PluginScanner &Workspace::getPluginManager() noexcept
{
    jassert(this->audioCore);
    jassert(this->pluginManager);
    return *this->pluginManager;
}

RootNode *Workspace::getTreeRoot() noexcept
{
    return this->treeRoot.get();
}

UserProfile &Workspace::getUserProfile() noexcept
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
        if (auto *p = this->treeRoot->addDefaultProject(fc.getResult()))
        {
            this->userProfile.onProjectLocalInfoUpdated(p->getId(),
                p->getName(), p->getDocument()->getFullPath());
            p->selectChildOfType<MidiTrackNode>();
        }
    }
#else
    if (auto *p = this->treeRoot->addDefaultProject(newProjectName))
    {
        this->userProfile.onProjectLocalInfoUpdated(p->getId(),
            p->getName(), p->getDocument()->getFullPath());
        p->selectChildOfType<MidiTrackNode>();
    }
#endif
}

bool Workspace::loadRecentProject(RecentProjectInfo::Ptr info)
{
    const File file(info->getLocalFile());
    if (file.existsAsFile())
    {
        return this->treeRoot->openProject(file) != nullptr;
    }
    else if (info->hasRemoteCopy()) // and not present locally
    {
        if (this->userProfile.isLoggedIn())
        {
            if (auto *p = this->treeRoot->checkoutProject(info->getProjectId(), info->getTitle()))
            {
                this->userProfile.onProjectLocalInfoUpdated(p->getId(),
                    p->getName(), p->getDocument()->getFullPath());

                return true;
            }

            return false;
        }
        else
        {
            // TODO show message "yo, login pls"
            return true;
        }
    }

    return true;
}

void Workspace::unloadProject(const String &projectId, bool deleteLocally, bool deleteRemotely)
{
    const auto projects = this->treeRoot->findChildrenOfType<ProjectNode>();
    TreeNode *currentShowingItem = this->getActiveTreeItem();
    ProjectNode *projectToDelete = nullptr;
    ProjectNode *projectToSwitchTo = nullptr;
    
    for (auto *project : projects)
    {
        if (project->getId() == projectId)
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
    Array<TreeNode *> childrenToDelete;
    
    if (projectToDelete != nullptr)
    {
        childrenToDelete = projectToDelete->findChildrenOfType<TreeNode>();
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
        const File localFile(projectToDelete->getDocument()->getFullPath());
        delete projectToDelete;

        if (deleteLocally)
        {
            this->userProfile.deleteProjectLocally(projectId);
        }

        if (deleteRemotely)
        {
            this->userProfile.deleteProjectRemotely(projectId);
        }
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

Array<ProjectNode *> Workspace::getLoadedProjects() const
{
    return this->treeRoot->findChildrenOfType<ProjectNode>();
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
    
    App::Config().save(this, Serialization::Config::activeWorkspace);
}

bool Workspace::autoload()
{
    if (App::Config().containsProperty(Serialization::Config::activeWorkspace))
    {
        App::Config().load(this, Serialization::Config::activeWorkspace);
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

    TreeNode *settings = new SettingsNode();
    this->treeRoot->addChildTreeItem(settings);
    
    TreeNode *instruments = new OrchestraPitNode();
    this->treeRoot->addChildTreeItem(instruments);
    
    if (auto *p = this->treeRoot->addDefaultProject(TRANS("defaults::newproject::name")))
    {
        this->userProfile.onProjectLocalInfoUpdated(p->getId(),
            p->getName(), p->getDocument()->getFullPath());
        p->selectChildOfType<MidiTrackNode>();
    }
    
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

static void addAllActiveItemIds(TreeNodeBase *item, ValueTree &parent)
{
    if (auto *treeItem = dynamic_cast<TreeNode *>(item))
    {
        if (treeItem->isSelected())
        {
            ValueTree child(Serialization::Core::selectedTreeNode);
            child.setProperty(Serialization::Core::treeNodeId, item->getNodeIdentifier(), nullptr);
            parent.appendChild(child, nullptr);
        }
        
        for (int i = 0; i < item->getNumChildren(); ++i)
        {
            addAllActiveItemIds(item->getChild(i), parent);
        }
    }
}

static TreeNode *selectActiveSubItemWithId(TreeNodeBase *item, const String &id)
{
    if (auto *treeItem = dynamic_cast<TreeNode *>(item))
    {
        if (treeItem->getNodeIdentifier() == id)
        {
            treeItem->setSelected();
            treeItem->showPage();
            return treeItem;
        }
        
        for (int i = 0; i < item->getNumChildren(); ++i)
        {
            if (auto *subItem = selectActiveSubItemWithId(item->getChild(i), id))
            {
                return subItem;
            }
        }
    }

    return nullptr;
}

void Workspace::activateTreeItem(const String &id)
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

    auto treeRootNodeLegacy = root.getChildWithName(Core::treeNode);
    auto treeRootNode = root.getChildWithName(Core::treeRoot);
    this->treeRoot->deserialize(treeRootNode.isValid() ? treeRootNode : treeRootNodeLegacy);
    
    bool foundActiveNode = false;
    const auto treeStateNode = root.getChildWithName(Core::treeState);
    if (treeStateNode.isValid())
    {
        forEachValueTreeChildWithType(treeStateNode, e, Core::selectedTreeNode)
        {
            const String id = e.getProperty(Core::treeNodeId);
            foundActiveNode = (nullptr != selectActiveSubItemWithId(this->treeRoot.get(), id));
        }
    }
    
    // TODO pass all opened projects to user profile?

    // If no instruments root item is found for whatever reason
    // (i.e. malformed tree), make sure to add one:
    if (nullptr == this->treeRoot->findChildOfType<OrchestraPitNode>())
    { this->treeRoot->addChildTreeItem(new OrchestraPitNode(), 0); }
    
    // The same hack for settings root:
    if (nullptr == this->treeRoot->findChildOfType<SettingsNode>())
    { this->treeRoot->addChildTreeItem(new SettingsNode(), 0); }

    if (! foundActiveNode)
    {
        // Fallback to the main page
        selectActiveSubItemWithId(this->treeRoot.get(), this->treeRoot->getNodeIdentifier());
    }
}

void Workspace::reset()
{
    this->userProfile.reset();
    this->audioCore->reset();
    this->treeRoot->reset();
}
