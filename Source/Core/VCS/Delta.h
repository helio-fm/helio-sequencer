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

#pragma once

#include "SerializationKeys.h"

namespace VCS
{
    class DeltaDescription final
    {
    public:

        DeltaDescription() :
            intParameter(defaultNumChanges),
            stringParameter() {}

        explicit DeltaDescription(const Identifier &text) :
            stringToTranslate(text.toString()),
            intParameter(defaultNumChanges),
            stringParameter() {}

        DeltaDescription(const String &text, int64 numChanges) :
            stringToTranslate(text),
            intParameter(numChanges),
            stringParameter() {}
        
        DeltaDescription(const String &text, const String &parameter) :
            stringToTranslate(text),
            intParameter(defaultNumChanges),
            stringParameter(parameter) {}
        
        DeltaDescription(const DeltaDescription &other) :
            stringToTranslate(other.stringToTranslate),
            intParameter(other.intParameter),
            stringParameter(other.stringParameter) {}

    private:
        
        static int64 defaultNumChanges;
        
        DeltaDescription(const String &text, int64 numChanges, const String &parameter) :
            stringToTranslate(text),
            intParameter(numChanges),
            stringParameter(parameter) {}

        String getFullText() const
        {
            if (this->intParameter != defaultNumChanges)
            {
                return TRANS_PLURAL(this->stringToTranslate, this->intParameter);
            }
            
            if (this->stringParameter.isNotEmpty())
            {
                return TRANS(this->stringToTranslate)
                    .replace(Serialization::Translations::metaSymbol,
                        this->stringParameter);
            }
            
            return TRANS(this->stringToTranslate);
        }
        
        String stringToTranslate;
        
        int64 intParameter;
        String stringParameter;
        
        friend class Delta;
    };
    
    class Delta final : public Serializable
    {
    public:

        Delta(const Delta &other);
        Delta(const DeltaDescription &deltaDescription, Identifier deltaType) :
            description(deltaDescription),
            type(deltaType) {}
        
        Delta *createCopy() const;

        // i.e. "added 45 notes" or "layer color changed"
        String getHumanReadableText() const;
        DeltaDescription getDescription() const;
        void setDescription(const DeltaDescription &newDescription);

        Identifier getType() const noexcept;
        bool hasType(const Identifier &id) const noexcept;

        //===--------------------------------------------------------------===//
        // Serializable
        //===--------------------------------------------------------------===//

        ValueTree serialize() const override;
        void deserialize(const ValueTree &tree) override;
        void reset() override;

    private:

        DeltaDescription description;
        Identifier type;

        JUCE_LEAK_DETECTOR(Delta)
    };

    struct DeltaDiff final
    {
        ScopedPointer<Delta> delta;
        ValueTree deltaData;
    };

    // A temporary workaround, see the comment in VersionControl::deserialize()
    typedef FlatHashMap<String, ValueTree, StringHash> DeltaDataLookup;

} // namespace VCS
