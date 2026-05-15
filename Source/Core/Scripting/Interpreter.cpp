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

    This scripting language implementation is based on a Lisp interpreter
    by Adam McDaniel, distributed under Apache-2.0 license,
    see README.md for details.
*/

#include "Common.h"
#include "Interpreter.h"

namespace script
{

//===----------------------------------------------------------------------===//
// Errors
//===----------------------------------------------------------------------===//

String EvaluationError::getDescription() const noexcept
{
    const auto descriptionOrType =
        this->description.isEmpty() ?
        this->getTypeName() : this->description;

    if (this->symbolName.isEmpty() ||
        this->symbolName == descriptionOrType)
    {
        return descriptionOrType;
    }

    return this->symbolName + ": " + descriptionOrType;
}

String EvaluationError::getTypeName() const noexcept
{
    switch (this->type)
    {
    case Type::TooFewArguments: return "too few arguments";
    case Type::TooManyArguments: return "too many arguments";
    case Type::InvalidArgument: return "invalid argument";
    case Type::CalledNonFunction: return "called non function";
    case Type::InvalidBinaryOperation: return "invalid binary operation";
    case Type::CannotCastToInt: return "cannot cast to int";
    case Type::CannotCastToFloat: return "cannot cast to float";
    case Type::ExpectedString: return "expected a string";
    case Type::ExpectedSymbol: return "expected a symbol";
    case Type::ExpectedList: return "expected a list";
    case Type::UndefinedSymbol: return "undefined symbol";
    case Type::IndexOutOfRange: return "index out of range";
    case Type::StackOverflow: return "too deep for me";
    case Type::TimeLimitExceed: return "evaluation time limit exceed";
    case Type::BreakpointHit: return "breakpoint hit";
    case Type::Aborted: return "aborted";
    default: return {};
    }
}

//===----------------------------------------------------------------------===//
// Built-in functions
//===----------------------------------------------------------------------===//

inline void checkNumArgs(const Value::List &args, int number)
{
    if (args.size() != number)
    {
        throw EvaluationError(args.size() > number ?
            EvaluationError::Type::TooManyArguments :
            EvaluationError::Type::TooFewArguments);
    }
}

inline Value::List evaluateArgs(const Value::List &args, Scope &scope, EvaluationContext &context)
{
    Value::List result;
    result.ensureStorageAllocated(args.size());

    for (const auto &value : args)
    {
        result.add(value.evaluate(scope, context));
    }

    return result;
}

namespace builtin
{

namespace specialForms // no argument evaluation
{

// Clojure-like -> and ->> arrows:
// the initial form is inserted as first or last argument,
// e.g. (->> (list 1 2 3) (map (lambda (x) (* 2 x))) first)
// is equal to (first (map (lambda (x) (* 2 x)) (list 1 2 3)))
Value arrow(const Value::List &unevaluatedArgs,
    Scope &scope, EvaluationContext &context, bool insertAsFirstArgument)
{
    if (unevaluatedArgs.size() < 2)
    {
        throw EvaluationError(EvaluationError::Type::TooFewArguments);
    }

    const auto &initialForm = unevaluatedArgs.getReference(0);
    Value acc = initialForm;

    for (int i = 1; i < unevaluatedArgs.size(); i++)
    {
        // if it's a symbol, expect a function name
        // if it's a list, expect a function, maybe with arguments
        auto &form = unevaluatedArgs.getReference(i);
        const auto formCodeRange = form.getSourceCodeRange();

        if (form.isSymbol())
        {
            Value::List list;
            list.add(form);
            list.add(move(acc));
            acc = Value(move(list), formCodeRange);
            continue;
        }

        Value::List modifiedForm = form.asList();
        if (modifiedForm.isEmpty())
        {
            throw form.makeError(EvaluationError::Type::TooFewArguments);
        }

        if (insertAsFirstArgument)
        {
            modifiedForm.insert(1, move(acc));
        }
        else
        {
            modifiedForm.add(move(acc));
        }

        acc = Value(move(modifiedForm), formCodeRange);
    }

    // DBG(acc.debug());
    return acc.evaluate(scope, context);
}

Value arrowFirst(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    return arrow(unevaluatedArgs, scope, context, true);
}

Value arrowLast(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    return arrow(unevaluatedArgs, scope, context, false);
}

Value beginBlock(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    Value acc;

    for (const auto &value : unevaluatedArgs)
    {
        acc = value.evaluate(scope, context);
    }

    return acc;
}

Value ifThenElse(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    checkNumArgs(unevaluatedArgs, 3);

    if (unevaluatedArgs.getReference(0).evaluate(scope, context).castToBoolean())
    {
        return unevaluatedArgs.getReference(1).evaluate(scope, context);
    }
    else
    {
        return unevaluatedArgs.getReference(2).evaluate(scope, context);
    }
}

Value cond(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    if (unevaluatedArgs.size() < 1)
    {
        throw EvaluationError(EvaluationError::Type::TooFewArguments);
    }

    for (const auto &value : unevaluatedArgs)
    {
        const auto &testAndActions = value.asList();
        if (testAndActions.size() < 2)
        {
            throw value.makeError(EvaluationError::Type::TooFewArguments);
        }

        const auto tested = testAndActions.getReference(0).evaluate(scope, context);
        if (tested.castToBoolean() || (tested.isSymbol() && tested.asSymbol() == "else"))
        {
            Value::List body;
            for (int i = 1; i < testAndActions.size(); i++)
            {
                body.add(testAndActions.getUnchecked(i));
            }

            return Value::makeProgram(move(body)).evaluate(scope, context);
        }
    }

    return {};
}

Value define(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    if (unevaluatedArgs.size() < 2)
    {
        throw EvaluationError(EvaluationError::Type::TooFewArguments);
    }

    const auto &firstArgument = unevaluatedArgs.getReference(0);

    // it's a name binding?
    if (firstArgument.isSymbol())
    {
        if (unevaluatedArgs.size() > 2)
        {
            throw EvaluationError(EvaluationError::Type::TooManyArguments);
        }

        Value result = unevaluatedArgs.getReference(1).evaluate(scope, context);
        scope.setValue(firstArgument.toString(), move(result));
        return firstArgument;
    }

    // it's a function definition?
    const auto &declaration = firstArgument.asList();
    if (declaration.isEmpty())
    {
        throw firstArgument.makeError(EvaluationError::Type::TooFewArguments);
    }
    else if (!declaration.getReference(0).isSymbol())
    {
        throw firstArgument.makeError(EvaluationError::Type::ExpectedSymbol);
    }

    auto parametersRange = firstArgument.getSourceCodeRange();

    Value::List parameters;
    for (int i = 1; i < declaration.size(); i++)
    {
        parameters.add(declaration.getUnchecked(i));
    }

    Value::List body;
    for (int i = 1; i < unevaluatedArgs.size(); i++)
    {
        body.add(unevaluatedArgs.getUnchecked(i));
    }

    const auto functionName = declaration.getReference(0).toString();
    scope.setValue(functionName,
        Value::makeClosure(context, functionName,
            move(parameters), move(parametersRange),
            Value::makeProgram(move(body)), scope));

    return Value::makeSymbol(functionName);
}

Value let(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    if (unevaluatedArgs.size() < 2)
    {
        throw EvaluationError(EvaluationError::Type::TooFewArguments);
    }

    const auto &bindings = unevaluatedArgs.getReference(0).asList();
    if (bindings.isEmpty())
    {
        throw unevaluatedArgs.getReference(0).makeError(EvaluationError::Type::TooFewArguments);
    }

    Scope localScope;
    localScope.setParent(&scope);
    for (const auto &binding : bindings)
    {
        const auto &list = binding.asList();
        checkNumArgs(list, 2);
        localScope.setValue(list.getReference(0).asSymbol(),
            list.getReference(1).evaluate(localScope, context));
    }

    Value::List body;
    for (int i = 1; i < unevaluatedArgs.size(); i++)
    {
        body.add(unevaluatedArgs.getUnchecked(i));
    }

    const auto lambda = Value::makeClosure(context, {}, {}, {},
        Value::makeProgram(move(body)), localScope);

    return lambda.apply({}, localScope, context);
}

Value lambda(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    if (unevaluatedArgs.size() < 2)
    {
        throw EvaluationError(EvaluationError::Type::TooFewArguments);
    }

    auto parameters = unevaluatedArgs.getReference(0).asList();
    if (parameters.isEmpty())
    {
        throw unevaluatedArgs.getReference(0).makeError(EvaluationError::Type::TooFewArguments);
    }

    Range<int> parametersRange =
        unevaluatedArgs.getReference(0).getSourceCodeRange();

    Value::List body;
    for (int i = 1; i < unevaluatedArgs.size(); i++)
    {
        body.add(unevaluatedArgs.getUnchecked(i));
    }

    return Value::makeClosure(context, {},
        move(parameters), move(parametersRange),
        Value::makeProgram(move(body)), scope);
}

Value whileLoop(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    if (unevaluatedArgs.size() < 2)
    {
        throw EvaluationError(EvaluationError::Type::TooFewArguments);
    }

    Value acc;

    while (unevaluatedArgs.getReference(0).evaluate(scope, context).castToBoolean())
    {
        for (int i = 1; i < unevaluatedArgs.size() - 1; i++)
        {
            unevaluatedArgs.getReference(i).evaluate(scope, context);
        }

        acc = unevaluatedArgs.getReference(unevaluatedArgs.size() - 1).evaluate(scope, context);
    }

    return acc;
}

Value scope(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    Scope e = scope;
    Value acc;

    for (const auto &value : unevaluatedArgs)
    {
        acc = value.evaluate(e, context);
    }

    return acc;
}

Value quote(const Value::List &unevaluatedArgs, Scope &, EvaluationContext &)
{
    Value::List list;

    for (const auto &value : unevaluatedArgs)
    {
        list.add(value);
    }

    return Value(move(list));
}

} // namespace specialForms

namespace meta
{

Value parse(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);

    if (!args.getReference(0).isString())
    {
        throw EvaluationError(EvaluationError::Type::InvalidArgument);
    }

    Array<Range<int>> blockRanges;
    return script::parse(args.getReference(0).asString(), blockRanges);
}

Value evaluate(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);
    return args.getReference(0).evaluate(scope, context);
}

Value getTypeName(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);
    return Value::makeString(args.getReference(0).getTypeName());
}

} // namespace meta

namespace math
{

Value sum(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);

    if (args.size() < 2)
    {
        throw EvaluationError(EvaluationError::Type::TooFewArguments);
    }

    Value acc = args.getFirst();
    for (int i = 1; i < args.size(); i++)
    {
        acc = acc + args.getReference(i);
    }

    return acc;
}

Value subtract(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);
    return args.getReference(0) - args.getReference(1);
}

Value multiply(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);

    if (args.size() < 2)
    {
        throw EvaluationError(EvaluationError::Type::TooFewArguments);
    }

    Value acc = args.getFirst();
    for (int i = 1; i < args.size(); i++)
    {
        acc = acc * args.getReference(i);
    }

    return acc;
}

Value divide(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);
    return args.getReference(0) / args.getReference(1);
}

Value remainder(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);
    return args.getReference(0) % args.getReference(1);
}

Value abs(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);

    if (!args.getReference(0).isNumber())
    {
        throw EvaluationError(EvaluationError::Type::InvalidArgument,
            args.getReference(0).debug());
    }

    if (args.getReference(0).isFloat())
    {
        return Value(std::abs(args.getReference(0).castToFloat()));
    }

    return Value(std::abs(args.getReference(0).castToInteger()));
}

Value min(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    if (args.size() < 2)
    {
        throw EvaluationError(EvaluationError::Type::TooFewArguments);
    }

    Value result = args.getFirst();
    for (int i = 1; i < args.size(); i++)
    {
        if (result > args.getReference(i))
        {
            result = args.getReference(i);
        }
    }

    return result;
}

Value max(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    if (args.size() < 2)
    {
        throw EvaluationError(EvaluationError::Type::TooFewArguments);
    }

    Value result = args.getFirst();
    for (int i = 1; i < args.size(); i++)
    {
        if (result < args.getReference(i))
        {
            result = args.getReference(i);
        }
    }

    return result;
}

Value logicalAnd(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    if (unevaluatedArgs.size() < 2)
    {
        throw EvaluationError(EvaluationError::Type::TooFewArguments);
    }

    // short-circuit, evaluating from left to right
    for (const auto &value : unevaluatedArgs)
    {
        if (!value.evaluate(scope, context).castToBoolean())
        {
            return Value(false);
        }
    }

    return Value(true);
}

Value logicalOr(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    if (unevaluatedArgs.size() < 2)
    {
        throw EvaluationError(EvaluationError::Type::TooFewArguments);
    }

    for (const auto &value : unevaluatedArgs)
    {
        if (value.evaluate(scope, context).castToBoolean())
        {
            return Value(true);
        }
    }

    return Value(false);
}

Value logicalNot(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);
    return Value(!args.getReference(0).castToBoolean());
}

Value sin(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);
    return Value(std::sin(args.getReference(0).castToFloat()));
}

Value cos(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);
    return Value(std::cos(args.getReference(0).castToFloat()));
}

Value round(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);
    return Value(std::round(args.getReference(0).castToFloat()));
}

Value floor(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);
    return Value(std::floor(args.getReference(0).castToFloat()));
}

} // namespace math

namespace comparison
{

Value equal(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);
    return Value(args.getReference(0) == args.getReference(1));
}

Value notEqual(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);
    return Value(args.getReference(0) != args.getReference(1));
}

Value greater(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);
    return Value(args.getReference(0) > args.getReference(1));
}

Value less(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);
    return Value(args.getReference(0) < args.getReference(1));
}

Value greaterOrEqual(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);
    return Value(args.getReference(0) >= args.getReference(1));
}

Value lessOrEqual(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);
    return Value(args.getReference(0) <= args.getReference(1));
}

} // namespace comparison

namespace list
{

Value make(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    return Value(evaluateArgs(unevaluatedArgs, scope, context));
}

Value index(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);

    const auto i = args.getReference(0).castToInteger();
    const auto &list = args.getReference(1).asList();
    if (list.isEmpty() || i >= list.size())
    {
        throw args.getReference(0).makeError(EvaluationError::Type::IndexOutOfRange);
    }

    return list.getUnchecked(i);
}

Value insert(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 3);

    auto list = args.getReference(0).asList();
    const auto i = args.getReference(1).castToInteger();
    if (i > list.size())
    {
        throw EvaluationError(EvaluationError::Type::IndexOutOfRange);
    }

    list.insert(args.getReference(1).castToInteger(), args.getReference(2));
    return Value(move(list));
}

Value remove(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);

    auto list = args.getReference(0).asList();
    const auto i = args.getReference(1).castToInteger();
    if (list.isEmpty() || i >= list.size())
    {
        throw EvaluationError(EvaluationError::Type::IndexOutOfRange);
    }

    list.remove(i);
    return Value(move(list));
}

Value length(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);
    return Value(Value::Integer(args.getReference(0).asList().size()));
}

Value empty(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);
    return Value(args.getReference(0).asList().isEmpty());
}

Value append(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    auto args = evaluateArgs(unevaluatedArgs, scope, context);

    if (args.size() == 0)
    {
        throw EvaluationError(EvaluationError::Type::TooFewArguments);
    }

    auto &result = args.getReference(0);
    for (int i = 1; i < args.size(); i++)
    {
        const auto &otherList = args.getReference(i).asList();
        for (int j = 0; j < otherList.size(); j++)
        {
            result.push(otherList.getUnchecked(j));
        }
    }

    return result;
}

Value reverse(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);

    const auto &list = args.getReference(0).asList();
    Value::List result;
    for (int i = list.size(); i --> 0 ;)
    {
        result.add(list.getUnchecked(i));
    }

    return Value(move(result));
}

Value head(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);

    const auto &list = args.getReference(0).asList();
    if (list.isEmpty())
    {
        throw EvaluationError(EvaluationError::Type::IndexOutOfRange);
    }

    return list.getFirst();
}

Value tail(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);

    Value::List result;
    const auto &list = args.getReference(0).asList();
    for (int i = 1; i < list.size(); i++)
    {
        result.add(list.getUnchecked(i));
    }

    return Value(move(result));
}

Value first(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    if (args.size() == 1)
    {
        const auto &list = args.getReference(0).asList();
        if (list.isEmpty())
        {
            throw EvaluationError(EvaluationError::Type::IndexOutOfRange);
        }

        return list.getFirst();
    }
    else if (args.size() == 2)
    {
        const auto numElements = args.getReference(0).castToInteger();
        const auto &list = args.getReference(1).asList();
        if (numElements <= 0 || numElements > list.size())
        {
            throw EvaluationError(EvaluationError::Type::IndexOutOfRange);
        }

        Value::List result;
        for (int i = 0; i < numElements; i++)
        {
            result.add(list.getUnchecked(i));
        }

        return Value(move(result));
    }

    throw EvaluationError(args.size() > 2 ?
        EvaluationError::Type::TooManyArguments :
        EvaluationError::Type::TooFewArguments);
}

Value last(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    if (args.size() == 1)
    {
        const auto &list = args.getReference(0).asList();
        if (list.isEmpty())
        {
            throw EvaluationError(EvaluationError::Type::IndexOutOfRange);
        }

        return list.getLast();
    }
    else if (args.size() == 2)
    {
        const auto numElements = args.getReference(0).castToInteger();
        const auto &list = args.getReference(1).asList();
        if (numElements <= 0 || numElements > list.size())
        {
            throw EvaluationError(EvaluationError::Type::IndexOutOfRange);
        }

        Value::List result;
        for (int i = list.size() - numElements; i < list.size(); i++)
        {
            result.add(list.getUnchecked(i));
        }

        return Value(move(result));
    }

    throw EvaluationError(args.size() > 2 ?
        EvaluationError::Type::TooManyArguments :
        EvaluationError::Type::TooFewArguments);
}

Value range(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);

    Value::List result;
    Value low = args[0];
    if (!low.isNumber())
    {
        throw low.makeError(EvaluationError::Type::InvalidArgument);
    }

    Value high = args[1];
    if (!high.isNumber())
    {
        throw high.makeError(EvaluationError::Type::InvalidArgument);
    }

    if (low >= high)
    {
        return Value(move(result));
    }

    while (low < high)
    {
        result.add(low);
        low = low + Value(1);
    }

    return Value(move(result));
}
} // namespace list

namespace functional
{

Value map(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);

    Value::List result, tmp;
    const auto &list = args.getReference(1).asList();
    for (int i = 0; i < list.size(); i++)
    {
        tmp.add(list.getUnchecked(i));
        result.add(args.getReference(0).apply(tmp, scope, context));
        tmp.clear();
    }

    return Value(move(result));
}

Value filter(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 2);

    Value::List result, tmp;
    const auto &list = args.getReference(1).asList();
    for (int i = 0; i < list.size(); i++)
    {
        const auto &current = list.getReference(i);
        tmp.add(current);
        if (args.getReference(0).apply(tmp, scope, context).castToBoolean())
        {
            result.add(current);
        }
        tmp.clear();
    }

    return Value(move(result));
}

Value reduce(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 3);

    Value::List tmp;
    auto acc = args[1];
    const auto &list = args.getReference(2).asList();
    jassert(args.getReference(0).isClosure());
    for (int i = 0; i < list.size(); i++)
    {
        tmp.add(acc);
        tmp.add(list.getUnchecked(i));
        acc = args.getReference(0).apply(tmp, scope, context);
        tmp.clear();
    }

    return acc;
}

} // namespace functional

namespace cast
{

Value toFloat(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);
    return Value(args.getReference(0).castToFloat());
}

Value toInt(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);
    return Value(args.getReference(0).castToInteger());
}

} // namespace cast

Value debug(const Value::List &unevaluatedArgs, Scope &scope, EvaluationContext &context)
{
    const auto args = evaluateArgs(unevaluatedArgs, scope, context);
    checkNumArgs(args, 1);
    return Value::makeString(args.getReference(0).toString());
}

} // namespace builtin

bool Scope::hasValue(const String &name) const
{
    if (this->symbols.find(name) != this->symbols.end())
    {
        return true;
    }
    else if (this->parentScope != nullptr)
    {
        return this->parentScope->hasValue(name);
    }

    return false;
}

const Value &Scope::findValue(const String &name) const
{
    const auto foundSymbol = this->symbols.find(name);
    if (foundSymbol != this->symbols.end())
    {
        return foundSymbol->second;
    }
    else if (this->parentScope != nullptr)
    {
        return this->parentScope->findValue(name);
    }

    jassertfalse; // please check hasValue before calling this
    throw EvaluationError(EvaluationError::Type::UndefinedSymbol, name);
}

Value Scope::makeValue(EvaluationContext &context, const String &name) const
{
    // the context can override or add any keyword:
    if (auto extension = context.makeLanguageExtension(name))
    {
        return *extension;
    }

    // the user can locally shadow any function name, except context-specific
    if (this->hasValue(name))
    {
        return this->findValue(name);
    }

    using namespace builtin;

    if (name == "parse") return Value::makeBuiltInFunction(name, meta::parse);
    if (name == "eval") return Value::makeBuiltInFunction(name, meta::evaluate);
    if (name == "type") return Value::makeBuiltInFunction(name, meta::getTypeName);

    if (name == "->") return Value::makeBuiltInFunction(name, specialForms::arrowFirst);
    if (name == "->>") return Value::makeBuiltInFunction(name, specialForms::arrowLast);
    if (name == "begin") return Value::makeBuiltInFunction(name, specialForms::beginBlock);
    if (name == "if") return Value::makeBuiltInFunction(name, specialForms::ifThenElse);
    if (name == "cond") return Value::makeBuiltInFunction(name, specialForms::cond);
    if (name == "else") return Value::makeSymbol(name); // a self-evaluating symbol only used in "cond"
    if (name == "while") return Value::makeBuiltInFunction(name, specialForms::whileLoop);
    if (name == "scope") return Value::makeBuiltInFunction(name, specialForms::scope);
    if (name == "quote") return Value::makeBuiltInFunction(name, specialForms::quote);
    if (name == "define") return Value::makeBuiltInFunction(name, specialForms::define);
    if (name == "let") return Value::makeBuiltInFunction(name, specialForms::let);
    if (name == "lambda" || name == CharPointer_UTF8("\xce\xbb"))
        return Value::makeBuiltInFunction(name, specialForms::lambda);

    if (name == "=") return Value::makeBuiltInFunction(name, comparison::equal);
    if (name == "!=") return Value::makeBuiltInFunction(name, comparison::notEqual);
    if (name == ">") return Value::makeBuiltInFunction(name, comparison::greater);
    if (name == "<") return Value::makeBuiltInFunction(name, comparison::less);
    if (name == ">=") return Value::makeBuiltInFunction(name, comparison::greaterOrEqual);
    if (name == "<=") return Value::makeBuiltInFunction(name, comparison::lessOrEqual);

    if (name == "+") return Value::makeBuiltInFunction(name, math::sum);
    if (name == "-") return Value::makeBuiltInFunction(name, math::subtract);
    if (name == "*") return Value::makeBuiltInFunction(name, math::multiply);
    if (name == "/") return Value::makeBuiltInFunction(name, math::divide);
    if (name == "%") return Value::makeBuiltInFunction(name, math::remainder);
    if (name == "abs") return Value::makeBuiltInFunction(name, math::abs);
    if (name == "min") return Value::makeBuiltInFunction(name, math::min);
    if (name == "max") return Value::makeBuiltInFunction(name, math::max);
    if (name == "and") return Value::makeBuiltInFunction(name, math::logicalAnd);
    if (name == "not") return Value::makeBuiltInFunction(name, math::logicalNot);
    if (name == "or") return Value::makeBuiltInFunction(name, math::logicalOr);
    if (name == "sin") return Value::makeBuiltInFunction(name, math::sin);
    if (name == "cos") return Value::makeBuiltInFunction(name, math::cos);
    if (name == "round") return Value::makeBuiltInFunction(name, math::round);
    if (name == "floor") return Value::makeBuiltInFunction(name, math::floor);

    if (name == "list") return Value::makeBuiltInFunction(name, list::make);
    if (name == "insert") return Value::makeBuiltInFunction(name, list::insert);
    if (name == "nth") return Value::makeBuiltInFunction(name, list::index);
    if (name == "remove") return Value::makeBuiltInFunction(name, list::remove);
    if (name == "length") return Value::makeBuiltInFunction(name, list::length);
    if (name == "empty") return Value::makeBuiltInFunction(name, list::empty);
    if (name == "append") return Value::makeBuiltInFunction(name, list::append);
    if (name == "reverse") return Value::makeBuiltInFunction(name, list::reverse);
    if (name == "head") return Value::makeBuiltInFunction(name, list::head);
    if (name == "tail") return Value::makeBuiltInFunction(name, list::tail);
    if (name == "first") return Value::makeBuiltInFunction(name, list::first);
    if (name == "last") return Value::makeBuiltInFunction(name, list::last);
    if (name == "range") return Value::makeBuiltInFunction(name, list::range);

    if (name == "map") return Value::makeBuiltInFunction(name, functional::map);
    if (name == "filter") return Value::makeBuiltInFunction(name, functional::filter);
    if (name == "reduce") return Value::makeBuiltInFunction(name, functional::reduce);

    if (name == "int") return Value::makeBuiltInFunction(name, cast::toInt);
    if (name == "float") return Value::makeBuiltInFunction(name, cast::toFloat);

    if (name == "debug") return Value::makeBuiltInFunction(name, builtin::debug);

    if (name == "true" || name == "#t") return Value(true);
    if (name == "false" || name == "#f") return Value(false);

    if (name == "pi" || name == CharPointer_UTF8("\xcf\x80"))
        return Value(MathConstants<Value::Float>::pi);

    if (name == "nil") return {};

    throw EvaluationError(EvaluationError::Type::UndefinedSymbol, name);
}

void Scope::setValue(const String &name, const Value &value) noexcept
{
    this->symbols[name] = value;
}

void Scope::setValue(const String &name, Value &&value) noexcept
{
    this->symbols[name] = move(value);
}

void Scope::include(const Scope &other)
{
    for (auto const &[otherName, otherValue] : other.symbols)
    {
        this->symbols[otherName] = otherValue;
    }
}

String Scope::toString() const
{
    String result;

    result += "{ ";
    for (auto const &[name, value] : this->symbols)
    {
        result += ('\'' + name + "' : " + value.toString() + ", ");
    }
    result += "}";

    return result;
}

StringArray Scope::findAllFunctionNames() const
{
    StringArray result;

    for (auto const &[name, value] : this->symbols)
    {
        if (value.isClosure())
        {
            result.add(name);
        }
    }

    return result;
}

//===----------------------------------------------------------------------===//
// Apply/eval
//===----------------------------------------------------------------------===//

struct ScopedStackSizeCounter final
{
    explicit ScopedStackSizeCounter(EvaluationContext &context) :
        context(context)
    {
        this->context.currentCallStackSize += 1;
    }

    ~ScopedStackSizeCounter()
    {
        this->context.currentCallStackSize -= 1;
        jassert(context.currentCallStackSize.get() >= 0);
    }

    EvaluationContext &context;
};

Value Value::apply(const List &args, Scope &scope, EvaluationContext &context) const
{
    if (context.shouldAbort())
    {
        throw this->makeError(EvaluationError::Type::Aborted);
    }

    if (context.getMaxEvaluationTimeMs() <
        int(Time::getMillisecondCounter() - context.startTime.get()))
    {
        throw this->makeError(EvaluationError::Type::TimeLimitExceed);
    }

    if (context.currentCallStackSize.get() > context.getMaxCallStackSize())
    {
        throw this->makeError(EvaluationError::Type::StackOverflow);
    }

    const ScopedStackSizeCounter stackSizeCounter(context);

    switch (this->type)
    {
    case Type::Closure:
    {
        if (this->list.size() < 2 ||
            !this->list.getReference(0).isList())
        {
            jassertfalse;
            throw this->makeError(EvaluationError::Type::InvalidArgument);
        }

        const auto &params = this->list.getReference(0);
        if (params.list.size() < args.size())
        {
            throw this->makeError(EvaluationError::Type::TooManyArguments);
        }

        // DBG("Applying " + this->toString());
        // DBG("Params " + params.toString());
        // DBG("Args " + Value(List(args)).toString());

        jassert(!this->closureCaptures.isEmpty());
        const auto &captures = this->closureCaptures.getReference(0);

        Scope evaluationScope = captures;
        evaluationScope.setParent(&scope);

        // insert the arguments into the scope
        for (int i = 0; i < params.list.size(); i++)
        {
            const auto &parameterValue = params.list.getReference(i);

            // symbol = parameter name, list = optional parameter with default value
            if (!parameterValue.isSymbol() && !parameterValue.isList())
            {
                throw this->makeError(EvaluationError::Type::InvalidArgument);
            }

            String parameterName;
            Range<int> parameterListRange;
            Optional<Value> defaultValue;

            if (parameterValue.isSymbol())
            {
                parameterName = parameterValue.asSymbol();
                parameterListRange = params.sourceCodeRange;
            }
            else if (parameterValue.isList())
            {
                const auto &optionalParameter = parameterValue.asList();
                if (optionalParameter.size() != 2 ||
                    !optionalParameter.getReference(0).isSymbol())
                {
                    parameterValue.makeError(EvaluationError::Type::InvalidArgument);
                }

                parameterName = optionalParameter.getReference(0).asSymbol();
                parameterListRange = parameterValue.sourceCodeRange;
                defaultValue = optionalParameter.getReference(1);
            }

            jassert(parameterName.isNotEmpty());
            if (!captures.hasValue(parameterName))
            {
                if (args.size() > i)
                {
                    if (context.shouldBreakAt(parameterValue, parameterListRange))
                    {
                        throw EvaluationError(EvaluationError::Type::BreakpointHit,
                            args.getReference(i).getTypeName(),
                            // parameterName + ", " + args.getReference(i).getTypeName(),
                            args.getReference(i).debug());
                    }

                    // DBG("Set " + parameterName + " to " + args.getReference(i).toString());
                    evaluationScope.setValue(parameterName, args.getReference(i));
                }
                else if (defaultValue.hasValue())
                {
                    Scope defaultScope;
                    defaultScope.setParent(&evaluationScope);
                    const auto evaluatedDefault = defaultValue->evaluate(defaultScope, context);

                    jassert(parameterValue.isList());
                    if (context.shouldBreakAt(parameterValue.asList().getReference(0), parameterListRange))
                    {
                        throw EvaluationError(EvaluationError::Type::BreakpointHit,
                            evaluatedDefault.getTypeName(),
                            // parameterName + ", " + evaluatedDefault.getTypeName(),
                            evaluatedDefault.debug());
                    }

                    evaluationScope.setValue(parameterName, evaluatedDefault);
                }
                else
                {
                    throw this->makeError(EvaluationError::Type::TooFewArguments);
                }
            }
            else if (context.shouldBreakAt(parameterValue, params.sourceCodeRange))
            {
                const auto &capturedValue = captures.findValue(parameterName);
                throw EvaluationError(EvaluationError::Type::BreakpointHit,
                    capturedValue.getTypeName(),
                    // parameterName + ", " + capturedValue.getTypeName(),
                    capturedValue.debug());
            }
        }

        return this->list.getReference(1).evaluate(evaluationScope, context);
    }
    case Type::BuiltInFunction:
        try
        {
            jassert(this->builtInFunction != nullptr);
            return (this->builtInFunction)(args, scope, context);
        }
        catch (EvaluationError error)
        {
            if (this->string.isNotEmpty() && error.symbolName.isEmpty())
            {
                error.symbolName = this->string;
            }

            throw error;
        }
    default:
        throw this->makeError(EvaluationError::Type::CalledNonFunction);
    }
}

Value Value::evaluate(Scope &scope, EvaluationContext &context) const
{
    switch (this->type)
    {
    case Type::Quote:
        jassert(!this->list.isEmpty());
        return this->list.getFirst();
    case Type::Symbol:
        jassert(!this->string.isEmpty());
        return scope.makeValue(context, this->string);
    case Type::Program:
    {
        if (this->list.isEmpty())
        {
            return *this;
        }

        try
        {
            Value acc;
            for (const auto &item : this->list)
            {
                acc = item.evaluate(scope, context);
            }

            return acc;
        }
        catch (EvaluationError error)
        {
            // if it hasn't been set by the child scope:
            if (error.sourceCodeRange.isEmpty())
            {
                error.sourceCodeRange = this->sourceCodeRange;
            }

            throw error;
        }
    }
    case Type::List:
    {
        if (this->list.isEmpty())
        {
            // empty list evaluates to itself (it is also the false value)
            return *this;
        }

        try
        {
            const auto &head = this->list.getReference(0);
            auto evaluatedHead = head.evaluate(scope, context);

            if (!evaluatedHead.isClosure() && !evaluatedHead.isBuiltInFunction())
            {
                throw EvaluationError(EvaluationError::Type::CalledNonFunction, head.debug());
            }

            // built-in functions can be special forms,
            // so we leave them to evaluate their arguments:
            Value::List arguments;
            for (int i = 1; i < this->list.size(); i++)
            {
                if (evaluatedHead.isBuiltInFunction())
                {
                    arguments.add(this->list.getUnchecked(i));
                }
                else
                {
                    arguments.add(this->list.getReference(i).evaluate(scope, context));
                }
            }

            auto result = evaluatedHead.apply(arguments, scope, context);

            if (context.hasBreakpoints())
            {
                if (context.shouldBreakAt(head, this->sourceCodeRange))
                {
                    throw EvaluationError(EvaluationError::Type::BreakpointHit,
                        "evaluated to " + result.getTypeName(),
                        //(evaluatedHead.debug() == result.getTypeName() ? result.getTypeName() :
                        //    evaluatedHead.debug() + " evaluated to " + result.getTypeName()),
                        result.debug());
                }

                for (int i = 1; i < this->list.size(); i++)
                {
                    const auto &unevaluated = this->list.getReference(i);
                    if (context.shouldBreakAt(unevaluated, this->sourceCodeRange))
                    {
                        Value evaluated;
                        try
                        {
                            evaluated = unevaluated.evaluate(scope, context);
                        }
                        catch (...)
                        {
                            break;
                        }

                        throw EvaluationError(EvaluationError::Type::BreakpointHit,
                            evaluated.getTypeName(),
                            //unevaluated.string + ", " + evaluated.getTypeName(),
                            evaluated.debug());
                    }
                }
            }

            return result;
        }
        catch (EvaluationError error)
        {
            // if it hasn't been set by the child scope:
            if (error.sourceCodeRange.isEmpty())
            {
                error.sourceCodeRange = this->sourceCodeRange;
            }

            throw error;
        }
    }
    default:
        return *this;
    }
}

Value Value::makeClosure(EvaluationContext &context,
    const String &name, List &&params, Range<int> &&paramsRange,
    Value &&body, Scope &scope)
{
    Scope captures;

    for (const auto &usedSymbol : body.getUsedSymbols())
    {
        if (scope.hasValue(usedSymbol))
        {
            captures.setValue(usedSymbol,
                scope.makeValue(context, usedSymbol));
        }
    }

    for (const auto &param : params)
    {
        if (param.isList() && param.asList().size() == 2)
        {
            for (const auto &usedSymbol : param.asList().getReference(1).getUsedSymbols())
            {
                if (scope.hasValue(usedSymbol))
                {
                    captures.setValue(usedSymbol,
                        scope.makeValue(context, usedSymbol));
                }
            }
        }
    }

    Value result(Type::Closure);
    result.string = name;
    result.list.add(Value(move(params), move(paramsRange)));
    jassert(body.type == Type::Program);
    result.list.add(move(body));
    result.closureCaptures.add(move(captures));
    return result;
}

//===----------------------------------------------------------------------===//
// Parsing
//===----------------------------------------------------------------------===//

String ParsingError::getDescription() const noexcept
{
    switch (this->type)
    {
    case Type::InvalidString: return "invalid string";
    case Type::InvalidNumber: return "invalid number";
    case Type::UnexpectedEndOfProgram: return "unexpected end of program";
    case Type::MismatchedParentheses: return "mismatched parentheses";
    case Type::SyntaxError: return "syntax error";
    default: return {};
    }
}

// a wrapper around CharPointerType to track symbol indices in a string:
struct CharPointer final
{
    int position = 0;
    String::CharPointerType pointer;

    explicit CharPointer(String::CharPointerType p) :
        pointer(p) {}

    juce_wchar getAndAdvance() noexcept
    {
        this->position++;
        return this->pointer.getAndAdvance();
    }

    bool isDigit() const noexcept
    {
        return this->pointer.isDigit();
    }

    bool operator==(CharPointer other) const noexcept
    {
        return this->pointer == other.pointer;
    }

    juce_wchar operator*() const noexcept
    {
        return *this->pointer;
    }

    juce_wchar operator[](int characterIndex) const noexcept
    {
        return this->pointer[characterIndex];
    }

    void operator+=(int numToSkip) noexcept
    {
        this->pointer += numToSkip;
        this->position += numToSkip;
    }

    CharPointer &operator++() noexcept
    {
        this->pointer++;
        this->position++;
        return *this;
    }

    CharPointer findEndOfWhitespace() const noexcept
    {
        CharPointer result(*this);

        while (result.pointer.isWhitespace())
        {
            result.position++;
            result.pointer++;
        }

        return result;
    }
};

static void findNextNewline(CharPointer &t)
{
    juce_wchar c = 0;
    do
    {
        if (*t.pointer == 0)
        {
            break;
        }
        c = t.getAndAdvance();
    } while (c != '\n' && c != '\r');
}

static void skipCommentsAndWhitespaces(CharPointer &t)
{
    t = t.findEndOfWhitespace();
    auto t2 = t;
    if (t2.getAndAdvance() == ';')
    {
        t = t2;
        findNextNewline(t);
        if (*t.pointer == 0)
        {
            return;
        }
        skipCommentsAndWhitespaces(t);
    }
}

static Value parseString(CharPointer &t)
{
    static MemoryOutputStream buffer(512);
    buffer.reset();

    const auto oldT = t;

    for (;;)
    {
        auto c = t.getAndAdvance();
        if (c == '"') { break; }
        if (c == '\\')
        {
            c = t.getAndAdvance();
            switch (c)
            {
            case '"': case '\'': case '\\': case '/': break;
            case 'a': c = '\a'; break;
            case 'b': c = '\b'; break;
            case 'f': c = '\f'; break;
            case 'n': c = '\n'; break;
            case 'r': c = '\r'; break;
            case 't': c = '\t'; break;
            case 'u':
            {
                c = 0;
                for (int i = 4; --i >= 0;)
                {
                    const auto digitValue =
                        CharacterFunctions::getHexDigitValue(t.getAndAdvance());

                    if (digitValue < 0)
                    {
                        throw ParsingError(ParsingError::Type::InvalidString);
                    }

                    c = (juce_wchar)((c << 4) + juce_wchar(digitValue));
                }
                break;
            }
            }
        }

        if (c == 0)
        {
            throw ParsingError(ParsingError::Type::UnexpectedEndOfProgram,
                { oldT.position - 1, t.position });
        }

        buffer.appendUTF8Char(c);
    }

    return Value::makeString(buffer.toUTF8());
}

static Value parseNumber(CharPointer &t, bool isNegative)
{
    const auto oldT = t;
    int intValue = t.getAndAdvance() - '0';
    jassert(intValue >= 0 && intValue < 10);
    for (;;)
    {
        auto previousChar = t;
        auto c = t.getAndAdvance();
        auto digit = ((int)c) - '0';

        if (isPositiveAndBelow(digit, 10))
        {
            intValue = intValue * 10 + digit;
            continue;
        }

        if (c == 'e' || c == 'E' || c == '.')
        {
            t = oldT;
            const auto asDouble = CharacterFunctions::readDoubleValue(t);
            return Value(float(isNegative ? -asDouble : asDouble));
        }

        if (CharacterFunctions::isWhitespace(c) ||
            c == '(' || c == ')' || c == '"' || c == '\'' || c == 0)
        {
            t = previousChar;
            break;
        }

        throw ParsingError(ParsingError::Type::InvalidNumber,
            { oldT.position, t.position });
    }

    return Value(isNegative ? -intValue : intValue);
}

bool isSymbolBody(juce_wchar ch) noexcept
{
    return CharacterFunctions::isLetterOrDigit(ch) || ch == '_' || ch == '~' ||
        (ch >= 35 && ch <= 38) || (ch >= 42 && ch <= 47) || (ch >= 58 && ch <= 64);
        //(ch >= '#' && ch <= '&') || (ch >= '*' && ch <= '/') || (ch >= ':' && ch <= '@');
};

static Value parseValue(CharPointer &t, Array<Range<int>> &blockRanges)
{
    skipCommentsAndWhitespaces(t);

    const auto blockStartPosition = t.position;

    auto t2 = t;
    auto c = t2.getAndAdvance();

    switch (c)
    {
    case 0:
        jassertfalse;
        throw ParsingError(ParsingError::Type::UnexpectedEndOfProgram);
    case '\'':
        t = t2;
        return Value::makeQuote(parseValue(t, blockRanges));
    case '(':
    {
        t = t2;
        auto result = Value::List();

        try
        {
            for (;;)
            {
                skipCommentsAndWhitespaces(t);

                auto oldT = t;
                c = t.getAndAdvance();

                if (c == ')') { break; }
                if (c == 0)
                {
                    throw ParsingError(ParsingError::Type::MismatchedParentheses,
                        { blockStartPosition, t.position });
                }

                t = oldT;
                auto value = parseValue(t, blockRanges);
                result.add(move(value));
            }

            const Range<int> sourceCodeRange(blockStartPosition, t.position);
            blockRanges.add(sourceCodeRange);

            return Value(move(result), sourceCodeRange);
        }
        catch (ParsingError error)
        {
            if (error.sourceCodeRange.isEmpty())
            {
                error.sourceCodeRange = { blockStartPosition, t.position };
            }

            throw error;
        }
    }
    case '"':
        t = t2;
        return parseString(t);
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return parseNumber(t, false);
    case '-':
        if (CharacterFunctions::isDigit(*t2.pointer))
        {
            t = t2;
            return parseNumber(t, true);
        }
    default:
        if (isSymbolBody(c))
        {
            static MemoryOutputStream buffer(256);
            buffer.reset();

            for (;;)
            {
                auto previousChar = t;
                c = t.getAndAdvance();

                if (isSymbolBody(c))
                {
                    buffer.appendUTF8Char(c);
                    continue;
                }

                t = previousChar;
                break;
            }

            return Value::makeSymbol(buffer.toUTF8(),
                { blockStartPosition, t.position });
        }
    }

    throw ParsingError(c == ')' ?
        ParsingError::Type::MismatchedParentheses : ParsingError::Type::SyntaxError,
        { blockStartPosition, blockStartPosition + 1 });
}

Value parse(const String &string, Array<Range<int>> &outBlockRanges)
{
    Value::List list;
    CharPointer p(string.getCharPointer());
    skipCommentsAndWhitespaces(p);
    while (*p.pointer != 0)
    {
        list.add(parseValue(p, outBlockRanges));
        skipCommentsAndWhitespaces(p);
    }

    return Value::makeProgram(move(list));
}

//===----------------------------------------------------------------------===//
// Tests
//===----------------------------------------------------------------------===//

#if JUCE_UNIT_TESTS

class ScriptInterpreterTests final : public UnitTest
{
public:

    ScriptInterpreterTests() :
        UnitTest("Script interpreter tests", UnitTestCategories::helio) {}

    struct TestContext final : public script::EvaluationContext
    {
        Optional<script::Value> makeLanguageExtension(const String &) const override { return {}; }
        bool shouldAbort() const override { return false; }
        bool hasBreakpoints() const override { return false; }
        bool shouldBreakAt(const Value &, const Range<int> &) const override { return false; }
        int getMaxCallStackSize() const override { return 420; }
        int getMaxEvaluationTimeMs() const override { return 42069; }
    };

    static script::Value evaluate(const String &code, script::Scope &scope)
    {
        TestContext context;
        Array<Range<int>> blockRanges;
        return script::parse(code, blockRanges).evaluate(scope, context);
    }

    void runTest() override
    {
        using namespace script;

        beginTest("Smoke test");

        {
            Scope scope;
            const auto result = evaluate("\
                (if (> (- 1 2) (+ 3 4)) (/ 5 6) (* 7 8) ; comment \n\
                ) \n\n ; comment",
                scope);
            expect(result.isNumber());
            expect(result.castToInteger() == 56);
        }

        beginTest("Lexical scoping test");

        {
            Scope scope;
            const auto result = evaluate("\
                (define y 69)             \
                (define (add-69 x)        \
                  (+ x y))                \
                (define (test x)          \
                  ((lambda (y)            \
                    (add-69 x))           \
                      96))                \
                (test 420)",
                scope);
            expect(result.isNumber());
            expect(result.castToInteger() == 489);
        }

        beginTest("Short circuit test");

        {
            Scope scope;
            const auto result = evaluate("\
                (define x (if (and true false never-evaluated (never-evaluated)) 0 1))\
                (define y (if (or false true never-evaluated (never-evaluated)) 2 4))\
                (+ x y)",
                scope);
            expect(result.isNumber());
            expect(result.castToInteger() == 3);
        }

        beginTest("Optional arguments test");

        {
            Scope scope;
            auto result = evaluate("\
                (define n 3)                    \
                (define                         \
                  (x a (b 69) (c (list 1 2 n))) \
                  (list a b c))                 \
                (define n 0)                    \
                (list                           \
                  (x 1)                         \
                  (x 1 2)                       \
                  (x 1 2 3))",
                scope);
            expect(result.isList());
            expect(result.toString() == "((1 69 (1 2 3)) (1 2 (1 2 3)) (1 2 3))");
            result = evaluate("\
                ((lambda                        \
                  (a (b 69) (c (list 1 2 3)))   \
                  (list a b c))                 \
                  1)",
                scope);
            expect(result.isList());
            expect(result.toString() == "(1 69 (1 2 3))");
        }

        beginTest("Local bindings test");

        {
            Scope scope;
            auto result = evaluate("\
                (define a 1)        \
                (define b 1)        \
                (define c 1)        \
                (let ((a 69) (b 420) (x (* a b c))) x)",
                scope);
            expect(result.isInteger());
            expect(result.castToInteger() == 28980);
        }

        beginTest("Arrows test");

        {
            Scope scope;
            auto result = evaluate("\
                (define a (-> (list 69 420 1337) reverse (last)))\
                (define b (last (reverse (list 69 420 1337))))\
                (= a b)",
                scope);
            expect(result.isBoolean());
            expect(result.castToBoolean());
            result = evaluate("\
                (->> (list 1 2 3) (map (lambda (x) (* 2 x))))",
                scope);
            expect(result.isList());
            expect(result.toString() == "(2 4 6)");
        }
    }
};

static ScriptInterpreterTests scriptInterpreterTests;

#endif

} // namespace script
