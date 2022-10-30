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
#include "Scale.h"
#include "PianoRoll.h"
#include "PianoSequence.h"

/*
    This tool attempts to parse chord symbols, as described in:
    https://en.wikipedia.org/wiki/Chord_names_and_symbols_(popular_music)#Rules_to_decode_chord_names_and_symbols

    To approach that, I'm going to do something similar to what compilers are doing
    (inspired by this awesome guide, by the way: https://github.com/jamiebuilds/the-super-tiny-compiler):
    user input -> tokens -> internal representation -> passes -> generation

    Step 1 is lexical analysis, operating on string, returning list of tokens of these types:
        note: e.g. B or F (upper case)
        sign: # or b (lower case)
        slash: just / used to override bass note or add keys
        number: a number which can be negative, e.g. for negative inversion
        keyword: aug (also '+'), dim, sus, add, inv, min (also 'm'), maj (also 'M'), dom, might be lower case or start with capital
        space: 1 or more spaces, this token is only needed to avoid confusion when parsing expressions, e.g. "C/Ab 11" != "C/A b11"
        (any unknown characters are skipped)

    Step 2 is syntactic analysis, operating on list of tokens, returning list of expressions:
        root key: note token + optional sign - e.g. Bb or E#, or just B or E
        chord quality: either a keyword, one of [major, minor, augmented, diminished], or a number, or both: e.g. 11, M7, aug 9
        bass note: slash token + note token + optional sign, e.g. /Eb
        key alteration: sign + number - e.g. b5, #11
        addition: add keyword + optional sign + number, or slash token + optional sign + number - e.g. add b13 or /9 or /#11
        inversion: inv keyword + number - e.g. inv -3
        suspension: sus keyword + number - e.g. sus2
        (any invalid expressions are skipped)

    Step 3 is a cleanup pass, operating on list of expressions, returning mostly valid chord description:
        remove duplicates, if any
        remove expressions that don't make any sense, like are contradictory to existing context
        refine existing expressions: for example, chord quality might have been parsed as several expressions

    Step 4 is chord generation, operating on chord description and providing these methods:
        isValid - checks if missing required expressions: root key must be present, others are not required
        toString - turns parsed description back into symbols (to be suggested to user as a cleaned up string)
        fillSuggestions - provides suggestions list, dependent on current rules
        hasChanges - indicates if the expression has changed with the new input
        generate - generates new chord, or returns the last one, if the description hasn't changed
*/

namespace ChordParsing
{

//===----------------------------------------------------------------------===//
// Tokens
//===----------------------------------------------------------------------===//

#pragma region tokens

struct Token
{
    enum class Type : int8
    {
        Note,
        Sign,
        Slash,
        Space,
        Keyword,
        Number
    };

    Token() = delete;
    explicit Token(Type type) : type(type) {}
    virtual ~Token() = default;

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
    SlashToken() : Token(Token::Type::Slash) {}
};

struct SpaceToken final : Token
{
    SpaceToken() : Token(Token::Type::Space) {}
};

struct KeywordToken final : Token
{
    enum class Type : int8
    {
        Added,
        Inverted,
        Augmented,
        Diminished,
        Dominant,
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

#pragma endregion tokens

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
        auto t2 = t;

        switch (t2.getAndAdvance())
        {
        case '/':
            t = t2;
            return make<SlashToken>();

        case ' ':
            t = t2.findEndOfWhitespace();
            return make<SpaceToken>();

        case '+':
            t = t2;
            return make<KeywordToken>(KeywordToken::Type::Augmented);

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
        return make<NumberToken>((int8)correctedValue);
    }

    static UniquePointer<Token> parseNoteToken(String::CharPointerType &t)
    {
        switch (t.getAndAdvance())
        {
        case 'C':
            return make<NoteToken>(int8(0));
        case 'D':
            return make<NoteToken>(int8(1));
        case 'E':
            return make<NoteToken>(int8(2));
        case 'F':
            return make<NoteToken>(int8(3));
        case 'G':
            return make<NoteToken>(int8(4));
        case 'A':
            return make<NoteToken>(int8(5));
        case 'B':
        case 'H':
            return make<NoteToken>(int8(6));
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
            return make<SignToken>(true);
        case 'b':
            return make<SignToken>(false);
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
                return make<KeywordToken>(KeywordToken::Type::Augmented);

            case 'a': case 'A': // "add", "aug"
            {
                auto t3 = t2;
                if (t2.getAndAdvance() == 'd' && t2.getAndAdvance() == 'd')
                {
                    t = t2;
                    return make<KeywordToken>(KeywordToken::Type::Added);
                }
                else if (t3.getAndAdvance() == 'u' && t3.getAndAdvance() == 'g')
                {
                    t = t3;
                    return make<KeywordToken>(KeywordToken::Type::Augmented);
                }
                break;
            }

            case 'i': case 'I': // "inv"
                if (t2.getAndAdvance() == 'n' && t2.getAndAdvance() == 'v')
                {
                    t = t2;
                    return make<KeywordToken>(KeywordToken::Type::Inverted);
                }
                break;

            case 's': case 'S': // "sus"
                if (t2.getAndAdvance() == 'u' && t2.getAndAdvance() == 's')
                {
                    t = t2;
                    return make<KeywordToken>(KeywordToken::Type::Suspeneded);
                }
                break;

            case 'd': case 'D': // "dim", "dom"
            {
                auto t3 = t2;
                if (t2.getAndAdvance() == 'i' && t2.getAndAdvance() == 'm')
                {
                    t = t2;
                    return make<KeywordToken>(KeywordToken::Type::Diminished);
                }
                else if (t3.getAndAdvance() == 'o' && t3.getAndAdvance() == 'm')
                {
                    t = t3;
                    return make<KeywordToken>(KeywordToken::Type::Dominant);
                }
                break;
            }

            case 'm':
            {
                auto t3 = t2, t4 = t2;
                if (t3.getAndAdvance() == 'i' && t3.getAndAdvance() == 'n')
                {
                    t = t3;
                    return make<KeywordToken>(KeywordToken::Type::Minor);
                }
                else if (t4.getAndAdvance() == 'a' && t4.getAndAdvance() == 'j')
                {
                    t = t4;
                    return make<KeywordToken>(KeywordToken::Type::Major);
                }
                else
                {
                    t = t2;
                    return make<KeywordToken>(KeywordToken::Type::Minor);
                }
                break;
            }

            case 'M':
            {
                auto t3 = t2, t4 = t2;
                if (t3.getAndAdvance() == 'i' && t3.getAndAdvance() == 'n')
                {
                    t = t3;
                    return make<KeywordToken>(KeywordToken::Type::Minor);
                }
                else if (t4.getAndAdvance() == 'a' && t4.getAndAdvance() == 'j')
                {
                    t = t4;
                    return make<KeywordToken>(KeywordToken::Type::Major);
                }
                else
                {
                    t = t2;
                    return make<KeywordToken>(KeywordToken::Type::Major);
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

#pragma region expressions

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
    virtual ~Expression() = default;

    virtual bool isValid() const = 0;
    virtual void fillDescription(String &out) const = 0;
    String toString() const
    {
        String desc;
        this->fillDescription(desc);
        return desc;
    }

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
        Expression(Expression::Type::RootKey), key(note->noteNumber) {}

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
    const bool sharp = false;
    const bool flat = false;
};

struct ChordQualityExpression final : Expression
{
    ChordQualityExpression() : Expression(Expression::Type::ChordQuality) {}

    explicit ChordQualityExpression(const NumberToken *number) :
        Expression(Expression::Type::ChordQuality), interval(number->number) {}

    explicit ChordQualityExpression(const KeywordToken *keyword) :
        Expression(Expression::Type::ChordQuality)
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
        switch (keyword->keyword)
        {
        case KeywordToken::Type::Minor:
            this->chordQuality = Quality::Minor;
            break;
        case KeywordToken::Type::Major:
            this->chordQuality = Quality::Major;
            break;
        case KeywordToken::Type::Augmented:
            this->intervalQuality = Quality::Augmented;
            break;
        case KeywordToken::Type::Diminished:
            this->chordQuality = Quality::Minor; // another exception
            this->intervalQuality = Quality::Diminished;
            break;
        case KeywordToken::Type::Dominant: // basically, a shorthand for Mm
            this->chordQuality = Quality::Major;
            this->intervalQuality = Quality::Minor;
            break;
        default:
            jassertfalse;
            break;
        }
    }

    void updateIntervalQuality(ChordQualityExpression *newExpr)
    {
        // update interval quality, if it wasn't specified
        if (this->interval == 3 && newExpr->interval != 3)
        {
            this->interval = newExpr->interval;
            this->intervalQuality =
                (newExpr->intervalQuality == Quality::Implied) ?
                newExpr->chordQuality : newExpr->intervalQuality;
        }
    }

    enum class Quality : int8
    {
        Implied,
        Major,
        Minor,
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
        // skip the default implied chord quality
        if (this->chordQuality != Quality::Implied)
        {
            fillQualityDescription(out, this->chordQuality);
        }

        if (this->interval != 3)
        {
            if (this->intervalQuality != Quality::Implied &&
                this->intervalQuality != this->chordQuality)
            {
                out << " ";
                fillQualityDescription(out, this->intervalQuality);
            }

            out << int(this->interval);
        }
        else if (this->intervalQuality == Quality::Augmented ||
            this->intervalQuality == Quality::Diminished)
        {
            out << " ";
            fillQualityDescription(out, this->intervalQuality);
        }
    }

    void fillQualityDescription(String &out, Quality q) const
    {
        switch (q)
        {
        case Quality::Implied:
        case Quality::Major: out << "M"; break;
        case Quality::Minor: out << "m"; break;
        case Quality::Augmented: out << "aug"; break;
        case Quality::Diminished: out << "dim"; break;
        default: break;
        }
    }

    Quality chordQuality = Quality::Implied;
    Quality intervalQuality = Quality::Implied;
    int8 interval = 3;
};

struct BassNoteExpression final : Expression
{
    explicit BassNoteExpression(const NoteToken *bassNote) :
        Expression(Expression::Type::BassNote), key(bassNote->noteNumber) {}

    BassNoteExpression(const NoteToken *bassNote, const SignToken *sign) :
        Expression(Expression::Type::BassNote), key(bassNote->noteNumber), sharp(sign->isSharp), flat(!sign->isSharp) {}

    bool isValid() const override
    {
        return (this->key >= 0 && this->key <= 6) && !(this->sharp && this->flat);
    }

    void fillDescription(String &out) const override
    {
        out << "/" << keyToLetter(this->key);

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
    const bool sharp = false;
    const bool flat = false;
};

struct AdditionExpression final : Expression
{
    explicit AdditionExpression(const NumberToken *number) :
        Expression(Expression::Type::Addition), addedKey(number->number) {}

    AdditionExpression(const SignToken *sign, const NumberToken *number) :
        Expression(Expression::Type::Addition), addedKey(number->number), sharp(sign->isSharp), flat(!sign->isSharp) {}

    bool isValid() const override
    {
        // what's the upper limit for the added key?
        return (this->addedKey > 0 && this->addedKey <= 32) && !(this->sharp && this->flat);
    }

    void fillDescription(String &out) const override
    {
        out << " add";
        
        if (this->sharp)
        {
            out << "#";
        }
        else if (this->flat)
        {
            out << "b";
        }

        out << (int)this->addedKey;
    }

    const int8 addedKey;
    const bool sharp = false;
    const bool flat = false;
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
        return (this->suspension == 2 || this->suspension == 4);
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

#pragma endregion expressions

class Parser final
{
public:

    explicit Parser(OwnedArray<Token> tokens) : tokens(move(tokens)) {}

    OwnedArray<Expression> getExpressions() const
    {
        OwnedArray<Expression> expressions;

        int idx = 0;
        while (idx < this->tokens.size())
        {
            if (auto expr = this->parseExpression(idx))
            {
                if (expr->isValid())
                {
                    expressions.add(expr.release());
                }
                else
                {
                    DBG("Expression " + expr->toString() + " is invalid, skipping");
                }
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
                return make<KeyAlterationExpression>(signToken, numberToken);
            }
            break;

        case Token::Type::Slash:
            if (nextToken != nullptr && nextToken->type == Token::Type::Note)
            {
                const auto *noteToken = static_cast<const NoteToken *>(nextToken);
                const auto *thirdToken = this->tokens[t + 2];

                if (thirdToken != nullptr && thirdToken->type == Token::Type::Sign)
                {
                    t += 3;
                    const auto *signToken = static_cast<const SignToken *>(thirdToken);
                    return make<BassNoteExpression>(noteToken, signToken);
                }

                t += 2;
                return make<BassNoteExpression>(noteToken);
            }
            else if (nextToken != nullptr && nextToken->type == Token::Type::Number)
            {
                t += 2;
                const auto *numberToken = static_cast<const NumberToken *>(nextToken);
                return make<AdditionExpression>(numberToken);
            }
            break;

        case Token::Type::Space:
        default:
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
            return make<RootKeyExpression>(token, signToken);
        }

        t++;
        return make<RootKeyExpression>(token);
    }

    UniquePointer<Expression> parseKeywords(int &t) const
    {
        auto t2 = t;
        const auto *token = static_cast<KeywordToken *>(this->tokens.getUnchecked(t));
        const auto *nextToken = this->tokens[++t2];

        // keyword expressions are allowed to have a space before a number, e.g. inv -2
        if (nextToken != nullptr && nextToken->type == Token::Type::Space)
        {
            nextToken = this->tokens[++t2];
        }

        switch (token->keyword)
        {
        case KeywordToken::Type::Added:
            if (nextToken != nullptr && nextToken->type == Token::Type::Number)
            {
                t = ++t2;
                const auto *numberToken = static_cast<const NumberToken *>(nextToken);
                return make<AdditionExpression>(numberToken);
            }
            else if (nextToken != nullptr && nextToken->type == Token::Type::Sign)
            {
                const auto *thirdToken = this->tokens[++t2];
                if (thirdToken != nullptr && thirdToken->type == Token::Type::Number)
                {
                    t = ++t2;
                    const auto *signToken = static_cast<const SignToken *>(nextToken);
                    const auto *numberToken = static_cast<const NumberToken *>(thirdToken);
                    return make<AdditionExpression>(signToken, numberToken);
                }
            }
            break;

        case KeywordToken::Type::Inverted:
            if (nextToken != nullptr && nextToken->type == Token::Type::Number)
            {
                t = ++t2;
                const auto *numberToken = static_cast<const NumberToken *>(nextToken);
                return make<InversionExpression>(numberToken);
            }
            break;

        case KeywordToken::Type::Suspeneded:
            if (nextToken != nullptr && nextToken->type == Token::Type::Number)
            {
                t = ++t2;
                const auto *numberToken = static_cast<const NumberToken *>(nextToken);
                return make<SuspensionExpression>(numberToken);
            }
            break;

        case KeywordToken::Type::Major:
        case KeywordToken::Type::Minor:
        case KeywordToken::Type::Augmented:
        case KeywordToken::Type::Diminished:
        case KeywordToken::Type::Dominant:
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
                return make<ChordQualityExpression>(keywordToken, numberToken);
            }

            t++;
            return make<ChordQualityExpression>(keywordToken);
        }
        else if (token->type == Token::Type::Number)
        {
            const auto *numberToken = static_cast<NumberToken *>(this->tokens.getUnchecked(t));
            if (nextToken != nullptr && nextToken->type == Token::Type::Keyword)
            {
                // keyword after a number is a weird case, but let's support it as well
                const auto *keywordToken = static_cast<const KeywordToken *>(nextToken);
                switch (keywordToken->keyword)
                {
                case KeywordToken::Type::Major:
                case KeywordToken::Type::Minor:
                case KeywordToken::Type::Augmented:
                case KeywordToken::Type::Diminished:
                    t += 2;
                    return make<ChordQualityExpression>(keywordToken, numberToken);
                default:
                    break;
                }
            }

            t++;
            return make<ChordQualityExpression>(numberToken);
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
};

class CleanupPass final
{
public:

    explicit CleanupPass(OwnedArray<Expression> expressions) : expressions(move(expressions)) {}

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
                    auto newExpr = this->removeAndReturnFirstAs<ChordQualityExpression>();
                    description.chordQuality->updateIntervalQuality(newExpr.get());
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

        // todo more specific rules for the plain interval number exceptions:
        if (description.chordQuality != nullptr &&
            description.chordQuality->chordQuality == ChordQualityExpression::Quality::Major &&
            description.chordQuality->intervalQuality == ChordQualityExpression::Quality::Major)
        {
            if (description.chordQuality->interval % 2 == 0)
            {
                // a major added tone chord 
            }
            else
            {
                // a dominant chord
            }
        }

        if (description.chordQuality != nullptr &&
            description.keyAlteration != nullptr &&
            description.keyAlteration->key == 5 &&
            (description.chordQuality->intervalQuality == ChordQualityExpression::Quality::Augmented ||
                description.chordQuality->intervalQuality == ChordQualityExpression::Quality::Diminished))
        {
            DBG("Fifth alteration makes no sense on aug/dim chords, removing");
            description.keyAlteration = nullptr;
        }

        if (description.suspension != nullptr &&
            (description.suspension->suspension == 2 || description.suspension->suspension == 4))
        {
            if (description.chordQuality != nullptr && description.chordQuality->interval == 5)
            {
                DBG("Suspended 2 or 4 make no sense on power chords, removing");
                description.suspension = nullptr;
            }
        }

        if (description.addition != nullptr &&
            description.chordQuality != nullptr &&
            description.addition->addedKey <= description.chordQuality->interval)
        {
            DBG("Addition doesn't make sense when less or equal to chord quality interval, removing");
            description.addition = nullptr;
        }

        if (description.rootKey != nullptr &&
            description.bassNote != nullptr &&
            (description.bassNote->key == description.rootKey->key &&
                description.bassNote->flat == description.rootKey->flat &&
                description.bassNote->sharp == description.rootKey->sharp))
        {
            DBG("Bass note override makes no sense when it equals the root key, removing");
            description.bassNote = nullptr;
        }

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

enum class KeyInfo : int8
{
    None,
    Default,
    Flat,
    Sharp
};

struct RenderedChord final
{
    RenderedChord()
    {
        this->reset();
    }
    
    void reset()
    {
        this->keys.clearQuick();
        this->keys.add(KeyInfo::Default);

        // todo what is the max chord size??
        for (int i = 0; i < 32; ++i)
        {
            this->keys.add(KeyInfo::None);
        }
    }

    bool operator ==(const RenderedChord &other) const
    {
        return this->keys == other.keys;
    }

    KeyInfo &operator[] (int index)
    {
        if (index < this->keys.size())
        {
            return this->keys.getReference(index);
        }

        return this->noKey;
    }

    Array<KeyInfo> keys;
    KeyInfo noKey = KeyInfo::None;
};

class ChordCompiler final
{
public:

    ChordCompiler()
    {
        this->initSuggestions(this->rootKeySuggestions,
            "Ab", "A", "A#", "Bb", "B", "C", "C#", "Db", "D", "D#", "Eb", "E", "F", "F#", "Gb", "G", "G#");

        this->initSuggestions(this->scaleSuggestions,
            "min", "maj", "m7", "M7", "mM7");

        this->initSuggestions(this->chordQualitySuggestions,
            "aug", "dim", "6", "7", "9", "11");

        this->initSuggestions(this->alterationSuggestions,
            "#5", "b5");

        this->initSuggestions(this->suspensionSuggestions,
            "sus 2", "sus 4");

        this->initSuggestions(this->additionSuggestions,
            "add 9", "add b9", "add #9", "add b11", "add 11", "add #11", "add b13", "add 13", "add #13");

        this->initSuggestions(this->inversionSuggestions,
            "inv -3", "inv -2", "inv -1", "inv 1", "inv 2", "inv 3");

        this->initSuggestions(this->bassNoteSuggestions,
            "/Ab", "/A", "/A#", "/Bb", "/B", "/C", "/C#", "/Db", "/D", "/D#", "/Eb", "/E", "/F", "/F#", "/Gb", "/G", "/G#");
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
        return this->chord.rootKey != nullptr;
    }

    bool generate(Array<int> &outResult) const
    {
        using namespace ChordParsing;

        if (!this->isValid())
        {
            return false;
        }

        const auto *quality = (this->chord.chordQuality != nullptr) ?
            this->chord.chordQuality.get() : this->qualityFallback.get();

        const auto scaleToUse = quality->chordQuality == ChordQualityExpression::Quality::Minor ? this->minor : this->major;

        const auto tonic = scaleToUse->getChromaticKey(this->chord.rootKey->key,
            this->chord.rootKey->sharp ? 1 : (this->chord.rootKey->flat ? -1 : 0), false);

        RenderedChord chord;

        const auto getKey = [scaleToUse, this](const ChordQualityExpression *quality, int targetInterval)
        {
            if (targetInterval == 5)
            {
                return (quality->intervalQuality == ChordQualityExpression::Quality::Augmented ? KeyInfo::Sharp :
                    quality->intervalQuality == ChordQualityExpression::Quality::Diminished ? KeyInfo::Flat : KeyInfo::Default);
            }

            // handle other exceptions:
            // (todo check if there are more weird exceptions)
            if (quality->chordQuality == ChordQualityExpression::Quality::Implied ||
                quality->intervalQuality == ChordQualityExpression::Quality::Implied)
            {
                if ((targetInterval == 6 || targetInterval == 13) && scaleToUse == this->minor)
                {
                    return KeyInfo::Sharp;
                }

                if ((targetInterval == 7) && scaleToUse != this->minor &&
                    quality->chordQuality == ChordQualityExpression::Quality::Implied)
                {
                    return KeyInfo::Flat;
                }
            }
            
            // if the interval quality is the same as the chord quality, it's up to scale to decide:
            if (quality->intervalQuality != quality->chordQuality)
            {
                const bool minOrDim = (quality->intervalQuality == ChordQualityExpression::Quality::Minor ||
                    quality->intervalQuality == ChordQualityExpression::Quality::Diminished);

                const bool maj = (quality->intervalQuality == ChordQualityExpression::Quality::Major);
                return minOrDim ? KeyInfo::Flat : (maj ? KeyInfo::Sharp : KeyInfo::Default);
            }

            return KeyInfo::Default;
        };

        // add triad, if not a power chord:
        if (quality->interval != 5)
        {
            chord[3] = KeyInfo::Default; // set by scale
        }

        // fifth is always present:
        chord[5] = getKey(quality, 5);

        // fill the remaining keys:
        const auto keyToStartFrom = (quality->interval % 2 == 0) ? 6 : 7;
        for (int i = keyToStartFrom; i <= quality->interval; i += 2)
        {
            chord[i] = getKey(quality, i);
        }
        
        const auto *sus = this->chord.suspension.get();
        if (sus != nullptr)
        {
            jassert(sus->isValid());
            chord[3] = KeyInfo::None;
            chord[sus->suspension] = KeyInfo::Default;
        }
        
        const auto *add = this->chord.addition.get();
        if (add != nullptr)
        {
            jassert(add->isValid());
            chord[add->addedKey] = add->flat ? KeyInfo::Flat :
                (add->sharp ? KeyInfo::Sharp : getKey(quality, add->addedKey));
        }

        const auto *alt = this->chord.keyAlteration.get();
        if (alt != nullptr)
        {
            jassert(alt->isValid());
            chord[alt->key] = alt->sharp ? KeyInfo::Sharp : KeyInfo::Flat;
        }

        outResult.clearQuick();
        outResult.add(tonic);
        for (int i = 1; i < chord.keys.size(); ++i)
        {
            const auto &key = chord[i];
            if (key != KeyInfo::None)
            {
                const auto chromaticKey = scaleToUse->getChromaticKey(i - 1,
                    key == KeyInfo::Sharp ? 1 : (key == KeyInfo::Flat ? -1 : 0), false);
                outResult.add(tonic + chromaticKey);
            }
        }

        const auto basePeriod = scaleToUse->getBasePeriod();

        const auto *bass = this->chord.bassNote.get();
        if (bass != nullptr)
        {
            jassert(bass->isValid());
            const auto bassNote = scaleToUse->getChromaticKey(bass->key,
                bass->sharp ? 1 : (bass->flat ? -1 : 0), false);

            // remove existing notes colliding with bass, if any
            outResult.removeAllInstancesOf(bassNote);
            outResult.insert(0, bassNote - basePeriod);
        }

        const auto *inv = this->chord.inversion.get();
        if (inv != nullptr)
        {
            jassert(inv->isValid());
            auto i = inv->inversion;
            while (i != 0)
            {
                if (inv->inversion > 0)
                {
                    auto inverted = outResult[0] + basePeriod;
                    outResult.remove(0);
                    outResult.add(inverted);
                    i--;
                }
                else
                {
                    auto inverted = outResult.getLast() - basePeriod;
                    outResult.removeLast();
                    outResult.insert(0, inverted);
                    i++;
                }
            }
        }

        const bool hasChanges = this->lastResult != outResult;
        this->lastResult = outResult;

        return hasChanges;
    }

    String getChordAsString() const
    {
        String result;

        if (!this->isValid())
        {
            return result;
        }

        if (this->chord.rootKey != nullptr)
        {
            this->chord.rootKey->fillDescription(result);
        }

        if (this->chord.chordQuality != nullptr)
        {
            this->chord.chordQuality->fillDescription(result);
        }

        if (this->chord.bassNote != nullptr)
        {
            this->chord.bassNote->fillDescription(result);
        }

        if (this->chord.suspension != nullptr)
        {
            this->chord.suspension->fillDescription(result);
        }

        if (this->chord.keyAlteration != nullptr)
        {
            this->chord.keyAlteration->fillDescription(result);
        }

        if (this->chord.addition != nullptr)
        {
            this->chord.addition->fillDescription(result);
        }

        if (this->chord.inversion != nullptr)
        {
            this->chord.inversion->fillDescription(result);
        }

        return result;
    }

    void fillSuggestions(CommandPaletteActionsProvider::Actions &actions) const
    {
        // todo be more smart

        if (this->chord.rootKey == nullptr)
        {
            actions.addArray(this->rootKeySuggestions);
            return;
        }

        if (this->chord.chordQuality == nullptr)
        {
            if (this->chord.suspension == nullptr)
            {
                actions.addArray(this->scaleSuggestions);
            }

            actions.addArray(this->chordQualitySuggestions);
            return;
        }

        if (this->chord.suspension == nullptr)
        {
            actions.addArray(this->suspensionSuggestions);
        }

        if (this->chord.addition == nullptr)
        {
            actions.addArray(this->additionSuggestions);
        }

        if (this->chord.inversion == nullptr)
        {
            actions.addArray(this->inversionSuggestions);
        }

        if (this->chord.bassNote == nullptr)
        {
            actions.addArray(this->bassNoteSuggestions);
        }
    }

private:

    // fixme naming?
    ChordParsing::ChordDescription chord;
    mutable Array<int> lastResult;

    const Scale::Ptr major = Scale::getNaturalMajorScale();
    const Scale::Ptr minor = Scale::getNaturalMinorScale();
    const UniquePointer<ChordParsing::ChordQualityExpression> qualityFallback =
        make<ChordParsing::ChordQualityExpression>();

    CommandPaletteActionsProvider::Actions rootKeySuggestions;
    CommandPaletteActionsProvider::Actions chordQualitySuggestions;
    CommandPaletteActionsProvider::Actions scaleSuggestions;
    CommandPaletteActionsProvider::Actions suspensionSuggestions;
    CommandPaletteActionsProvider::Actions inversionSuggestions;
    CommandPaletteActionsProvider::Actions additionSuggestions;
    CommandPaletteActionsProvider::Actions bassNoteSuggestions;
    CommandPaletteActionsProvider::Actions alterationSuggestions;

    template <typename... OtherElements>
    void initSuggestions(CommandPaletteActionsProvider::Actions &array,
        const String &firstElement, OtherElements... otherElements)
    {
        array.add(this->createSuggestion(firstElement));
        this->initSuggestions(array, otherElements...);
    }

    void initSuggestions(CommandPaletteActionsProvider::Actions &array, const String &firstElement)
    {
        array.add(this->createSuggestion(firstElement));
    }

    CommandPaletteAction::Ptr createSuggestion(String text)
    {
        return CommandPaletteAction::action(text, TRANS(I18n::CommandPalette::chordSuggestion), 0.f)->
            unfiltered()->withCallback([this, text](TextEditor &ed)
        {
            // not just append a piece of text, but also turn in into a valid description:
            this->parse(ed.getText() + " " + text);
            ed.setText("!" + this->getChordAsString());
            return false;
        });
    }
};

//===----------------------------------------------------------------------===//
// CommandPaletteChordConstructor
//===----------------------------------------------------------------------===//

CommandPaletteChordConstructor::CommandPaletteChordConstructor(PianoRoll &roll) :
    CommandPaletteActionsProvider(TRANS(I18n::CommandPalette::chordBuilder), '!', -3.f),
    chordCompiler(make<ChordCompiler>()),
    roll(roll) {}

CommandPaletteChordConstructor::~CommandPaletteChordConstructor() = default;

void CommandPaletteChordConstructor::updateFilter(const String &pattern, bool skipPrefix)
{
    auto inputPtr = pattern.getCharPointer();
    if (skipPrefix)
    {
        inputPtr.getAndAdvance();
    }

    this->chordCompiler->parse(inputPtr);
    const auto chordAsString = this->chordCompiler->getChordAsString();

    this->actions.clearQuick();
    if (this->chordCompiler->isValid())
    {
        this->actions.add(CommandPaletteAction::action(chordAsString, TRANS(I18n::CommandPalette::chordGenerate), -10.f)->
            unfiltered()->withCallback([this, chordAsString](TextEditor &ed)
        {
            this->previewIfNeeded();
            ed.setText("!" + chordAsString);
            return false;
        }));
    }
    else
    {
        this->undoIfNeeded();
    }

    this->chordCompiler->fillSuggestions(this->actions);

    CommandPaletteActionsProvider::updateFilter(pattern, skipPrefix);
}

void CommandPaletteChordConstructor::clearFilter()
{
    this->actions.clearQuick();
    this->chordCompiler->parse({});
    this->chordCompiler->fillSuggestions(this->actions);
    this->undoIfNeeded();

    CommandPaletteActionsProvider::clearFilter();
}

const CommandPaletteActionsProvider::Actions &CommandPaletteChordConstructor::getActions() const
{
    return this->actions;
}

void CommandPaletteChordConstructor::undoIfNeeded()
{
    if (this->hasMadeChanges)
    {
        auto *sequence = this->roll.getActiveTrack()->getSequence();
        this->roll.getTransport().stopSound(sequence->getTrackId());
        sequence->undoCurrentTransactionOnly();
        this->hasMadeChanges = false;
    }
}

void CommandPaletteChordConstructor::previewIfNeeded()
{
    const bool hasNewChord = this->chordCompiler->generate(this->chord);
    if (hasNewChord)
    {
        this->undoIfNeeded();

        if (auto *pianoSequence =
            dynamic_cast<PianoSequence *>(this->roll.getActiveTrack()->getSequence()))
        {
            const auto clipKey = this->roll.getActiveClip().getKey();
            const auto clipBeat = this->roll.getActiveClip().getBeat();
            const auto targetBeat = this->roll.getTransport().getSeekBeat() - clipBeat;

            int minKey = this->roll.getNumKeys();
            int maxKey = 0;

            bool atLeastOneNoteShowsInViewport = false;

            for (const auto &relativeKey : this->chord)
            {
                const auto key = jlimit(0, this->roll.getNumKeys(),
                    this->roll.getMiddleC() + relativeKey);

                minKey = jmin(minKey, key);
                maxKey = jmax(maxKey, key);

                const Note note(pianoSequence, key, targetBeat,
                    CommandPaletteChordConstructor::noteLength,
                    CommandPaletteChordConstructor::noteVelocity);

                pianoSequence->insert(note, true);
                this->hasMadeChanges = true;

                atLeastOneNoteShowsInViewport = atLeastOneNoteShowsInViewport ||
                    this->roll.isNoteVisible(clipKey + key, clipBeat + targetBeat,
                        CommandPaletteChordConstructor::noteLength);

                this->roll.getTransport().previewKey(pianoSequence->getTrackId(),
                    key + clipKey,
                    CommandPaletteChordConstructor::noteVelocity,
                    CommandPaletteChordConstructor::noteLength);
            }

            if (!atLeastOneNoteShowsInViewport)
            {
                constexpr auto keyMarginTop = 8; // leave some space for the command palette
                constexpr auto keyMarginBottom = 2;
                constexpr auto beatMargin = 2.f;
                this->roll.zoomToArea(clipKey + minKey - keyMarginBottom,
                    clipKey + maxKey + keyMarginTop,
                    clipBeat + targetBeat - beatMargin,
                    clipBeat + targetBeat + CommandPaletteChordConstructor::noteLength + beatMargin);
            }
        }
    }
}


//===----------------------------------------------------------------------===//
// Tests
//===----------------------------------------------------------------------===//

#if JUCE_UNIT_TESTS

class ChordCompilerTests final : public UnitTest
{
public:
    ChordCompilerTests() : UnitTest("Chord compiler tests", UnitTestCategories::helio) {}

    void runTest() override
    {
        ChordCompiler cc;

        beginTest("Parsing and validation");

        cc.parse("Bb 9 inv -2 sus 2 b5  ");
        expect(cc.isValid());
        expectEquals(cc.getChordAsString(), { "Bb9 sus2 b5 inv-2" });

        cc.parse("  A#mM7 add 13");
        expect(cc.isValid());
        expectEquals(cc.getChordAsString(), { "A#m M7 add13" });

        cc.parse(" C# aug ");
        expect(cc.isValid());
        expectEquals(cc.getChordAsString(), { "C# aug" });

        cc.parse("yo yo sus4 b11 inv7");
        expect(!cc.isValid());
        expectEquals(cc.getChordAsString(), { "" });

        cc.parse("");
        expect(!cc.isValid());
        expectEquals(cc.getChordAsString(), { "" });

        beginTest("Suggestions");

        CommandPaletteActionsProvider::Actions actions;

        cc.parse("E min ");
        cc.fillSuggestions(actions);
        expectEquals(actions.getFirst()->getName(), { "sus 2" });

        // todo: suggestions need to be smarter, so more tests to come

        beginTest("Chords generation, basic rules");

        testRenderedChord(cc, "C#5", { 1, 8 });
        testRenderedChord(cc, "C maj", { 0, 4, 7 });
        testRenderedChord(cc, "C min", { 0, 3, 7 });
        testRenderedChord(cc, "C maj6", { 0, 4, 7, 9 });
        testRenderedChord(cc, "C min6", { 0, 3, 7, 9 });
        testRenderedChord(cc, "C maj7", { 0, 4, 7, 11 });
        testRenderedChord(cc, "C min7", { 0, 3, 7, 10 });
        testRenderedChord(cc, "C mM7", { 0, 3, 7, 11 });
        testRenderedChord(cc, "C min maj7", { 0, 3, 7, 11 });
        testRenderedChord(cc, "C Mm7", { 0, 4, 7, 10 });
        testRenderedChord(cc, "C dom7", { 0, 4, 7, 10 });
        testRenderedChord(cc, "C maj9", { 0, 4, 7, 11, 14 });
        testRenderedChord(cc, "C min9", { 0, 3, 7, 10, 14 });
        testRenderedChord(cc, "C maj11", { 0, 4, 7, 11, 14, 17 });
        testRenderedChord(cc, "C min11", { 0, 3, 7, 10, 14, 17 });
        testRenderedChord(cc, "C maj13", { 0, 4, 7, 11, 14, 17, 21 });
        testRenderedChord(cc, "C min13", { 0, 3, 7, 10, 14, 17, 21 });

        beginTest("Chords generation, augmented and diminished");

        testRenderedChord(cc, "C aug", { 0, 4, 8 });
        testRenderedChord(cc, "C dim", { 0, 3, 6 });
        testRenderedChord(cc, "C aug7", { 0, 4, 8, 10 });
        testRenderedChord(cc, "C dim7", { 0, 3, 6, 9 });

        beginTest("Chords generation, key alterations");

        testRenderedChord(cc, "C M7#5", { 0, 4, 8, 11 });
        testRenderedChord(cc, "C m7b5", { 0, 3, 6, 10 });

        beginTest("Chords generation, suspensions");

        testRenderedChord(cc, "C sus2", { 0, 2, 7 });
        testRenderedChord(cc, "C sus4", { 0, 5, 7 });
        testRenderedChord(cc, "C M7sus2", { 0, 2, 7, 11 });
        testRenderedChord(cc, "C M7sus4", { 0, 5, 7, 11 });
        testRenderedChord(cc, "C 7sus4", { 0, 5, 7, 10 });

        beginTest("Chords generation, additions");

        testRenderedChord(cc, "Cm6/9", { 0, 3, 7, 9, 14 });
        testRenderedChord(cc, "Cm7add #11", { 0, 3, 7, 10, 18 });
        testRenderedChord(cc, "CM9/13", { 0, 4, 7, 11, 14, 21 });
        testRenderedChord(cc, "Cm9 add13", { 0, 3, 7, 10, 14, 21 });

        beginTest("Chords generation, inversions");

        testRenderedChord(cc, "C maj inv 2", { 7, 12, 16 });
        testRenderedChord(cc, "C min inv -3", { -12, -9, -5 });

        beginTest("Chords generation, bass notes");

        testRenderedChord(cc, "Cm6/9 /E", { -9, 0, 7, 9, 14 });
        testRenderedChord(cc, "C mM7 /G", { -5, 0, 3, 11 });
    }

    void testRenderedChord(ChordCompiler &compiler, const String &chord,
        const Array<int> &expect)
    {
        Array<int> render;
        compiler.parse(chord);
        compiler.generate(render);
        expectEquals(render.size(), expect.size());
        for (int i = 0; i < render.size(); ++i)
        {
            expectEquals(render[i], expect[i]);
        }
    }
};

static ChordCompilerTests chordCompilerTests;

#endif
