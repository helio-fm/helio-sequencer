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

class SerializedData final
{
public:

    SerializedData() noexcept;
    explicit SerializedData(const Identifier &type);

    SerializedData(const SerializedData &) noexcept;
    SerializedData(SerializedData &&) noexcept;
    SerializedData &operator= (const SerializedData &);

    ~SerializedData();

    bool operator== (const SerializedData &) const noexcept;
    bool operator!= (const SerializedData &) const noexcept;

    bool isEquivalentTo(const SerializedData &) const;
    bool isValid() const noexcept;
    bool isEmpty() const noexcept;

    SerializedData createCopy() const;

    Identifier getType() const noexcept;
    bool hasType(const Identifier &type) const noexcept;

    const var &getProperty(const Identifier &name) const noexcept;
    var getProperty(const Identifier &name, const var &defaultValue) const;
    
    SerializedData &setProperty(const Identifier &name, const var &newValue);
    bool hasProperty(const Identifier &name) const noexcept;
    int getNumProperties() const noexcept;
    Identifier getPropertyName(int index) const noexcept;

    int getNumChildren() const noexcept;
    SerializedData getChild(int index) const;
    SerializedData getChildWithName(const Identifier &type) const;
    void addChild(const SerializedData &child, int index);
    void appendChild(const SerializedData &child);

    SerializedData getParent() const noexcept;

    UniquePointer<XmlElement> writeToXml() const;
    static SerializedData readFromXml(const XmlElement &xml);

    void writeToStream(OutputStream &output) const;
    static SerializedData readFromStream(InputStream &input);
    static SerializedData readFromData(const void *data, size_t numBytes);

    struct Iterator final
    {
        Iterator(const SerializedData &, bool isEnd);
        Iterator &operator++();

        bool operator== (const Iterator &) const;
        bool operator!= (const Iterator &) const;
        SerializedData operator*() const;

        using difference_type = std::ptrdiff_t;
        using value_type = SerializedData;
        using reference = SerializedData &;
        using pointer = SerializedData *;
        using iterator_category = std::forward_iterator_tag;

    private:
        void *internal;
    };

    Iterator begin() const noexcept;
    Iterator end() const noexcept;

private:

    class SharedData;
    ReferenceCountedObjectPtr<SharedData> data;
    
    friend class SharedData;
    explicit SerializedData(ReferenceCountedObjectPtr<SharedData>) noexcept;
    explicit SerializedData(SharedData &) noexcept;
};
