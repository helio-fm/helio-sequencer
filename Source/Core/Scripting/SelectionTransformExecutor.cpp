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
#include "SelectionTransformExecutor.h"
#include "NoteComponent.h"

#include "NoteClass.h"
#include "ScaleClass.h"
#include "KeySignatureClass.h"
#include "TimeSignatureClass.h"

using namespace Scripting;

SelectionTransformExecutor::SelectionTransformExecutor()
{
    // TODO init engine
}

Array<Note> Scripting::SelectionTransformExecutor::execute(Lasso &selection,
    TimeSignatureEvent &time, KeySignatureEvent &key)
{
    Array<var> selectionWrapper;
    for (int i = 0; i <= selection.getNumSelected(); ++i)
    {
        const auto *nc = selection.getItemAs<NoteComponent>(i);
        NoteClass::Ptr noteWrapper(new NoteClass(nc->getNote()));
        selectionWrapper.add(var(noteWrapper.get()));
    }

    TimeSignatureClass::Ptr timeSignatureWrapper(new TimeSignatureClass(time));
    KeySignatureClass::Ptr keySignatureWrapper(new KeySignatureClass(key));

    var args;
    args.append(var(selectionWrapper));
    args.append(var(timeSignatureWrapper.get()));
    args.append(var(keySignatureWrapper.get()));

    // TODO any root object API in future?
    var self(var::undefined());

    Result result = Result::fail("Unknown");

    const auto call = this->engine.callFunction("transform",
        var::NativeFunctionArgs(self, &args, args.size()), &result);

    Array<Note> updates;
    return updates;
    // TODO apply transforms?
}
