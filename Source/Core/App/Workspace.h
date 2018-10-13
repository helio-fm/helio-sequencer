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
#include "ProjectsList.h"
#include "TreeNavigationHistory.h"

class Workspace final : public ProjectsList::Owner,
                        private Serializable
{
public:
    
    Workspace() = default;
    ~Workspace() override;

    void init();
    void shutdown();
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

    ProjectsList &getProjectsList() const override;
    bool onClickedLoadRecentFile(RecentFileDescription::Ptr file) override;
    void onClickedUnloadRecentFile(RecentFileDescription::Ptr file) override;

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
    
private:

    bool wasInitialized = false;

    UniquePointer<ProjectsList> recentProjectsList;
    
    UniquePointer<AudioCore> audioCore;
    UniquePointer<PluginScanner> pluginManager;
    
    UniquePointer<RootTreeItem> treeRoot;
    TreeNavigationHistory navigationHistory;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Workspace)
};
