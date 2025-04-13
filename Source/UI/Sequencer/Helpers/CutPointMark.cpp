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

#include "Common.h"
#include "CutPointMark.h"
#include "Clip.h"
#include "HelioTheme.h"

CutPointMark::CutPointMark(SafePointer<Component> targetComponent, float absPosX) :
    targetComponent(targetComponent),
    absPosX(absPosX)
{
    this->setAlpha(0.f);
    this->setPaintingIsUnclipped(true);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
}

CutPointMark::~CutPointMark()
{
    App::cancelAnimation(this);
    if (this->initialized)
    {
        App::animateComponent(this,
            this->getBounds(), 0.f, Globals::UI::fadeOutShort, true, 0.0, 0.0);
    }
}

void CutPointMark::fadeIn()
{
    App::animateComponent(this,
        this->getBounds(), 1.f, Globals::UI::fadeInShort, false, 0.0, 0.0);
}

void CutPointMark::updateBounds(bool forceNoAnimation)
{
    App::cancelAnimation(this);

    if (this->absPosX <= 0.f || this->absPosX >= 1.f)
    {
        if (this->initialized)
        {
            this->setVisible(false);
            this->initialized = false;
        }
    }
    else if (this->targetComponent != nullptr)
    {
        const float wt = float(this->targetComponent->getWidth());
        const int ht = this->targetComponent->getHeight();
        const int x = this->targetComponent->getX() + int(wt * this->absPosX);
        const int y = this->targetComponent->getY();
        const Rectangle<int> newBounds(x - 2, y, 5, ht);

        if (!this->initialized || forceNoAnimation)
        {
            this->initialized = true;
            this->setVisible(true);
            this->setBounds(newBounds);
        }
        else
        {
            App::animateComponent(this, newBounds, 1.f, 20, false, 1.0, 0.0);
        }
    }
}

void CutPointMark::updatePosition(float pos)
{
    this->absPosX = pos;
    this->updateBounds();
}

Component *CutPointMark::getComponent() const noexcept
{
    return this->targetComponent.getComponent();
}

float CutPointMark::getCutPosition() const noexcept
{
    return this->absPosX;
}

//===----------------------------------------------------------------------===//
// For the piano roll
//===----------------------------------------------------------------------===//

NoteCutPointMark::NoteCutPointMark(SafePointer<Component> targetComponent, float absPosX) :
    CutPointMark(targetComponent, absPosX) {}

void NoteCutPointMark::paint(Graphics &g)
{
    const auto w = float(this->getWidth());
    const auto h = float(this->getHeight());

    g.setColour(this->outlineColour);
    g.fillRect(w / 2.f - 1.5f, 1.f, 3.f, h - 2.f);

    g.setColour(this->markColour);
    g.fillRect(w / 2.f - 0.75f, 1.f, 1.5f, h - 2.f);

    Path p;
    p.addTriangle(0.f, 0.f, w, 0.f, w / 2.f, 2.f);
    g.fillPath(p);

    p.clear();
    p.addTriangle(0.f, h, w, h, w / 2.f, h - 2.f);
    g.fillPath(p);
}

//===----------------------------------------------------------------------===//
// For the pattern roll
//===----------------------------------------------------------------------===//

ClipCutPointMark::ClipCutPointMark(SafePointer<Component> targetComponent, const Clip &clip) :
    CutPointMark(targetComponent, 0.f),
    markColour(clip.getTrackColour().
        interpolatedWith(findDefaultColour(ColourIDs::Roll::cuttingGuide), 0.75f)) {}

void ClipCutPointMark::paint(Graphics &g)
{
    const auto w = this->getWidth();

    g.setColour(this->outlineColour);
    g.fillRect(w / 2 - 1, 2, 3, this->getHeight() - 3);

    g.setColour(this->markColour);
    HelioTheme::drawDashedVerticalLine(g,
        2.f, 1.f,
        float(this->getHeight() - 2),
        3.f);

    g.fillRect(w / 2 - 1, 1, 3, 1);
    g.fillRect(w / 2 - 1, this->getHeight() - 1, 3, 1);
}

void ClipCutPointMark::updatePositionFromMouseEvent(int mouseX, int mouseY)
{
    if (mouseY < this->targetComponent->getY() ||
        mouseY > this->targetComponent->getBottom())
    {
        this->absPosX = -1.f;
    }
    else
    {
        this->absPosX = jlimit(0.f, 1.f,
            float(mouseX - this->targetComponent->getX()) /
            float(this->targetComponent->getWidth()));
    }

    this->updateBounds();
}

//===----------------------------------------------------------------------===//
// For the automation editors
//===----------------------------------------------------------------------===//

AutomationEditorCutPointMark::AutomationEditorCutPointMark(SafePointer<Component> targetComponent, const Clip &clip) :
    ClipCutPointMark(targetComponent, clip) {}

// these two are similar to the above, but both omit the targetComponent->getX() offset:

void AutomationEditorCutPointMark::updateBounds(bool forceNoAnimation /*= false*/)
{
    App::cancelAnimation(this);

    if (this->absPosX <= 0.f || this->absPosX >= 1.f)
    {
        if (this->initialized)
        {
            this->setVisible(false);
            this->initialized = false;
        }
    }
    else if (this->targetComponent != nullptr)
    {
        const float wt = float(this->targetComponent->getWidth());
        const int ht = this->targetComponent->getHeight();
        const int x = int(wt * this->absPosX);
        const int y = this->targetComponent->getY();
        const Rectangle<int> newBounds(x - 2, y, 5, ht);

        if (!this->initialized || forceNoAnimation)
        {
            this->initialized = true;
            this->setVisible(true);
            this->setBounds(newBounds);
        }
        else
        {
            App::animateComponent(this, newBounds, 1.f, 20, false, 1.0, 0.0);
        }
    }
}

void AutomationEditorCutPointMark::updatePositionFromMouseEvent(int mouseX, int mouseY)
{
    if (mouseY < this->targetComponent->getY() ||
        mouseY > this->targetComponent->getBottom())
    {
        this->absPosX = -1.f;
    }
    else
    {
        this->absPosX = jlimit(0.f, 1.f,
            float(mouseX) /
            float(this->targetComponent->getWidth()));
    }

    this->updateBounds();
}
