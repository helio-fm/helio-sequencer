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

#include "Common.h"
#include "Document.h"
#include "DocumentOwner.h"
#include "DocumentHelpers.h"
#include "MainLayout.h"

Document::Document(DocumentOwner &documentOwner,
    const String &defaultName,
    const String &defaultExtension) :
    owner(documentOwner),
    extension(defaultExtension),
    hasChanges(true)
{
    if (defaultName.isNotEmpty())
    {
        const auto safeName = File::createLegalFileName(defaultName + "." + defaultExtension);
        this->workingFile = DocumentHelpers::getDocumentSlot(safeName);
        if (this->workingFile.existsAsFile())
        {
            this->workingFile = this->workingFile.getNonexistentSibling(true);
        }
    }

    this->owner.addChangeListener(this);
}

Document::Document(DocumentOwner &documentOwner, const File &existingFile) :
    owner(documentOwner),
    extension(existingFile.getFileExtension().replace(",", ""))
{
    this->workingFile = existingFile;
    this->owner.addChangeListener(this);
}

Document::~Document()
{
    this->owner.removeChangeListener(this);
}

void Document::changeListenerCallback(ChangeBroadcaster *source)
{
    this->hasChanges = true;
}

File Document::getFile() const
{
    return this->workingFile;
}

String Document::getFullPath() const
{
    return this->workingFile.getFullPathName();
}

void Document::renameFile(const String &newName)
{
    if (newName == this->workingFile.getFileNameWithoutExtension())
    { return; }

    const String safeNewName = File::createLegalFileName(newName);

    File newFile(this->workingFile.getSiblingFile(safeNewName + "." + this->extension));

    if (newFile.existsAsFile())
    {
        newFile = newFile.getNonexistentSibling(true);
    }

    if (this->workingFile.moveFileTo(newFile))
    {
        DBG("Renaming to " + newFile.getFileName());
        this->workingFile = newFile;
    }
}


//===----------------------------------------------------------------------===//
// Save
//===----------------------------------------------------------------------===//

void Document::save()
{
    if (this->hasChanges && this->workingFile.getFullPathName().isNotEmpty())
    {
        this->internalSave(this->workingFile);
    }
}

void Document::forceSave()
{
    if (this->workingFile.getFullPathName().isNotEmpty())
    {
        this->internalSave(this->workingFile);
    }
}

void Document::saveAs()
{
#if HELIO_DESKTOP

        FileChooser fc(TRANS("dialog::document::save"),
                       File::getCurrentWorkingDirectory(), ("*." + this->extension), true);

        if (fc.browseForFileToSave(true))
        {
            this->internalSave(fc.getResult());
        }

#endif
}

void Document::exportAs(const String &exportExtension,
    const String &defaultFilenameWithExtension)
{
#if HELIO_DESKTOP

    FileChooser fc(TRANS("dialog::document::export"),
        DocumentHelpers::getDocumentSlot(File::createLegalFileName(defaultFilenameWithExtension)),
        exportExtension, true);

    if (fc.browseForFileToSave(true))
    {
        File result(fc.getResult());
        const bool savedOk = this->owner.onDocumentExport(result);
        
        if (savedOk)
        {
            App::Layout().showTooltip("dialog::document::export::done", 3000);
        }
    }

#endif
}


//===----------------------------------------------------------------------===//
// Load
//===----------------------------------------------------------------------===//

bool Document::load(const File &file, const File &relativeFile)
{
    if (!file.existsAsFile())
    {

        if (!relativeFile.existsAsFile())
        {
#if HELIO_DESKTOP
            FileChooser fc(TRANS("dialog::document::load"),
                File::getCurrentWorkingDirectory(), ("*." + this->extension), true);

            if (fc.browseForFileToOpen())
            {
                File result(fc.getResult());
                return this->internalLoad(result);
            }
#endif
        }
        else
        {
            return this->internalLoad(relativeFile);
        }
    }
    else
    {
        return this->internalLoad(file);
    }
    
    return false;
}

void Document::import(const String &filePattern)
{
#if HELIO_DESKTOP

    FileChooser fc(TRANS("dialog::document::import"),
                   File::getCurrentWorkingDirectory(), (filePattern), true);

    if (fc.browseForFileToOpen())
    {
        File result(fc.getResult());
        this->owner.onDocumentImport(result);
    }

#endif
}

bool Document::fileHasBeenModified() const
{
    return this->fileModificationTime != this->workingFile.getLastModificationTime()
           && (this->fileSize != this->workingFile.getSize()
               || this->calculateFileHashCode(this->workingFile) != this->fileHashCode);
}

void Document::updateHash()
{
    this->fileModificationTime = this->workingFile.getLastModificationTime();
    this->fileSize = this->workingFile.getSize();
    this->fileHashCode = this->calculateFileHashCode(this->workingFile);
}

bool Document::hasUnsavedChanges() const noexcept
{
    return this->hasChanges;
}

//===----------------------------------------------------------------------===//
// Protected
//===----------------------------------------------------------------------===//

int64 Document::calculateStreamHashCode(InputStream &in) const
{
    int64 t = 0;

    const int bufferSize = 4096;
    HeapBlock <uint8> buffer;
    buffer.malloc(bufferSize);

    for (;;)
    {
        const int num = in.read(buffer, bufferSize);

        if (num <= 0)
        { break; }

        for (int i = 0; i < num; ++i)
        { t = t * 65599 + buffer[i]; }
    }

    return t;
}

int64 Document::calculateFileHashCode(const File &file) const
{
    ScopedPointer<FileInputStream> stream(file.createInputStream());
    return stream != nullptr ? calculateStreamHashCode(*stream) : 0;
}

bool Document::internalSave(File result)
{
    const String fullPath = result.getFullPathName();
    const auto firstCharAfterLastSlash = fullPath.lastIndexOfChar(File::getSeparatorChar()) + 1;
    const auto lastDot = fullPath.lastIndexOfChar('.');
    const bool hasEmptyName = (lastDot == firstCharAfterLastSlash);
    if (hasEmptyName)
    {
        return false;
    }

    const bool savedOk = this->owner.onDocumentSave(result);

    if (savedOk)
    {
        this->workingFile = result;
        this->hasChanges = false;
        this->owner.onDocumentDidSave(result);
        DBG("Document saved: " + result.getFullPathName());
        return true;
    }

    DBG("Document save failed: " + result.getFullPathName());
    return false;
}

bool Document::internalLoad(File result)
{
    const bool loadedOk = this->owner.onDocumentLoad(result);

    if (loadedOk)
    {
        this->workingFile = result;
        this->hasChanges = false;
        this->owner.onDocumentDidLoad(result);
        return true;
    }
    
    DBG("Document load failed: " + result.getFullPathName());
    return false;
}
