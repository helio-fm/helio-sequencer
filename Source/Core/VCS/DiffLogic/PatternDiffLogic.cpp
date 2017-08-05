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

#include "Common.h"
#include "PatternDiffLogic.h"
#include "Clip.h"
#include "Pattern.h"
#include "SerializationKeys.h"
#include "ProjectEventDispatcher.h"

using namespace VCS;

void PatternDiffLogic::deserializePatternChanges(Pattern &pattern, 
    const XmlElement *state, const XmlElement *changes,
    Array<Clip> &stateClips, Array<Clip> &changesClips)
{
    if (state != nullptr)
    {
        forEachXmlChildElementWithTagName(*state, e, Serialization::Core::clip)
        {
            Clip clip(&pattern);
            clip.deserialize(*e);
            stateClips.addSorted(clip, clip);
        }
    }

    if (changes != nullptr)
    {
        forEachXmlChildElementWithTagName(*changes, e, Serialization::Core::clip)
        {
            Clip clip(&pattern);
            clip.deserialize(*e);
            changesClips.addSorted(clip, clip);
        }
    }
}

XmlElement *PatternDiffLogic::serializePattern(Array<Clip> changes,
    const String &tag)
{
    auto xml = new XmlElement(tag);

    for (int i = 0; i < changes.size(); ++i)
    {
        xml->addChildElement(changes.getUnchecked(i).serialize());
    }

    return xml;
}

NewSerializedDelta PatternDiffLogic::serializePatternChanges(Array<Clip> changes,
    const String &description, int64 numChanges, const String &deltaType)
{
    NewSerializedDelta changesFullDelta;
    changesFullDelta.delta = new Delta(DeltaDescription(description, numChanges), deltaType);
    changesFullDelta.deltaData = serializePattern(changes, deltaType);
    return changesFullDelta;
}

bool PatternDiffLogic::checkIfDeltaIsPatternType(const Delta *delta)
{
    return (delta->getType() == PatternDeltas::clipsAdded ||
        delta->getType() == PatternDeltas::clipsRemoved ||
        delta->getType() == PatternDeltas::clipsChanged);
}

XmlElement *PatternDiffLogic::mergeClipsAdded(const XmlElement *state,
    const XmlElement *changes)
{
    EmptyEventDispatcher dispatcher;
    Pattern emptyPattern(dispatcher);
    Array<Clip> stateClips;
    Array<Clip> changesClips;
    deserializePatternChanges(emptyPattern, state, changes, stateClips, changesClips);

    Array<Clip> result;
    result.addArray(stateClips);

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

    return serializePattern(result, PatternDeltas::clipsAdded);
}

XmlElement *PatternDiffLogic::mergeClipsRemoved(const XmlElement *state, 
    const XmlElement *changes)
{
    EmptyEventDispatcher dispatcher;
    Pattern emptyPattern(dispatcher);
    Array<Clip> stateClips;
    Array<Clip> changesClips;
    deserializePatternChanges(emptyPattern, state, changes, stateClips, changesClips);

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

    return serializePattern(result, PatternDeltas::clipsAdded);
}

XmlElement *PatternDiffLogic::mergeClipsChanged(const XmlElement *state, 
    const XmlElement *changes)
{
    EmptyEventDispatcher dispatcher;
    Pattern emptyPattern(dispatcher);
    Array<Clip> stateClips;
    Array<Clip> changesClips;
    deserializePatternChanges(emptyPattern, state, changes, stateClips, changesClips);

    Array<Clip> result;
    result.addArray(stateClips);

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

    return serializePattern(result, PatternDeltas::clipsAdded);
}

Array<VCS::NewSerializedDelta> PatternDiffLogic::createClipsDiffs(
    const XmlElement *state, const XmlElement *changes)
{
    EmptyEventDispatcher dispatcher;
    Pattern emptyPattern(dispatcher);
    Array<Clip> stateClips;
    Array<Clip> changesClips;

    deserializePatternChanges(emptyPattern, state, changes, stateClips, changesClips);

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
        res.add(serializePatternChanges(addedClips,
            "added {x} clips",
            addedClips.size(),
            PatternDeltas::clipsAdded));
    }

    if (removedClips.size() > 0)
    {
        res.add(serializePatternChanges(removedClips,
            "removed {x} clips",
            removedClips.size(),
            PatternDeltas::clipsRemoved));
    }

    if (changedClips.size() > 0)
    {
        res.add(serializePatternChanges(changedClips,
            "changed {x} clips",
            changedClips.size(),
            PatternDeltas::clipsChanged));
    }

    return res;
}
