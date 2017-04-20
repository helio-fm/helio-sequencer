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

Revision::Revision() :
    ValueTree(Identifier(Serialization::VCS::revision))
{
}

Revision::Revision(Pack::Ptr pack, const String &name) :
    ValueTree(Identifier(Serialization::VCS::revision))
{
    Uuid id;
    this->setProperty(Serialization::VCS::commitId, var(id.toString()), nullptr);
    this->setProperty(Serialization::VCS::commitMessage, var(name), nullptr);
    this->setProperty(Serialization::VCS::commitTimeStamp, var(Time::getCurrentTime().toMilliseconds()), nullptr);
    this->setProperty(Serialization::VCS::commitVersion, var(int64(1)), nullptr);
    this->setProperty(Serialization::VCS::pack, var(pack), nullptr);
}

Revision::Revision(ValueTree other) :
    ValueTree(std::move(other))
{
}

void Revision::copyPropertiesFrom(const Revision &other)
{
    // убрать все свойства, кроме пака
    Pack::Ptr pack(this->getPackPtr());
    this->removeAllProperties(nullptr);
    this->setProperty(Serialization::VCS::pack, var(pack), nullptr);

    for (int i = 0; i < other.getNumProperties(); ++i)
    {
        const Identifier id(other.getPropertyName(i));

        // пак пропускаем
        if (id.toString() == Serialization::VCS::pack)
        { continue; }

        const var& property(other.getProperty(id));

        if (RevisionItem *revItem = dynamic_cast<RevisionItem *>(property.getObject()))
        {
            this->copyProperty(id, revItem);
        }
        else
        {
            // остальное копируем через setProperty
            this->setProperty(id, property, nullptr);
        }
    }
}

void Revision::copyDeltasFrom(const Revision &other)
{
    this->resetAllDeltas();
    
    for (int i = 0; i < other.getNumProperties(); ++i)
    {
        const Identifier id(other.getPropertyName(i));
        const var& property(other.getProperty(id));
        
        if (RevisionItem *revItem = dynamic_cast<RevisionItem *>(property.getObject()))
        {
            this->copyProperty(id, revItem);
        }
    }
}

void Revision::resetAllDeltas()
{
    for (int i = 0; i < this->getNumProperties(); )
    {
        const Identifier id(this->getPropertyName(i));
        const var property(this->getProperty(id));
        
        if (RevisionItem *revItem = dynamic_cast<RevisionItem *>(property.getObject()))
        {
            this->removeProperty(id, nullptr);
        }
        else
        {
            ++i;
        }
    }
}

void Revision::copyProperty(Identifier id, const RevisionItem::Ptr itemToCopy)
{
    if (this->getPackPtr() == itemToCopy->getPackPtr())
    {
        this->setProperty(id, var(itemToCopy), nullptr);
    }
    else
    {
        // для ситуации, когда мы мержим два дерева с разными паками -
        // айтемы копируем через конструктор айтема, чтоб забрать данные из старого пака
        RevisionItem::Ptr newItem(new RevisionItem(this->getPackPtr(), itemToCopy->getType(), itemToCopy));
        this->setProperty(id, var(newItem), nullptr);
    }
}


RevisionItem::Ptr Revision::getItemWithUuid(const Uuid &uuid)
{
    for (int i = 0; i < this->getNumProperties(); ++i)
    {
        const Identifier id(this->getPropertyName(i));
        const var property(this->getProperty(id));

        if (RevisionItem *revItem = dynamic_cast<RevisionItem *>(property.getObject()))
        {
            if (revItem->getUuid() == uuid) { return revItem; }
        }
    }

    return nullptr;
}

void Revision::flushData()
{
    for (int i = 0; i < this->getNumProperties(); ++i)
    {
        const Identifier id(this->getPropertyName(i));
        const var property(this->getProperty(id));

        if (property.isObject())
        {
            if (RevisionItem *revItem = dynamic_cast<RevisionItem *>(property.getObject()))
            {
                revItem->flushData();
            }
        }
    }
}

Pack::Ptr Revision::getPackPtr() const
{
    ReferenceCountedObject *rc =
        this->getProperty(Serialization::VCS::pack).getObject();

    if (Pack *pack = dynamic_cast<Pack *>(rc))
    {
        return Pack::Ptr(pack);
    }

    return nullptr;
}

String Revision::getMessage() const
{
    return this->getProperty(Serialization::VCS::commitMessage).toString();
}

String Revision::getUuid() const
{
    return this->getProperty(Serialization::VCS::commitId).toString();
}

int64 Revision::getVersion() const
{
    return this->getProperty(Serialization::VCS::commitVersion);
}

int64 Revision::getTimeStamp() const
{
    return this->getProperty(Serialization::VCS::commitTimeStamp);
}

void Revision::incrementVersion()
{
    this->setProperty(Serialization::VCS::commitVersion, int64(this->getVersion() + 1), nullptr);
}

MD5 Revision::calculateHash() const
{
    StringArray sum;

    // идем по всем свойствам, кроме ссылки на пак
    for (int i = 0; i < this->getNumProperties(); ++i)
    {
        const Identifier id(this->getPropertyName(i));

        // пак пропускаем
        if (id.toString() == Serialization::VCS::pack)
        { continue; }

        const var property(this->getProperty(id));

        if (RevisionItem *revItem = dynamic_cast<RevisionItem *>(property.getObject()))
        {
            sum.add(revItem->getUuid().toString());
        }
    }

    sum.sort(true);
    return MD5(sum.joinIntoString("").toUTF8());
}

bool Revision::isEmpty() const
{
    return (this->getMessage().isEmpty());
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *Revision::serialize() const
{
    XmlElement *const xml = new XmlElement(this->getType().toString());

    for (int i = 0; i < this->getNumProperties(); ++i)
    {
        const Identifier id(this->getPropertyName(i));
        const var property(this->getProperty(id));

        // сохраняем только RevisionItem'ы, строки и int64
        if (property.isObject())
        {
            if (const RevisionItem *revItem = dynamic_cast<RevisionItem *>(property.getObject()))
            {
                xml->prependChildElement(revItem->serialize());
            }
        }
        else if (property.isString() || property.isInt64())
        {
            xml->setAttribute(id.toString(), property.toString());
        }
    }

    for (int i = 0; i < this->getNumChildren(); ++i)
    {
        const Revision child(this->getChild(i));
        xml->prependChildElement(child.serialize());
    }

    return xml;
}

void Revision::deserialize(const XmlElement &xml)
{
    this->reset();

    const XmlElement *mainSlot = xml.hasTagName(this->getType().toString()) ?
                                 &xml : xml.getChildByName(this->getType().toString());

    if (mainSlot == nullptr) { return; }

    for (int i = 0; i < mainSlot->getNumAttributes(); ++i)
    {
        const String &name = mainSlot->getAttributeName(i);
        const String &value = mainSlot->getAttributeValue(i);
        this->setProperty(Identifier(name), var(value), nullptr);
    }

    forEachXmlChildElement(*mainSlot, e)
    {
        if (e->getTagName() == this->getType().toString())
        {
            Revision child(this->createCopy());
            child.deserialize(*e);
            this->addChild(child, 0, nullptr);
        }
        else
        {
            RevisionItem::Ptr item(new RevisionItem(this->getPackPtr(), RevisionItem::Undefined, nullptr));
            item->deserialize(*e);
            this->setProperty(item->getUuid().toString(), var(item), nullptr);
        }
    }
}

void Revision::reset()
{
    //this->removeAllProperties(nullptr); // свойства удалять нельзя?

    // убрать все свойства, кроме пака:
    //Pack::Ptr pack(this->getPackPtr());
    //this->removeAllProperties(nullptr);
    //this->setProperty(Serialization::VCS::pack, var(pack), nullptr);

    this->resetAllDeltas();
    this->removeAllChildren(nullptr);
}
