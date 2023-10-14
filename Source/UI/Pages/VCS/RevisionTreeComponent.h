/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class VersionControl;
class RevisionComponent;
class HistoryComponent;

#include "Revision.h"

class RevisionTreeComponent final : public Component
{
public:

    explicit RevisionTreeComponent(VersionControl &owner);
    ~RevisionTreeComponent() override;

    void deselectAll(bool sendNotification = true);
    void selectComponent(RevisionComponent *revComponent, bool deselectOthers, bool sendNotification = true);

    VCS::Revision::Ptr getSelectedRevision() const noexcept;

    void handleCommandMessage(int commandId) override;

private:

    // Tree layout methods:

    RevisionComponent *initComponents(int depth,
        const VCS::Revision::Ptr revision, RevisionComponent *parentRevisionComponent);

    RevisionComponent *firstWalk(RevisionComponent *v, float distance = 1.f);

    RevisionComponent *apportion(RevisionComponent *v,
        RevisionComponent *default_ancestor, float distance);

    void moveSubtree(RevisionComponent *wl, RevisionComponent *wr, float shift);
    void executeShifts(RevisionComponent *v);

    RevisionComponent *ancestor(RevisionComponent *vil,
        RevisionComponent *v, RevisionComponent *default_ancestor);

    float secondWalk(RevisionComponent *v, float &min, float m = 0.f, float depth = 0.f);
    void thirdWalk(RevisionComponent *v, float n);
    void postWalk(RevisionComponent *v);

    float treeDepth = 0.f;

private:

    static constexpr auto connectorHeight = 18;

    HistoryComponent *findParentEditor() const;

    VersionControl &vcs;
    VCS::Revision::Ptr selectedRevision;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RevisionTreeComponent)

};
