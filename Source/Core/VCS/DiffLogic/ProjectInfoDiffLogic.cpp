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
#include "SerializationKeys.h"
#include "Diff.h"
#include "ProjectInfo.h"

using namespace VCS;
using namespace Serialization::VCS;

static ValueTree mergeLicense(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeFullName(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeAuthor(const ValueTree &state, const ValueTree &changes);
static ValueTree mergeDescription(const ValueTree &state, const ValueTree &changes);

static DeltaDiff createLicenseDiff(const ValueTree &state, const ValueTree &changes);
static DeltaDiff createFullNameDiff(const ValueTree &state, const ValueTree &changes);
static DeltaDiff createAuthorDiff(const ValueTree &state, const ValueTree &changes);
static DeltaDiff createDescriptionDiff(const ValueTree &state, const ValueTree &changes);

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
    auto diff = new Diff(this->target);

    for (int i = 0; i < this->target.getNumDeltas(); ++i)
    {
        const Delta *myDelta = this->target.getDelta(i);

        const auto myDeltaData(this->target.getDeltaData(i));
        ValueTree stateDeltaData;

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

        if (!deltaFoundInState || dataHasChanged)
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
        }
    }

    return diff;
}

Diff *ProjectInfoDiffLogic::createMergedItem(const TrackedItem &initialState) const
{
    auto diff = new Diff(this->target);

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
                    ScopedPointer<Delta> diffDelta(new Delta(targetDelta->getDescription(), targetDelta->getType()));
                    ValueTree diffDeltaData = mergeLicense(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
                else if (targetDelta->hasType(ProjectInfoDeltas::projectTitle))
                {
                    ScopedPointer<Delta> diffDelta(new Delta(targetDelta->getDescription(), targetDelta->getType()));
                    ValueTree diffDeltaData = mergeFullName(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
                else if (targetDelta->hasType(ProjectInfoDeltas::projectAuthor))
                {
                    ScopedPointer<Delta> diffDelta(new Delta(targetDelta->getDescription(), targetDelta->getType()));
                    ValueTree diffDeltaData = mergeAuthor(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta.release(), diffDeltaData);
                }
                else if (targetDelta->hasType(ProjectInfoDeltas::projectDescription))
                {
                    ScopedPointer<Delta> diffDelta(new Delta(targetDelta->getDescription(), targetDelta->getType()));
                    ValueTree diffDeltaData = mergeDescription(stateDeltaData, targetDeltaData);
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

    return diff;
}


//===----------------------------------------------------------------------===//
// Diffs
//===----------------------------------------------------------------------===//

ValueTree mergeLicense(const ValueTree &state, const ValueTree &changes)
{
    return changes.createCopy();
}

ValueTree mergeFullName(const ValueTree &state, const ValueTree &changes)
{
    return changes.createCopy();
}

ValueTree mergeAuthor(const ValueTree &state, const ValueTree &changes)
{
    return changes.createCopy();
}

ValueTree mergeDescription(const ValueTree &state, const ValueTree &changes)
{
    return changes.createCopy();
}

DeltaDiff createLicenseDiff(const ValueTree &state, const ValueTree &changes)
{
    DeltaDiff res;
    res.delta = new Delta(DeltaDescription("license changed"), ProjectInfoDeltas::projectLicense);
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff createFullNameDiff(const ValueTree &state, const ValueTree &changes)
{
    DeltaDiff res;
    res.delta = new Delta(DeltaDescription("title changed"), ProjectInfoDeltas::projectTitle);
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff createAuthorDiff(const ValueTree &state, const ValueTree &changes)
{
    DeltaDiff res;
    res.delta = new Delta(DeltaDescription("author changed"), ProjectInfoDeltas::projectAuthor);
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff createDescriptionDiff(const ValueTree &state, const ValueTree &changes)
{
    DeltaDiff res;
    res.delta = new Delta(DeltaDescription("description changed"), ProjectInfoDeltas::projectDescription);
    res.deltaData = changes.createCopy();
    return res;
}
