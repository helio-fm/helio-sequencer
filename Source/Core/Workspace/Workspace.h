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
class ProjectNode;
class RootNode;
class PluginScanner;

#include "DocumentOwner.h"
#include "UserProfile.h"
#include "NavigationHistory.h"

class Workspace final : private Serializable
{
public:
    
    Workspace() = default;
    ~Workspace() override;

    void init();
    void shutdown();
    bool isInitialized() const noexcept;
    void stopPlaybackForAllProjects(); // on app suspend / shutdown

    void activateTreeItem(const String &id);
    WeakReference<TreeNode> getActiveTreeItem() const;

    NavigationHistory &getNavigationHistory();
    void navigateBackwardIfPossible();
    void navigateForwardIfPossible();

    AudioCore &getAudioCore() noexcept;
    PluginScanner &getPluginManager() noexcept;
    UserProfile &getUserProfile() noexcept;
    RootNode *getTreeRoot() noexcept;

    //===------------------------------------------------------------------===//
    // Project management
    //===------------------------------------------------------------------===//

    void createEmptyProject();
    bool loadRecentProject(RecentProjectInfo::Ptr file);
    Array<ProjectNode *> getLoadedProjects() const;
    void unloadProject(const String &id, bool deleteLocally, bool deleteRemotely);

    //===------------------------------------------------------------------===//
    // Save/Load
    //===------------------------------------------------------------------===//

    bool autoload();
    void autosave();
    void importProject(const String &filePattern);

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//
    
    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:

    bool wasInitialized = false;

    UserProfile userProfile;
    
    UniquePointer<AudioCore> audioCore;
    UniquePointer<PluginScanner> pluginManager;
    
    UniquePointer<RootNode> treeRoot;
    NavigationHistory navigationHistory;

    void failedDeserializationFallback();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Workspace)
};
