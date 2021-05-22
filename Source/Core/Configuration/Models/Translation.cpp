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

SerializedData Translation::serialize() const
{
    SerializedData emptyXml(Serialization::Translations::locale);
    jassertfalse; // translations are meant to be read-only
    return emptyXml;
}

void Translation::deserialize(const SerializedData &data)
{
    // never reset here: translations are to be extended
    // this->reset();

    using namespace Serialization;

    const auto root = data.hasType(Translations::locale) ?
        data : data.getChildWithName(Translations::locale);

    if (!root.isValid()) { return; }

    this->id = root.getProperty(Translations::localeId).toString().toLowerCase();
    this->name = root.getProperty(Translations::localeName);

    this->pluralEquation = Translations::wrapperClassName + "." +
        Translations::wrapperMethodName + "(" +
        root.getProperty(Translations::pluralEquation, "1").toString() + ")";

    forEachChildWithType(root, pluralLiteral, Translations::pluralLiteral)
    {
        I18n::Key literalKey = I18n::Key(int64(pluralLiteral.getProperty(Translations::translationId)));
        if (literalKey == 0)
        {
            // deprecated format support
            const String literalName = pluralLiteral.getProperty(Translations::translationIdOld);
            literalKey = constexprHash(literalName.getCharPointer());
        }

        auto *formsAndTranslations = new Plurals();
        this->plurals[literalKey] = UniquePointer<Plurals>(formsAndTranslations);

        forEachChildWithType(pluralLiteral, pluralTranslation, Translations::translationValue)
        {
            const String translatedLiteral = pluralTranslation.getProperty(Translations::translationId);
            const String pluralForm = pluralTranslation.getProperty(Translations::pluralForm);
            (*formsAndTranslations)[pluralForm] = translatedLiteral;
        }

        // deprecated format support
        forEachChildWithType(pluralLiteral, pluralTranslation, Translations::translationValueOld)
        {
            const String translatedLiteral = pluralTranslation.getProperty(Translations::translationIdOld);
            const String pluralForm = pluralTranslation.getProperty(Translations::pluralForm);
            (*formsAndTranslations)[pluralForm] = translatedLiteral;
        }
    }

    forEachChildWithType(root, literal, Translations::literal)
    {
        auto literalKey = I18n::Key(int64(literal.getProperty(Translations::translationId)));
        if (literalKey == 0)
        {
            // deprecated format support
            const String literalName = literal.getProperty(Translations::translationIdOld);
            literalKey = constexprHash(literalName.getCharPointer());
        }

        String translatedLiteral = literal.getProperty(Translations::translationValue);
        if (translatedLiteral.isEmpty())
        {
            // deprecated format support
            translatedLiteral = literal.getProperty(Translations::translationValueOld);
        }

        jassert(translatedLiteral.isNotEmpty());
        this->singulars[literalKey] = translatedLiteral;
    }

    // and let's make sure we have no hash collisions in translation keys
#if DEBUG

    FlatHashSet<I18n::Key> usedKeys;

    // if any assertion is hit, consider renaming the newly added key
    forEachChildWithType(root, literal, Translations::literal)
    {
        I18n::Key literalKey = I18n::Key(int64(literal.getProperty(Translations::translationId)));
        if (literalKey == 0)
        {
            // deprecated format support
            const String literalName = literal.getProperty(Translations::translationIdOld);
            literalKey = constexprHash(literalName.getCharPointer());
        }

        jassert(literalKey != 0);
        jassert(!usedKeys.contains(literalKey));
        usedKeys.insert(literalKey);
    }

    forEachChildWithType(root, pluralLiteral, Translations::pluralLiteral)
    {
        I18n::Key literalKey = I18n::Key(int64(pluralLiteral.getProperty(Translations::translationId)));
        if (literalKey == 0)
        {
            // deprecated format support
            const String literalName = pluralLiteral.getProperty(Translations::translationIdOld);
            literalKey = constexprHash(literalName.getCharPointer());
        }

        jassert(literalKey != 0);
        jassert(!usedKeys.contains(literalKey));
        usedKeys.insert(literalKey);
    }
#endif
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
