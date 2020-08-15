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
#include "ProjectMetadataActions.h"
#include "TreeNode.h"
#include "MidiTrackSource.h"
#include "SerializationKeys.h"

//===----------------------------------------------------------------------===//
// Change Temperament
//===----------------------------------------------------------------------===//

ProjectTemperamentChangeAction::ProjectTemperamentChangeAction(ProjectNode &project,
    const Temperament &newParameters) noexcept :
    UndoAction(project),
    temperamentBefore(*project.getProjectInfo()->getTemperament()),
    temperamentAfter(newParameters) {}

ProjectTemperamentChangeAction::ProjectTemperamentChangeAction(ProjectNode &project,
    const Temperament &before, const Temperament &after) noexcept :
    UndoAction(project),
    temperamentBefore(before),
    temperamentAfter(after) {}

bool ProjectTemperamentChangeAction::perform()
{
    auto *metadata = this->getProject().getProjectInfo();
    metadata->setTemperament(this->temperamentAfter);
    return true;
}

bool ProjectTemperamentChangeAction::undo()
{
    auto *metadata = this->getProject().getProjectInfo();
    metadata->setTemperament(this->temperamentBefore);
    return true;
}

ProjectNode &ProjectTemperamentChangeAction::getProject() const noexcept
{
    return static_cast<ProjectNode &>(this->source);
}

int ProjectTemperamentChangeAction::getSizeInUnits()
{
    return sizeof(Temperament) * 2;
}

UndoAction *ProjectTemperamentChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (auto *nextChanger = dynamic_cast<ProjectTemperamentChangeAction *>(nextAction))
    {
        if (this->getProject().getId() == nextChanger->getProject().getId())
        {
            return new ProjectTemperamentChangeAction(this->getProject(),
                this->temperamentBefore, nextChanger->temperamentAfter);
        }
    }

    (void)nextAction;
    return nullptr;
}

SerializedData ProjectTemperamentChangeAction::serialize() const
{
    SerializedData tree(Serialization::Undo::projectTemperamentChangeAction);

    SerializedData instanceBeforeChild(Serialization::Undo::instanceBefore);
    instanceBeforeChild.appendChild(this->temperamentBefore.serialize());
    tree.appendChild(instanceBeforeChild);

    SerializedData instanceAfterChild(Serialization::Undo::instanceAfter);
    instanceAfterChild.appendChild(this->temperamentAfter.serialize());
    tree.appendChild(instanceAfterChild);

    return tree;
}

void ProjectTemperamentChangeAction::deserialize(const SerializedData &data)
{
    auto instanceBeforeChild = data.getChildWithName(Serialization::Undo::instanceBefore);
    auto instanceAfterChild = data.getChildWithName(Serialization::Undo::instanceAfter);

    this->temperamentBefore.deserialize(instanceBeforeChild.getChild(0));
    this->temperamentAfter.deserialize(instanceAfterChild.getChild(0));
}

void ProjectTemperamentChangeAction::reset()
{
    this->temperamentBefore.reset();
    this->temperamentAfter.reset();
}
