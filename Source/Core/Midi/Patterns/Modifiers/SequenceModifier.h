/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class ProjectNode;
class PianoSequence;
class Clip;

#include "Serializable.h"
#include "Icons.h"

// even though the class is called SequenceModifier,
// all such modifiers are stacked on clips, not sequences
// to allow having different modifiers on different sequence instances:
class SequenceModifier : public Serializable, public ReferenceCountedObject
{
public:

    SequenceModifier() = default;

    // using reference counting because clips want
    // to be easily and cheaply copied here and there:
    using Ptr = ReferenceCountedObjectPtr<SequenceModifier>;

    // for now it will only support transforming notes:
    virtual void processSequence(const ProjectNode &project, 
        const Clip &clip, const PianoSequence &sequence) = 0;

    virtual bool hasParameters() const = 0;
    virtual String getDescription() const = 0;
    virtual Icons::Id getIconId() const = 0;

    virtual Ptr withEnabledFlag(bool shouldBeEnabled) const = 0;

    inline bool isEnabled() const noexcept
    {
        return this->enabled;
    }

    // to be used in version control:
    virtual bool isEquivalentTo(SequenceModifier::Ptr other) const = 0;

protected:

    bool enabled = true;

};
