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
    extension(defaultExtension)
{
    if (defaultName.isNotEmpty())
    {
        jassert(!defaultExtension.startsWithChar('.'));
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
    extension(existingFile.getFileExtension().replace(".", "")),
    workingFile(existingFile)
{
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

String Document::getFullPath() const
{
    return this->workingFile.getFullPathName();
}

void Document::revealToUser() const
{
    this->workingFile.revealToUser();
}

void Document::renameFile(const String &newName)
{
    if (newName == this->workingFile.getFileNameWithoutExtension())
    {
        return;
    }

    const auto safeNewName = File::createLegalFileName(newName).trimCharactersAtEnd(".");

    jassert(!this->extension.startsWithChar('.'));
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
    if (this->hasChanges &&
        this->workingFile.getFullPathName().isNotEmpty())
    {
        const String fullPath = this->workingFile.getFullPathName();
        const auto firstCharAfterLastSlash = fullPath.lastIndexOfChar(File::getSeparatorChar()) + 1;
        const auto lastDot = fullPath.lastIndexOfChar('.');
        const bool hasEmptyName = (lastDot == firstCharAfterLastSlash);
        if (hasEmptyName)
        {
            return;
        }

        const bool savedOk = this->owner.onDocumentSave(this->workingFile);

        if (savedOk)
        {
            this->hasChanges = false;
            DBG("Document saved: " + this->workingFile.getFullPathName());
            return;
        }

        DBG("Document save failed: " + this->workingFile.getFullPathName());
    }
}

void Document::exportAs(const String &exportExtension,
    const String &defaultFilenameWithExtension)
{
    if (!FileChooser::isPlatformDialogAvailable())
    {
        return;
    }

    this->exportFileChooser = make<FileChooser>(TRANS(I18n::Dialog::documentExport),
        DocumentHelpers::getDocumentSlot(File::createLegalFileName(defaultFilenameWithExtension)),
        exportExtension, true);

    DocumentHelpers::showFileChooser(this->exportFileChooser,
        Globals::UI::FileChooser::forFileToSave,
        [this](URL &url)
    {
        if (url.isLocalFile() && url.getLocalFile().exists())
        {
            url.getLocalFile().deleteFile();
        }

        if (auto outStream = url.createOutputStream())
        {
            outStream->setPosition(0); // just in case
            if (this->owner.onDocumentExport(*outStream.get()))
            {
                App::Layout().showTooltip(TRANS(I18n::Dialog::documentExportDone));
            }
        }
    });
}

//===----------------------------------------------------------------------===//
// Load
//===----------------------------------------------------------------------===//

bool Document::load(const File &file)
{
    if (!file.existsAsFile())
    {
        jassertfalse; // never happens?
        return false;
    }

    this->workingFile = file;
    this->hasChanges = false;

    if (!this->owner.onDocumentLoad(file))
    {
        DBG("Document load failed: " + this->workingFile.getFullPathName());
        return false;
    }

    return true;
}

void Document::import(const String &filePattern)
{
#if JUCE_ANDROID
    const auto filter = "*/*";
#else
    const auto filter = filePattern;
#endif

    this->importFileChooser = make<FileChooser>(TRANS(I18n::Dialog::documentImport),
        File::getSpecialLocation(File::userDocumentsDirectory), filter, true);

    DocumentHelpers::showFileChooser(this->importFileChooser,
        Globals::UI::FileChooser::forFileToOpen,
        [this](URL &url)
    {
        // todo someday:
        // on some platforms (Android, for example) it's not permitted to do any network
        // action from the message thread, so you must only call it from a background thread.

        if (auto inStream = url.createInputStream(false))
        {
            this->owner.onDocumentImport(*inStream.get());
        }
    });
}
