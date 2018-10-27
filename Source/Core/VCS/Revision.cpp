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
#include "SerializationKeys.h"
#include "App.h"

using namespace VCS;

Revision::Revision(Pack::Ptr pack, const String &name /*= String::empty*/) :
    pack(pack),
    message(name),
    id(Uuid().toString()),
    timestamp(Time::getCurrentTime().toMilliseconds()) {}

Revision::Revision(const RevisionDto &dto) :
    pack(nullptr),
    message(dto.getMessage()),
    id(dto.getId()),
    timestamp(dto.getTimestamp()) {}

void Revision::copyDeltasFrom(Revision::Ptr other)
{
    this->deltas.clearQuick();
    for (auto *revItem : other->deltas)
    {
        if (this->pack == other->pack)
        {
            this->deltas.add(revItem);
        }
        else
        {
            // when merging two trees with different delta packs,
            // copy items with new RevisionItem to copy data from the old pack:
            RevisionItem::Ptr newItem(new RevisionItem(this->pack, revItem->getType(), revItem));
            this->deltas.add(newItem);
        }
    }
}

bool Revision::isEmpty() const noexcept
{
    return this->deltas.isEmpty() && this->children.isEmpty();
}

bool Revision::isShallowCopy() const noexcept
{
    return this->isEmpty() && this->pack == nullptr;
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
    this->children.add(child); // todo test
    //this->children.insert(0, child);
}

void VCS::Revision::addChild(Revision::Ptr revision)
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

void VCS::Revision::removeChild(Revision::Ptr revision)
{
    this->removeChild(revision.get());
}

void Revision::addItem(RevisionItem *item)
{
    this->deltas.add(item);
}

void VCS::Revision::addItem(RevisionItem::Ptr item)
{
    this->deltas.add(item);
}

WeakReference<Revision> Revision::getParent() const noexcept
{
    return this->parent;
}

// moves items' data from memory to pack
void Revision::flush()
{
    for (auto *revItem : this->deltas)
    {
        revItem->flushData();
    }
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree Revision::serialize() const
{
    ValueTree tree(Serialization::VCS::revision);

    tree.setProperty(Serialization::VCS::commitId, this->id, nullptr);
    tree.setProperty(Serialization::VCS::commitMessage, this->message, nullptr);
    tree.setProperty(Serialization::VCS::commitTimeStamp, this->timestamp, nullptr);

    for (const auto *revItem : this->deltas)
    {
        tree.appendChild(revItem->serialize(), nullptr);
    }

    for (const auto *child : this->children)
    {
        tree.appendChild(child->serialize(), nullptr);
    }

    return tree;
}

void Revision::deserialize(const ValueTree &tree)
{
    this->reset();

    const auto root =
        tree.hasType(Serialization::VCS::revision) ?
        tree : tree.getChildWithName(Serialization::VCS::revision);

    if (!root.isValid()) { return; }

    // don't reset pack pointer here
    this->id = root.getProperty(Serialization::VCS::commitId);
    this->message = root.getProperty(Serialization::VCS::commitMessage);
    this->timestamp = root.getProperty(Serialization::VCS::commitTimeStamp);

    for (const auto &e : root)
    {
        if (e.hasType(Serialization::VCS::revision))
        {
            Revision::Ptr child(new Revision(this->pack));
            child->deserialize(e);
            this->addChild(child);
        }
        else if (e.hasType(Serialization::VCS::revisionItem))
        {
            RevisionItem::Ptr item(new RevisionItem(this->pack, RevisionItem::Undefined, nullptr));
            item->deserialize(e);
            this->addItem(item);
        }
    }
}

void Revision::reset()
{
    // never reset pack pointer here
    this->id = {};
    this->message = {};
    this->timestamp = 0;
    this->deltas.clearQuick();
    this->children.clearQuick();
}
