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

//===----------------------------------------------------------------------===//
// (only used for basic lisp code highlighting, not a real tokeniser)
//===----------------------------------------------------------------------===//

class ScriptTokeniser final : public CodeTokeniser
{
public:

    ScriptTokeniser() = default;

    int readNextToken(CodeDocument::Iterator &source) override
    {
        source.skipWhitespace();

        const auto firstChar = source.peekNextChar();

        if (!this->errorRange.isEmpty() &&
            //this->errorRange.contains(source.getPosition()))
            (source.getPosition() == this->errorRange.getStart() ||
                source.getPosition() == this->errorRange.getEnd() - 1))
        {
            source.skip();
            return TokenType::tokenTypeError;
        }

        if (!this->caretBracketRange.isEmpty() &&
            (source.getPosition() == this->caretBracketRange.getStart() ||
                source.getPosition() == this->caretBracketRange.getEnd() - 1))
        {
            source.skip();
            return TokenType::tokenTypeBracketMatch;
        }

        switch (firstChar)
        {
        case 0:
            break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '.': case '-':
        {
            if (CppTokeniserFunctions::parseNumber(source) == TokenType::tokenTypeError)
            {
                source.skip();

                if (firstChar == '-')
                {
                    return TokenType::tokenTypeReservedKeyword;
                }
            }
            return TokenType::tokenTypeLiteral;
        }
        case '(': case ')':
            source.skip();
            return TokenType::tokenTypeBracket;
        case '"':
            CppTokeniserFunctions::skipQuotedString(source);
            return TokenType::tokenTypeLiteral;
        case '\'': case '=': case '%': case '+': case '*': case '/':
            source.skip();
            return TokenType::tokenTypeReservedKeyword;
        case '<': case '>': case '!':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches(source, '=');
            return TokenType::tokenTypeReservedKeyword;
        case '#':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches(source, 't', 'f');
            return TokenType::tokenTypeReservedKeyword;
        case ';':
        {
            source.skipToEndOfLine();
            return TokenType::tokenTypeComment;
        }
        default:
            if (script::isSymbolBody(firstChar))
            {
                return this->parseIdentifier(source);
            }

            source.skip();
            break;
        }

        return TokenType::tokenTypeError;
    }

    CodeEditorComponent::ColourScheme getDefaultColourScheme() override
    {
        const CodeEditorComponent::ColourScheme::TokenType types[] =
        {
            { "Error",          findDefaultColour(ColourIDs::CodeEditor::error) },
            { "Comment",        findDefaultColour(ColourIDs::CodeEditor::comment) },
            { "Keyword",        findDefaultColour(ColourIDs::CodeEditor::keyword) },
            { "Function",       findDefaultColour(ColourIDs::CodeEditor::function) },
            { "Identifier",     findDefaultColour(ColourIDs::CodeEditor::identifier) },
            { "Literal",        findDefaultColour(ColourIDs::CodeEditor::literal) },
            { "Bracket",        findDefaultColour(ColourIDs::CodeEditor::bracket) },
            { "BracketMatch",   findDefaultColour(ColourIDs::CodeEditor::bracketMatch) },
            { "Highlight",      findDefaultColour(ColourIDs::CodeEditor::highlight) }
        };

        CodeEditorComponent::ColourScheme cs;
        for (auto &t : types)
        {
            cs.set(t.name, t.colour);
        }

        return cs;
    }

    void updateParsingData(const Array<Range<int>> &ranges)
    {
        this->bracketRanges = ranges;
    }

    void updateEvaluationData(const StringArray &functionNames)
    {
        this->userFunctions.clear();
        for (const auto &name : functionNames)
        {
            this->userFunctions.insert(name);
        }
    }

    void setCaretPosition(int caretPosition)
    {
        this->caretBracketRange = {};

        int minRange = INT_MAX;
        for (const auto &range : this->bracketRanges)
        {
            if (range.contains(caretPosition))
            {
                if (range.getLength() < minRange)
                {
                    minRange = range.getLength();
                    this->caretBracketRange = range;
                }
                else
                {
                    break;
                }
            }
        }
    }

    void setHighlightedToken(const String &token)
    {
        String::CharPointerType writer(this->highlightedToken);
        writer.writeAll(token.getCharPointer());
    }

    void setErrorRange(Range<int> range)
    {
        this->errorRange = range;
    }

    void onTextInserted(int startIndex, int length)
    {
        // update these two ranges only to avoid flickering while editing
        // (bracketRanges will update after parsing, that's fine)
        if (this->errorRange.contains(startIndex))
        {
            this->errorRange.setLength(this->errorRange.getLength() + length);
        }
        else if (this->errorRange.getStart() > startIndex)
        {
            this->errorRange += length;
        }

        if (this->caretBracketRange.contains(startIndex))
        {
            this->caretBracketRange.setLength(this->caretBracketRange.getLength() + length);
        }
    }

    void onTextDeleted(int startIndex, int endIndex)
    {
        if (this->errorRange.contains(startIndex) ||
            this->errorRange.contains(endIndex))
        {
            const auto deletedLength =
                jmin(endIndex, this->errorRange.getEnd()) -
                jmax(startIndex, this->errorRange.getStart());
            this->errorRange.setLength(this->errorRange.getLength() - deletedLength);
        }
        else if (this->errorRange.getStart() > startIndex)
        {
            if (this->errorRange.getEnd() < endIndex)
            {
                this->errorRange = {};
            }
            else
            {
                this->errorRange -= (endIndex - startIndex);
            }
        }

        if (this->caretBracketRange.contains(startIndex) ||
            this->caretBracketRange.contains(endIndex))
        {
            const auto deletedLength =
                jmin(endIndex, this->caretBracketRange.getEnd()) -
                jmax(startIndex, this->caretBracketRange.getStart());
            this->caretBracketRange.setLength(this->caretBracketRange.getLength() - deletedLength);
        }
    }

    enum TokenType
    {
        tokenTypeError = 0,
        tokenTypeComment,
        tokenTypeReservedKeyword,
        tokenTypeFunction,
        tokenTypeIdentifier,
        tokenTypeLiteral,
        tokenTypeBracket,
        tokenTypeBracketMatch,
        tokenTypeHighlight
    };

    static constexpr auto maxTokenLength = 69;

private:

    static bool isReservedKeyword(String::CharPointerType token, const int tokenLength) noexcept
    {
        static const char *const keywords1Char[] =
            { "\xce\xbb", "\xcf\x80", nullptr }; // lambda and pi
        static const char *const keywords2Char[] =
            { "if", "or", "pi", "->",
              "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", nullptr };
        static const char *const keywords3Char[] =
            { "and", "not", "map", "let", "nil", "->>",
              "d10", "d11", "d12", "d13", "d14", "d15", "d16", nullptr };
        static const char *const keywords4Char[] =
            { "cond", "else", "true", nullptr };
        static const char *const keywords5Char[] =
            { "begin", "quote", "false", nullptr };
        static const char *const keywords6Char[] =
            { "define", "lambda", "filter", "reduce", "while", nullptr };
        static const char *const keywordsOther[] =
            { "map-indexed", "filter-indexed", nullptr };

        const char *const *k;
        switch (tokenLength)
        {
        case 1: k = keywords1Char; break;
        case 2: k = keywords2Char; break;
        case 3: k = keywords3Char; break;
        case 4: k = keywords4Char; break;
        case 5: k = keywords5Char; break;
        case 6: k = keywords6Char; break;
        default:
            if (tokenLength > 11)
            {
                return false;
            }
            k = keywordsOther;
            break;
        }

        for (int i = 0; k[i] != nullptr; ++i)
        {
            if (token.compare(CharPointer_UTF8(k[i])) == 0)
            {
                return true;
            }
        }

        return false;
    }

    static bool isBuiltInSymbol(String::CharPointerType token, const int tokenLength) noexcept
    {
        static const char *const keywords3Char[] =
            { "abs", "min", "max", "sin", "cos", "nth", nullptr };
        static const char *const keywords4Char[] =
            { "list", "head", "tail", "last", "push", "seed", "sus2", "sus4", nullptr };
        static const char *const keywords5Char[] =
            { "empty", "first", "range", "round", "floor", "triad", "tonic", nullptr };
        static const char *const keywords6Char[] =
            { "append", "insert", "length", "random", "remove", nullptr };
        static const char *const keywordsOther[] =
            { "reverse",
              "supertonic", "mediant", "subdominant",
              "dominant", "submediant", "subtonic",
              "project:period-size", "project:reset",
              "timeline:reset", "timeline:add-key",
              "track:make", "track:add-notes",
              "scale:find", "scale:render-key",
              "refactor:align-to-scale", "refactor:arpeggiate", 
              "refactor:cleanup-overlaps", "refactor:invert-chord",
              "refactor:invert-melody", "refactor:join-adjacent", 
              "refactor:legato", "refactor:quantize",
              "refactor:retrograde", "refactor:staccato",
              "refactor:transpose-in-scale",
              "triad:tonic", "triad:supertonic", "triad:mediant",
              "triad:subdominant", "triad:dominant", "triad:submediant", "triad:subtonic",
              "seventh", "seventh:tonic", "seventh:supertonic", "seventh:mediant",
              "seventh:subdominant", "seventh:dominant", "seventh:submediant", "seventh:subtonic",
              "sus2:tonic", "sus2:supertonic", "sus2:mediant",
              "sus2:subdominant", "sus2:dominant", "sus2:submediant", "sus2:subtonic",
              "sus4:tonic", "sus4:supertonic", "sus4:mediant",
              "sus4:subdominant", "sus4:dominant", "sus4:submediant", "sus4:subtonic",
               nullptr };

        const char *const *k;
        switch (tokenLength)
        {
        case 3: k = keywords3Char; break;
        case 4: k = keywords4Char; break;
        case 5: k = keywords5Char; break;
        case 6: k = keywords6Char; break;
        default:
            if (tokenLength < 3 || tokenLength > 27)
            {
                return false;
            }
            k = keywordsOther;
            break;
        }

        for (int i = 0; k[i] != nullptr; ++i)
        {
            if (token.compare(CharPointer_ASCII(k[i])) == 0)
            {
                return true;
            }
        }

        return false;
    }

    template <typename Iterator>
    int parseIdentifier(Iterator &source) noexcept
    {
        int tokenLength = 0;
        String::CharPointerType possible(this->possibleIdentifier);

        while (script::isSymbolBody(source.peekNextChar()))
        {
            const auto c = source.nextChar();

            if (tokenLength < ScriptTokeniser::maxTokenLength)
            {
                possible.write(c);
            }

            ++tokenLength;
        }

        possible.writeNull();

        const auto tokenCharPointer = String::CharPointerType(possibleIdentifier);

        if (tokenLength <= 11 &&
            isReservedKeyword(tokenCharPointer, tokenLength))
        {
            return TokenType::tokenTypeReservedKeyword;
        }

        if (tokenLength <= 27 &&
            isBuiltInSymbol(tokenCharPointer, tokenLength))
        {
            return TokenType::tokenTypeFunction;
        }

        if (this->userFunctions.contains(String(tokenCharPointer, tokenLength)))
        {
            return TokenType::tokenTypeFunction;
        }

        if (tokenCharPointer.compare(String::CharPointerType(this->highlightedToken)) == 0)
        {
            return TokenType::tokenTypeHighlight;
        }

        return TokenType::tokenTypeIdentifier;
    }

    String::CharPointerType::CharType highlightedToken[420] = {};
    String::CharPointerType::CharType possibleIdentifier[420] = {};

    FlatHashSet<String, StringHash> userFunctions;

    Array<Range<int>> bracketRanges;
    Range<int> caretBracketRange;
    Range<int> errorRange;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScriptTokeniser)
};
