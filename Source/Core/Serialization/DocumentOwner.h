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

    explicit DocumentOwner(const File &existingFile) :
        document(make<Document>(*this, existingFile)) {}

    DocumentOwner(const String &name, const String &extension) :
        document(make<Document>(*this, name, extension)) {}

    Document *getDocument() const noexcept
    {
        return this->document.get();
    }

protected:

    virtual bool onDocumentLoad(const File &file) = 0;
    virtual bool onDocumentSave(const File &file) = 0;
    virtual void onDocumentImport(InputStream &stream) = 0;
    virtual bool onDocumentExport(OutputStream &stream) = 0;

    friend class Document;

private:

    const UniquePointer<Document> document;

};
