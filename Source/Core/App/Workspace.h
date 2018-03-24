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

#pragma once

class AudioCore;
class ProjectTreeItem;
class RootTreeItem;
class PluginScanner;

#include "DocumentOwner.h"
#include "RecentFilesList.h"
#include "TreeNavigationHistory.h"

class Workspace : public RecentFilesList::Owner,
                  private ChangeListener, // listens to RecentFilesList
                  private Serializable
{
public:
    
    Workspace();
    ~Workspace() override;

    void init();
    bool isInitialized() const noexcept;

    WeakReference<TreeItem> getActiveTreeItem() const;
    TreeNavigationHistory &getNavigationHistory();
    void navigateBackwardIfPossible();
    void navigateForwardIfPossible();

    AudioCore &getAudioCore();
    PluginScanner &getPluginManager();
    RootTreeItem *getTreeRoot() const;
    void activateSubItemWithId(const String &id);

    //===------------------------------------------------------------------===//
    // RecentFilesListOwner
    //===------------------------------------------------------------------===//

    RecentFilesList &getRecentFilesList() const override;
    bool onClickedLoadRecentFile(RecentFileDescription::Ptr fileDescription) override;
    void onClickedUnloadRecentFile(RecentFileDescription::Ptr fileDescription) override;

    //===------------------------------------------------------------------===//
    // Project management
    //===------------------------------------------------------------------===//

    void createEmptyProject();
    void unloadProjectById(const String &id);
    Array<ProjectTreeItem *> getLoadedProjects() const;
    void stopPlaybackForAllProjects();

    //===------------------------------------------------------------------===//
    // Save/Load
    //===------------------------------------------------------------------===//

    bool autoload();
    void autosave();
    void importProject(const String &filePattern);

private:
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:
    
    void failedDeserializationFallback();
    void changeListenerCallback(ChangeBroadcaster *source) override;
    
private:
    
    // Needs to outlive the tree
    ScopedPointer<RecentFilesList> recentFilesList;
    
    bool wasInitialized;
    
    ProjectTreeItem *currentProject;
    
    ScopedPointer<AudioCore> audioCore;
    ScopedPointer<PluginScanner> pluginManager;
    
    ScopedPointer<RootTreeItem> treeRoot;
    TreeNavigationHistory navigationHistory;

    // A quick hack to have some kind of backwards compatibility.
    // If a previous version of tree is found, it is loaded as is
    // and then saved along with the new one.
    ScopedPointer<XmlElement> previousVersionTree;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Workspace)

};
