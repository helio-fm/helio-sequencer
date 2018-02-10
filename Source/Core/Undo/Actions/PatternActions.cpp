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
#include "PatternActions.h"
#include "MidiTrackSource.h"
#include "MidiSequence.h"
#include "TreeItem.h"
#include "SerializationKeys.h"

//===----------------------------------------------------------------------===//
// Insert Clip
//===----------------------------------------------------------------------===//

PatternClipInsertAction::PatternClipInsertAction(MidiTrackSource &source,
    String trackId, const Clip &target) :
    UndoAction(source),
    trackId(std::move(trackId)),
    clip(target) {}

bool PatternClipInsertAction::perform()
{
    if (Pattern *pattern = 
        this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->insert(this->clip, false);
    }

    return false;
}

bool PatternClipInsertAction::undo()
{
    if (Pattern *pattern =
        this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->remove(this->clip, false);
    }

    return false;
}

int PatternClipInsertAction::getSizeInUnits()
{
    return sizeof(Clip);
}

ValueTree PatternClipInsertAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::patternClipInsertAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(this->clip.serialize());
    return xml;
}

void PatternClipInsertAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    this->clip.deserialize(*tree.getFirstChildElement());
}

void PatternClipInsertAction::reset()
{
    this->clip.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Remove Instance
//===----------------------------------------------------------------------===//

PatternClipRemoveAction::PatternClipRemoveAction(MidiTrackSource &source,
    String trackId, const Clip &target) :
    UndoAction(source),
    trackId(std::move(trackId)),
    clip(target) {}

bool PatternClipRemoveAction::perform()
{
    if (Pattern *pattern =
        this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->remove(this->clip, false);
    }

    return false;
}

bool PatternClipRemoveAction::undo()
{
    if (Pattern *pattern =
        this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->insert(this->clip, false);
    }

    return false;
}

int PatternClipRemoveAction::getSizeInUnits()
{
    return sizeof(Clip);
}

ValueTree PatternClipRemoveAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::patternClipRemoveAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);
    xml->prependChildElement(this->clip.serialize());
    return xml;
}

void PatternClipRemoveAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);
    this->clip.deserialize(*tree.getFirstChildElement());
}

void PatternClipRemoveAction::reset()
{
    this->clip.reset();
    this->trackId.clear();
}

//===----------------------------------------------------------------------===//
// Change Instance
//===----------------------------------------------------------------------===//

PatternClipChangeAction::PatternClipChangeAction(MidiTrackSource &source,
    String trackId,
    const Clip &target,
    const Clip &newParameters) :
    UndoAction(source),
    trackId(std::move(trackId)),
    clipBefore(target),
    clipAfter(newParameters)
{
    jassert(target.getId() == newParameters.getId());
}

bool PatternClipChangeAction::perform()
{
    if (Pattern *pattern =
        this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->change(this->clipBefore, this->clipAfter, false);
    }

    return false;
}

bool PatternClipChangeAction::undo()
{
    if (Pattern *pattern =
        this->source.findPatternByTrackId(this->trackId))
    {
        return pattern->change(this->clipAfter, this->clipBefore, false);
    }

    return false;
}

int PatternClipChangeAction::getSizeInUnits()
{
    return sizeof(Clip) * 2;
}

UndoAction *PatternClipChangeAction::createCoalescedAction(UndoAction *nextAction)
{
    if (Pattern *pattern = 
        this->source.findPatternByTrackId(this->trackId))
    {
        if (PatternClipChangeAction *nextChanger =
            dynamic_cast<PatternClipChangeAction *>(nextAction))
        {
            const bool idsAreEqual =
                (this->clipBefore.getId() == nextChanger->clipAfter.getId() &&
                this->trackId == nextChanger->trackId);

            if (idsAreEqual)
            {
                return new PatternClipChangeAction(this->source,
                    this->trackId, this->clipBefore, nextChanger->clipAfter);
            }
        }
    }

    (void)nextAction;
    return nullptr;
}

ValueTree PatternClipChangeAction::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::patternClipChangeAction);
    xml->setAttribute(Serialization::Undo::trackId, this->trackId);

    auto instanceBeforeChild = new XmlElement(Serialization::Undo::instanceBefore);
    instanceBeforeChild->prependChildElement(this->clipBefore.serialize());
    xml->prependChildElement(instanceBeforeChild);

    auto instanceAfterChild = new XmlElement(Serialization::Undo::instanceAfter);
    instanceAfterChild->prependChildElement(this->clipAfter.serialize());
    xml->prependChildElement(instanceAfterChild);

    return xml;
}

void PatternClipChangeAction::deserialize(const ValueTree &tree)
{
    this->trackId = tree.getStringAttribute(Serialization::Undo::trackId);

    auto instanceBeforeChild = tree.getChildByName(Serialization::Undo::instanceBefore);
    auto instanceAfterChild = tree.getChildByName(Serialization::Undo::instanceAfter);

    this->clipBefore.deserialize(*instanceBeforeChild->getFirstChildElement());
    this->clipAfter.deserialize(*instanceAfterChild->getFirstChildElement());
}

void PatternClipChangeAction::reset()
{
    this->clipBefore.reset();
    this->clipAfter.reset();
    this->trackId.clear();
}
