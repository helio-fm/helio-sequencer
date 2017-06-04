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
#include "Pattern.h"
#include "PatternActions.h"
#include "ProjectTreeItem.h"
#include "UndoStack.h"
#include "SerializationKeys.h"

Pattern::Pattern(MidiLayerOwner &parent) :
    owner(parent)
{
}

Pattern::~Pattern()
{
    this->masterReference.clear();
}

MidiLayerOwner *Pattern::getOwner() const
{
    return &this->owner;
}

void Pattern::sort()
{
	if (this->clips.size() > 0)
	{
		this->clips.sort(this->clips.getUnchecked(0));
	}
}

//===----------------------------------------------------------------------===//
// Undoing // TODO move this to project interface
//

void Pattern::checkpoint()
{
    this->getUndoStack()->beginNewTransaction(String::empty);
}

void Pattern::undo()
{
    if (this->getUndoStack()->canUndo())
    {
        this->checkpoint();
        this->getUndoStack()->undo();
    }
}

void Pattern::redo()
{
    if (this->getUndoStack()->canRedo())
    {
        this->getUndoStack()->redo();
    }
}

void Pattern::clearUndoHistory()
{
    this->getUndoStack()->clearUndoHistory();
}

//===----------------------------------------------------------------------===//
// Clip Actions
//===----------------------------------------------------------------------===//

Array<Clip> &Pattern::getClips() noexcept
{
	return this->clips;
}

bool Pattern::insert(Clip clip, const bool undoable)
{
	if (this->clips.contains(clip))
	{
		return false;
	}

	if (undoable)
	{
		this->getUndoStack()->perform(new PatternClipInsertAction(*this->getProject(),
			this->getPatternIdAsString(),
			clip));
	}
	else
	{
		this->clips.addSorted(clip, clip);
		this->notifyClipAdded(clip);
	}

	return true;
}

bool Pattern::remove(Clip clip, const bool undoable)
{
	if (undoable)
	{
		this->getUndoStack()->perform(new PatternClipRemoveAction(*this->getProject(),
			this->getPatternIdAsString(),
			clip));
	}
	else
	{
		int index = this->clips.indexOfSorted(clip, clip);
		if (index >= 0)
		{
			this->clips.remove(index);
			this->notifyClipRemoved(clip);
			return true;
		}

		return false;
	}

	return true;
}

bool Pattern::change(Clip clip, Clip newClip, const bool undoable)
{
	if (undoable)
	{
		this->getUndoStack()->perform(new PatternClipChangeAction(*this->getProject(),
			this->getPatternIdAsString(),
			clip,
			newClip));
	}
	else
	{
		int index = this->clips.indexOfSorted(clip, clip);
		if (index >= 0)
		{
			this->clips.remove(index);
			this->clips.addSorted(newClip, newClip);
			this->notifyClipChanged(clip, newClip);
			return true;
		}

		return false;
	}

	return true;
}


//===----------------------------------------------------------------------===//
// Accessors
//

ProjectTreeItem *Pattern::getProject()
{
    return this->owner.getProject();
}

UndoStack *Pattern::getUndoStack()
{
    return this->owner.getProject()->getUndoStack();
}

Uuid Pattern::getPatternId() const noexcept
{
	return this->id;
}

String Pattern::getPatternIdAsString() const
{
	return this->id.toString();
}


//===----------------------------------------------------------------------===//
// Events change listener
//

void Pattern::notifyClipChanged(const Clip &oldClip, const Clip &newClip)
{
	//TODO
    //this->owner.onEventChanged(oldEvent, newEvent);
}

void Pattern::notifyClipAdded(const Clip &clip)
{
	//TODO
	//this->owner.onEventAdded(event);
}

void Pattern::notifyClipRemoved(const Clip &clip)
{
	//TODO
	//this->owner.onEventRemoved(event);
}

void Pattern::notifyPatternChanged()
{
	//TODO
	//this->owner.onPatternChanged(this);
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *Pattern::serialize() const
{
	auto xml = new XmlElement(Serialization::Core::pattern);
	xml->setAttribute("id", this->getPatternIdAsString());

	for (int i = 0; i < this->clips.size(); ++i)
	{
		const Clip clip = this->clips.getUnchecked(i);
		xml->prependChildElement(clip.serialize());
	}

	return xml;
}

void Pattern::deserialize(const XmlElement &xml)
{
	this->clearQuick();

	const XmlElement *root =
		(xml.getTagName() == Serialization::Core::pattern) ?
		&xml : xml.getChildByName(Serialization::Core::pattern);

	if (root == nullptr)
	{
		return;
	}

	this->id = Uuid(root->getStringAttribute("id", this->getPatternIdAsString()));

	forEachXmlChildElementWithTagName(*root, e, Serialization::Core::clip)
	{
		Clip c;
		c.deserialize(*e);
		this->clips.add(c);
	}

	// Fallback to single instance at zero bar, if no instances found
	if (this->clips.size() == 0)
	{
		this->clips.add(Clip());
	}

	this->sort();
	this->notifyPatternChanged();
}

void Pattern::reset()
{
	this->clearQuick();
	this->notifyPatternChanged();
}

void Pattern::clearQuick()
{
	this->clips.clearQuick();
}
