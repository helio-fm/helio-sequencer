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
#include "ConsoleActionsProvider.h"

struct ConsoleActionSortByMatch final
{
    static int compareElements(const ConsoleAction::Ptr first, const ConsoleAction::Ptr second)
    {
        return int(second->getMatchScore() - first->getMatchScore());
    }
};

void ConsoleAction::setMatch(int score, const uint8 *matches)
{
    // todo
}

// Actions filtering makes use of Sublime-like fuzzy matcher,
// taken from this public domain library by Forrest Smith,
// adapted for JUCE String class for the sake of Unicode support:
// https://github.com/forrestthewoods/lib_fts/blob/master/code/fts_fuzzy_match.h
// https://www.forrestthewoods.com/blog/reverse_engineering_sublime_texts_fuzzy_match

// Original code had a limit of 256, but I really expect it to be way lower:
#define FUZZY_MAX_MATCHES (32)
#define FUZZY_MAX_RECURSION (8)

static bool fuzzyMatch(String::CharPointerType pattern, String::CharPointerType str,
    int &outScore, String::CharPointerType strBegin, uint8 const *srcMatches, uint8 *newMatches,
    int nextMatch, int &recursionCount);

static bool fuzzyMatch(String::CharPointerType pattern, String::CharPointerType str, int &outScore, uint8 *matches)
{
    int recursionCount = 0;
    return fuzzyMatch(pattern, str, outScore, str, nullptr, matches, 0, recursionCount);
}

void ConsoleActionsProvider::updateFilter(const String &pattern, bool skipPrefix)
{
    this->filteredActions.clearQuick();

    auto patternPtr = pattern.getCharPointer();
    if (skipPrefix)
    {
        patternPtr.getAndAdvance();
    }

    for (const auto &action : this->getActions())
    {
        int outScore = 0;
        uint8 matches[FUZZY_MAX_MATCHES] = {};
        const auto match = fuzzyMatch(patternPtr, action->getName().getCharPointer(), outScore, matches);
        if (match)
        {
            action->setMatch(outScore, matches);
            this->filteredActions.add(action);
        }
    }

    static ConsoleActionSortByMatch comparator;
    this->filteredActions.sort(comparator);
}

void ConsoleActionsProvider::clearFilter()
{
    this->filteredActions.clearQuick();
    this->filteredActions.addArray(this->getActions());
    for (const auto &action : this->filteredActions)
    {
        action->setMatch(0, nullptr);
    }

    // how to sort?
}

static bool fuzzyMatch(String::CharPointerType pattern, String::CharPointerType str, int &outScore,
    String::CharPointerType strBegin, uint8 const *srcMatches, uint8 *matches, int nextMatch, int &recursionCount)
{
    ++recursionCount;
    if (recursionCount >= FUZZY_MAX_RECURSION)
    {
        return false;
    }

    if (*pattern == 0 || *str == 0)
    {
        return false;
    }

    bool recursiveMatch = false;
    uint8 bestRecursiveMatches[FUZZY_MAX_MATCHES] = {};
    int bestRecursiveScore = 0;

    // Loop through pattern and str looking for a match
    bool firstMatch = true;
    while (*pattern != 0 && *str != 0)
    {
        // Found match
        if (CharacterFunctions::toLowerCase(*pattern) == CharacterFunctions::toLowerCase(*str))
        {
            // Supplied matches buffer was too short
            if (nextMatch >= FUZZY_MAX_MATCHES)
            {
                return false;
            }

            // "Copy-on-Write" srcMatches into matches
            if (firstMatch && srcMatches)
            {
                memcpy(matches, srcMatches, nextMatch);
                firstMatch = false;
            }

            // Recursive call that "skips" this match
            uint8 recursiveMatches[FUZZY_MAX_MATCHES] = {};
            int recursiveScore;
            if (fuzzyMatch(pattern, str + 1, recursiveScore, strBegin, matches, recursiveMatches, nextMatch, recursionCount))
            {
                // Pick best recursive score
                if (!recursiveMatch || recursiveScore > bestRecursiveScore)
                {
                    memcpy(bestRecursiveMatches, recursiveMatches, FUZZY_MAX_MATCHES);
                    bestRecursiveScore = recursiveScore;
                }

                recursiveMatch = true;
            }

            // Advance
            matches[nextMatch++] = (uint8)(str - strBegin);
            ++pattern;
        }
        ++str;
    }

    // Determine if full pattern was matched
    const bool matched = *pattern == 0 ? true : false;

    // Calculate score
    if (matched)
    {
        constexpr int sequentialBonus = 15;             // bonus for adjacent matches
        constexpr int separatorBonus = 30;              // bonus if match occurs after a separator
        constexpr int camelBonus = 25;                  // bonus if match is uppercase and prev is lower
        constexpr int firstLetterBonus = 15;            // bonus if the first letter is matched
        constexpr int leadingLetterPenalty = -5;        // penalty applied for every letter in str before the first match
        constexpr int maxLeadingLetterPenalty = -15;    // maximum penalty for leading letters
        constexpr int unmatchedLetterPenalty = -1;      // penalty for every letter that doesn't matter

        // Iterate str to end
        while (*str != 0)
        {
            ++str;
        }

        // Initialize score
        outScore = 100;

        // Apply leading letter penalty
        auto penalty = leadingLetterPenalty * matches[0];
        if (penalty < maxLeadingLetterPenalty)
        {
            penalty = maxLeadingLetterPenalty;
        }

        outScore += penalty;

        // Apply unmatched penalty
        const auto unmatched = (int)(str - strBegin) - nextMatch;
        outScore += unmatchedLetterPenalty * unmatched;

        // Apply ordering bonuses
        for (int i = 0; i < nextMatch; ++i)
        {
            const auto currIdx = matches[i];

            if (i > 0)
            {
                const auto prevIdx = matches[i - 1];
                if (currIdx == (prevIdx + 1))
                {
                    outScore += sequentialBonus;
                }
            }

            // Check for bonuses based on neighbor character value
            if (currIdx > 0)
            {
                // Camel case
                const auto neighbor = strBegin[currIdx - 1];
                const auto curr = strBegin[currIdx];
                if (CharacterFunctions::isLowerCase(neighbor) && CharacterFunctions::isUpperCase(curr))
                {
                    outScore += camelBonus;
                }

                // Separator
                const bool neighborSeparator = CharacterFunctions::isWhitespace(neighbor) || neighbor == '_';
                if (neighborSeparator)
                {
                    outScore += separatorBonus;
                }
            }
            else
            {
                // First letter
                outScore += firstLetterBonus;
            }
        }
    }

    // Return best result
    if (recursiveMatch && (!matched || bestRecursiveScore > outScore))
    {
        // Recursive score is better than "this"
        memcpy(matches, bestRecursiveMatches, FUZZY_MAX_MATCHES);
        outScore = bestRecursiveScore;
        return true;
    }
    else if (matched)
    {
        // "this" score is better than recursive
        return true;
    }
    else
    {
        // no match
        return false;
    }
}
