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
#include "DiffLogic.h"
#include "SerializationKeys.h"

#include "PianoTrackDiffLogic.h"
#include "AutomationTrackDiffLogic.h"
#include "ProjectTimelineDiffLogic.h"
#include "ProjectInfoDiffLogic.h"

using namespace VCS;

class DummyDiffLogic final : public AutomationTrackDiffLogic
{
public:
    
    explicit DummyDiffLogic(TrackedItem &targetItem) :
        AutomationTrackDiffLogic(targetItem) {}
    
    const Identifier getType() const override
    {
        return "none";
    }
};

DiffLogic *DiffLogic::createLogicCopy(TrackedItem &copyFrom, TrackedItem &targetItem)
{
    const Identifier &type = copyFrom.getDiffLogic()->getType();
    return DiffLogic::createLogicFor(targetItem, type);
}

DiffLogic *DiffLogic::createLogicFor(TrackedItem &targetItem, const Identifier &type)
{
    if (type == Serialization::Core::pianoTrack)
    {
        return new PianoTrackDiffLogic(targetItem);
    }
    else if (type == Serialization::Core::automationTrack)
    {
        return new AutomationTrackDiffLogic(targetItem);
    }
    else if (type == Serialization::Core::projectTimeline)
    {
        return new ProjectTimelineDiffLogic(targetItem);
    }
    else if (type == Serialization::Core::projectInfo)
    {
        return new ProjectInfoDiffLogic(targetItem);
    }

    //jassertfalse;
    return new DummyDiffLogic(targetItem);
}
