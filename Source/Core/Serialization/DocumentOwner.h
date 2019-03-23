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

#include "Document.h"

class DocumentOwner : public virtual ChangeBroadcaster
{
public:

    DocumentOwner(const String &name, const String &extension)
    {
        this->document.reset(new Document(*this, name, extension));
    }

    DocumentOwner(const File &existingFile)
    {
        this->document.reset(new Document(*this, existingFile));
    }

    Document *getDocument() const noexcept
    {
        return this->document.get();
    }

protected:

    virtual bool onDocumentLoad(File &file) = 0;
    virtual void onDocumentDidLoad(File &file) {}
    virtual bool onDocumentSave(File &file) = 0;
    virtual void onDocumentDidSave(File &file) {}
    virtual void onDocumentImport(File &file) = 0;
    virtual bool onDocumentExport(File &file) = 0;

    friend class Document;

private:

    UniquePointer<Document> document;

};
