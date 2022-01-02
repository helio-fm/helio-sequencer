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
#include "ProjectInfoDiffLogic.h"
#include "Diff.h"

namespace VCS
{

static SerializedData mergeLicense(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeFullName(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeAuthor(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeDescription(const SerializedData &state, const SerializedData &changes);
static SerializedData mergeTemperament(const SerializedData &state, const SerializedData &changes);

static DeltaDiff createLicenseDiff(const SerializedData &state, const SerializedData &changes);
static DeltaDiff createFullNameDiff(const SerializedData &state, const SerializedData &changes);
static DeltaDiff createAuthorDiff(const SerializedData &state, const SerializedData &changes);
static DeltaDiff createDescriptionDiff(const SerializedData &state, const SerializedData &changes);
static DeltaDiff createTemperamentDiff(const SerializedData &state, const SerializedData &changes);

ProjectInfoDiffLogic::ProjectInfoDiffLogic(TrackedItem &targetItem) :
    DiffLogic(targetItem) {}

//===----------------------------------------------------------------------===//
// DiffLogic
//===----------------------------------------------------------------------===//

const Identifier ProjectInfoDiffLogic::getType() const
{
    return Serialization::Core::projectInfo;
}

Diff *ProjectInfoDiffLogic::createDiff(const TrackedItem &initialState) const
{
    using namespace Serialization::VCS;

    auto *diff = new Diff(this->target);

    for (int i = 0; i < this->target.getNumDeltas(); ++i)
    {
        const Delta *myDelta = this->target.getDelta(i);
        const auto myDeltaData(this->target.getDeltaData(i));
        const bool deltaIsDefault = this->target.deltaHasDefaultData(i);

        SerializedData stateDeltaData;

        bool deltaFoundInState = false;
        bool dataHasChanged = false;

        for (int j = 0; j < initialState.getNumDeltas(); ++j)
        {
            const Delta *stateDelta = initialState.getDelta(j);

            if (myDelta->hasType(stateDelta->getType()))
            {
                deltaFoundInState = true;
                stateDeltaData = initialState.getDeltaData(j);
                dataHasChanged = (! myDeltaData.isEquivalentTo(stateDeltaData));
                break;
            }
        }

        if ((!deltaFoundInState && !deltaIsDefault) || (deltaFoundInState && dataHasChanged))
        {
            if (myDelta->hasType(ProjectInfoDeltas::projectLicense))
            {
                diff->applyDelta(createLicenseDiff(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(ProjectInfoDeltas::projectTitle))
            {
                diff->applyDelta(createFullNameDiff(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(ProjectInfoDeltas::projectAuthor))
            {
                diff->applyDelta(createAuthorDiff(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(ProjectInfoDeltas::projectDescription))
            {
                diff->applyDelta(createDescriptionDiff(stateDeltaData, myDeltaData));
            }
            else if (myDelta->hasType(ProjectInfoDeltas::projectTemperament))
            {
                diff->applyDelta(createTemperamentDiff(stateDeltaData, myDeltaData));
            }
        }
    }

    return diff;
}

Diff *ProjectInfoDiffLogic::createMergedItem(const TrackedItem &initialState) const
{
    using namespace Serialization::VCS;

    auto *diff = new Diff(this->target);

    // merge-политика по умолчанию:
    // на каждую дельту таргета пытаемся наложить все дельты изменений.
    // (если типы дельт соответствуют друг другу)

    for (int i = 0; i < initialState.getNumDeltas(); ++i)
    {
        const Delta *stateDelta = initialState.getDelta(i);
        const auto stateDeltaData(initialState.getDeltaData(i));

        bool deltaFoundInChanges = false;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            const auto targetDeltaData(this->target.getDeltaData(j));

            if (stateDelta->hasType(targetDelta->getType()))
            {
                deltaFoundInChanges = true;

                if (targetDelta->hasType(ProjectInfoDeltas::projectLicense))
                {
                    auto diffDelta = make<Delta>(targetDelta->getDescription(), targetDelta->getType());
                    auto diffDeltaData = mergeLicense(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
                else if (targetDelta->hasType(ProjectInfoDeltas::projectTitle))
                {
                    auto diffDelta = make<Delta>(targetDelta->getDescription(), targetDelta->getType());
                    auto diffDeltaData = mergeFullName(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
                else if (targetDelta->hasType(ProjectInfoDeltas::projectAuthor))
                {
                    auto diffDelta = make<Delta>(targetDelta->getDescription(), targetDelta->getType());
                    auto diffDeltaData = mergeAuthor(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
                else if (targetDelta->hasType(ProjectInfoDeltas::projectDescription))
                {
                    auto diffDelta = make<Delta>(targetDelta->getDescription(), targetDelta->getType());
                    auto diffDeltaData = mergeDescription(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
                else if (targetDelta->hasType(ProjectInfoDeltas::projectTemperament))
                {
                    auto diffDelta = make<Delta>(targetDelta->getDescription(), targetDelta->getType());
                    auto diffDeltaData = mergeTemperament(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
            }
        }

        // не нашли ни одного изменения? копируем оригинальную дельту.
        if (! deltaFoundInChanges)
        {
            diff->applyDelta(stateDelta->createCopy(), stateDeltaData);
        }
    }

    // step 2:
    // resolve new delta types that may be missing in project history state

    bool stateHasTemperaments = false;

    for (int i = 0; i < initialState.getNumDeltas(); ++i)
    {
        const Delta *stateDelta = initialState.getDelta(i);
        stateHasTemperaments = stateHasTemperaments || stateDelta->hasType(ProjectInfoDeltas::projectTemperament);
    }

    {
        auto temperamentsDelta = make<Delta>(
            DeltaDescription(Serialization::VCS::headStateDelta),
            ProjectInfoDeltas::projectTemperament);

        SerializedData temperamentsDeltaData;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            const auto targetDeltaData(this->target.getDeltaData(j));

            const bool foundMissingTemperament = !stateHasTemperaments &&
				targetDelta->hasType(ProjectInfoDeltas::projectTemperament);

            if (foundMissingTemperament)
            {
                SerializedData emptyTemperamentDeltaData(ProjectInfoDeltas::projectTemperament);
                temperamentsDeltaData = mergeTemperament(emptyTemperamentDeltaData, targetDeltaData);
            }
        }

        if (temperamentsDeltaData.isValid())
        {
            diff->applyDelta(temperamentsDelta.release(), temperamentsDeltaData);
        }
    }

    return diff;
}

//===----------------------------------------------------------------------===//
// Diffs
//===----------------------------------------------------------------------===//

SerializedData mergeLicense(const SerializedData &state, const SerializedData &changes)
{
    return changes.createCopy();
}

SerializedData mergeFullName(const SerializedData &state, const SerializedData &changes)
{
    return changes.createCopy();
}

SerializedData mergeAuthor(const SerializedData &state, const SerializedData &changes)
{
    return changes.createCopy();
}

SerializedData mergeDescription(const SerializedData &state, const SerializedData &changes)
{
    return changes.createCopy();
}

SerializedData mergeTemperament(const SerializedData &state, const SerializedData &changes)
{
    return changes.createCopy();
}

DeltaDiff createLicenseDiff(const SerializedData &state, const SerializedData &changes)
{
    DeltaDiff res;
    using namespace Serialization::VCS;
    res.delta = make<Delta>(DeltaDescription("license changed"), ProjectInfoDeltas::projectLicense);
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff createFullNameDiff(const SerializedData &state, const SerializedData &changes)
{
    DeltaDiff res;
    using namespace Serialization::VCS;
    res.delta = make<Delta>(DeltaDescription("title changed"), ProjectInfoDeltas::projectTitle);
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff createAuthorDiff(const SerializedData &state, const SerializedData &changes)
{
    DeltaDiff res;
    using namespace Serialization::VCS;
    res.delta = make<Delta>(DeltaDescription("author changed"), ProjectInfoDeltas::projectAuthor);
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff createDescriptionDiff(const SerializedData &state, const SerializedData &changes)
{
    DeltaDiff res;
    using namespace Serialization::VCS;
    res.delta = make<Delta>(DeltaDescription("description changed"), ProjectInfoDeltas::projectDescription);
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff createTemperamentDiff(const SerializedData &state, const SerializedData &changes)
{
    DeltaDiff res;
    using namespace Serialization::VCS;
    res.delta = make<Delta>(DeltaDescription("temperament changed"), ProjectInfoDeltas::projectTemperament);
    res.deltaData = changes.createCopy();
    return res;
}

}
