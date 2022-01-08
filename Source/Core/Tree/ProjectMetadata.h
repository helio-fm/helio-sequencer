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

#include "Temperament.h"
#include "ProjectInfoDiffLogic.h"
#include "TrackedItem.h"
#include "Delta.h"

class ProjectMetadata final :
    public Serializable,
    public VCS::TrackedItem
{
public:

    explicit ProjectMetadata(ProjectNode &parent);

    int64 getStartTimestamp() const noexcept;

    String getLicense() const noexcept;
    void setLicense(String val);

    String getFullName() const noexcept;
    void setFullName(String val);

    String getAuthor() const noexcept;
    void setAuthor(String val);

    String getDescription() const noexcept;
    void setDescription(String val);

    Temperament::Ptr getTemperament() const noexcept;
    void setTemperament(const Temperament &temperament);
    int getKeyboardSize() const noexcept;
    int getPeriodSize() const noexcept;
    double getPeriodRange() const noexcept;

    //===------------------------------------------------------------------===//
    // VCS::TrackedItem
    //===------------------------------------------------------------------===//

    String getVCSName() const override;
    int getNumDeltas() const override;
    VCS::Delta *getDelta(int index) const override;
    SerializedData getDeltaData(int deltaIndex) const override;
    bool deltaHasDefaultData(int deltaIndex) const override;

    VCS::DiffLogic *getDiffLogic() const override;
    void resetStateTo(const VCS::TrackedItem &newState) override;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Deltas
    //===------------------------------------------------------------------===//

    SerializedData serializeLicenseDelta() const;
    SerializedData serializeFullNameDelta() const;
    SerializedData serializeAuthorDelta() const;
    SerializedData serializeDescriptionDelta() const;
    SerializedData serializeTemperamentDelta() const;

    void resetLicenseDelta(const SerializedData &state);
    void resetFullNameDelta(const SerializedData &state);
    void resetAuthorDelta(const SerializedData &state);
    void resetDescriptionDelta(const SerializedData &state);
    void resetTemperamentDelta(const SerializedData &state);

private:

    UniquePointer<VCS::ProjectInfoDiffLogic> vcsDiffLogic;
    OwnedArray<VCS::Delta> deltas;

private:

    ProjectNode &project;

    String author;
    String description;
    String license;
    int64 initTimestamp;

    Temperament::Ptr temperament;
    void deserializeTemperament(const SerializedData &state);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProjectMetadata);
};
