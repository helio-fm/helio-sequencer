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
#include "NoteClass.h"
#include "SerializationKeys.h"
#include "ScriptingHelpers.h"

using namespace Scripting;
using namespace Serialization::Scripts;

NoteClass::NoteClass(const Note &note) : note(note)
{
    // TODO transform position
    this->setProperty(Api::Note::key, note.getKey());
    this->setProperty(Api::Note::position, note.getBeat());
    this->setProperty(Api::Note::length, note.getLength());
    this->setProperty(Api::Note::volume, note.getVelocity());
}

bool Scripting::NoteClass::hasBeenChanged() const
{
    // TODO
    return false;
}

Note Scripting::NoteClass::getUpdatedNote() const
{
    // TODO
    return Note();
}
