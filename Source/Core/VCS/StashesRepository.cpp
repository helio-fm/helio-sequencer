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
    userStashes(pack, "Stashes Root"),
    quickStash(pack)
{
    
}

int StashesRepository::getNumUserStashes() const
{
    return this->userStashes.getNumChildren();
}

String StashesRepository::getUserStashDescription(int index) const
{
    return this->getUserStash(index).getMessage();
}

Revision StashesRepository::getUserStash(int index) const
{
    return Revision(this->userStashes.getChild(index));
}

Revision StashesRepository::getUserStashWithName(const String &stashName) const
{
    for (int i = 0; i < this->userStashes.getNumChildren(); ++i)
    {
        Revision child(this->userStashes.getChild(i));
        
        if (child.getMessage() == stashName)
        {
            return child;
        }
    }
    
    return Revision(this->pack, "");
}

void StashesRepository::addStash(Revision newStash)
{
    this->userStashes.addChild(newStash, -1, nullptr);
    newStash.flushData();
    this->pack->flush();
}

void StashesRepository::removeStash(Revision stashToRemove)
{
    // todo sanity checks?
    this->userStashes.removeChild(stashToRemove, nullptr);
}

Revision StashesRepository::getQuickStash() const
{
    return this->quickStash;
}

bool StashesRepository::hasQuickStash() const
{
    return this->quickStash.isEmpty();
}

void StashesRepository::storeQuickStash(Revision newStash)
{
    this->quickStash.copyDeltasFrom(newStash);
    this->quickStash.setProperty(Serialization::VCS::commitMessage, Serialization::VCS::quickStashId, nullptr);
    this->quickStash.flushData();
    this->pack->flush();
}

void StashesRepository::resetQuickStash()
{
    this->quickStash = Revision(this->pack);
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *StashesRepository::serialize() const
{
    auto xml = new XmlElement(Serialization::VCS::stashesRepository);
    
    auto userStashesXml = new XmlElement(Serialization::VCS::userStashes);
    xml->addChildElement(userStashesXml);
    
    userStashesXml->addChildElement(this->userStashes.serialize());

    auto quickStashXml = new XmlElement(Serialization::VCS::quickStash);
    xml->addChildElement(quickStashXml);

    quickStashXml->addChildElement(this->quickStash.serialize());
    
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
        this->userStashes.deserialize(*userStashesXml);
    }
    
    XmlElement *quickStashXml = root->getChildByName(Serialization::VCS::quickStash);

    if (quickStashXml != nullptr) {
        this->quickStash.deserialize(*quickStashXml);
    }
}

void StashesRepository::reset()
{
    this->userStashes.reset();
    this->quickStash.reset();
}
