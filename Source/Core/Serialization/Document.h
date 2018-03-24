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

#include "Common.h"

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

    void changeListenerCallback(ChangeBroadcaster* source) override;

    File getFile() const;
    String getFullPath() const;

    void renameFile(const String &newName);

    //===------------------------------------------------------------------===//
    // Save
    //===------------------------------------------------------------------===//

    void save();
    void forceSave();
    void saveAs();
    void exportAs(const String &exportExtension,
                  const String &defaultFilename = "");

    void updateHash();
    bool hasUnsavedChanges() const noexcept;

    //===------------------------------------------------------------------===//
    // Load
    //===------------------------------------------------------------------===//

    bool load(const File &file, const File &relativeFile = {});
    void import(const String &filePattern);


protected:

    bool internalSave(File result);
    bool internalLoad(File result);
    bool fileHasBeenModified() const;

    int64 calculateStreamHashCode(InputStream &in) const;
    int64 calculateFileHashCode(const File &file) const;

protected:

    DocumentOwner &owner;

    bool hasChanges;
    File workingFile;
    String extension;
    Time fileModificationTime;
    int64 fileHashCode;
    int64 fileSize;

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Document)
};
