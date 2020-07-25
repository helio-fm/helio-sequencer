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

#include "BaseResource.h"

class Temperament final : public BaseResource
{
public:

    Temperament() = default;
    Temperament(const Temperament &other) noexcept;
    explicit Temperament(const String &name) noexcept;

    String getResourceId() const noexcept override;
    Identifier getResourceType() const noexcept override;
    using Ptr = ReferenceCountedObjectPtr<Temperament>;
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override;
    void deserialize(const SerializedData &data) override;
    void reset() override;

    //===------------------------------------------------------------------===//
    // Operators
    //===------------------------------------------------------------------===//

    Temperament &operator=(const Temperament &other);
    friend bool operator==(const Temperament &l, const Temperament &r);
    friend bool operator!=(const Temperament &l, const Temperament &r);
    
    int hashCode() const noexcept;

private:

    String name;
    StringArray noteNames;

    int period = Globals::chromaticScaleSize;
    int middleC = Globals::middleC;

    JUCE_LEAK_DETECTOR(Temperament)
};
