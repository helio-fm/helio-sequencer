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
#include "Revision.h"

namespace VCS
{

Revision::Revision(const String &name /*= String::empty*/) :
    message(name),
    id(Uuid().toString()),
    timestamp(Time::getCurrentTime().toMilliseconds()) {}

#if !NO_NETWORK

Revision::Revision(const RevisionDto &dto) :
    message(dto.getMessage()),
    id(dto.getId()),
    timestamp(dto.getTimestamp()) {}

#endif

void Revision::copyDeltasFrom(Revision::Ptr other)
{
    this->deltas.clearQuick();
    for (auto *revItem : other->deltas)
    {
        this->deltas.add(revItem);
    }
}

bool Revision::isEmpty() const noexcept
{
    return this->deltas.isEmpty() && this->children.isEmpty();
}

bool Revision::isShallowCopy() const noexcept
{
    // children might me not empty though:
    return this->deltas.isEmpty();
}

int64 Revision::getTimeStamp() const noexcept
{
    return this->timestamp;
}

String Revision::getUuid() const noexcept
{
    return this->id;
}

String Revision::getMessage() const noexcept
{
    return this->message;
}

const ReferenceCountedArray<RevisionItem> &Revision::getItems() const noexcept
{
    return this->deltas;
}

const ReferenceCountedArray<Revision> &Revision::getChildren() const  noexcept
{
    return this->children;
}

void Revision::addChild(Revision *child)
{
    child->parent = this;
    this->children.add(child);
}

void Revision::addChild(Revision::Ptr revision)
{
    this->addChild(revision.get());
}

void Revision::removeChild(Revision *revision)
{
    if (this->children.contains(revision))
    {
        revision->parent = nullptr;
        this->children.removeObject(revision);
    }
}

void Revision::removeChild(Revision::Ptr revision)
{
    this->removeChild(revision.get());
}

void Revision::addItem(RevisionItem *item)
{
    this->deltas.add(item);
}

void Revision::addItem(RevisionItem::Ptr item)
{
    this->deltas.add(item);
}

WeakReference<Revision> Revision::getParent() const noexcept
{
    return this->parent;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData Revision::serializeDeltas() const
{
    SerializedData tree(Serialization::VCS::revision);

    for (const auto *revItem : this->deltas)
    {
        tree.appendChild(revItem->serialize());
    }

    return tree;
}

void Revision::deserializeDeltas(SerializedData data)
{
    jassert(this->isShallowCopy());

    const auto root =
        data.hasType(Serialization::VCS::revision) ?
        data : data.getChildWithName(Serialization::VCS::revision);

    if (!root.isValid()) { return; }

    this->deltas.clearQuick();

    forEachChildWithType(root, e, Serialization::VCS::revisionItem)
    {
        RevisionItem::Ptr item(new RevisionItem(RevisionItem::Type::Undefined, nullptr));
        item->deserialize(e);
        this->addItem(item);
    }
}

SerializedData Revision::serialize() const
{
    SerializedData tree(Serialization::VCS::revision);

    tree.setProperty(Serialization::VCS::commitId, this->id);
    tree.setProperty(Serialization::VCS::commitMessage, this->message);
    tree.setProperty(Serialization::VCS::commitTimeStamp, this->timestamp);

    for (const auto *revItem : this->deltas)
    {
        tree.appendChild(revItem->serialize());
    }

    for (const auto *child : this->children)
    {
        tree.appendChild(child->serialize());
    }

    return tree;
}

void Revision::deserialize(const SerializedData &data)
{
    this->reset();

    const auto root =
        data.hasType(Serialization::VCS::revision) ?
        data : data.getChildWithName(Serialization::VCS::revision);

    if (!root.isValid()) { return; }

    this->id = root.getProperty(Serialization::VCS::commitId);
    this->message = root.getProperty(Serialization::VCS::commitMessage);
    this->timestamp = root.getProperty(Serialization::VCS::commitTimeStamp);

    for (const auto &e : root)
    {
        if (e.hasType(Serialization::VCS::revision))
        {
            Revision::Ptr child(new Revision());
            child->deserialize(e);
            this->addChild(child);
        }
        else if (e.hasType(Serialization::VCS::revisionItem))
        {
            RevisionItem::Ptr item(new RevisionItem(RevisionItem::Type::Undefined, nullptr));
            item->deserialize(e);
            this->addItem(item);
        }
    }
}

void Revision::reset()
{
    this->id = {};
    this->message = {};
    this->timestamp = 0;
    this->deltas.clearQuick();
    this->children.clearQuick();
}

}
