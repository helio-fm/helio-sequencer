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
    userStashes(Revision::create(pack, "Stashes Root")),
    quickStash(Revision::create(pack))
{
}

int StashesRepository::getNumUserStashes() const
{
    return this->userStashes.getNumChildren();
}

String StashesRepository::getUserStashDescription(int index) const
{
    return Revision::getMessage(this->getUserStash(index));
}

ValueTree VCS::StashesRepository::getUserStash(int index) const
{
    return this->userStashes.getChild(index);
}

ValueTree VCS::StashesRepository::getUserStashWithName(const String &stashName) const
{
    for (int i = 0; i < this->userStashes.getNumChildren(); ++i)
    {
        ValueTree child(this->userStashes.getChild(i));
        if (Revision::getMessage(child) == stashName)
        {
            return child;
        }
    }
    
    return Revision::create(this->pack);
}

void VCS::StashesRepository::addStash(ValueTree newStash)
{
    this->userStashes.addChild(newStash, -1, nullptr);
    Revision::flush(newStash);
    this->pack->flush();
}

void VCS::StashesRepository::removeStash(ValueTree stashToRemove)
{
    // todo sanity checks?
    this->userStashes.removeChild(stashToRemove, nullptr);
}

ValueTree VCS::StashesRepository::getQuickStash() const
{
    return this->quickStash;
}

bool StashesRepository::hasQuickStash() const
{
    return Revision::isEmpty(this->quickStash);
}

void VCS::StashesRepository::storeQuickStash(ValueTree newStash)
{
    Revision::copyDeltas(this->quickStash, newStash);
    this->quickStash.setProperty(Serialization::VCS::commitMessage, Serialization::VCS::quickStashId, nullptr);
    Revision::flush(this->quickStash);
    this->pack->flush();
}

void StashesRepository::resetQuickStash()
{
    this->quickStash = Revision::create(this->pack);
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *StashesRepository::serialize() const
{
    auto xml = new XmlElement(Serialization::VCS::stashesRepository);
    
    auto userStashesXml = new XmlElement(Serialization::VCS::userStashes);
    xml->addChildElement(userStashesXml);
    
    userStashesXml->addChildElement(Revision::serialize(this->userStashes));

    auto quickStashXml = new XmlElement(Serialization::VCS::quickStash);
    xml->addChildElement(quickStashXml);

    quickStashXml->addChildElement(Revision::serialize(this->quickStash));
    
    return xml;
}

void StashesRepository::deserialize(const XmlElement &xml)
{
    this->reset();
    
    const XmlElement *root = xml.hasTagName(Serialization::VCS::stashesRepository) ?
        &xml : xml.getChildByName(Serialization::VCS::stashesRepository);
    
    if (root == nullptr) { return; }

    XmlElement *userStashesXml = root->getChildByName(Serialization::VCS::userStashes);

    if (userStashesXml != nullptr) {
        Revision::deserialize(this->userStashes, *userStashesXml);
    }
    
    XmlElement *quickStashXml = root->getChildByName(Serialization::VCS::quickStash);

    if (quickStashXml != nullptr) {
        Revision::deserialize(this->quickStash, *quickStashXml);
    }
}

void StashesRepository::reset()
{
    Revision::reset(this->userStashes);
    Revision::reset(this->quickStash);
}
