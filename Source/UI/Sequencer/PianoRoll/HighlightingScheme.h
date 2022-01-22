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

#include "Note.h"
#include "Scale.h"
#include "Temperament.h"
#include "HelioTheme.h"
#include "KeySignatureEvent.h"

class HighlightingScheme final
{
public:

    HighlightingScheme(Note::Key rootKey, const Scale::Ptr scale) noexcept;

    template<typename T1, typename T2>
    static int compareElements(const T1 *const l, const T2 *const r)
    {
        const int keyDiff = l->getRootKey() - r->getRootKey();
        const int keyResult = (keyDiff > 0) - (keyDiff < 0);
        if (keyResult != 0) { return keyDiff; }

        if (l->getScale()->isEquivalentTo(r->getScale())) { return 0; }

        const int scaleDiff = l->getScale()->hashCode() - r->getScale()->hashCode();
        return (scaleDiff > 0) - (scaleDiff < 0);
    }

    const Scale::Ptr getScale() const noexcept { return this->scale; }
    const Note::Key getRootKey() const noexcept { return this->rootKey; }
    const Image getUnchecked(int i) const noexcept { return this->rows.getUnchecked(i); }
    
    void renderBackgroundCache(Temperament::Ptr temperament);

    static Image renderRowsPattern(const HelioTheme &theme,
        const Temperament::Ptr temperament, const Scale::Ptr scale,
        Note::Key root, int height);

private:

    Scale::Ptr scale;
    Note::Key rootKey;
    Array<Image> rows;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighlightingScheme)
};
