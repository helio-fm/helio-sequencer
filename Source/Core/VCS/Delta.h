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
    class DeltaDescription
    {
    public:
        
        explicit DeltaDescription(String text) :
        stringToTranslate(std::move(text)), intParameter(defaultNumChanges), stringParameter(String::empty) {}
        
        DeltaDescription(String text, int64 numChanges) :
            stringToTranslate(std::move(text)), intParameter(numChanges), stringParameter(String::empty) {}
        
        DeltaDescription(String text, String parameter) :
            stringToTranslate(std::move(text)), intParameter(defaultNumChanges), stringParameter(std::move(parameter)) {}
        
        DeltaDescription(const DeltaDescription &other) :
            stringToTranslate(other.stringToTranslate),
            intParameter(other.intParameter),
            stringParameter(other.stringParameter) {}

    private:
        
        static int64 defaultNumChanges;
        
        DeltaDescription(String text, int64 numChanges, String parameter) :
            stringToTranslate(std::move(text)), intParameter(numChanges), stringParameter(std::move(parameter)) {}

        String getFullText() const
        {
            if (this->intParameter != defaultNumChanges)
            {
                return TRANS_PLURAL(this->stringToTranslate, this->intParameter);
            }
            
            if (this->stringParameter.isNotEmpty())
            {
                return TRANS(this->stringToTranslate).replace(Serialization::Locales::metaSymbol, this->stringParameter);
            }
            
            return TRANS(this->stringToTranslate);
        }
        
        String stringToTranslate;
        
        int64 intParameter;
        String stringParameter;
        
        friend class Delta;
    };
    
    class Delta : public Serializable
    {
    public:

        Delta(const Delta &other);

        Delta(const DeltaDescription &deltaDescription, String deltaType) :
            description(deltaDescription),
            type(std::move(deltaType))
        {}

        ~Delta() override {}
        
        // i.e. "added 45 notes" or "layer color changed"
        String getHumanReadableText() const;

        DeltaDescription getDescription() const;

        void setDescription(const DeltaDescription &newDescription);
        
        Uuid getUuid() const;

        String getType() const;


        //===------------------------------------------------------------------===//
        // Serializable
        //

        ValueTree serialize() const override;

        void deserialize(const ValueTree &tree) override;

        void reset() override;

    private:

        Uuid vcsUuid;

        DeltaDescription description;

        String type;

        JUCE_LEAK_DETECTOR(Delta)

    };

    struct NewSerializedDelta
    {
        Delta *delta;
        XmlElement *deltaData;
    };
} // namespace VCS
