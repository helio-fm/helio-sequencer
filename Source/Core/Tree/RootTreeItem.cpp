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
#include "RootTreeItem.h"

#include "TreeItemChildrenSerializer.h"
#include "ProjectTreeItem.h"
#include "VersionControlTreeItem.h"
#include "PatternEditorTreeItem.h"
#include "TrackGroupTreeItem.h"
#include "PianoTrackTreeItem.h"
#include "AutomationTrackTreeItem.h"

#include "Pattern.h"
#include "MidiTrack.h"
#include "MidiSequence.h"
#include "AutomationSequence.h"
#include "AutomationEvent.h"
#include "ProjectInfo.h"
#include "WorkspaceMenu.h"

#include "ResourceSyncService.h"
#include "MainLayout.h"
#include "Workspace.h"
#include "Icons.h"
#include "App.h"

RootTreeItem::RootTreeItem(const String &name) :
    TreeItem(name, Serialization::Core::root) {}

String RootTreeItem::getName() const noexcept
{
    // TODO: if user is logged in, show his name rather than default value?
    return TRANS("tree::root");
}

Image RootTreeItem::getIcon() const noexcept
{
    return Icons::findByName(Icons::helio, HEADLINE_ICON_SIZE);
}

void RootTreeItem::showPage()
{
    if (this->dashboard == nullptr)
    {
        this->recreatePage();
    }

    App::Layout().showPage(this->dashboard, this);
}

void RootTreeItem::recreatePage()
{
    this->dashboard = new Dashboard(App::Layout());
}

void RootTreeItem::importMidi(const File &file)
{
    auto *project = new ProjectTreeItem(file.getFileNameWithoutExtension());
    this->addChildTreeItem(project);
    this->addVCS(project);
    project->importMidi(file);
}

//===----------------------------------------------------------------------===//
// Children
//===----------------------------------------------------------------------===//

ProjectTreeItem *RootTreeItem::openProject(const File &file)
{
    const auto myProjects(this->findChildrenOfType<ProjectTreeItem>());

    // предварительная проверка на дубликаты - по полному пути
    for (auto *myProject : myProjects)
    {
        if (myProject->getDocument()->getFullPath() == file.getFullPathName())
        {
            return nullptr;
        }
    }

    Logger::writeToLog("Opening: " + file.getFullPathName());
    
    if (file.existsAsFile())
    {
        UniquePointer<ProjectTreeItem> project(new ProjectTreeItem(file));
        this->addChildTreeItem(project.get(), 1);

        if (!project->getDocument()->load(file.getFullPathName()))
        {
            return nullptr;
        }

        // вторая проверка на дубликаты - по id
        for (auto *myProject : myProjects)
        {
            if (myProject->getId() == project->getId())
            {
                return nullptr;
            }
        }

        App::Workspace().autosave();
        return project.release();
    }

    return nullptr;
}

ProjectTreeItem *RootTreeItem::checkoutProject(const String &id, const String &name)
{
    const auto myProjects(this->findChildrenOfType<ProjectTreeItem>());
    Logger::writeToLog("Cloning project: " + name);

    if (id.isNotEmpty())
    {
        UniquePointer<ProjectTreeItem> project(new ProjectTreeItem(name));
        this->addChildTreeItem(project.get(), 1);
        App::Helio().getResourceSyncService()->cloneProject(nullptr, id); // TODO project->getVersionControl()
        return project.release();
    }

    return nullptr;
}

ProjectTreeItem *RootTreeItem::addDefaultProject(const String &projectName)
{
    this->setOpen(true);
    auto *newProject = new ProjectTreeItem(projectName);
    this->addChildTreeItem(newProject);
    return this->createDefaultProjectChildren(newProject);
}

ProjectTreeItem *RootTreeItem::addDefaultProject(const File &projectLocation)
{
    this->setOpen(true);
    auto *newProject = new ProjectTreeItem(projectLocation);
    this->addChildTreeItem(newProject);
    return this->createDefaultProjectChildren(newProject);
}

ProjectTreeItem *RootTreeItem::createDefaultProjectChildren(ProjectTreeItem *newProject)
{
    this->addVCS(newProject);
    newProject->addChildTreeItem(new PatternEditorTreeItem());

    this->addPianoTrack(newProject, "Arps")->setTrackColour(Colours::orangered, true);
    this->addPianoTrack(newProject, "Counterpoint")->setTrackColour(Colours::gold, true);
    this->addPianoTrack(newProject, "Melodic")->setTrackColour(Colours::chartreuse, true);
    this->addAutoLayer(newProject, "Tempo", MidiTrack::tempoController)->setTrackColour(Colours::floralwhite, true);

    newProject->broadcastReloadProjectContent();
    const auto range = newProject->broadcastChangeProjectBeatRange();
    newProject->broadcastChangeViewBeatRange(range.getX(), range.getY());
    newProject->getDocument()->save();

    // notify recent files list
    App::Workspace().getUserProfile().onProjectLocalInfoUpdated(newProject->getId(),
        newProject->getName(), newProject->getDocument()->getFullPath());

    return newProject;
}

VersionControlTreeItem *RootTreeItem::addVCS(TreeItem *parent)
{
    auto vcs = new VersionControlTreeItem();
    parent->addChildTreeItem(vcs);

    // при создании рутовой ноды vcs, туда надо первым делом коммитить пустой ProjectInfo,
    // чтобы оной в списке изменений всегда показывался как измененный (не добавленный)
    // т.к. удалить его нельзя. и смущать юзера подобными надписями тоже не айс.
    vcs->commitProjectInfo();

    return vcs;
}

TrackGroupTreeItem *RootTreeItem::addGroup(TreeItem *parent, const String &name)
{
    auto *group = new TrackGroupTreeItem(name);
    parent->addChildTreeItem(group);
    return group;
}

MidiTrackTreeItem *RootTreeItem::addPianoTrack(TreeItem *parent, const String &name)
{
    auto *item = new PianoTrackTreeItem(name);
    const Clip clip(item->getPattern());
    item->getPattern()->insert(clip, false);
    parent->addChildTreeItem(item);
    return item;
}

MidiTrackTreeItem *RootTreeItem::addAutoLayer(TreeItem *parent, const String &name, int controllerNumber)
{
    auto *item = new AutomationTrackTreeItem(name);
    const Clip clip(item->getPattern());
    item->getPattern()->insert(clip, false);
    item->setTrackControllerNumber(controllerNumber, false);
    auto *itemLayer = static_cast<AutomationSequence *>(item->getSequence());
    parent->addChildTreeItem(item);
    itemLayer->insert(AutomationEvent(itemLayer, 0.f, 0.5f), false);
    itemLayer->insert(AutomationEvent(itemLayer, BEATS_PER_BAR, 0.5f), false);
    return item;
}

//===----------------------------------------------------------------------===//
// Menu
//===----------------------------------------------------------------------===//

bool RootTreeItem::hasMenu() const noexcept
{
    return true;
}

ScopedPointer<Component> RootTreeItem::createMenu()
{
    return new WorkspaceMenu(App::Workspace());
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

void RootTreeItem::deserialize(const ValueTree &tree)
{
    const auto root = tree.hasType(Serialization::Core::treeItem) ?
        tree : tree.getChildWithName(Serialization::Core::treeItem);

    if (root.isValid())
    {
        TreeItem::deserialize(root);
    }
}
