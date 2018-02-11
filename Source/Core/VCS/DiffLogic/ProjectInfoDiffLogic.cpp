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
#include "ProjectInfoDeltas.h"

using namespace VCS;

ProjectInfoDiffLogic::ProjectInfoDiffLogic(TrackedItem &targetItem) :
    DiffLogic(targetItem) {}

//===----------------------------------------------------------------------===//
// DiffLogic
//===----------------------------------------------------------------------===//

const juce::Identifier VCS::ProjectInfoDiffLogic::getType() const
{
    return Serialization::Core::projectInfo;
}

void ProjectInfoDiffLogic::resetStateTo(const TrackedItem &newState)
{
    this->target.resetStateTo(newState);
}

Diff *ProjectInfoDiffLogic::createDiff(const TrackedItem &initialState) const
{
    auto diff = new Diff(this->target);

    for (int i = 0; i < this->target.getNumDeltas(); ++i)
    {
        const Delta *myDelta = this->target.getDelta(i);

        const auto myDeltaData(this->target.serializeDeltaData(i));
        ValueTree stateDeltaData;

        bool deltaFoundInState = false;
        bool dataHasChanged = false;

        for (int j = 0; j < initialState.getNumDeltas(); ++j)
        {
            const Delta *stateDelta = initialState.getDelta(j);

            if (myDelta->getType() == stateDelta->getType())
            {
                deltaFoundInState = true;
                stateDeltaData = initialState.serializeDeltaData(j);
                dataHasChanged = (! myDeltaData.isEquivalentTo(stateDeltaData));
                break;
            }
        }

        if (!deltaFoundInState || (deltaFoundInState && dataHasChanged))
        {
            if (myDelta->hasType(ProjectInfoDeltas::projectLicense))
            {
                DeltaDiff fullDelta = this->createPathDiff(stateDeltaData, myDeltaData);
                diff->applyDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->hasType(ProjectInfoDeltas::projectFullName))
            {
                DeltaDiff fullDelta = this->createFullNameDiff(stateDeltaData, myDeltaData);
                diff->applyDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->hasType(ProjectInfoDeltas::projectAuthor))
            {
                DeltaDiff fullDelta = this->createAuthorDiff(stateDeltaData, myDeltaData);
                diff->applyDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->hasType(ProjectInfoDeltas::projectDescription))
            {
                DeltaDiff fullDelta = this->createDescriptionDiff(stateDeltaData, myDeltaData);
                diff->applyDelta(fullDelta.delta, fullDelta.deltaData);
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
        const auto stateDeltaData(initialState.serializeDeltaData(i));

        bool deltaFoundInChanges = false;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            const auto targetDeltaData(this->target.serializeDeltaData(j));

            const bool typesMatchStrictly =
                (stateDelta->getType() == targetDelta->getType());

            if (typesMatchStrictly)
            {
                deltaFoundInChanges = true;

                if (targetDelta->hasType(ProjectInfoDeltas::projectLicense))
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    ValueTree diffDeltaData = this->mergePath(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->hasType(ProjectInfoDeltas::projectFullName))
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    ValueTree diffDeltaData = this->mergeFullName(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->hasType(ProjectInfoDeltas::projectAuthor))
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    ValueTree diffDeltaData = this->mergeAuthor(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->hasType(ProjectInfoDeltas::projectDescription))
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    ValueTree diffDeltaData = this->mergeDescription(stateDeltaData, targetDeltaData);
                    diff->applyDelta(diffDelta, diffDeltaData);
                }
            }
        }

        // не нашли ни одного изменения? копируем оригинальную дельту.
        if (! deltaFoundInChanges)
        {
            auto stateDeltaCopy = new Delta(*stateDelta);
            diff->applyDelta(stateDeltaCopy, stateDeltaData);
        }
    }

    return diff;
}


//===----------------------------------------------------------------------===//
// Diffs
//===----------------------------------------------------------------------===//

ValueTree ProjectInfoDiffLogic::mergePath(const ValueTree &state, const ValueTree &changes) const
{
    return changes.createCopy();
}

ValueTree ProjectInfoDiffLogic::mergeFullName(const ValueTree &state, const ValueTree &changes) const
{
    return changes.createCopy();
}

ValueTree ProjectInfoDiffLogic::mergeAuthor(const ValueTree &state, const ValueTree &changes) const
{
    return changes.createCopy();
}

ValueTree ProjectInfoDiffLogic::mergeDescription(const ValueTree &state, const ValueTree &changes) const
{
    return changes.createCopy();
}

DeltaDiff ProjectInfoDiffLogic::createPathDiff(const ValueTree &state, const ValueTree &changes) const
{
    DeltaDiff res;
    res.delta = new Delta(DeltaDescription("license changed"), ProjectInfoDeltas::projectLicense);
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff ProjectInfoDiffLogic::createFullNameDiff(const ValueTree &state, const ValueTree &changes) const
{
    DeltaDiff res;
    res.delta = new Delta(DeltaDescription("title changed"), ProjectInfoDeltas::projectFullName);
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff ProjectInfoDiffLogic::createAuthorDiff(const ValueTree &state, const ValueTree &changes) const
{
    DeltaDiff res;
    res.delta = new Delta(DeltaDescription("author changed"), ProjectInfoDeltas::projectAuthor);
    res.deltaData = changes.createCopy();
    return res;
}

DeltaDiff ProjectInfoDiffLogic::createDescriptionDiff(const ValueTree &state, const ValueTree &changes) const
{
    DeltaDiff res;
    res.delta = new Delta(DeltaDescription("description changed"), ProjectInfoDeltas::projectDescription);
    res.deltaData = changes.createCopy();
    return res;
}
