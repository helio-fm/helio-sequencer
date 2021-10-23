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

    // asks required permissions, if needed,
    // then displays the async file chooser,
    // then filters out the invalid results:
    static void showFileChooser(UniquePointer<FileChooser> &chooser,
        int flags, Function<void(URL &url)> successCallback);

    template <typename T>
    static SerializedData read(const String &data)
    {
        static T serializer;
        return serializer.loadFromString(data);
    }

    // This tries to auto-detect serializer type
    // for the given file by extension and return a valid tree
    static SerializedData load(const File &file);
    static SerializedData load(const String &string);

    template <typename T>
    static SerializedData load(const File &file)
    {
        static T serializer;
        return serializer.loadFromFile(file);
    }

    template <typename T>
    static SerializedData load(InputStream &stream)
    {
        static T serializer;
        return serializer.loadFromStream(stream);
    }

    template <typename T>
    static bool save(const File &file, const SerializedData &tree)
    {
        static T serializer;
        TempDocument tempDoc(file);
        if (serializer.saveToFile(tempDoc.getFile(), tree).wasOk())
        {
            return tempDoc.overwriteTargetFileWithTemporary();
        }

        return false;
    }

    template <typename T>
    static bool save(const File &file, const Serializable &serializable)
    {
        static T serializer;
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
