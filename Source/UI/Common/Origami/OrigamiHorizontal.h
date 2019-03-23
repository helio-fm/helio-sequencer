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

#include "Origami.h"

class OrigamiHorizontal : public Origami
{
public:

    void addFlexiblePage(Component *component) override;
    void addFixedPage(Component *component) override;
    void addShadowAtTheStart() override;
    void addShadowAtTheEnd() override;
    void addResizer(int minSize, int maxSize) override;

    void onPageResized(Component *component) override;
    void resized() override;

private:

    int getCommonFixedHeight() const;
    void updateLayout(const Origami::Page *page, Rectangle<int> bounds);

};
