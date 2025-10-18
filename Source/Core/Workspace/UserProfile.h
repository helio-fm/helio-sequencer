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

#include "Serializable.h"
#include "RecentProjectInfo.h"

class UserProfile final : public Serializable,
                          public ChangeBroadcaster
{
public:

    UserProfile() = default;

    void onProjectUnloaded(const String &id);
    void onProjectLocalInfoUpdated(const String &id, const String &title, const String &path);

    void deleteProjectLocally(const String &id);

    using ProjectsList = ReferenceCountedArray<RecentProjectInfo, CriticalSection>;
    const ProjectsList &getProjects() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    virtual SerializedData serialize() const override;
    virtual void deserialize(const SerializedData &data) override;
    virtual void reset() override;

private:

    RecentProjectInfo *findProject(const String &id) const;

    ProjectsList projects;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UserProfile)
};
