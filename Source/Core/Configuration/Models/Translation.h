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

class Translation final : public Serializable, public ReferenceCountedObject
{
public:

    String getId() const noexcept;
    String getName() const noexcept;

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;

    typedef ReferenceCountedObjectPtr<Translation> Ptr;

    static int compareElements(const Translation &first, const Translation &second);
    static int compareElements(const Translation::Ptr first, const Translation::Ptr second);

private:

    String id;
    String name;
    String author;
    String pluralEquation;

    StringPairArray singulars;
    HashMap<String, StringPairArray> plurals;

    friend class TranslationsManager;

    JUCE_LEAK_DETECTOR(Translation)
};
