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

#pragma once

namespace script
{

struct EvaluationError final
{
    enum class Type
    {
        TooFewArguments,
        TooManyArguments,
        InvalidArgument,
        CalledNonFunction,
        InvalidBinaryOperation,
        CannotCastToInt,
        CannotCastToFloat,
        ExpectedString,
        ExpectedSymbol,
        ExpectedList,
        UndefinedSymbol,
        IndexOutOfRange,
        StackOverflow,
        TimeLimitExceed,
        BreakpointHit,
        Aborted
    };

    explicit EvaluationError(Type type,
        const String &symbolName = "",
        const String &description = "") :
        type(type), symbolName(symbolName), description(description) {}

    String getDescription() const noexcept;
    String getTypeName() const noexcept;

    Type type;
    String symbolName;
    String description;
    Range<int> sourceCodeRange;
};

class Value;

struct EvaluationContext
{
    EvaluationContext() = default;
    virtual ~EvaluationContext() = default;

    virtual Optional<Value>
        makeLanguageExtension(const String &valueName) const = 0;

    virtual bool shouldAbort() const = 0;

    virtual bool hasBreakpoints() const = 0;
    virtual bool shouldBreakAt(const Value &value,
        const Range<int> &parentListRange) const = 0;

    Atomic<int> currentCallStackSize = 0;
    virtual int getMaxCallStackSize() const = 0;

    Atomic<uint32> startTime = Time::getMillisecondCounter();
    virtual int getMaxEvaluationTimeMs() const = 0;

    void resetEvaluationContext()
    {
        this->currentCallStackSize = 0;
        this->startTime = Time::getMillisecondCounter();
    }
};

class Scope;

class Value final
{
public:

    using Integer = int32;
    using Float = float;
    using Symbol = String;
    using List = Array<Value>;
    using BuiltInFunction =
        Value (*)(const List &unevaluatedArguments,
            Scope &scope, EvaluationContext &context);

    enum class Type
    {
        Nil,     // casts to false, like an empty list
        Symbol,  // a named identifier
        Boolean, // some self-evaluating values
        Integer, // for primitive data types
        Float,
        String,
        Quote,   // keeps an unevaluated value
        Closure, // captures all used symbols from the scope
        BuiltInFunction,
        List,    // a list of values, evaluated as a function call
        Program  // a list, but evaluates all items and returns the last one,
                 // it's either a main program body or a body of a closure
    };

    inline Value() = default;
    explicit inline Value(Type type) : type(type) {}
    explicit inline Value(bool b) : type(Type::Boolean), boolValue(b) {}
    explicit inline Value(Integer i) : type(Type::Integer), intValue(i) {}
    explicit inline Value(Float f) : type(Type::Float), floatValue(f) {}
    explicit inline Value(List &&list) : type(Type::List), list(move(list)) {}
    inline Value(List &&list, Range<int> sourceCodeRange) :
        type(Type::List), list(move(list)), sourceCodeRange(sourceCodeRange) {}

    EvaluationError makeError(const EvaluationError::Type type) const
    {
        EvaluationError error(type, this->debug());
        error.sourceCodeRange = this->sourceCodeRange;
        return error;
    }

    static Value makeQuote(Value &&quoted)
    {
        Value result(Type::Quote);
        result.list.add(move(quoted));
        return result;
    }

    static Value makeString(String s)
    {
        Value result(Type::String);
        result.string = move(s);
        return result;
    }

    static Value makeSymbol(const String &s, Range<int> sourceCodeRange = {})
    {
        Value result(Type::Symbol);
        result.string = s;
        result.sourceCodeRange = move(sourceCodeRange);
        return result;
    }

    static Value makeClosure(EvaluationContext &context,
        const String &name, List &&params, Range<int> &&paramsRange,
        Value &&body, Scope &scope);

    static Value makeBuiltInFunction(const String &name, BuiltInFunction f)
    {
        Value result(Type::BuiltInFunction);
        result.string = name;
        result.builtInFunction = f;
        return result;
    }

    static Value makeProgram(List &&list)
    {
        Value result(Type::Program);
        result.list = move(list);
        return result;
    }

    Array<Symbol> getUsedSymbols() const
    {
        Array<String> result;
        switch (this->type)
        {
        case Type::Quote:
            return this->list.getReference(0).getUsedSymbols();
        case Type::Symbol:
            result.add(this->asSymbol());
            return result;
        case Type::Closure:
        {
            for (int i = 1; i < this->list.size(); i++)
            {
                result.addArray(this->list.getReference(i).getUsedSymbols());
            }
            return result;
        }
        case Type::List:
        case Type::Program:
        {
            for (int i = 0; i < this->list.size(); i++)
            {
                result.addArray(this->list.getReference(i).getUsedSymbols());
            }
            return result;
        }
        default:
            return result;
        }
    }

    Value apply(const List &args, Scope &scope, EvaluationContext &context) const;
    Value evaluate(Scope &scope, EvaluationContext &context) const;

    bool isBuiltInFunction() const noexcept
    {
        return this->type == Type::BuiltInFunction;
    }

    bool isClosure() const noexcept
    {
        return this->type == Type::Closure;
    }

    bool isBoolean() const noexcept
    {
        return this->type == Type::Boolean;
    }

    bool isInteger() const noexcept
    {
        return this->type == Type::Integer;
    }

    bool isFloat() const noexcept
    {
        return this->type == Type::Float;
    }

    bool isNumber() const noexcept
    {
        return this->type == Type::Integer || this->type == Type::Float;
    }

    bool isList() const noexcept
    {
        return this->type == Type::List;
    }

    bool isListOf(Type childType) const noexcept
    {
        if (!this->isList())
        {
            return false;
        }

        for (const auto &child : this->list)
        {
            if (child.type != childType)
            {
                return false;
            }
        }

        return true;
    }

    bool isListOf(Type childType1, Type childType2) const noexcept
    {
        if (!this->isList())
        {
            return false;
        }

        for (const auto &child : this->list)
        {
            if (child.type != childType1 && child.type != childType2)
            {
                return false;
            }
        }

        return true;
    }

    bool isProgram() const noexcept
    {
        return this->type == Type::Program;
    }

    bool isString() const noexcept
    {
        return this->type == Type::String;
    }

    bool isSymbol() const noexcept
    {
        return this->type == Type::Symbol;
    }

    bool isNil() const noexcept
    {
        return this->type == Type::Nil;
    }

    //===------------------------------------------------------------------===//
    // Typecasting
    //===------------------------------------------------------------------===//

    bool castToBoolean() const noexcept
    {
        return this->type != Type::Nil &&
            (this->type != Type::Boolean || this->boolValue) &&
            (this->type != Type::List || !this->list.isEmpty());
    }

    Integer castToInteger() const
    {
        switch (this->type)
        {
        case Type::Integer: return this->intValue;
        case Type::Float: return Integer(this->floatValue);
        default:
            throw this->makeError(EvaluationError::Type::CannotCastToInt);
        }
    }

    Float castToFloat() const
    {
        switch (this->type)
        {
        case Type::Float: return this->floatValue;
        case Type::Integer: return Float(this->intValue);
        default:
            throw this->makeError(EvaluationError::Type::CannotCastToFloat);
        }
    }

    const String &asString() const
    {
        if (this->type != Type::String)
        {
            throw this->makeError(EvaluationError::Type::ExpectedString);
        }

        return this->string;
    }

    const Symbol &asSymbol() const
    {
        if (this->type != Type::Symbol)
        {
            throw this->makeError(EvaluationError::Type::ExpectedSymbol);
        }

        return this->string;
    }

    const List &asList() const
    {
        if (this->type != Type::List)
        {
            throw this->makeError(EvaluationError::Type::ExpectedList);
        }

        return this->list;
    }

    const Range<int> &getSourceCodeRange() const noexcept
    {
        return this->sourceCodeRange;
    }

    //===------------------------------------------------------------------===//
    // List methods
    //===------------------------------------------------------------------===//

    void push(Value &&val)
    {
        if (this->type != Type::List)
        {
            throw this->makeError(EvaluationError::Type::InvalidArgument);
        }

        this->list.add(move(val));
    }

    void push(const Value &val)
    {
        if (this->type != Type::List)
        {
            throw this->makeError(EvaluationError::Type::InvalidArgument);
        }

        this->list.add(val);
    }

    //===------------------------------------------------------------------===//
    // Comparison operations
    //===------------------------------------------------------------------===//

    bool operator==(const Value &other) const
    {
        if (this->type == Type::Float && other.type == Type::Integer)
        {
            return this->floatValue == other.castToFloat();
        }
        else if (this->type == Type::Integer && other.type == Type::Float)
        {
            return this->castToFloat() == other.floatValue;
        }
        else if (this->type != other.type)
        {
            return false;
        }

        switch (this->type)
        {
        case Type::Boolean:
            return this->boolValue == other.boolValue;
        case Type::Integer:
            return this->intValue == other.intValue;
        case Type::Float:
            return this->floatValue == other.floatValue;
        case Type::BuiltInFunction:
            return this->builtInFunction == other.builtInFunction;
        case Type::String:
        case Type::Symbol:
            return this->string == other.string;
        case Type::Closure:
        case Type::List:
        case Type::Program:
            return this->list == other.list;
        case Type::Quote:
            return this->list.getReference(0) == other.list.getReference(0);
        default:
            return true;
        }
    }

    bool operator!=(const Value &other) const
    {
        return !(*this == other);
    }

    bool operator>=(const Value &other) const
    {
        return !(*this < other);
    }

    bool operator<=(const Value &other) const
    {
        return (*this == other) || (*this < other);
    }

    bool operator>(const Value &other) const
    {
        return !(*this <= other);
    }

    bool operator<(const Value &other) const
    {
        if (!this->isNumber() || !other.isNumber())
        {
            throw this->makeError(EvaluationError::Type::InvalidBinaryOperation);
        }

        switch (this->type)
        {
        case Type::Float:
            return this->floatValue < other.castToFloat();
        case Type::Integer:
            if (other.type == Type::Float)
            {
                return this->castToFloat() < other.floatValue;
            }
            else
            {
                return this->intValue < other.intValue;
            }
        default:
            throw this->makeError(EvaluationError::Type::InvalidBinaryOperation);
        }
    }

    //===------------------------------------------------------------------===//
    // Operations
    //===------------------------------------------------------------------===//

    Value operator+(const Value &other) const
    {
        if (other.type == Type::Nil)
        {
            return other;
        }

        if (!this->isList() &&
            (this->isNumber() != other.isNumber() || this->isString() != other.isString()))
        {
            throw EvaluationError(EvaluationError::Type::InvalidBinaryOperation,
                (this->debug() + " + " + other.debug()));
        }

        switch (this->type)
        {
        case Type::Float:
            return Value(this->floatValue + other.castToFloat());
        case Type::Integer:
            if (other.type == Type::Float)
            {
                return Value(this->castToFloat() + other.floatValue);
            }
            return Value(this->intValue + other.castToInteger());
        case Type::String:
            return Value::makeString(this->string + other.string);
        case Type::List:
        {
            Value result = *this;
            result.push(other);
            return result;
        }
        case Type::Nil:
            return *this;
        default:
            throw this->makeError(EvaluationError::Type::InvalidBinaryOperation);
        }
    }

    Value operator-(const Value &other) const
    {
        if (other.type == Type::Nil)
        {
            return other;
        }

        if (other.type != Type::Float && other.type != Type::Integer)
        {
            throw EvaluationError(EvaluationError::Type::InvalidBinaryOperation,
                (this->debug() + " - " + other.debug()));
        }

        switch (this->type)
        {
        case Type::Float:
            return Value(this->floatValue - other.castToFloat());
        case Type::Integer:
            if (other.type == Type::Float)
            {
                return Value(this->castToFloat() - other.floatValue);
            }
            else
            {
                return Value(this->intValue - other.castToInteger());
            }
        case Type::Nil:
            return *this;
        default:
            throw this->makeError(EvaluationError::Type::InvalidBinaryOperation);
        }
    }

    Value operator*(const Value &other) const
    {
        if (other.type == Type::Nil)
        {
            return other;
        }

        if (other.type != Type::Float && other.type != Type::Integer)
        {
            throw EvaluationError(EvaluationError::Type::InvalidBinaryOperation,
                (this->debug() + " * " + other.debug()));
        }

        switch (this->type)
        {
        case Type::Float:
            return Value(this->floatValue * other.castToFloat());
        case Type::Integer:
            if (other.type == Type::Float)
            {
                return Value(this->castToFloat() * other.floatValue);
            }
            else
            {
                return Value(this->intValue * other.castToInteger());
            }
        case Type::Nil:
            return *this;
        default:
            throw this->makeError(EvaluationError::Type::InvalidBinaryOperation);
        }
    }

    Value operator/(const Value &other) const
    {
        if (other.type == Type::Nil)
        {
            return other;
        }

        const auto divisionByZero =
            ((other.type == Type::Integer && other.intValue == 0) ||
            (other.type == Type::Float && other.floatValue == 0.f));

        if (divisionByZero ||
            (other.type != Type::Float && other.type != Type::Integer))
        {
            throw EvaluationError(EvaluationError::Type::InvalidBinaryOperation,
                (this->debug() + " / " + other.debug()));
        }

        switch (this->type)
        {
        case Type::Float:
            return Value(this->floatValue / other.castToFloat());
        case Type::Integer:
            if (other.type == Type::Float)
            {
                return Value(this->castToFloat() / other.floatValue);
            }
            else
            {
                return Value(this->intValue / other.castToInteger());
            }
        case Type::Nil:
            return *this;
        default:
            throw this->makeError(EvaluationError::Type::InvalidBinaryOperation);
        }
    }

    Value operator%(const Value &other) const
    {
        if (other.type == Type::Nil)
        {
            return other;
        }

        const auto divisionByZero =
            ((other.type == Type::Integer && other.intValue == 0) ||
            (other.type == Type::Float && other.floatValue == 0.f));

        if (divisionByZero ||
            (other.type != Type::Float && other.type != Type::Integer))
        {
            throw EvaluationError(EvaluationError::Type::InvalidBinaryOperation,
                (this->debug() + " % " + other.debug()));
        }

        switch (this->type)
        {
        case Type::Float:
            return Value(fmodf(this->floatValue, other.castToFloat()));
        case Type::Integer:
            if (other.type == Type::Float)
            {
                return Value(fmodf(this->castToFloat(), other.floatValue));
            }
            else
            {
                return Value(this->intValue % other.castToInteger());
            }
        case Type::Nil:
            return *this;
        default:
            throw this->makeError(EvaluationError::Type::InvalidBinaryOperation);
        }
    }

    String getTypeName() const noexcept
    {
        switch (this->type)
        {
        case Type::Quote: return "quote";
        case Type::Symbol: return "symbol";
        case Type::Boolean: return "boolean";
        case Type::Integer: return "integer";
        case Type::Float: return "float";
        case Type::Program:
        case Type::List: return "list";
        case Type::String: return "string";
        case Type::BuiltInFunction:
        case Type::Closure: return "function";
        case Type::Nil: return "nil";
        default:
            jassertfalse;
            return {};
        }
    }

    String toString() const
    {
        String result;
        switch (this->type)
        {
        case Type::Quote:
            jassert(!this->list.isEmpty());
            return "'" + this->list.getReference(0).toString();
        case Type::Symbol:
            return this->string;
        case Type::Boolean:
            return String(this->boolValue ? "true" : "false");
        case Type::Integer:
            return String(this->intValue);
        case Type::Float:
            return String(this->floatValue);
        case Type::String:
            return this->string;
        case Type::Closure:
            for (int i = 0; i < this->list.size(); i++)
            {
                result += this->list.getReference(i).toString();
                if (i < this->list.size() - 1)
                {
                    result += " ";
                }
            }
            return "(" + String(CharPointer_UTF8("\xce\xbb ")) + result + ")";
        case Type::Program:
            for (int i = 0; i < this->list.size(); i++)
            {
                result += this->list.getReference(i).toString();
                if (i < this->list.size() - 1)
                {
                    result += " ";
                }
            }
            return result;
        case Type::List:
            for (int i = 0; i < this->list.size(); i++)
            {
                result += this->list.getReference(i).toString();
                if (i < this->list.size() - 1)
                {
                    result += " ";
                }
            }
            return "(" + result + ")";
        case Type::BuiltInFunction:
            return "<" + this->string + ">";
        case Type::Nil:
            return this->getTypeName();
        default:
            jassertfalse;
            return {};
        }
    }

    String debug() const
    {
        String result;
        switch (this->type)
        {
        case Type::String:
            return "\"" + this->string + "\"";
        case Type::Closure:
            if (this->string.isNotEmpty() && !this->list.isEmpty())
            {
                String signature = this->string;
                const auto &params = this->list.getReference(0).asList();
                for (int i = 0; i < params.size(); i++)
                {
                    signature += " ";
                    signature += params.getReference(i).toString();
                }
                return "(" + signature + ")";
            }
        default:
            if (this->string.isNotEmpty())
            {
                return this->string; // prefer the name, if any
            }
            constexpr auto maxLen = 64;
            const auto asString = this->toString();
            const auto numDroppedChars = asString.length() - maxLen;
            return (numDroppedChars > 0) ?
                (asString.dropLastCharacters(numDroppedChars) + "...") : asString;
        }
    }

private:

    Type type = Type::Nil;
    bool boolValue = false;
    Integer intValue = 0;
    Float floatValue = 0.f;
    BuiltInFunction builtInFunction = nullptr;
    String string;
    List list; // also a container for a quote, or closure params and body
    Array<Scope> closureCaptures;
    Range<int> sourceCodeRange;
};

class Scope final
{
public:

    inline Scope() = default;

    bool hasValue(const String &name) const;
    const Value &findValue(const String &name) const;
    Value makeValue(EvaluationContext &context, const String &name) const;
    void setValue(const String &name, const Value &value) noexcept;
    void setValue(const String &name, Value &&value) noexcept;
    void include(const Scope &other);

    void setParent(Scope *parent)
    {
        this->parentScope = parent;
    }

    String toString() const;

    StringArray findAllFunctionNames() const;

private:

    FlatHashMap<String, Value, StringHash> symbols;

    Scope *parentScope = nullptr;
};

struct ParsingError final
{
    enum class Type
    {
        InvalidString,
        InvalidNumber,
        UnexpectedEndOfProgram,
        MismatchedParentheses,
        SyntaxError
    };

    explicit ParsingError(Type type, Range<int> range = {}) :
        type(type), sourceCodeRange(move(range)) {}

    String getDescription() const noexcept;

    Type type;
    Range<int> sourceCodeRange;
};

bool isSymbolBody(juce_wchar ch) noexcept;

void checkNumArgs(const Value &value, int number);
void checkNumArgs(const Value::List &args, int number);

Value::List evaluateArgs(const Value::List &args,
    Scope &scope, EvaluationContext &context);

Value parse(const String &string, Array<Range<int>> &outBlockRanges);

} // namespace script
