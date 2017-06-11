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
#include "PatternDiffLogic.h"
#include "PatternDeltas.h"

#include "Clip.h"
#include "Pattern.h"
#include "ProjectEventDispatcher.h"
#include "SerializationKeys.h"

using namespace VCS;

PatternDiffLogic::PatternDiffLogic(TrackedItem &targetItem) :
    DiffLogic(targetItem)
{
}

PatternDiffLogic::~PatternDiffLogic()
{
}

const String PatternDiffLogic::getType() const
{
    return Serialization::Core::pattern;
}

void PatternDiffLogic::resetStateTo(const TrackedItem &newState)
{
    this->target.resetStateTo(newState);
}

Diff *PatternDiffLogic::createDiff(const TrackedItem &initialState) const
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
                dataHasChanged = (!myDeltaData->isEquivalentTo(stateDeltaData, true));
                break;
            }
        }

        if (!deltaFoundInState || (deltaFoundInState && dataHasChanged))
        {
            if (myDelta->getType() == PatternDeltas::clipsAdded)
            {
                Array<NewSerializedDelta> fullDeltas =
                    this->createEventsDiffs(stateDeltaData, myDeltaData);

                for (auto fullDelta : fullDeltas)
                {
                    diff->addOwnedDelta(fullDelta.delta, fullDelta.deltaData);
                }
            }
        }
    }

    return diff;
}

Diff *PatternDiffLogic::createMergedItem(const TrackedItem &initialState) const
{
    auto diff = new Diff(this->target);

    for (int i = 0; i < initialState.getNumDeltas(); ++i)
    {
        const Delta *stateDelta = initialState.getDelta(i);
        ScopedPointer<XmlElement> stateDeltaData(initialState.createDeltaDataFor(i));

        bool deltaFoundInChanges = false;

        ScopedPointer<Delta> clipsDelta(new Delta(
            DeltaDescription(Serialization::VCS::headStateDelta),
            PatternDeltas::clipsAdded));

        ScopedPointer<XmlElement> clipsDeltaData;

        for (int j = 0; j < this->target.getNumDeltas(); ++j)
        {
            const Delta *targetDelta = this->target.getDelta(j);
            ScopedPointer<XmlElement> targetDeltaData(this->target.createDeltaDataFor(j));

            const bool typesMatchStrictly =
                (stateDelta->getType() == targetDelta->getType());

            const bool bothDeltasAreClips =
                this->checkIfDeltaIsPatternType(stateDelta) && 
                this->checkIfDeltaIsPatternType(targetDelta);

            if (bothDeltasAreClips)
            {
                deltaFoundInChanges = true;

                if (targetDelta->getType() == PatternDeltas::clipsAdded)
                {
                    if (clipsDeltaData != nullptr)
                    {
                        clipsDeltaData = this->mergeClipsAdded(clipsDeltaData, targetDeltaData);
                    }
                    else
                    {
                        clipsDeltaData = this->mergeClipsAdded(stateDeltaData, targetDeltaData);
                    }
                }
                else if (targetDelta->getType() == PatternDeltas::clipsRemoved)
                {
                    if (clipsDeltaData != nullptr)
                    {
                        clipsDeltaData = this->mergeClipsRemoved(clipsDeltaData, targetDeltaData);
                    }
                    else
                    {
                        clipsDeltaData = this->mergeClipsRemoved(stateDeltaData, targetDeltaData);
                    }
                }
                else if (targetDelta->getType() == PatternDeltas::clipsChanged)
                {
                    if (clipsDeltaData != nullptr)
                    {
                        clipsDeltaData = this->mergeClipsChanged(clipsDeltaData, targetDeltaData);
                    }
                    else
                    {
                        clipsDeltaData = this->mergeClipsChanged(stateDeltaData, targetDeltaData);
                    }
                }
            }
        }

        if (clipsDeltaData != nullptr)
        {
            diff->addOwnedDelta(clipsDelta.release(), clipsDeltaData.release());
        }

        if (!deltaFoundInChanges)
        {
            auto stateDeltaCopy = new Delta(*stateDelta);
            diff->addOwnedDelta(stateDeltaCopy, stateDeltaData.release());
        }
    }

    return diff;
}


//===----------------------------------------------------------------------===//
// Merge
//===----------------------------------------------------------------------===//

XmlElement *PatternDiffLogic::mergeClipsAdded(const XmlElement *state,
    const XmlElement *changes) const
{
    EmptyEventDispatcher dispatcher;
    Pattern emptyPattern(dispatcher);
    Array<Clip> stateClips;
    Array<Clip> changesClips;
    this->deserializeChanges(emptyPattern, state, changes, stateClips, changesClips);

    Array<Clip> result;
    result.addArray(stateClips);

    // на всякий пожарный, ищем, нет ли в состоянии нот с теми же id, где нет - добавляем
    HashMap<Clip::Id, int> stateIDs;

    for (int j = 0; j < stateClips.size(); ++j)
    {
        stateIDs.set(stateClips.getUnchecked(j).getId(), j);
    }

    for (int i = 0; i < changesClips.size(); ++i)
    {
        const Clip changesClip(changesClips.getUnchecked(i));
        bool foundInState = stateIDs.contains(changesClip.getId());

        if (!foundInState)
        {
            result.add(changesClip);
        }
    }

    return this->serializePattern(result, PatternDeltas::clipsAdded);
}

XmlElement *PatternDiffLogic::mergeClipsRemoved(const XmlElement *state,
    const XmlElement *changes) const
{
    EmptyEventDispatcher dispatcher;
    Pattern emptyPattern(dispatcher);
    Array<Clip> stateClips;
    Array<Clip> changesClips;
    this->deserializeChanges(emptyPattern, state, changes, stateClips, changesClips);

    Array<Clip> result;
    HashMap<Clip::Id, int> changesIDs;

    for (int j = 0; j < changesClips.size(); ++j)
    {
        changesIDs.set(changesClips.getUnchecked(j).getId(), j);
    }

    for (int i = 0; i < stateClips.size(); ++i)
    {
        const Clip stateClip(stateClips.getUnchecked(i));
        bool foundInChanges = changesIDs.contains(stateClip.getId());

        if (!foundInChanges)
        {
            result.add(stateClip);
        }
    }

    return this->serializePattern(result, PatternDeltas::clipsAdded);
}

XmlElement *PatternDiffLogic::mergeClipsChanged(const XmlElement *state,
    const XmlElement *changes) const
{
    EmptyEventDispatcher dispatcher;
    Pattern emptyPattern(dispatcher);
    Array<Clip> stateClips;
    Array<Clip> changesClips;
    this->deserializeChanges(emptyPattern, state, changes, stateClips, changesClips);

    Array<Clip> result;
    result.addArray(stateClips);

    // снова ищем по id и заменяем
    HashMap<Clip::Id, Clip> changesIDs;

    for (int j = 0; j < changesClips.size(); ++j)
    {
        const Clip changesClip(changesClips.getUnchecked(j));
        changesIDs.set(changesClip.getId(), changesClip);
    }

    for (int i = 0; i < stateClips.size(); ++i)
    {
        const Clip stateClip(stateClips.getUnchecked(i));

        if (changesIDs.contains(stateClip.getId()))
        {
            const Clip changesClip = changesIDs[stateClip.getId()];
            result.removeAllInstancesOf(stateClip);
            result.addIfNotAlreadyThere(changesClip);
        }
    }

    return this->serializePattern(result, PatternDeltas::clipsAdded);
}


//===----------------------------------------------------------------------===//
// Diff
//===----------------------------------------------------------------------===//

Array<NewSerializedDelta> PatternDiffLogic::createEventsDiffs(const XmlElement *state, const XmlElement *changes) const
{
    EmptyEventDispatcher dispatcher;
    Pattern emptyPattern(dispatcher);
    Array<Clip> stateClips;
    Array<Clip> changesClips;

    this->deserializeChanges(emptyPattern, state, changes, stateClips, changesClips);

    Array<NewSerializedDelta> res;

    Array<Clip> addedClips;
    Array<Clip> removedClips;
    Array<Clip> changedClips;

    for (int i = 0; i < stateClips.size(); ++i)
    {
        bool foundInChanges = false;
        const Clip stateClip(stateClips.getUnchecked(i));

        for (int j = 0; j < changesClips.size(); ++j)
        {
            const Clip changesClip(changesClips.getUnchecked(j));

            if (stateClip.getId() == changesClip.getId())
            {
                foundInChanges = true;
                if (stateClip.getStartBeat() != changesClip.getStartBeat())
                {
                    changedClips.add(changesClip);
                }

                break;
            }
        }

        if (!foundInChanges)
        {
            removedClips.add(stateClip);
        }
    }

    for (int i = 0; i < changesClips.size(); ++i)
    {
        bool foundInState = false;
        const Clip changesClip(changesClips.getUnchecked(i));

        for (int j = 0; j < stateClips.size(); ++j)
        {
            const Clip stateClip(stateClips.getUnchecked(j));
            if (stateClip.getId() == changesClip.getId())
            {
                foundInState = true;
                break;
            }
        }

        if (!foundInState)
        {
            addedClips.add(changesClip);
        }
    }

    if (addedClips.size() > 0)
    {
        res.add(this->serializeChanges(addedClips,
            "added {x} clips",
            addedClips.size(),
            PatternDeltas::clipsAdded));
    }

    if (removedClips.size() > 0)
    {
        res.add(this->serializeChanges(removedClips,
            "removed {x} clips",
            removedClips.size(),
            PatternDeltas::clipsRemoved));
    }

    if (changedClips.size() > 0)
    {
        res.add(this->serializeChanges(changedClips,
            "changed {x} clips",
            changedClips.size(),
            PatternDeltas::clipsChanged));
    }

    return res;
}


void PatternDiffLogic::deserializeChanges(Pattern &pattern,
    const XmlElement *state,
    const XmlElement *changes,
    Array<Clip> &stateClips,
    Array<Clip> &changesClips) const
{
    if (state != nullptr)
    {
        forEachXmlChildElementWithTagName(*state, e, Serialization::Core::clip)
        {
            Clip clip;
            clip.deserialize(*e);
            stateClips.addSorted(clip, clip);
        }
    }

    if (changes != nullptr)
    {
        forEachXmlChildElementWithTagName(*changes, e, Serialization::Core::clip)
        {
            Clip clip;
            clip.deserialize(*e);
            changesClips.addSorted(clip, clip);
        }
    }
}

NewSerializedDelta PatternDiffLogic::serializeChanges(Array<Clip> changes,
    const String &description, int64 numChanges, const String &deltaType) const
{
    NewSerializedDelta changesFullDelta;
    changesFullDelta.delta = new Delta(DeltaDescription(description, numChanges), deltaType);
    changesFullDelta.deltaData = this->serializePattern(changes, deltaType);
    return changesFullDelta;
}

XmlElement *PatternDiffLogic::serializePattern(Array<Clip> changes,
    const String &tag) const
{
    auto xml = new XmlElement(tag);

    for (int i = 0; i < changes.size(); ++i)
    {
        xml->addChildElement(changes.getUnchecked(i).serialize());
    }

    return xml;
}

bool PatternDiffLogic::checkIfDeltaIsPatternType(const Delta *delta) const
{
    return (delta->getType() == PatternDeltas::clipsAdded ||
        delta->getType() == PatternDeltas::clipsRemoved ||
        delta->getType() == PatternDeltas::clipsChanged);
}
