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
#include "RootNode.h"

#include "TreeNodeSerializer.h"
#include "ProjectNode.h"
#include "ProjectTimeline.h"
#include "VersionControlNode.h"
#include "PatternEditorNode.h"
#include "TrackGroupNode.h"
#include "PianoTrackNode.h"
#include "AutomationTrackNode.h"

#include "Pattern.h"
#include "MidiTrack.h"
#include "PianoSequence.h"
#include "AutomationSequence.h"
#include "KeySignaturesSequence.h"
#include "TimeSignaturesSequence.h"
#include "AutomationEvent.h"
#include "ProjectMetadata.h"
#include "WorkspaceMenu.h"
#include "JsonSerializer.h"

#include "MainLayout.h"
#include "Workspace.h"
#include "Icons.h"

RootNode::RootNode(const String &name) :
    TreeNode(name, Serialization::Core::root) {}

String RootNode::getName() const noexcept
{
    return TRANS(I18n::Tree::root);
}

Image RootNode::getIcon() const noexcept
{
    return Icons::findByName(Icons::helio, HEADLINE_ICON_SIZE);
}

void RootNode::showPage()
{
    if (this->dashboard == nullptr)
    {
        this->recreatePage();
    }

    App::Layout().showPage(this->dashboard.get(), this);
}

void RootNode::recreatePage()
{
    this->dashboard = makeUnique<Dashboard>(App::Layout());
}

//===----------------------------------------------------------------------===//
// Children
//===----------------------------------------------------------------------===//

ProjectNode *RootNode::openProject(const File &file)
{
    const auto myProjects(this->findChildrenOfType<ProjectNode>());

    // first check for duplicates (full path)
    for (auto *myProject : myProjects)
    {
        if (myProject->getDocument()->getFullPath() == file.getFullPathName())
        {
            myProject->selectChildOfType<PianoTrackNode>();
            return nullptr;
        }
    }

    DBG("Opening project: " + file.getFullPathName());
    if (file.existsAsFile())
    {
        UniquePointer<ProjectNode> project(new ProjectNode(file));
        this->addChildNode(project.get(), 1);

        if (!project->getDocument()->load(file.getFullPathName()))
        {
            return nullptr;
        }

        // second check for duplicates (project id)
        for (auto *myProject : myProjects)
        {
            if (myProject->getId() == project->getId())
            {
                myProject->selectChildOfType<PianoTrackNode>();
                return nullptr;
            }
        }

        App::Workspace().autosave();

        project->selectChildOfType<PianoTrackNode>();
        return project.release();
    }

    return nullptr;
}

ProjectNode *RootNode::checkoutProject(const String &id, const String &name)
{
    DBG("Cloning project: " + name);
    if (id.isNotEmpty())
    {
        // construct a stub project with no first revision and no tracks,
        // only the essential stuff it will need anyway:
        UniquePointer<ProjectNode> project(new ProjectNode(name, id));
        this->addChildNode(project.get(), 1);
        UniquePointer<VersionControlNode> vcs(new VersionControlNode());
        project->addChildNode(vcs.get());
        project->addChildNode(new PatternEditorNode());
        vcs->cloneProject();
        vcs.release();
        return project.release();
    }

    return nullptr;
}

// this one is for desktops
ProjectNode *RootNode::addDefaultProject(const File &projectLocation)
{
    auto *newProject = new ProjectNode(projectLocation);
    this->addChildNode(newProject);
    return this->createDefaultProjectChildren(newProject);
}

// this one is for mobiles, where we don't have file chooser dialog
ProjectNode *RootNode::addDefaultProject(const String &projectName)
{
    auto *newProject = new ProjectNode(projectName);
    this->addChildNode(newProject);
    return this->createDefaultProjectChildren(newProject);
}

static VersionControlNode *addVCS(TreeNode *parent)
{
    auto *vcs = new VersionControlNode();
    parent->addChildNode(vcs);

    // при создании рутовой ноды vcs, туда надо первым делом коммитить пустой ProjectInfo,
    // чтобы оной в списке изменений всегда показывался как измененный (не добавленный)
    // т.к. удалить его нельзя. и смущать юзера подобными надписями тоже не айс.
    vcs->commitProjectInfo();
    return vcs;
}

void RootNode::importMidi(const File &file)
{
    auto *project = new ProjectNode(file.getFileNameWithoutExtension());
    this->addChildNode(project);
    addVCS(project);
    project->importMidi(file);
}

ProjectNode *RootNode::createDefaultProjectChildren(ProjectNode *project)
{
    addVCS(project);
    project->addChildNode(new PatternEditorNode());

    static JsonSerializer js;
    static const auto exampleData =
        String(BinaryData::exampleTemplate_json, BinaryData::exampleTemplate_jsonSize);

    const auto exampleProject = js.loadFromString(exampleData);

    // only load the content, i.e. tracks and the timeline:
    TreeNodeSerializer::deserializeChildren(*project, exampleProject);

    forEachChildWithType(exampleProject, node, Serialization::Core::projectTimeline)
    {
        project->getTimeline()->deserialize(node);
    }

    project->broadcastReloadProjectContent();
    const auto range = project->broadcastChangeProjectBeatRange();
    project->broadcastChangeViewBeatRange(range.getX(), range.getY());
    project->getDocument()->save();

    return project;
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

bool RootNode::hasMenu() const noexcept
{
    return true;
}

UniquePointer<Component> RootNode::createMenu()
{
    return makeUnique<WorkspaceMenu>(App::Workspace());
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

void RootNode::deserialize(const SerializedData &data)
{
    const auto root = data.hasType(Serialization::Core::treeNode) ?
        data : data.getChildWithName(Serialization::Core::treeNode);

    if (root.isValid())
    {
        TreeNode::deserialize(root);
    }
}
