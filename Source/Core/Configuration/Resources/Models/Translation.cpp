/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
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

bool Translation::isEmpty() const noexcept
{
    return this->singulars.empty();
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData Translation::serialize() const noexcept
{
    SerializedData emptyXml(Serialization::Translations::locale);
    jassertfalse; // translations are meant to be read-only
    return emptyXml;
}

void Translation::deserialize(const SerializedData &root) noexcept
{
    // don't reset so that user's translation appends the built-in one instead of replacing it
    // this->reset();

    using namespace Serialization;

    this->id = root.getProperty(Translations::localeId, String()).toString().toLowerCase();
    this->name = root.getProperty(Translations::localeName, String());

    forEachChildWithType(root, pluralLiteral, Translations::pluralLiteral)
    {
        const auto literalKey = I18n::Key(int64(pluralLiteral.getProperty(Translations::translationId)));
        if (literalKey == 0)
        {
            jassertfalse; // unsupported deprecated format 
            continue;
        }

        auto *formsAndTranslations = new Plurals();
        this->plurals[literalKey] = UniquePointer<Plurals>(formsAndTranslations);

        forEachChildWithType(pluralLiteral, pluralTranslation, Translations::translationValue)
        {
            const String translatedLiteral = pluralTranslation.getProperty(Translations::pluralName);
            const String pluralForm = pluralTranslation.getProperty(Translations::pluralForm);
            (*formsAndTranslations)[pluralForm] = translatedLiteral;
        }
    }

    forEachChildWithType(root, literal, Translations::literal)
    {
        const auto literalKey = I18n::Key(int64(literal.getProperty(Translations::translationId)));
        if (literalKey == 0)
        {
            jassertfalse; // unsupported deprecated format 
            continue;
        }

        const String translatedLiteral = literal.getProperty(Translations::translationValue);
        if (translatedLiteral.isEmpty())
        {
            jassertfalse; // unsupported deprecated format 
            continue;
        }

        this->singulars[literalKey] = translatedLiteral;
    }

    // and let's make sure we have no hash collisions in translation keys
#if DEBUG

    FlatHashSet<I18n::Key> usedKeys;

    // if any assertion is hit, consider renaming the newly added key
    forEachChildWithType(root, literal, Translations::literal)
    {
        const auto literalKey = I18n::Key(int64(literal.getProperty(Translations::translationId)));
        jassert(literalKey != 0);
        jassert(!usedKeys.contains(literalKey));
        usedKeys.insert(literalKey);
    }

    forEachChildWithType(root, pluralLiteral, Translations::pluralLiteral)
    {
        const auto literalKey = I18n::Key(int64(pluralLiteral.getProperty(Translations::translationId)));
        jassert(literalKey != 0);
        jassert(!usedKeys.contains(literalKey));
        usedKeys.insert(literalKey);
    }
#endif
}

void Translation::reset() noexcept
{
    this->singulars.clear();
    this->plurals.clear();
}

//===----------------------------------------------------------------------===//
// ConfigurationResource
//===----------------------------------------------------------------------===//

String Translation::getResourceId() const noexcept
{
    return this->id; // i.e. "en", "ru" - should be unique
}
