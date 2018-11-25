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
#include "RevisionItem.h"
#include "DiffLogic.h"

using namespace VCS;

RevisionItem::RevisionItem(Type type, TrackedItem *targetToCopy) :
    vcsItemType(type)
{
    if (targetToCopy != nullptr)
    {
        this->description = targetToCopy->getVCSName();
        this->vcsUuid = targetToCopy->getUuid();

        this->logic = DiffLogic::createLogicCopy(*targetToCopy, *this);

        // just deep-copy all deltas:
        for (int i = 0; i < targetToCopy->getNumDeltas(); ++i)
        {
            const auto targetDelta = targetToCopy->getDelta(i);
            this->deltas.add(targetDelta->createCopy());
            ValueTree data(targetToCopy->getDeltaData(i));
            this->deltasData.add(data);
            //jassert(!data.getParent().isValid());
        }
    }
}

RevisionItem::Type RevisionItem::getType() const noexcept
{
    return this->vcsItemType;
}

String RevisionItem::getTypeAsString() const
{
    if (this->vcsItemType == Added)
    {
        return TRANS("vcs::delta::type::added");
    }
    if (this->vcsItemType == Removed)
    {
        return TRANS("vcs::delta::type::removed");
    }
    else if (this->vcsItemType == Changed)
    {
        return TRANS("vcs::delta::type::changed");
    }

    return {};
}

//===----------------------------------------------------------------------===//
// TrackedItem
//===----------------------------------------------------------------------===//

int RevisionItem::getNumDeltas() const noexcept
{
    return this->deltas.size();
}

Delta *RevisionItem::getDelta(int index) const noexcept
{
    return this->deltas[index];
}

ValueTree RevisionItem::getDeltaData(int deltaIndex) const noexcept
{
    return this->deltasData[deltaIndex];
}

String RevisionItem::getVCSName() const noexcept
{
    return this->description;
}

DiffLogic *RevisionItem::getDiffLogic() const noexcept
{
    return this->logic;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree RevisionItem::serialize() const
{
    ValueTree tree(Serialization::VCS::revisionItem);

    this->serializeVCSUuid(tree);

    tree.setProperty(Serialization::VCS::revisionItemType, this->getType(), nullptr);
    tree.setProperty(Serialization::VCS::revisionItemName, this->getVCSName(), nullptr);
    tree.setProperty(Serialization::VCS::revisionItemDiffLogic, this->getDiffLogic()->getType().toString(), nullptr);

    for (int i = 0; i < this->deltas.size(); ++i)
    {
        const auto *delta = this->deltas.getUnchecked(i);
        ValueTree deltaNode(delta->serialize());
        const ValueTree deltaData(this->getDeltaData(i));

        // sometimes we need to create copy since value trees cannot be shared between two parents
        // but Snapshot seems to share revision items on checkout; need to fix this someday:
        deltaNode.appendChild(deltaData.getParent().isValid() ? deltaData.createCopy() : deltaData, nullptr);
        tree.appendChild(deltaNode, nullptr);
    }

    return tree;
}

void RevisionItem::deserialize(const ValueTree &tree)
{
    // Use deserialize/2 workaround (see the comment in VersionControl.cpp)
    jassertfalse;
}

void RevisionItem::deserialize(const ValueTree &tree, const DeltaDataLookup &dataLookup)
{
    this->reset();

    const auto root = tree.hasType(Serialization::VCS::revisionItem) ?
        tree : tree.getChildWithName(Serialization::VCS::revisionItem);

    if (!root.isValid()) { return; }

    this->deserializeVCSUuid(root);

    this->description = root.getProperty(Serialization::VCS::revisionItemName);

    const int type = root.getProperty(Serialization::VCS::revisionItemType, Undefined);
    this->vcsItemType = static_cast<Type>(type);

    const String logicType = root.getProperty(Serialization::VCS::revisionItemDiffLogic);
    jassert(logicType.isNotEmpty());

    this->logic = DiffLogic::createLogicFor(*this, logicType);

    for (const auto &e : root)
    {
        ScopedPointer<Delta> delta(new Delta({}, {}));
        delta->deserialize(e);

        // either we already have saved data, or the lookup table is provided:
        jassert((e.getNumChildren() == 0 && dataLookup.size() > 0) || (e.getNumChildren() == 1 && dataLookup.size() == 0));

        if (e.getNumChildren() == 1)
        {
            this->deltasData.add(e.getChild(0));
        }
        else
        {
            const String deltaId = e.getProperty(Serialization::VCS::deltaId);
            if (dataLookup.contains(deltaId))
            {
                const auto data = dataLookup.at(deltaId);
                this->deltasData.add(data);
            }
        }

        this->deltas.add(delta.release());
        jassert(this->deltasData.size() == this->deltas.size());
    }
}

void RevisionItem::reset()
{
    this->deltas.clear();
    this->description.clear();
    this->vcsItemType = Undefined;
}
