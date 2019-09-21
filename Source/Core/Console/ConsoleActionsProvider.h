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

#pragma once

class ConsoleAction final : public ReferenceCountedObject
{
public:

    using Ptr = ReferenceCountedObjectPtr<ConsoleAction>;

    ConsoleAction() = default;
    ConsoleAction(String text) :
        name(std::move(text)) {}

    virtual ~ConsoleAction() {};

    const String &getName() const noexcept
    {
        return this->name;
    }

    int getMatchScore() const noexcept
    {
        return this->matchScore;
    }

    void setMatch(int score, const uint8 *matches);

private:

    String name;
    int matchScore;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConsoleAction)
};

class ConsoleActionsProvider
{
public:

    ConsoleActionsProvider() = default;
    virtual ~ConsoleActionsProvider() {};

    using Prefix = juce_wchar;
    virtual bool usesPrefix(const Prefix prefix) const = 0;

    using Actions = ReferenceCountedArray<ConsoleAction>;
    const Actions &getFilteredActions() const
    {
        return this->filteredActions;
    }

    void updateFilter(const String &pattern, bool skipPrefix);
    void clearFilter();

protected:

    virtual const Actions &getActions() const = 0;

private:

    Actions filteredActions;

    JUCE_DECLARE_WEAK_REFERENCEABLE(ConsoleActionsProvider)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConsoleActionsProvider)
};
