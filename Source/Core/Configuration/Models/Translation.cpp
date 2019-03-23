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
#include "Translation.h"
#include "SerializationKeys.h"
#include "ResourceCache.h"

String Translation::getName() const noexcept
{
    return this->name;
}

String Translation::getId() const noexcept
{
    return this->id;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

ValueTree Translation::serialize() const
{
    ValueTree emptyXml(Serialization::Translations::locale);
    // TODO
    return emptyXml;
}

void Translation::deserialize(const ValueTree &tree)
{
    this->reset();
    using namespace Serialization;

    const auto root = tree.hasType(Translations::locale) ?
        tree : tree.getChildWithName(Translations::locale);

    if (!root.isValid()) { return; }

    this->id = root.getProperty(Translations::id).toString().toLowerCase();

    this->name = root.getProperty(Translations::name);
    this->author = root.getProperty(Translations::author);
    this->pluralEquation = Translations::wrapperClassName + "." +
        Translations::wrapperMethodName + "(" +
        root.getProperty(Translations::pluralEquation, "1").toString() + ")";

    forEachValueTreeChildWithType(root, pluralLiteral, Translations::pluralLiteral)
    {
        const String baseLiteral = pluralLiteral.getProperty(Translations::name);

        auto *formsAndTranslations = new TranslationMap();
        this->plurals[baseLiteral] = UniquePointer<TranslationMap>(formsAndTranslations);
        
        forEachValueTreeChildWithType(pluralLiteral, pluralTranslation, Translations::translation)
        {
            const String translatedLiteral = pluralTranslation.getProperty(Translations::name);
            const String pluralForm = pluralTranslation.getProperty(Translations::pluralForm);
            (*formsAndTranslations)[pluralForm] = translatedLiteral;
        }
    }

    forEachValueTreeChildWithType(root, literal, Translations::literal)
    {
        const String literalName = literal.getProperty(Translations::name);
        const String translatedLiteral = literal.getProperty(Translations::translation);
        this->singulars[literalName] = translatedLiteral;
    }
}

void Translation::reset()
{
    this->singulars.clear();
    this->plurals.clear();
}

//===----------------------------------------------------------------------===//
// BaseResource
//===----------------------------------------------------------------------===//

String Translation::getResourceId() const noexcept
{
    return this->id; // i.e. "en", "ru" - should be unique
}

Identifier Translation::getResourceType() const noexcept
{
    return Serialization::Resources::translations;
}
