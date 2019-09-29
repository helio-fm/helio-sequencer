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
        RootKey,
        ChordQuality,
        Addition,
        Inversion,
        Suspension,
        KeyAlteration,
        BassNote
    };

    Expression() = delete;
    explicit Expression(Type type) : type(type) {}
    virtual ~Expression() {}

    virtual void fillDescription(String &out) const = 0;
    virtual bool isValid() const = 0;

    static juce_wchar keyToLetter(int8 key)
    {
        switch (key)
        {
        case 0: return 'C';
        case 1: return 'D';
        case 2: return 'E';
        case 3: return 'F';
        case 4: return 'G';
        case 5: return 'A';
        case 6: return 'B';
        default: return 0;
        }
    }

    const Type type;
};

struct RootKeyExpression final : Expression
{
    explicit RootKeyExpression(const NoteToken *note) :
        Expression(Expression::Type::RootKey), key(note->noteNumber), sharp(false), flat(false) {}

    RootKeyExpression(const NoteToken *note, const SignToken *sign) :
        Expression(Expression::Type::RootKey), key(note->noteNumber), sharp(sign->isSharp), flat(!sign->isSharp) {}

    bool isValid() const override
    {
        return (this->key >= 0 && this->key <= 6) && !(this->sharp && this->flat);
    }

    void fillDescription(String &out) const override
    {
        out << keyToLetter(this->key);

        if (this->sharp)
        {
            out << "#";
        }
        else if (this->flat)
        {
            out << "b";
        }
    }

    const int8 key;
    const bool sharp;
    const bool flat;
};

struct ChordQualityExpression final : Expression
{
    ChordQualityExpression() : Expression(Expression::Type::ChordQuality),
        interval(3), third(Third::Major), fifth(Fifth::Perfect) {}

    explicit ChordQualityExpression(const NumberToken *number) : Expression(Expression::Type::ChordQuality),
        interval(number->number), third(Third::Major), fifth(Fifth::Perfect) {}

    explicit ChordQualityExpression(const KeywordToken *keyword) :
        Expression(Expression::Type::ChordQuality), interval(3)
    {
        this->initFromKeyword(keyword);
    }

    ChordQualityExpression(const KeywordToken *keyword, const NumberToken *number) :
        Expression(Expression::Type::ChordQuality), interval(number->number)
    {
        this->initFromKeyword(keyword);
    }

    void initFromKeyword(const KeywordToken *keyword)
    {
        this->third = keyword->keyword == KeywordToken::Type::Minor ? Third::Minor :
            (keyword->keyword == KeywordToken::Type::Major ? Third::Major : Third::Default);
        this->fifth = keyword->keyword == KeywordToken::Type::Augmented ? Fifth::Augmented :
            (keyword->keyword == KeywordToken::Type::Diminished ? Fifth::Diminished : Fifth::Perfect);
    }

    enum class Third : int8
    {
        Default,
        Major,
        Minor
    };

    enum class Fifth : int8
    {
        Perfect,
        Augmented,
        Diminished
    };

    bool isValid() const override
    {
        // what's the upper limit for the interval?
        return (this->interval >= 0 && this->interval <= 32);
    }

    void fillDescription(String &out) const override
    {
        switch (this->third)
        {
        case Third::Major: out << "M"; break;
        case Third::Minor: out << "m"; break;
        default: break;
        }

        if (this->interval != 3)
        {
            out << int(this->interval);
        }

        switch (this->fifth)
        {
        case Fifth::Augmented: out << " aug"; break;
        case Fifth::Diminished: out << " dim"; break;
        default: break;
        }
    }

    int8 interval;
    Third third;
    Fifth fifth;
};

struct BassNoteExpression final : Expression
{
    explicit BassNoteExpression(const NoteToken *bassNote) :
        Expression(Expression::Type::BassNote), key(bassNote->noteNumber) {}

    bool isValid() const override
    {
        return (this->key >= 0 && this->key <= 6);
    }

    void fillDescription(String &out) const override
    {
        out << "/" << keyToLetter(this->key);
    }

    const int8 key;
};

struct AdditionExpression final : Expression
{
    explicit AdditionExpression(const NumberToken *number) :
        Expression(Expression::Type::Addition), addedKey(number->number) {}

    bool isValid() const override
    {
        // what's the upper limit for the added key?
        return (this->addedKey > 0 && this->addedKey <= 32);
    }

    void fillDescription(String &out) const override
    {
        out << " add" << (int)this->addedKey;
    }

    const int8 addedKey;
};

struct InversionExpression final : Expression
{
    explicit InversionExpression(const NumberToken *number) :
        Expression(Expression::Type::Inversion), inversion(number->number) {}

    bool isValid() const override
    {
        return (this->inversion != 0 && this->inversion >= -7 && this->inversion <= 7);
    }

    void fillDescription(String &out) const override
    {
        out << " inv" << (int)this->inversion;
    }

    const int8 inversion;
};

struct SuspensionExpression final : Expression
{
    explicit SuspensionExpression(const NumberToken *number) :
        Expression(Expression::Type::Suspension), suspension(number->number) {}

    bool isValid() const override
    {
        return (this->suspension > 0 && this->suspension <= 32);
    }

    void fillDescription(String &out) const override
    {
        out << " sus" << (int)this->suspension;
    }

    const int8 suspension;
};

struct KeyAlterationExpression final : Expression
{
    KeyAlterationExpression(const SignToken *sign, const NumberToken *number) :
        Expression(Expression::Type::KeyAlteration), key(number->number), sharp(sign->isSharp) {}

    bool isValid() const override
    {
        return (this->key >= 0 && this->key <= 32);
    }

    void fillDescription(String &out) const override
    {
        out << " " << (this->sharp ? "#" : "b") << (int)this->key;
    }

    const int8 key;
    const bool sharp; // if false, means flat
};

class Parser final
{
public:

    explicit Parser(OwnedArray<Token> tokens) : tokens(std::move(tokens)) {}

    OwnedArray<Expression> getExpressions() const
    {
        OwnedArray<Expression> expressions;

        int idx = 0;
        while (idx < this->tokens.size())
        {
            if (auto expr = this->parseExpression(idx))
            {
                expressions.add(expr.release());
            }
        }

        return expressions;
    }

private:

    UniquePointer<Expression> parseExpression(int &t) const
    {
        const auto *token = this->tokens.getUnchecked(t);
        const auto *nextToken = this->tokens[t + 1];

        switch (token->type)
        {
        case Token::Type::Note:
            return parseRootKey(t);

        case Token::Type::Number:
            return parseChordQuality(t);

        case Token::Type::Keyword:
            return parseKeywords(t);

        case Token::Type::Sign:
            if (nextToken != nullptr && nextToken->type == Token::Type::Number)
            {
                t += 2;
                const auto *signToken = static_cast<const SignToken *>(token);
                const auto *numberToken = static_cast<const NumberToken *>(nextToken);
                return makeUnique<KeyAlterationExpression>(signToken, numberToken);
            }
            break;

        case Token::Type::Slash:
            if (nextToken != nullptr && nextToken->type == Token::Type::Note)
            {
                t += 2;
                const auto *noteToken = static_cast<const NoteToken *>(nextToken);
                return makeUnique<BassNoteExpression>(noteToken);
            }
            else if (nextToken != nullptr && nextToken->type == Token::Type::Number)
            {
                t += 2;
                const auto *numberToken = static_cast<const NumberToken *>(nextToken);
                return makeUnique<AdditionExpression>(numberToken);
            }
            break;

        default:
            jassertfalse;
            break;
        }

        t++;
        return {};
    };

    UniquePointer<Expression> parseRootKey(int &t) const
    {
        const auto *token = static_cast<NoteToken *>(this->tokens.getUnchecked(t));
        const auto *nextToken = this->tokens[t + 1];

        if (nextToken != nullptr && nextToken->type == Token::Type::Sign)
        {
            t += 2;
            const auto *signToken = static_cast<const SignToken *>(nextToken);
            return makeUnique<RootKeyExpression>(token, signToken);
        }

        t++;
        return makeUnique<RootKeyExpression>(token);
    }

    UniquePointer<Expression> parseKeywords(int &t) const
    {
        const auto *token = static_cast<KeywordToken *>(this->tokens.getUnchecked(t));
        const auto *nextToken = this->tokens[t + 1];

        switch (token->keyword)
        {
        case KeywordToken::Type::Added:
            if (nextToken != nullptr && nextToken->type == Token::Type::Number)
            {
                t += 2;
                const auto *numberToken = static_cast<const NumberToken *>(nextToken);
                return makeUnique<AdditionExpression>(numberToken);
            }
            break;

        case KeywordToken::Type::Inverted:
            if (nextToken != nullptr && nextToken->type == Token::Type::Number)
            {
                t += 2;
                const auto *numberToken = static_cast<const NumberToken *>(nextToken);
                return makeUnique<InversionExpression>(numberToken);
            }
            break;

        case KeywordToken::Type::Suspeneded:
            if (nextToken != nullptr && nextToken->type == Token::Type::Number)
            {
                t += 2;
                const auto *numberToken = static_cast<const NumberToken *>(nextToken);
                return makeUnique<SuspensionExpression>(numberToken);
            }
            break;

        case KeywordToken::Type::Major:
        case KeywordToken::Type::Minor:
        case KeywordToken::Type::Augmented:
        case KeywordToken::Type::Diminished:
            return parseChordQuality(t);

        default:
            jassertfalse;
            break;
        }

        t++;
        return {};
    }

    UniquePointer<Expression> parseChordQuality(int &t) const
    {
        const auto *token = this->tokens.getUnchecked(t);
        const auto *nextToken = this->tokens[t + 1];

        if (token->type == Token::Type::Keyword)
        {
            const auto *keywordToken = static_cast<KeywordToken *>(this->tokens.getUnchecked(t));
            if (nextToken != nullptr && nextToken->type == Token::Type::Number)
            {
                t += 2;
                const auto *numberToken = static_cast<const NumberToken *>(nextToken);
                return makeUnique<ChordQualityExpression>(keywordToken, numberToken);
            }

            t++;
            return makeUnique<ChordQualityExpression>(keywordToken);
        }
        else if (token->type == Token::Type::Number)
        {
            const auto *numberToken = static_cast<NumberToken *>(this->tokens.getUnchecked(t));
            if (nextToken != nullptr && nextToken->type == Token::Type::Keyword)
            {
                t += 2;
                const auto *keywordToken = static_cast<const KeywordToken *>(nextToken);
                return makeUnique<ChordQualityExpression>(keywordToken, numberToken);
            }

            t++;
            return makeUnique<ChordQualityExpression>(numberToken);
        }
        else
        {
            jassertfalse;
        }

        t++;
        return {};
    }

    const OwnedArray<Token> tokens;
};

//===----------------------------------------------------------------------===//
// Cleaned up chord description
//===----------------------------------------------------------------------===//

struct ChordDescription final
{
    UniquePointer<RootKeyExpression> rootKey;
    UniquePointer<ChordQualityExpression> chordQuality;
    UniquePointer<BassNoteExpression> bassNote;
    UniquePointer<SuspensionExpression> suspension;
    UniquePointer<KeyAlterationExpression> keyAlteration;
    UniquePointer<AdditionExpression> addition;
    UniquePointer<InversionExpression> inversion;

    // todo equals operator
};

class CleanupPass final
{
public:

    explicit CleanupPass(OwnedArray<Expression> expressions) : expressions(std::move(expressions)) {}

    ChordDescription getChord()
    {
        ChordDescription description;

        while (!this->expressions.isEmpty())
        {
            const auto *e = this->expressions.getFirst();
            switch (e->type)
            {
            case Expression::Type::RootKey:
                if (description.rootKey == nullptr)
                {
                    description.rootKey = this->removeAndReturnFirstAs<RootKeyExpression>();
                }
                else
                {
                    DBG("Duplicate root key expression");
                    this->expressions.remove(0);
                }
                break;

            case Expression::Type::ChordQuality:
                if (description.chordQuality == nullptr)
                {
                    description.chordQuality = this->removeAndReturnFirstAs<ChordQualityExpression>();
                }
                else
                {
                    DBG("Duplicate chord quality expression");
                    auto newExpr = this->removeAndReturnFirstAs<ChordQualityExpression>();

                    // refine the existing rule:
                    if (description.chordQuality->third == ChordQualityExpression::Third::Default &&
                        newExpr->third != ChordQualityExpression::Third::Default)
                    {
                        description.chordQuality->third = newExpr->third;
                    }

                    if (description.chordQuality->fifth == ChordQualityExpression::Fifth::Perfect &&
                        newExpr->fifth != ChordQualityExpression::Fifth::Perfect)
                    {
                        description.chordQuality->fifth = newExpr->fifth;
                    }

                    if (description.chordQuality->interval == 3 && newExpr->interval != 3)
                    {
                        description.chordQuality->interval = newExpr->interval;
                    }
                }
                break;

            case Expression::Type::Addition:
                if (description.addition == nullptr)
                {
                    description.addition = this->removeAndReturnFirstAs<AdditionExpression>();
                }
                else
                {
                    DBG("Duplicate addition expression");
                    this->expressions.remove(0);
                }
                break;

            case Expression::Type::Inversion:
                if (description.inversion == nullptr)
                {
                    description.inversion = this->removeAndReturnFirstAs<InversionExpression>();
                }
                else
                {
                    DBG("Duplicate inversion expression");
                    this->expressions.remove(0);
                }
                break;

            case Expression::Type::Suspension:
                if (description.suspension == nullptr)
                {
                    description.suspension = this->removeAndReturnFirstAs<SuspensionExpression>();
                }
                else
                {
                    DBG("Duplicate inversion expression");
                    this->expressions.remove(0);
                }
                break;

            case Expression::Type::KeyAlteration:
                if (description.keyAlteration == nullptr)
                {
                    description.keyAlteration = this->removeAndReturnFirstAs<KeyAlterationExpression>();
                }
                else
                {
                    DBG("Duplicate key alteration expression");
                    this->expressions.remove(0);
                }
                break;

            case Expression::Type::BassNote:
                if (description.bassNote == nullptr)
                {
                    description.bassNote = this->removeAndReturnFirstAs<BassNoteExpression>();
                }
                else
                {
                    DBG("Duplicate bass note expression");
                    this->expressions.remove(0);
                }
                break;

            default:
                break;
            }
        }

        // apply defaults

        //if (description.chordQuality != nullptr &&
        //    description.chordQuality->third == ChordQualityExpression::Third::Default)
        //{
        //    description.chordQuality->third = ChordQualityExpression::Third::Major;
        //}

        // if interval == 5, make third = default

        return description;
    }

private:

    template<typename T>
    UniquePointer<T> removeAndReturnFirstAs()
    {
        return UniquePointer<T>(static_cast<T *>(this->expressions.removeAndReturn(0)));
    }

    OwnedArray<Expression> expressions;
};

}

//===----------------------------------------------------------------------===//
// Chord and helpers
//===----------------------------------------------------------------------===//

class ChordCompiler final
{
public:

    ChordCompiler()
    {
        this->initSuggestions(this->rootKeySuggestions,
            "A", "Ab", "A#", "B", "Bb", "C", "C#", "D", "Db", "D#", "E", "Eb", "F", "F#", "G", "Gb", "G#");

        this->initSuggestions(this->chordQualitySuggestions,
            "min", "maj", "aug", "dim", "6", "7", "9", "11");

        this->initSuggestions(this->suspensionSuggestions,
            "sus2", "sus4");

        this->initSuggestions(this->additionSuggestions,
            "add9", "add11", "add13");

        this->initSuggestions(this->inversionSuggestions,
            "inv-3", "inv-2", "inv-1", "inv1", "inv2", "inv3");
    }

    void parse(const String &input)
    {
        using namespace ChordParsing;
        this->chord = CleanupPass(Parser(Lexer(input.getCharPointer()).getTokens()).getExpressions()).getChord();
    }

    void parse(String::CharPointerType input)
    {
        using namespace ChordParsing;
        this->chord = CleanupPass(Parser(Lexer(input).getTokens()).getExpressions()).getChord();
    }

    bool isValid() const noexcept
    {
        return this->chord.rootKey != nullptr && this->chord.rootKey->isValid();
    }

    String getChordAsString() const
    {
        String result;

        if (this->chord.rootKey != nullptr && this->chord.rootKey->isValid())
        {
            this->chord.rootKey->fillDescription(result);
        }

        if (this->chord.chordQuality != nullptr && this->chord.chordQuality->isValid())
        {
            this->chord.chordQuality->fillDescription(result);
        }

        if (this->chord.bassNote != nullptr && this->chord.bassNote->isValid())
        {
            this->chord.bassNote->fillDescription(result);
        }

        if (this->chord.suspension != nullptr && this->chord.suspension->isValid())
        {
            this->chord.suspension->fillDescription(result);
        }

        if (this->chord.keyAlteration != nullptr && this->chord.keyAlteration->isValid())
        {
            this->chord.keyAlteration->fillDescription(result);
        }

        if (this->chord.addition != nullptr && this->chord.addition->isValid())
        {
            this->chord.addition->fillDescription(result);
        }

        if (this->chord.inversion != nullptr && this->chord.inversion->isValid())
        {
            this->chord.inversion->fillDescription(result);
        }

        return result;
    }

    void fillSuggestions(CommandPaletteActionsProvider::Actions &actions) const
    {
        if (this->chord.rootKey == nullptr || !this->chord.rootKey->isValid())
        {
            actions.addArray(this->rootKeySuggestions);
            return;
        }

        if (this->chord.chordQuality == nullptr || !this->chord.chordQuality->isValid())
        {
            actions.addArray(this->chordQualitySuggestions);
            return;
        }

        if (this->chord.suspension == nullptr || !this->chord.suspension->isValid())
        {
            actions.addArray(this->suspensionSuggestions);
        }

        if (this->chord.inversion == nullptr || !this->chord.inversion->isValid())
        {
            actions.addArray(this->inversionSuggestions);
        }

        if (this->chord.addition == nullptr || !this->chord.addition->isValid())
        {
            actions.addArray(this->additionSuggestions);
        }
    }

private:

    ChordParsing::ChordDescription chord;
    StringArray lastChords;

    CommandPaletteActionsProvider::Actions rootKeySuggestions;
    CommandPaletteActionsProvider::Actions chordQualitySuggestions;
    CommandPaletteActionsProvider::Actions suspensionSuggestions;
    CommandPaletteActionsProvider::Actions inversionSuggestions;
    CommandPaletteActionsProvider::Actions additionSuggestions;

    template <typename... OtherElements>
    void initSuggestions(CommandPaletteActionsProvider::Actions &array, const String &firstElement, OtherElements... otherElements)
    {
        array.add(this->createSuggeation(firstElement));
        this->initSuggestions(array, otherElements...);
    }

    void initSuggestions(CommandPaletteActionsProvider::Actions &array, const String &firstElement)
    {
        array.add(this->createSuggeation(firstElement));
    }

    CommandPaletteAction::Ptr createSuggeation(String text)
    {
        return CommandPaletteAction::action(text, "suggestion", 0.f)->unfiltered()->
            withCallback([this, text](TextEditor &ed)
        {
            this->parse(ed.getText() + " " + text);
            ed.setText("$" + this->getChordAsString());
            return false;
        });
    }
};

//===----------------------------------------------------------------------===//
// CommandPaletteChordConstructor
//===----------------------------------------------------------------------===//

CommandPaletteChordConstructor::CommandPaletteChordConstructor() :
    chordCompiler(makeUnique<ChordCompiler>()) {}

CommandPaletteChordConstructor::~CommandPaletteChordConstructor() {}

void CommandPaletteChordConstructor::updateFilter(const String &pattern, bool skipPrefix)
{
    auto inputPtr = pattern.getCharPointer();
    if (skipPrefix)
    {
        inputPtr.getAndAdvance();
    }

    this->chordCompiler->parse(inputPtr);
    const auto chordAsString = this->chordCompiler->getChordAsString();
    DBG(chordAsString);

    this->actions.clearQuick();
    if (this->chordCompiler->isValid())
    {
        this->actions.add(CommandPaletteAction::action(chordAsString, "preview", -10.f)->
            withCallback([chordAsString](TextEditor &ed) { ed.setText("$" + chordAsString); return false; })->
            unfiltered());
    }

    this->chordCompiler->fillSuggestions(this->actions);

    CommandPaletteActionsProvider::updateFilter(pattern, skipPrefix);
}

void CommandPaletteChordConstructor::clearFilter()
{
    this->chordCompiler->parse("");
    this->actions.clearQuick();
    this->chordCompiler->fillSuggestions(this->actions);
    CommandPaletteActionsProvider::clearFilter();
}

const CommandPaletteActionsProvider::Actions &CommandPaletteChordConstructor::getActions() const
{
    return this->actions;
}
