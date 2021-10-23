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
#include "DocumentHelpers.h"
#include "AudioCore.h"
#include "RootNode.h"
#include "PluginScanner.h"
#include "SettingsNode.h"
#include "OrchestraPitNode.h"
#include "ProjectNode.h"
#include "SerializationKeys.h"
#include "CommandPaletteProjectsList.h"

Workspace::Workspace() = default;

Workspace::~Workspace()
{
    this->shutdown();
}

void Workspace::init()
{
    if (! this->wasInitialized)
    {
        this->audioCore = make<AudioCore>();
        this->pluginManager = make<PluginScanner>();
        this->treeRoot = make<RootNode>("Workspace");

        this->consoleProjectsList = make<CommandPaletteProjectsList>(*this);

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

        this->consoleProjectsList = nullptr;

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
    const String newProjectName = TRANS(I18n::Defaults::newProjectName);
    const String fileName = newProjectName + ".helio";

    this->newProjectFileChooser = make<FileChooser>(
        TRANS(I18n::Dialog::workspaceCreateProjectCaption),
        DocumentHelpers::getDocumentSlot(fileName), "*.helio", true);

    DocumentHelpers::showFileChooser(this->newProjectFileChooser,
        Globals::UI::FileChooser::forFileToSave,
        [this](URL &url)
    {
        if (!url.isLocalFile())
        {
            App::Layout().showTooltip({}, MainLayout::TooltipIcon::Failure);
            return;
        }

        if (auto *p = this->treeRoot->addEmptyProject(url.getLocalFile(), {}))
        {
            this->userProfile.onProjectLocalInfoUpdated(p->getId(),
                p->getName(), p->getDocument()->getFullPath());
        }
    });
}

bool Workspace::loadRecentProject(RecentProjectInfo::Ptr info)
{
    const File file(info->getLocalFile());
    if (file.existsAsFile())
    {
        return this->treeRoot->openProject(file) != nullptr;
    }
#if !NO_NETWORK
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
#endif

    return false;
}

void Workspace::unloadProject(const String &projectId, bool deleteLocally, bool deleteRemotely)
{
    const auto projects = this->treeRoot->findChildrenOfType<ProjectNode>();
    TreeNode *currentShowingItem = this->navigationHistory.getCurrentItem();
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

#if !NO_NETWORK
        if (deleteRemotely)
        {
            this->userProfile.deleteProjectRemotely(projectId);
        }
#endif
    }
    
    if (shouldSwitchToOtherPage)
    {
        if (projectToSwitchTo)
        {
            projectToSwitchTo->setSelected();
        }
        else
        {
            this->treeRoot->setSelected();
        }
    }
}

Array<ProjectNode *> Workspace::getLoadedProjects() const
{
    return this->treeRoot->findChildrenOfType<ProjectNode>();
}

bool Workspace::hasLoadedProject(const RecentProjectInfo::Ptr file) const
{
    for (const auto *loadedProject : this->getLoadedProjects())
    {
        if (loadedProject->getId() == file->getProjectId())
        {
            return true;
        }
    }

    return false;
}

void Workspace::stopPlaybackForAllProjects()
{
    for (auto *project : this->getLoadedProjects())
    {
        project->getTransport().stopPlaybackAndRecording();
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

    return false; // the very first run
}

void Workspace::failedDeserializationFallback()
{
    this->getAudioCore().autodetectAudioDeviceSetup();
    this->getAudioCore().autodetectMidiDeviceSetup();
    this->getAudioCore().initDefaultInstrument();

    TreeNode *settings = new SettingsNode();
    this->treeRoot->addChildNode(settings);
    
    TreeNode *instruments = new OrchestraPitNode();
    this->treeRoot->addChildNode(instruments);
    
    if (auto *p = this->treeRoot->addExampleProject())
    {
        this->userProfile.onProjectLocalInfoUpdated(p->getId(),
            p->getName(), p->getDocument()->getFullPath());
    }
    
    this->wasInitialized = true;
    this->autosave();
}

void Workspace::importProject(const String &filePattern)
{
    this->importFileChooser = make<FileChooser>(TRANS(I18n::Dialog::documentImport),
        File::getCurrentWorkingDirectory(), filePattern, true);

    DocumentHelpers::showFileChooser(this->importFileChooser,
        Globals::UI::FileChooser::forFileToOpen,
        [this](URL &url)
    {
        if (!url.isLocalFile())
        {
            return;
        }

        const auto file = url.getLocalFile();
        const auto extension = file.getFileExtension();

        if (extension == ".mid" || extension == ".midi" || extension == ".smf")
        {
            if (auto *p = this->treeRoot->importMidi(file))
            {
                this->userProfile.onProjectLocalInfoUpdated(p->getId(),
                    p->getName(), p->getDocument()->getFullPath());
                this->autosave();
            }
        }
        else
        {
            if (auto *p = this->treeRoot->openProject(file))
            {
                this->userProfile.onProjectLocalInfoUpdated(p->getId(),
                    p->getName(), p->getDocument()->getFullPath());
                this->autosave();
            }
        }
    });
}

//===----------------------------------------------------------------------===//
// Command Palette
//===----------------------------------------------------------------------===//

Array<CommandPaletteActionsProvider *> Workspace::getCommandPaletteActionProviders() const
{
    return { this->consoleProjectsList.get() };
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

static void addAllActiveItemIds(TreeNodeBase *item, SerializedData &parent)
{
    if (auto *treeItem = dynamic_cast<TreeNode *>(item))
    {
        if (treeItem->isSelected())
        {
            SerializedData child(Serialization::Core::selectedTreeNode);
            child.setProperty(Serialization::Core::treeNodeId, item->getNodeIdentifier());
            parent.appendChild(child);
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

void Workspace::selectTreeNodeWithId(const String &id)
{
    selectActiveSubItemWithId(this->treeRoot.get(), id);
}

SerializedData Workspace::serialize() const
{
    using namespace Serialization;
    SerializedData tree(Core::workspace);

    // TODO serialize window size and position

    tree.appendChild(this->userProfile.serialize());
    tree.appendChild(this->audioCore->serialize());
    tree.appendChild(this->pluginManager->serialize());

    SerializedData treeRootNode(Core::treeRoot);
    treeRootNode.appendChild(this->treeRoot->serialize());
    tree.appendChild(treeRootNode);
    
    // TODO serialize tree openness state?
    SerializedData treeStateNode(Core::treeState);
    addAllActiveItemIds(this->treeRoot.get(), treeStateNode);
    tree.appendChild(treeStateNode);
    
    return tree;
}

void Workspace::deserialize(const SerializedData &data)
{
    this->reset();
    using namespace Serialization;
    
    auto root = data.hasType(Core::workspace) ?
        data : data.getChildWithName(Core::workspace);
    
    if (!root.isValid())
    {
        // Always fallback to default workspace
        this->failedDeserializationFallback();
        return;
    }

    this->userProfile.deserialize(root);
    this->audioCore->deserialize(root);
    this->pluginManager->deserialize(root);

    const auto treeRootNode = root.getChildWithName(Core::treeRoot);
    jassert(treeRootNode.isValid());

    this->treeRoot->deserialize(treeRootNode);
    
    bool foundActiveNode = false;
    const auto treeStateNode = root.getChildWithName(Core::treeState);
    if (treeStateNode.isValid())
    {
        forEachChildWithType(treeStateNode, e, Core::selectedTreeNode)
        {
            const String id = e.getProperty(Core::treeNodeId);
            foundActiveNode = (nullptr != selectActiveSubItemWithId(this->treeRoot.get(), id));
        }
    }
    
    // TODO pass all opened projects to user profile?

    // If no instruments root item is found for whatever reason
    // (i.e. malformed tree), make sure to add one:
    if (nullptr == this->treeRoot->findChildOfType<OrchestraPitNode>())
    {
        this->treeRoot->addChildNode(new OrchestraPitNode(), 0);
    }
    
    // The same hack for settings root:
    if (nullptr == this->treeRoot->findChildOfType<SettingsNode>())
    {
        this->treeRoot->addChildNode(new SettingsNode(), 0);
    }

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
