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

#include "XmlSerializer.h"

class Clipboard final
{
public:

    Clipboard() = default;

    void copy(const ValueTree &data, bool mirrorToSystemClipboard = false)
    {
        this->clipboard = data;

        if (mirrorToSystemClipboard)
        {
            SystemClipboard::copyTextToClipboard(this->getCurrentContentAsString());
        }
    }

    const ValueTree &getData() const noexcept
    {
        return clipboard;
    }

private:

    String getCurrentContentAsString() const
    {
        if (this->clipboard.isValid())
        {
            String text;
            static  XmlSerializer serializer;
            serializer.saveToString(text, this->clipboard);
            return text;
        }

        return {};
    }

private:

    ValueTree clipboard;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Clipboard)
    JUCE_PREVENT_HEAP_ALLOCATION
};
