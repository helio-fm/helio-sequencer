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

#include "PianoLayerDiffLogic.h"
#include "AutomationLayerDiffLogic.h"
#include "ProjectTimelineDiffLogic.h"
#include "ProjectInfoDiffLogic.h"

using namespace VCS;


class DummyDiffLogic : public AutomationLayerDiffLogic
{
public:
    
    explicit DummyDiffLogic(TrackedItem &targetItem) : AutomationLayerDiffLogic(targetItem)
    {
    }
    
    const String getType() const override
    {
        return "DummyDiffLogic";
    }
};


DiffLogic *DiffLogic::createLogicCopy(TrackedItem &copyFrom, TrackedItem &targetItem)
{
    const String &type = copyFrom.getDiffLogic()->getType();
    return DiffLogic::createLogicFor(targetItem, type);
}

DiffLogic *DiffLogic::createLogicFor(TrackedItem &targetItem, const String &type)
{
    if (type == Serialization::Core::pianoLayer)
    {
        return new PianoLayerDiffLogic(targetItem);
    }
    if (type == Serialization::Core::autoLayer)
    {
        return new AutomationLayerDiffLogic(targetItem);
    }
    else if (type == Serialization::Core::projectTimeline)
    {
        return new ProjectTimelineDiffLogic(targetItem);
    }
    else if (type == Serialization::Core::projectInfo)
    {
        return new ProjectInfoDiffLogic(targetItem);
    }

    return new DummyDiffLogic(targetItem);
    
//    jassertfalse;
//    return nullptr;
}
