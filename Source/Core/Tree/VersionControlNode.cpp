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
#include "VersionControlNode.h"
#include "TreeNodeSerializer.h"
#include "ProjectNode.h"
#include "VersionControl.h"
#include "Head.h"
#include "ProjectInfo.h"
#include "ProjectTimeline.h"
#include "FailTooltip.h"
#include "SuccessTooltip.h"
#include "VersionControlMenu.h"
#include "VersionControlEditor.h"
#include "Icons.h"
#include "Workspace.h"
#include "MainLayout.h"
#include "Network.h"
#include "ProjectSyncService.h"

VersionControlNode::VersionControlNode() :
    TreeNode("Versions", Serialization::Core::versionControl),
    vcs(nullptr)
{
    this->initVCS();
    this->initEditor();
}

VersionControlNode::~VersionControlNode()
{
    this->shutdownEditor();
    this->shutdownVCS();
}

Image VersionControlNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::versionControl, HEADLINE_ICON_SIZE);
}

void VersionControlNode::showPage()
{
    if (this->vcs == nullptr)
    {
        this->initVCS();
    }
    
    if (this->editor == nullptr)
    {
        this->initEditor();
    }
    
    if (this->editor != nullptr)
    {
        this->editor->updateState();
        App::Layout().showPage(this->editor.get(), this);
    }
}

void VersionControlNode::recreatePage()
{
    this->shutdownEditor();
    this->initEditor();
}

String VersionControlNode::getName() const noexcept
{
    return TRANS("tree::vcs");
}

//===----------------------------------------------------------------------===//
// Stats
//===----------------------------------------------------------------------===//

static void countStatsFor(const VCS::Revision::Ptr rootRevision, int &numRevisions, int &numDeltas)
{
    for (const auto *revItem : rootRevision->getItems())
    {
        numDeltas += revItem->getNumDeltas();
    }

    numRevisions += rootRevision->getChildren().size();
    for (auto *childRevision : rootRevision->getChildren())
    {
        countStatsFor(childRevision, numRevisions, numDeltas);
    }
}

String VersionControlNode::getStatsString() const
{
    if (this->vcs)
    {
        const auto root(this->vcs->getRoot());

        int numRevisions = 1;
        int numDeltas = 0;
        countStatsFor(root, numRevisions, numDeltas);

        return String(TRANS_PLURAL("{x} revisions", numRevisions) + " " +
            TRANS("common::and") + " " + TRANS_PLURAL("{x} deltas", numDeltas));
    }

    return{};
}

//===----------------------------------------------------------------------===//
// Version Control
//===----------------------------------------------------------------------===//

void VersionControlNode::commitProjectInfo()
{
    const auto *parentProject = this->findParentOfType<ProjectNode>();
    if (parentProject != nullptr && this->vcs != nullptr)
    {
        this->vcs->quickAmendItem(parentProject->getProjectInfo());
        this->vcs->quickAmendItem(parentProject->getTimeline());
    }
}

void VersionControlNode::toggleQuickStash()
{
    if (! this->vcs)
    { return; }

    this->vcs->getHead().rebuildDiffSynchronously();
    
    if (this->vcs->hasQuickStash())
    {
        if (this->vcs->getHead().hasAnythingOnTheStage())
        {
            App::Layout().showTooltip("Cannot revert, stage is not empty!");
            App::Layout().showModalComponentUnowned(new FailTooltip());
            return;
        }
        
        this->vcs->applyQuickStash();
        App::Layout().showTooltip("Toggle changes: latest state");
    }
    else
    {
        if (! this->vcs->getHead().hasAnythingOnTheStage())
        {
            App::Layout().showTooltip("Cannot reset, stage is empty!");
            App::Layout().showModalComponentUnowned(new FailTooltip());
            return;
        }
        
        this->vcs->quickStashAll();
        App::Layout().showTooltip("Toggle changes: original state");
    }
}

//===----------------------------------------------------------------------===//
// Tree
//===----------------------------------------------------------------------===//

void VersionControlNode::onItemAddedToTree(bool sendNotifications)
{
    // Could be still uninitialized at this moment
    if (this->vcs == nullptr)
    {
        this->initVCS();
        this->initEditor();
    }
}

void VersionControlNode::onItemDeletedFromTree(bool sendNotifications)
{
    this->shutdownEditor();
    this->shutdownVCS();
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

bool VersionControlNode::hasMenu() const noexcept
{
    return this->vcs != nullptr;
}

ScopedPointer<Component> VersionControlNode::createMenu()
{
    if (this->vcs != nullptr)
    {
        return new VersionControlMenu(*this->vcs);
    }
    
    return nullptr;
}

//===----------------------------------------------------------------------===//
// Network
//===----------------------------------------------------------------------===//

void VersionControlNode::cloneProject()
{
    auto *parentProject = this->findParentOfType<ProjectNode>();
    if (parentProject != nullptr && this->vcs != nullptr)
    {
        App::Network().getProjectSyncService()->
            cloneProject(this->vcs.get(), parentProject->getId());
    }
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree VersionControlNode::serialize() const
{
    ValueTree tree(Serialization::Core::treeNode);
    tree.setProperty(Serialization::Core::treeNodeType, this->type, nullptr);

    if (this->vcs != nullptr)
    {
        tree.appendChild(this->vcs->serialize(), nullptr);
    }

    TreeNodeSerializer::serializeChildren(*this, tree);
    return tree;
}

void VersionControlNode::deserialize(const ValueTree &tree)
{
    this->reset();

    if (this->vcs != nullptr)
    {
        forEachValueTreeChildWithType(tree, e, Serialization::Core::versionControl)
        {
            this->vcs->deserialize(e);
        }
    }

    // Proceed with basic properties and children
    TreeNode::deserialize(tree);
}

void VersionControlNode::reset()
{
    if (this->vcs != nullptr)
    {
        this->vcs->reset();
    }

    TreeNode::reset();
}


void VersionControlNode::initVCS()
{
    auto *parentProject = this->findParentOfType<ProjectNode>();
    if (parentProject != nullptr && this->vcs == nullptr)
    {
        this->vcs.reset(new VersionControl(*parentProject));
        this->vcs->addChangeListener(parentProject);
        parentProject->addChangeListener(this->vcs.get());
    }
}

void VersionControlNode::shutdownVCS()
{
    auto *parentProject = this->findParentOfType<ProjectNode>();
    if (parentProject != nullptr && this->vcs != nullptr)
    {
        parentProject->removeChangeListener(this->vcs.get());
        this->vcs->removeChangeListener(parentProject);
    }
}

void VersionControlNode::initEditor()
{
    this->shutdownEditor();
    
    auto *parentProject = this->findParentOfType<ProjectNode>();
    if (parentProject != nullptr && this->vcs != nullptr)
    {
        this->editor.reset(this->vcs->createEditor());
        this->vcs->addChangeListener(this->editor.get());
        parentProject->addChangeListener(this->editor.get());
    }
}

void VersionControlNode::shutdownEditor()
{
    auto *parentProject = this->findParentOfType<ProjectNode>();
    if (parentProject != nullptr && this->vcs != nullptr && this->editor != nullptr)
    {
        parentProject->removeChangeListener(this->editor.get());
        this->vcs->removeChangeListener(this->editor.get());
        this->editor = nullptr;
    }
}
