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

StashesRepository::StashesRepository() :
    userStashes(new Revision("Stashes Root")),
    quickStash(new Revision()) {}

int StashesRepository::getNumUserStashes() const
{
    return this->userStashes->getChildren().size();
}

String StashesRepository::getUserStashDescription(int index) const
{
    return this->getUserStash(index)->getMessage();
}

Revision::Ptr StashesRepository::getUserStash(int index) const
{
    return this->userStashes->getChildren()[index];
}

Revision::Ptr StashesRepository::getUserStashWithName(const String &stashName) const
{
    for (auto *child : this->userStashes->getChildren())
    {
        if (child->getMessage() == stashName)
        {
            return child;
        }
    }
    
    return { new Revision() };
}

void StashesRepository::addStash(Revision::Ptr newStash)
{
    this->userStashes->addChild(newStash);
}

void StashesRepository::removeStash(Revision::Ptr stashToRemove)
{
    // todo sanity checks?
    this->userStashes->removeChild(stashToRemove);
}

Revision::Ptr StashesRepository::getQuickStash() const noexcept
{
    return this->quickStash;
}

bool StashesRepository::hasQuickStash() const noexcept
{
    return this->quickStash->isEmpty();
}

void StashesRepository::storeQuickStash(Revision::Ptr newStash)
{
    this->resetQuickStash();
    this->quickStash->copyDeltasFrom(newStash);
}

void StashesRepository::resetQuickStash()
{
    this->quickStash = { new Revision(Serialization::VCS::quickStashId.toString()) };
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree StashesRepository::serialize() const
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

void StashesRepository::deserialize(const ValueTree &tree)
{
    // Use deserialize/2 workaround (see the comment in VersionControl.cpp)
    jassertfalse;
}

void StashesRepository::deserialize(const ValueTree &tree, const DeltaDataLookup &dataLookup)
{
    this->reset();

    const auto root = tree.hasType(Serialization::VCS::stashesRepository) ?
        tree : tree.getChildWithName(Serialization::VCS::stashesRepository);

    if (!root.isValid()) { return; }

    const auto userStashesParams = root.getChildWithName(Serialization::VCS::userStashes);
    if (userStashesParams.isValid())
    {
        this->userStashes->deserialize(userStashesParams, dataLookup);
    }

    const auto quickStashParams = root.getChildWithName(Serialization::VCS::quickStash);
    if (quickStashParams.isValid())
    {
        this->quickStash->deserialize(quickStashParams, dataLookup);
    }
}

void StashesRepository::reset()
{
    this->userStashes->reset();
    this->quickStash->reset();
}
