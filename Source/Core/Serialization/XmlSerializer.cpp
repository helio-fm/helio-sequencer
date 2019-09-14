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
#include "XmlSerializer.h"

static const String xmlEncoding = "UTF-8";

static XmlElement::TextFormat getXmlFormat()
{
    static XmlElement::TextFormat format;
    format.dtd = {};
    format.lineWrapLength = 120;
    format.customEncoding = xmlEncoding;
    return format;
}

Result XmlSerializer::saveToFile(File file, const SerializedData &tree) const
{
    UniquePointer<XmlElement> xml(tree.writeToXml());
    if (xml != nullptr)
    {
        const auto saved = xml->writeTo(file, getXmlFormat());
        return saved ? Result::ok() : Result::fail({});
    }

    return Result::fail({});
}

SerializedData XmlSerializer::loadFromFile(const File &file) const
{
    XmlDocument document(file);
    UniquePointer<XmlElement> xml(document.getDocumentElement());
    if (xml != nullptr)
    {
        return SerializedData::readFromXml(*xml);
    }

    return {};
}

Result XmlSerializer::saveToString(String &string, const SerializedData &tree) const
{
    UniquePointer<XmlElement> xml(tree.writeToXml());
    if (xml != nullptr)
    {
        string = xml->toString(getXmlFormat());
        return Result::ok();
    }

    return Result::fail({});
}

SerializedData XmlSerializer::loadFromString(const String &string) const
{
    XmlDocument document(string);
    UniquePointer<XmlElement> xml(document.getDocumentElement());
    return SerializedData::readFromXml(*xml);
}

bool XmlSerializer::supportsFileWithExtension(const String &extension) const
{
    return extension.endsWithIgnoreCase("xml") || extension.endsWithIgnoreCase("helio");
}

bool XmlSerializer::supportsFileWithHeader(const String &header) const
{
    return header.startsWithIgnoreCase("<?xml");
}
