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

class DocumentHelpers final
{
public:

    static String getTemporaryFolder();
    static File getDocumentSlot(const String &fileName);
    static File getConfigSlot(const String &fileName);
    static File getTempSlot(const String &fileName);

    template<typename T>
    static ValueTree read(const String &data)
    {
        T serializer;
        ValueTree tree;
        serializer.loadFromString(data, tree);
        return tree;
    }

    // This tries to auto-detect a serializer type for a given file
    // by extension and return a valid value tree
    static ValueTree load(const File &file);
    static ValueTree load(const String &string);

    template<typename T>
    static ValueTree load(const File &file)
    {
        T serializer;
        ValueTree tree;
        serializer.loadFromFile(file, tree);
        return tree;
    }

    template<typename T>
    static bool save(const File &file, const ValueTree &tree)
    {
        T serializer;
        TempDocument tempDoc(file);
        if (serializer.saveToFile(tempDoc.getFile(), tree).wasOk())
        {
            return tempDoc.overwriteTargetFileWithTemporary();
        }

        return false;
    }

    template<typename T>
    static bool save(const File &file, const Serializable &serializable)
    {
        T serializer;
        TempDocument tempDoc(file);
        const auto treeNode(serializable.serialize());
        if (serializer.saveToFile(tempDoc.getFile(), treeNode).wasOk())
        {
            return tempDoc.overwriteTargetFileWithTemporary();
        }

        return false;
    }

    class TempDocument final
    {
    public:

        explicit TempDocument(const File &target);
        ~TempDocument();

        const File &getFile() const noexcept;
        const File &getTargetFile() const noexcept;

        bool overwriteTargetFileWithTemporary() const;
        bool deleteTemporaryFile() const;

    private:

        const File targetFile;
        const File temporaryFile;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempDocument)
    };
};
