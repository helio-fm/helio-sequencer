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
#include "StashesRepository.h"

using namespace VCS;

StashesRepository::StashesRepository(Pack::Ptr pack) :
    pack(pack),
    userStashes(new Revision(pack, "Stashes Root")),
    quickStash(new Revision(pack)) {}

int StashesRepository::getNumUserStashes() const
{
    return this->userStashes->getChildren().size();
}

String StashesRepository::getUserStashDescription(int index) const
{
    return this->getUserStash(index)->getMessage();
}

Revision::Ptr VCS::StashesRepository::getUserStash(int index) const
{
    return this->userStashes->getChildren()[index];
}

Revision::Ptr VCS::StashesRepository::getUserStashWithName(const String &stashName) const
{
    for (auto *child : this->userStashes->getChildren())
    {
        if (child->getMessage() == stashName)
        {
            return child;
        }
    }
    
    return { new Revision(this->pack) };
}

void VCS::StashesRepository::addStash(Revision::Ptr newStash)
{
    this->userStashes->addChild(newStash);
    newStash->flush();
    this->pack->flush();
}

void VCS::StashesRepository::removeStash(Revision::Ptr stashToRemove)
{
    // todo sanity checks?
    this->userStashes.removeChild(stashToRemove, nullptr);
}

Revision::Ptr VCS::StashesRepository::getQuickStash() const noexcept
{
    return this->quickStash;
}

bool StashesRepository::hasQuickStash() const noexcept
{
    return this->quickStash->isEmpty();
}

void VCS::StashesRepository::storeQuickStash(Revision::Ptr newStash)
{
    this->resetQuickStash();
    this->quickStash->copyDeltasFrom(newStash);
    this->quickStash->flush();
    this->pack->flush();
}

void StashesRepository::resetQuickStash()
{
    this->quickStash = { new Revision(this->pack, Serialization::VCS::quickStashId.toString()) };
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree VCS::StashesRepository::serialize() const
{
    ValueTree tree(Serialization::VCS::stashesRepository);
    
    ValueTree userStashesXml(Serialization::VCS::userStashes);
    tree.appendChild(userStashesXml, nullptr);
    
    userStashesXml.appendChild(this->userStashes->serialize(), nullptr);

    ValueTree quickStashXml(Serialization::VCS::quickStash);
    tree.appendChild(quickStashXml, nullptr);

    quickStashXml.appendChild(this->quickStash->serialize(), nullptr);
    
    return tree;
}

void VCS::StashesRepository::deserialize(const ValueTree &tree)
{
    this->reset();
    
    const auto root = tree.hasType(Serialization::VCS::stashesRepository) ?
        tree : tree.getChildWithName(Serialization::VCS::stashesRepository);
    
    if (!root.isValid()) { return; }

    const auto userStashesParams = root.getChildWithName(Serialization::VCS::userStashes);
    if (userStashesParams.isValid())
    {
        this->userStashes->deserialize(userStashesParams);
    }
    
    const auto quickStashParams = root.getChildWithName(Serialization::VCS::quickStash);
    if (quickStashParams.isValid())
    {
        this->quickStash->deserialize(quickStashParams);
    }
}

void StashesRepository::reset()
{
    this->userStashes->reset();
    this->quickStash->reset();
}
