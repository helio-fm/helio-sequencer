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

class DocumentOwner;

class Document : public ChangeListener
{
public:

    Document(DocumentOwner &documentOwner,
             const String &defaultName,
             const String &defaultExtension);

    Document(DocumentOwner &documentOwner,
             const File &existingFile);

    ~Document() override;

    String getFullPath() const;
    void revealToUser() const;

    void renameFile(const String &newName);


    //===------------------------------------------------------------------===//
    // Save
    //===------------------------------------------------------------------===//

    void save();
    void exportAs(const String &exportExtension,
        const String &defaultFilename = "");

    //===------------------------------------------------------------------===//
    // Load
    //===------------------------------------------------------------------===//

    bool load(const File &file);
    void import(const String &filePattern);

    void changeListenerCallback(ChangeBroadcaster* source) override;

private:

    DocumentOwner &owner;

    const String extension;
    bool hasChanges = true;
    File workingFile;

    // async-launched file choosers must have long enough lifetime
    UniquePointer<FileChooser> exportFileChooser;
    UniquePointer<FileChooser> importFileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Document)
};
