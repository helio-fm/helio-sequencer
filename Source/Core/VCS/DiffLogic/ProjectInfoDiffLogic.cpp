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
    DiffLogic(targetItem)
{

}

ProjectInfoDiffLogic::~ProjectInfoDiffLogic()
{

}


//===----------------------------------------------------------------------===//
// DiffLogic
//===----------------------------------------------------------------------===//

const String ProjectInfoDiffLogic::getType() const
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

        ScopedPointer<XmlElement> myDeltaData(this->target.createDeltaDataFor(i));
        ScopedPointer<XmlElement> stateDeltaData;

        bool deltaFoundInState = false;
        bool dataHasChanged = false;

        for (int j = 0; j < initialState.getNumDeltas(); ++j)
        {
            const Delta *stateDelta = initialState.getDelta(j);

            if (myDelta->getType() == stateDelta->getType())
            {
                deltaFoundInState = true;
                stateDeltaData = initialState.createDeltaDataFor(j);
                dataHasChanged = (! myDeltaData->isEquivalentTo(stateDeltaData, true));
                break;
            }
        }

        if (!deltaFoundInState || (deltaFoundInState && dataHasChanged))
        {
            if (myDelta->getType() == ProjectInfoDeltas::projectLicense)
            {
                NewSerializedDelta fullDelta = this->createPathDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == ProjectInfoDeltas::projectFullName)
            {
                NewSerializedDelta fullDelta = this->createFullNameDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == ProjectInfoDeltas::projectAuthor)
            {
                NewSerializedDelta fullDelta = this->createAuthorDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
            }
            else if (myDelta->getType() == ProjectInfoDeltas::projectDescription)
            {
                NewSerializedDelta fullDelta = this->createDescriptionDiff(stateDeltaData, myDeltaData);
                diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
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
        ScopedPointer<XmlElement> stateDeltaData(initialState.createDeltaDataFor(i));

        bool deltaFoundInChanges = false;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            ScopedPointer<XmlElement> targetDeltaData(this->target.createDeltaDataFor(j));

            const bool typesMatchStrictly =
                (stateDelta->getType() == targetDelta->getType());

            if (typesMatchStrictly)
            {
                deltaFoundInChanges = true;

                if (targetDelta->getType() == ProjectInfoDeltas::projectLicense)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = this->mergePath(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == ProjectInfoDeltas::projectFullName)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = this->mergeFullName(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == ProjectInfoDeltas::projectAuthor)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = this->mergeAuthor(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
                else if (targetDelta->getType() == ProjectInfoDeltas::projectDescription)
                {
                    Delta *diffDelta = new Delta(targetDelta->getDescription(), targetDelta->getType());
                    XmlElement *diffDeltaData = this->mergeDescription(stateDeltaData, targetDeltaData);
                    diff->addOwnedDelta(diffDelta, diffDeltaData);
                }
            }
        }

        // не нашли ни одного изменения? копируем оригинальную дельту.
        if (! deltaFoundInChanges)
        {
            auto stateDeltaCopy = new Delta(*stateDelta);
            diff->addOwnedDelta(stateDeltaCopy, stateDeltaData.release());
        }
    }

    return diff;
}


//===----------------------------------------------------------------------===//
// Diffs
//===----------------------------------------------------------------------===//

XmlElement *ProjectInfoDiffLogic::mergePath(const XmlElement *state, const XmlElement *changes) const
{
    return new XmlElement(*changes);
}

XmlElement *ProjectInfoDiffLogic::mergeFullName(const XmlElement *state, const XmlElement *changes) const
{
    return new XmlElement(*changes);
}

XmlElement *ProjectInfoDiffLogic::mergeAuthor(const XmlElement *state, const XmlElement *changes) const
{
    return new XmlElement(*changes);
}

XmlElement *ProjectInfoDiffLogic::mergeDescription(const XmlElement *state, const XmlElement *changes) const
{
    return new XmlElement(*changes);
}

NewSerializedDelta ProjectInfoDiffLogic::createPathDiff(const XmlElement *state, const XmlElement *changes) const
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("license changed"), ProjectInfoDeltas::projectLicense);
    res.deltaData = new XmlElement(*changes);
    return res;
}

NewSerializedDelta ProjectInfoDiffLogic::createFullNameDiff(const XmlElement *state, const XmlElement *changes) const
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("title changed"), ProjectInfoDeltas::projectFullName);
    res.deltaData = new XmlElement(*changes);
    return res;
}

NewSerializedDelta ProjectInfoDiffLogic::createAuthorDiff(const XmlElement *state, const XmlElement *changes) const
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("author changed"), ProjectInfoDeltas::projectAuthor);
    res.deltaData = new XmlElement(*changes);
    return res;
}

NewSerializedDelta ProjectInfoDiffLogic::createDescriptionDiff(const XmlElement *state, const XmlElement *changes) const
{
    NewSerializedDelta res;
    res.delta = new Delta(DeltaDescription("description changed"), ProjectInfoDeltas::projectDescription);
    res.deltaData = new XmlElement(*changes);
    return res;
}
