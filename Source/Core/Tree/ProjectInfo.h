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

class ProjectNode;

#include "ProjectInfoDiffLogic.h"
#include "TrackedItem.h"
#include "Delta.h"

class ProjectInfo :
    public Serializable,
    public VCS::TrackedItem
{
public:

    explicit ProjectInfo(ProjectNode &parent);

    int64 getStartTimestamp() const;

    String getLicense() const;
    void setLicense(String val);

    String getFullName() const;
    void setFullName(String val);

    String getAuthor() const;
    void setAuthor(String val);

    String getDescription() const;
    void setDescription(String val);

    //===------------------------------------------------------------------===//
    // VCS::TrackedItem
    //===------------------------------------------------------------------===//

    String getVCSName() const override;
    int getNumDeltas() const override;
    VCS::Delta *getDelta(int index) const override;
    ValueTree getDeltaData(int deltaIndex) const override;
    VCS::DiffLogic *getDiffLogic() const override;
    void resetStateTo(const VCS::TrackedItem &newState) override;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Deltas
    //===------------------------------------------------------------------===//

    ValueTree serializeLicenseDelta() const;
    ValueTree serializeFullNameDelta() const;
    ValueTree serializeAuthorDelta() const;
    ValueTree serializeDescriptionDelta() const;

    void resetLicenseDelta(const ValueTree &state);
    void resetFullNameDelta(const ValueTree &state);
    void resetAuthorDelta(const ValueTree &state);
    void resetDescriptionDelta(const ValueTree &state);

private:

    ScopedPointer<VCS::ProjectInfoDiffLogic> vcsDiffLogic;
    OwnedArray<VCS::Delta> deltas;

private:

    ProjectNode &project;

    String author;
    String description;
    String license;
    int64 initTimestamp;

    // TODO! ability to set up middle c
    // and temperament in general
    // int32 getMiddleC() const noexcept;
    // Temperament temperament;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectInfo);
};
