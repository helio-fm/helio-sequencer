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
#include "ProjectInfo.h"
#include "WorkspaceMenu.h"

#include "MainLayout.h"
#include "Workspace.h"
#include "Icons.h"

RootNode::RootNode(const String &name) :
    TreeNode(name, Serialization::Core::root) {}

String RootNode::getName() const noexcept
{
    // TODO: if user is logged in, show his name rather than default value?
    return TRANS("tree::root");
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

    App::Layout().showPage(this->dashboard, this);
}

void RootNode::recreatePage()
{
    this->dashboard = new Dashboard(App::Layout());
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
        this->addChildTreeItem(project.get(), 1);

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
        this->addChildTreeItem(project.get(), 1);
        UniquePointer<VersionControlNode> vcs(new VersionControlNode());
        project->addChildTreeItem(vcs.get());
        project->addChildTreeItem(new PatternEditorNode());
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
    this->addChildTreeItem(newProject);
    return this->createDefaultProjectChildren(newProject);
}

// this one is for mobiles, where we don't have file chooser dialog
ProjectNode *RootNode::addDefaultProject(const String &projectName)
{
    auto *newProject = new ProjectNode(projectName);
    this->addChildTreeItem(newProject);
    return this->createDefaultProjectChildren(newProject);
}

static VersionControlNode *addVCS(TreeNode *parent)
{
    auto vcs = new VersionControlNode();
    parent->addChildTreeItem(vcs);

    // при создании рутовой ноды vcs, туда надо первым делом коммитить пустой ProjectInfo,
    // чтобы оной в списке изменений всегда показывался как измененный (не добавленный)
    // т.к. удалить его нельзя. и смущать юзера подобными надписями тоже не айс.
    vcs->commitProjectInfo();
    return vcs;
}

static PianoTrackNode *addPianoTrack(TreeNode *parent, const String &name)
{
    auto *item = new PianoTrackNode(name);
    const Clip clip(item->getPattern());
    item->getPattern()->insert(clip, false);
    parent->addChildTreeItem(item);
    return item;
}

static MidiTrackNode *addAutoLayer(TreeNode *parent, const String &name, int controllerNumber)
{
    auto *item = new AutomationTrackNode(name);
    const Clip clip(item->getPattern());
    item->getPattern()->insert(clip, false);
    item->setTrackControllerNumber(controllerNumber, false);
    auto *itemLayer = static_cast<AutomationSequence *>(item->getSequence());
    parent->addChildTreeItem(item);
    itemLayer->insert(AutomationEvent(itemLayer, 0.f, 0.5f), false);
    itemLayer->insert(AutomationEvent(itemLayer, BEATS_PER_BAR * 4, 0.5f), false);
    return item;
}

void RootNode::importMidi(const File &file)
{
    auto *project = new ProjectNode(file.getFileNameWithoutExtension());
    this->addChildTreeItem(project);
    addVCS(project);
    project->importMidi(file);
}

// someday this all should be reworked into xml/json template based generator:
ProjectNode *RootNode::createDefaultProjectChildren(ProjectNode *project)
{
    addVCS(project);
    project->addChildTreeItem(new PatternEditorNode());

    {
        auto *t1 = addPianoTrack(project, "Melodic");
        auto *s1 = static_cast<PianoSequence *>(t1->getSequence());
        // the lick reigns supreme:
        s1->insert(Note(s1, MIDDLE_C, 0.f, 1.f, 0.5f), false);
        s1->insert(Note(s1, MIDDLE_C + 2, 1.f, 1.f, 0.5f), false);
        s1->insert(Note(s1, MIDDLE_C + 3, 2.f, 1.f, 0.5f), false);
        s1->insert(Note(s1, MIDDLE_C + 5, 3.f, 1.f, 0.5f), false);
        s1->insert(Note(s1, MIDDLE_C + 2, 4.f, 2.f, 0.5f), false);
        s1->insert(Note(s1, MIDDLE_C - 2, 6.f, 1.f, 0.5f), false);
        s1->insert(Note(s1, MIDDLE_C, 7.f, 9.f, 0.5f), false);
        t1->setTrackColour(Colours::orangered, true);
    }

    {
        auto *t2 = addPianoTrack(project, "Arps");
        auto *s2 = static_cast<PianoSequence *>(t2->getSequence());
        s2->insert(Note(s2, MIDDLE_C - 12, 0.f, 16.f, 0.25f), false);
        t2->setTrackColour(Colours::royalblue, true);
    }

    {
        auto *t3 = addPianoTrack(project, "Counterpoint");
        auto *s3 = static_cast<PianoSequence *>(t3->getSequence());
        s3->insert(Note(s3, MIDDLE_C - 24, 0.f, 16.f, 0.25f), false);
        t3->setTrackColour(Colours::gold, true);
    }

    addAutoLayer(project, "Tempo", MidiTrack::tempoController)->setTrackColour(Colours::floralwhite, true);

    auto *ks = static_cast<KeySignaturesSequence *>(project->getTimeline()->getKeySignatures()->getSequence());
    ks->insert(KeySignatureEvent(ks, Scale::getNaturalMinorScale(), 0.f, 0), false);

    auto *ts = static_cast<TimeSignaturesSequence *>(project->getTimeline()->getTimeSignatures()->getSequence());
    ts->insert(TimeSignatureEvent(ts, 0.f, 4, 4), false);

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

ScopedPointer<Component> RootNode::createMenu()
{
    return new WorkspaceMenu(App::Workspace());
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

void RootNode::deserialize(const ValueTree &tree)
{
    const auto root = tree.hasType(Serialization::Core::treeNode) ?
        tree : tree.getChildWithName(Serialization::Core::treeNode);

    if (root.isValid())
    {
        TreeNode::deserialize(root);
    }
}
