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
#include "TreeItemComponentCompact.h"
#include "TreeItemMarkerCompact.h"
#include "TreeItem.h"
#include "Icons.h"

TreeItemComponentCompact::TreeItemComponentCompact(TreeItem &i) :
    TreeItemComponent(i),
    markerIsVisible(false)
{
    this->pageMarker = new TreeItemMarkerCompact();
    this->addChildComponent(this->pageMarker);
}

//===----------------------------------------------------------------------===//
// Component
//===----------------------------------------------------------------------===//

void TreeItemComponentCompact::paint(Graphics &g)
{
    this->paintIcon(g);
    
    const bool markerVisibilityChanged = (this->item.isMarkerVisible() != this->markerIsVisible);
    this->markerIsVisible = this->item.isMarkerVisible();
    
    if (markerVisibilityChanged)
    {
        if (this->markerIsVisible)
        {
            this->animator.fadeIn(this->pageMarker, 150);
        }
        else
        {
            this->pageMarker->setVisible(false);
        }
    }
}

void TreeItemComponentCompact::resized()
{
    this->pageMarker->setBounds(this->getLocalBounds());
}

void TreeItemComponentCompact::paintIcon(Graphics &g)
{
    const Image icon(this->item.getIcon());

    g.setColour(this->getItemColour().withAlpha(0.6f));
    const float yMarginDrawed = 3.f;
    const float circleSizeDrawed = this->getHeight() - (yMarginDrawed * 2.f);
    const float xMarginDrawed = ((this->getWidth() / 2) - (circleSizeDrawed / 2.f));
    g.drawEllipse(xMarginDrawed, yMarginDrawed, circleSizeDrawed, circleSizeDrawed, 1.5f);
    
    //g.setColour(this->getItemColour().interpolatedWith(Colours::white, 0.25f).withAlpha(0.3f));
    //const float yMarginFilled = 4.5f;
    //const float circleSizeFilled = this->getHeight() - (yMarginFilled * 2.f);
    //const float xMarginFilled = ((this->getWidth() / 2) - (circleSizeFilled / 2.f));
    //g.fillEllipse(xMarginFilled, yMarginFilled, circleSizeFilled, circleSizeFilled);
    
    const int cx = int(this->getWidth() / 2);
    const int cy = int(this->getHeight() / 2);
    
    // a hack -_-
    Icons::drawImageRetinaAware(icon, g, cx, cy);
    Icons::drawImageRetinaAware(icon, g, cx, cy);
    Icons::drawImageRetinaAware(icon, g, cx, cy);
    Icons::drawImageRetinaAware(icon, g, cx, cy);
}

void TreeItemComponentCompact::paintBackground(Graphics &g,
                                               int width, int height,
                                               bool isSelected, bool isActive)
{
    if (isSelected || isActive)
    {
        g.fillAll(Colours::white.withAlpha(0.1f));
    }
    
    g.fillAll(Colours::white.withAlpha(0.01f));
}

//===----------------------------------------------------------------------===//
// LongTapListener
//===----------------------------------------------------------------------===//

void TreeItemComponentCompact::longTapEvent(const MouseEvent &e)
{
    this->emitCallout();
}

//===----------------------------------------------------------------------===//
// HighlightedComponent
//===----------------------------------------------------------------------===//

Component *TreeItemComponentCompact::createHighlighterComponent()
{
    //return new TreeItemComponentFrame();
    return new TreeItemComponentCompact(this->item);
}
