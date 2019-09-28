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
#include "CommandPaletteChordConstructor.h"

/*
    Reference:
    https://en.wikipedia.org/wiki/Chord_names_and_symbols_(popular_music)#Rules_to_decode_chord_names_and_symbols

    What I'm going to do here is somewhat similar to what compilers are doing:
        source -> tokens -> internal representation -> passes -> generation

    Step 1 is lexical analysis, operating on string, returning list of tokens of these types:
        note: e.g. B or F (upper case)
        sign: # or b (lower case)
        slash: just / used to override bass note or add keys
        number: a number which can be negative, e.g. for negative inversion
        keyword: aug (also '+'), dim, sus, add, inv, min (also 'm'), maj (also 'M'), might be lower case or start with capital
        (any unknown characters are skipped)

    Step 2 is syntactic analysis, operating on list of tokens, returning list of expressions:
        root key: note token + optional sign  - e.g. Bb or E#, or just B or E
        chord quality: either a keyword, one of [major, minor, augmented, diminished], or a number, or both: e.g. 11, M7, aug 9
        bass note: slash token + note token
        key alteration: sign + number - e.g. b5, #11
        addition: add keyword + number, or slash token + number - e.g. add 13 or /9
        inversion: inv keyword + number - e.g. inv -3
        suspension: sus keyword + number - e.g. sus2

    Step 3 is a cleanup pass, operating on list of expressions, returning chord description:
        remove duplicates, if any
        add defaults and apply specific rules:
            when the fifth interval is diminished, the third must be minor
            todo

    Step 4 is chord generation, operating on cleaned up chord description
        isValid() - checks if missing required expressions
            1 root key must be present, others are not required
        toString()
        provides suggestions
        remembers last valid expressions
        indicates if the expression has changed with the new input
*/

namespace ChordParsing
{

//===----------------------------------------------------------------------===//
// Tokens
//===----------------------------------------------------------------------===//

struct Token
{
    enum class Type : int8
    {
        Note,
        Sign,
        Slash,
        Keyword,
        Number
    };

    Token() = delete;
    explicit Token(Type type) : type(type) {}
    virtual ~Token() {}

    const Type type;
};

struct NoteToken final : Token
{
    explicit NoteToken(int8 number) : Token(Token::Type::Note), noteNumber(number) {}
    const int8 noteNumber;
};

struct SignToken final : Token
{
    explicit SignToken(bool isSharp) : Token(Token::Type::Sign), isSharp(isSharp) {}
    const bool isSharp = true;
};

struct SlashToken final : Token
{
    explicit SlashToken() : Token(Token::Type::Slash) {}
};

struct KeywordToken final : Token
{
    enum class Type : int8
    {
        Added,
        Inverted,
        Augmented,
        Diminished,
        Suspeneded,
        Major,
        Minor
    };
    explicit KeywordToken(Type keyword) : Token(Token::Type::Keyword), keyword(keyword) {}
    const Type keyword;
};

struct NumberToken final : Token
{
    explicit NumberToken(int8 number) : Token(Token::Type::Number), number(number) {}
    const int8 number;
};

class Lexer final
{
public:

    explicit Lexer(String::CharPointerType input) : input(input) {}

    OwnedArray<Token> getTokens()
    {
        OwnedArray<Token> tokens;

        while (*this->input != 0)
        {
            if (auto token = parseToken(this->input))
            {
                tokens.add(token.release());
            }
        }

        return tokens;
    }

private:

    static UniquePointer<Token> parseToken(String::CharPointerType &t)
    {
        t = t.findEndOfWhitespace();
        auto t2 = t;

        switch (t2.getAndAdvance())
        {
        case '/':
            t = t2;
            return makeUnique<SlashToken>();

        case '+':
            t = t2;
            return makeUnique<KeywordToken>(KeywordToken::Type::Augmented);

        case '#': case 'b':
            return parseSignToken(t);

        case '-':
            t = t2.findEndOfWhitespace();
            if (!CharacterFunctions::isDigit(*t))
            {
                break;
            }

            return parseNumberToken(t, true);

        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            return parseNumberToken(t, false);

        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
            if (*t2 != 'd' && *t2 != 'u' && *t2 != 'i')
            {
                return parseNoteToken(t);
            }
            else
            {
                return parseKeywordToken(t);
            }

        default:
            return parseKeywordToken(t);
        }

        return {};
    }

    static UniquePointer<Token> parseNumberToken(String::CharPointerType &t, bool isNegative)
    {
        int64 intValue = t.getAndAdvance() - '0';
        jassert(intValue >= 0 && intValue < 10);

        for (;;)
        {
            auto previousChar = t;
            auto c = t.getAndAdvance();
            auto digit = ((int)c) - '0';
            if (isPositiveAndBelow(digit, 10))
            {
                intValue = intValue * 10 + digit;
            }
            else
            {
                t = previousChar;
                break;
            }
        }

        const auto correctedValue = isNegative ? -intValue : intValue;
        return makeUnique<NumberToken>((int8)correctedValue);
    }

    static UniquePointer<Token> parseNoteToken(String::CharPointerType &t)
    {
        switch (t.getAndAdvance())
        {
        case 'C':
            return makeUnique<NoteToken>(int8(0));
        case 'D':
            return makeUnique<NoteToken>(int8(1));
        case 'E':
            return makeUnique<NoteToken>(int8(2));
        case 'F':
            return makeUnique<NoteToken>(int8(3));
        case 'G':
            return makeUnique<NoteToken>(int8(4));
        case 'A':
            return makeUnique<NoteToken>(int8(5));
        case 'B':
        case 'H':
            return makeUnique<NoteToken>(int8(6));
        default:
            break;
        }

        return {};
    }

    static UniquePointer<Token> parseSignToken(String::CharPointerType &t)
    {
        switch (t.getAndAdvance())
        {
        case '#':
            return makeUnique<SignToken>(true);
        case 'b':
            return makeUnique<SignToken>(false);
        default:
            break;
        }

        return {};
    }

    static UniquePointer<Token> parseKeywordToken(String::CharPointerType &t)
    {
        auto t2 = t;

        // parse keywords with hard-coded checks, I want it to be *fast*
        switch (t2.getAndAdvance())
        {
            case '+':
                t = t2;
                return makeUnique<KeywordToken>(KeywordToken::Type::Augmented);

            case 'a': case 'A': // "add", "aug"
            {
                auto t3 = t2;
                if (t2.getAndAdvance() == 'd' && t2.getAndAdvance() == 'd')
                {
                    t = t2;
                    return makeUnique<KeywordToken>(KeywordToken::Type::Added);
                }
                else if (t3.getAndAdvance() == 'u' && t3.getAndAdvance() == 'g')
                {
                    t = t3;
                    return makeUnique<KeywordToken>(KeywordToken::Type::Augmented);
                }
                break;
            }

            case 'i': case 'I': // "inv"
                if (t2.getAndAdvance() == 'n' && t2.getAndAdvance() == 'v')
                {
                    t = t2;
                    return makeUnique<KeywordToken>(KeywordToken::Type::Inverted);
                }
                break;

            case 's': case 'S': // "sus"
                if (t2.getAndAdvance() == 'u' && t2.getAndAdvance() == 's')
                {
                    t = t2;
                    return makeUnique<KeywordToken>(KeywordToken::Type::Suspeneded);
                }
                break;

            case 'd': case 'D': // "dim"
                if (t2.getAndAdvance() == 'i' && t2.getAndAdvance() == 'm')
                {
                    t = t2;
                    return makeUnique<KeywordToken>(KeywordToken::Type::Diminished);
                }
                break;

            case 'm':
            {
                if (!CharacterFunctions::isLetter(*t2))
                {
                    t = t2;
                    return makeUnique<KeywordToken>(KeywordToken::Type::Minor);
                }

                auto t3 = t2;
                if (t2.getAndAdvance() == 'i' && t2.getAndAdvance() == 'n')
                {
                    t = t2;
                    return makeUnique<KeywordToken>(KeywordToken::Type::Minor);
                }
                else if (t3.getAndAdvance() == 'a' && t3.getAndAdvance() == 'j')
                {
                    t = t3;
                    return makeUnique<KeywordToken>(KeywordToken::Type::Major);
                }
                break;
            }

            case 'M':
            {
                if (!CharacterFunctions::isLetter(*t2))
                {
                    t = t2;
                    return makeUnique<KeywordToken>(KeywordToken::Type::Major);
                }

                auto t3 = t2;
                if (t2.getAndAdvance() == 'i' && t2.getAndAdvance() == 'n')
                {
                    t = t2;
                    return makeUnique<KeywordToken>(KeywordToken::Type::Minor);
                }
                else if (t3.getAndAdvance() == 'a' && t3.getAndAdvance() == 'j')
                {
                    t = t3;
                    return makeUnique<KeywordToken>(KeywordToken::Type::Major);
                }
                break;
            }

        default:
            break;
        }

        // met the unknown character, so skip one
        t.getAndAdvance();
        return {};
    }
    
    String::CharPointerType input;
};

//===----------------------------------------------------------------------===//
// Expressions
//===----------------------------------------------------------------------===//

struct Expression
{
    enum class Type : int8
    {
        Invalid,
        RootKey,
        ChordQuality,
        Addition,
        Inversion,
        Suspension,
        KeyAlteration,
        BassNote
    };

    Expression() = default;
    explicit Expression(Type type) : type(type) {}
    virtual ~Expression() {}

    const Type type = Type::Invalid;

    bool isValid() const noexcept
    {
        return this->type != Type::Invalid;
    }
};

struct RootKeyExpression final : Expression
{
    explicit RootKeyExpression(NoteToken note) : Expression(Expression::Type::RootKey) {}
    RootKeyExpression(NoteToken note, SignToken sign) : Expression(Expression::Type::RootKey) {}

    int absouleChromaticKey = 0;
};

struct ChordQualityExpression final : Expression
{
    ChordQualityExpression(KeywordToken keyword, NumberToken number) : Expression(Expression::Type::ChordQuality) {}
};

struct AdditionExpression final : Expression
{
    AdditionExpression(NumberToken number) : Expression(Expression::Type::Addition) {}
};

struct InversionExpression final : Expression
{
    InversionExpression(NumberToken number) : Expression(Expression::Type::Inversion) {}
};

struct SuspensionExpression final : Expression
{
    SuspensionExpression(NumberToken number) : Expression(Expression::Type::Suspension) {}
};

struct KeyAlterationExpression final : Expression
{
    KeyAlterationExpression(NumberToken number, SignToken sign) : Expression(Expression::Type::KeyAlteration) {}
};

struct BassNoteExpression final : Expression
{
    BassNoteExpression(NoteToken bassNote) : Expression(Expression::Type::BassNote) {}
};

class Parser final
{
public:

    explicit Parser(OwnedArray<Token> tokens) : tokens(std::move(tokens)) {}

    OwnedArray<Expression> getExpressions() const
    {
        OwnedArray<Expression> expressions;

        int idx = 0;
        for (;;)
        {
            if (idx >= this->tokens.size())
            {
                break;
            }

            auto expr = this->parseExpression(idx);

            if (expr != nullptr && expr->isValid())
            {
                expressions.add(expr.release());
            }
        }

        return expressions;
    }

private:

    UniquePointer<Expression> parseExpression(int &idx) const
    {
        // todo

        switch (this->tokens.getUnchecked(idx)->type)
        {
        case Token::Type::Note:

            break;

        default:
            jassertfalse;
            break;
        }

        return {};
    };

    const OwnedArray<Token> tokens;
};

//===----------------------------------------------------------------------===//
// Cleaned up chord description
//===----------------------------------------------------------------------===//

struct ChordDescription final
{
    UniquePointer<RootKeyExpression> rootKey;
    UniquePointer<ChordQualityExpression> chordQuality;
    UniquePointer<AdditionExpression> addition;
    UniquePointer<InversionExpression> inversion;
    UniquePointer<SuspensionExpression> suspension;
    UniquePointer<KeyAlterationExpression> keyAlteration;
    UniquePointer<BassNoteExpression> bassNote;

    // todo equals operator
};

class CleanupPass final
{
public:

    explicit CleanupPass(OwnedArray<Expression> expressions) : expressions(std::move(expressions)) {}

    ChordDescription getChordDescription() const
    {
        ChordDescription description;

        // todo while
        //this->expressions.removeAndReturn()

        return description;
    }

private:

    const OwnedArray<Expression> expressions;
};

//===----------------------------------------------------------------------===//
// Chord and helpers
//===----------------------------------------------------------------------===//

class Generator final
{
public:

    Generator() = default;

    void parse(const String &input)
    {
        this->description = CleanupPass(Parser(Lexer(input).getTokens()).getExpressions()).getChordDescription();

    }

private:

    ChordDescription description;
    StringArray lastChords;

};

}



const CommandPaletteActionsProvider::Actions &CommandPaletteChordConstructor::getActions() const
{
    // todo
    return this->actions;
}
