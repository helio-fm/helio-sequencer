/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Interpreter.h"
#include "Scale.h"
#include "KeySignatureEvent.h"
#include "Note.h"
#include "Arpeggiator.h"

namespace script::interop
{

// (beat key optional-length optional-velocity), e.g. (12.5 64 4.0 0.5)
Value::List makeNoteValue(const Note &note)
{
    Value::List result;
    result.add(Value(note.getBeat()));
    result.add(Value(note.getKey()));
    result.add(Value(note.getLength()));
    result.add(Value(note.getVelocity()));
    return result;
}

void validateNoteValue(const Value &value)
{
    if (!value.isListOf(Value::Type::Integer, Value::Type::Float) ||
        value.asList().size() < 2)
    {
        throw EvaluationError(EvaluationError::Type::InvalidArgument, "",
            "expected a note: (beat key optional-length optional-velocity), e.g. (12.5 64 4.0 1.0)");
    }
}

Note makeNote(const Value &value)
{
    validateNoteValue(value);

    const auto &args = value.asList();
    const auto beat = args[0].castToFloat();
    const auto key = args[1].castToInteger();
    const auto length = (args.size() > 2) ?
        args[2].castToFloat() : Globals::Defaults::newNoteLength;
    const auto velocity = (args.size() > 3) ?
        args[3].castToFloat() : Globals::Defaults::newNoteVelocity;

    return Note(nullptr, key, beat, length, velocity);
}

// (intervals), e.g. (2 2 1 2 2 2 1)
// note: using this intervals list instead of a key map
// to be able to reuse random scales from common sources,
// e.g. https://en.xen.wiki/w/22edo_modes or The Scale Omnibus
Value makeScaleValue(const Scale::Ptr scale)
{
    int prevKey = 0;
    Value::List result;
    for (const auto &key : scale->getKeys())
    {
        if (key > 0)
        {
            result.add(Value(key - prevKey));
            prevKey = key;
        }
    }

    result.add(Value(scale->getBasePeriod() - prevKey));
    return Value(move(result));
}

void validateScaleValue(const Value &value)
{
    if (!value.isListOf(Value::Type::Integer))
    {
        throw EvaluationError(EvaluationError::Type::InvalidArgument, "",
            "expected a scale: (intervals), e.g. (2 2 1 2 2 2 1)");
    }
}

Scale::Ptr makeScale(const Value &value,
    const Array<Scale::Ptr> &allScales, int basePeriodSize)
{
    validateScaleValue(value);

    int key = 0;
    Array<int> keys;
    for (const auto &intervalValue : value.asList())
    {
        keys.add(key);
        key = jlimit(0, basePeriodSize, key + intervalValue.castToInteger());
    }

    for (auto &scale : allScales)
    {
        if (scale->getKeys() == keys &&
            scale->getBasePeriod() == basePeriodSize)
        {
            return scale;
        }
    }

    StringArray defaultName;
    for (const auto &intervalValue : value.asList())
    {
        defaultName.add(String(intervalValue.castToInteger()));
    }

    return Scale::Ptr(new Scale(defaultName.joinIntoString(" "), keys, basePeriodSize));
}

// (beat key-offset scale), e.g. (62.5 7 (2 2 1 2 2 2 1))
Value makeKeySignatureValue(const KeySignatureEvent &key)
{
    Value::List result;
    result.add(Value(key.getBeat()));
    result.add(Value(key.getRootKey())); // todo key name
    result.add(makeScaleValue(key.getScale()));
    return Value(move(result));
}

void validateKeySignatureValue(const Value &value)
{
    if (!value.isList() ||
        value.asList().size() != 3 ||
        !value.asList().getUnchecked(0).isNumber() ||
        !value.asList().getUnchecked(1).isNumber())
    {
        throw EvaluationError(EvaluationError::Type::InvalidArgument, "",
            "expected a key: (beat key-offset scale), e.g. (62.5 7 (2 2 1 2 2 2 1))");
    }

    if (value.asList().size() >= 3)
    {
        validateScaleValue(value.asList().getLast());
    }
}

// todo working with key names?
KeySignatureEvent makeKeySignature(const Value &value,
    const Array<Scale::Ptr> &allScales, int basePeriodSize)
{
    validateKeySignatureValue(value);

    const auto &args = value.asList();
    const auto beat = args[0].castToFloat();
    const auto rootKey = Scale::wrapKey(args[1].castToInteger(), 0, basePeriodSize);
    const auto scale = makeScale(args[2], allScales, basePeriodSize);

    return KeySignatureEvent(nullptr, scale, beat, rootKey);
}

// (beat in-scale-key optional-length optional-velocity), e.g. (2.0 1 4.0 0.5)
Value::List makeArpeggiatorKeyValue(const Arpeggiator::Key key)
{
    Value::List result;
    result.add(Value(key.beat));
    result.add(Value((key.key + 1) + (key.period * 7)));
    result.add(Value(key.length));
    result.add(Value(key.velocity));
    return result;
}

void validateArpeggiatorKeyValue(const Value &value)
{
    if (!value.isListOf(Value::Type::Integer, Value::Type::Float) ||
        value.asList().size() < 3)
    {
        throw EvaluationError(EvaluationError::Type::InvalidArgument, "",
            "expected an in-scale note: (beat in-scale-key optional-length optional-velocity), e.g. (2.0 1 4.0 0.5)");
    }
}

Arpeggiator::Key makeArpeggiatorKey(const Value &value)
{
    validateArpeggiatorKeyValue(value);

    const auto &args = value.asList();
    const auto beat = args[0].castToFloat();
    const auto inScaleKey = args[1].castToInteger();
    const auto length = (args.size() > 2) ?
        args[2].castToFloat() : Globals::Defaults::newNoteLength;
    const auto velocity = (args.size() > 3) ?
        args[3].castToFloat() : Globals::Defaults::newNoteVelocity;

    Arpeggiator::Key key;
    key.beat = beat;
    key.isBarStart = (fmodf(beat, Globals::beatsPerBar) == 0.f);
    key.key = Scale::wrapKey((inScaleKey - 1), 0, 7);
    key.period = (inScaleKey - 1) / 7;
    key.length = length;
    key.velocity = velocity;

    return key;
}

void validateArpeggiatorValue(const Value &value)
{
    if (!value.isList() || value.asList().isEmpty())
    {
        throw EvaluationError(EvaluationError::Type::InvalidArgument, "",
            "expected a list of in-scale notes, e.g. ((0.0 1 4.0 0.69) (4.0 5 4.0 0.420) ...)");
    }
}

Arpeggiator::Ptr makeArpeggiator(const Value &value)
{
    validateArpeggiatorValue(value);

    Array<Arpeggiator::Key> keys;

    const auto &args = value.asList();
    for (const auto &arg : args)
    {
        keys.add(makeArpeggiatorKey(arg));
    }

    Arpeggiator::Ptr arp(new Arpeggiator("(temp arpeggiator)", move(keys)));
    return arp;
}

} // namespace script::interop
