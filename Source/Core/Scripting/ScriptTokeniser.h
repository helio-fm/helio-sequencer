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
            return TokenType::tokenTypeBracketHighlighted;
        }

        switch (firstChar)
        {
        case 0:
            break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '.':
        {
            const auto result = CppTokeniserFunctions::parseNumber(source);
            if (result == TokenType::tokenTypeError)
            {
                source.skip();
            }
            return result;
        }
        case '(': case ')':
            source.skip();
            return TokenType::tokenTypeBracket;
        case '"':
            CppTokeniserFunctions::skipQuotedString(source);
            return TokenType::tokenTypeString;
        // todo:
        //case '\'':
        //    skipQuote(source);
        //    return TokenType::tokenTypeQuote;
        case '\'':
        case '=': case '%':
        case '+': case '-':
        case '*': case '/':
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
            if (CppTokeniserFunctions::isIdentifierStart(firstChar))
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
            { "Integer",        findDefaultColour(ColourIDs::CodeEditor::integer) },
            { "Float",          findDefaultColour(ColourIDs::CodeEditor::real) },
            { "String",         findDefaultColour(ColourIDs::CodeEditor::string) },
            { "Punctuation",    findDefaultColour(ColourIDs::CodeEditor::punctuation) },
            { "Bracket",        findDefaultColour(ColourIDs::CodeEditor::bracket) },
            { "BracketMatch",   findDefaultColour(ColourIDs::CodeEditor::bracketMatch) }
        };

        //const auto textColour = findDefaultColour(Label::textColourId);
        CodeEditorComponent::ColourScheme cs;
        for (auto &t : types)
        {
            cs.set(t.name, t.colour); // t.colour.interpolatedWith(textColour, 0.25f));
        }

        return cs;
    }

    void updateParsingData(const Array<Range<int>> &ranges, int caretPosition)
    {
        this->bracketRanges = ranges;
        this->setCaretPosition(caretPosition);
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

    void setErrorRange(Range<int> range)
    {
        this->errorRange = range;
    }

    enum TokenType
    {
        tokenTypeError = 0,
        tokenTypeComment,
        tokenTypeReservedKeyword,
        tokenTypeUserFunction,
        tokenTypeIdentifier,
        tokenTypeInteger,
        tokenTypeFloat,
        tokenTypeString,
        tokenTypePunctuation,
        tokenTypeBracket,
        tokenTypeBracketHighlighted
    };

private:

    static bool isReservedKeyword(String::CharPointerType token, const int tokenLength) noexcept
    {
        static const char *const keywords1Char[] =
            { "\xce\xbb", nullptr };
        static const char *const keywords2Char[] =
            { "if", "or", nullptr };
        static const char *const keywords3Char[] =
            { "and", "not", "exp", "map", "abs", "let", "min", "max",
              "sin", "cos", "tan", "log", "int", "pop", "nil", "nth", nullptr };
        static const char *const keywords4Char[] =
            { "eval", "push", "head", "tail", "type", "true", "last", "list", nullptr };
        static const char *const keywords5Char[] =
            { "begin", "empty", "false", "first", "float", "parse",
              "print", "quote", "range", "scope", "while" "round", "floor", nullptr };
        static const char *const keywords6Char[] =
            { "filter", "insert", "append", "lambda", "define",
              "length", "random", "reduce", "remove", nullptr };
        static const char *const keywordsOther[] =
            { "newline", "reverse", nullptr }; // todo all the interop stuff

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
                if (tokenLength > 13)
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

    template <typename Iterator>
    int parseIdentifier(Iterator &source) noexcept
    {
        int tokenLength = 0;
        String::CharPointerType::CharType possibleIdentifier[200] = {};
        String::CharPointerType possible(possibleIdentifier);

        while (script::isSymbolBody(source.peekNextChar()))
        {
            const auto c = source.nextChar();

            if (tokenLength < 40)
            {
                possible.write(c);
            }

            ++tokenLength;
        }

        if (this->userFunctions.contains(String(String::CharPointerType(possibleIdentifier), tokenLength)))
        {
            return TokenType::tokenTypeUserFunction;
        }

        if (tokenLength <= 10)
        {
            possible.writeNull();

            if (isReservedKeyword(String::CharPointerType(possibleIdentifier), tokenLength))
            {
                return TokenType::tokenTypeReservedKeyword;
            }
        }

        return TokenType::tokenTypeIdentifier;
    }

    FlatHashSet<String, StringHash> userFunctions;

    Array<Range<int>> bracketRanges;
    Range<int> caretBracketRange;
    Range<int> errorRange;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScriptTokeniser)
};
