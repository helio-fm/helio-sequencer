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
#include "KnifeToolHelper.h"
#include "NoteComponent.h"
#include "CutPointMark.h"
#include "HybridRoll.h"
#include "ColourIDs.h"

#define PADDING 2

static inline ComponentAnimator &rootAnimator()
{
    return Desktop::getInstance().getAnimator();
}

KnifeToolHelper::KnifeToolHelper(HybridRoll &roll) :
    roll(roll)
{
    this->setAlpha(0.f);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
}

KnifeToolHelper::~KnifeToolHelper()
{
    rootAnimator().animateComponent(this, this->getBounds(), 0.f, 150, true, 0.f, 0.f);
}

Line<float> KnifeToolHelper::getLine() const noexcept
{
    return this->line;
}

void KnifeToolHelper::paint(Graphics &g)
{
    g.setColour(Colours::black);
    g.strokePath(this->path, PathStrokeType(.5f));

    g.setColour(Colours::white.withAlpha(0.55f));
    g.fillPath(this->path);
}

void KnifeToolHelper::updateBounds()
{
    const Point<double> parentSize(this->getParentSize());
    const auto start = (this->startPosition * parentSize).toFloat();
    const auto end = (this->endPosition * parentSize).toFloat();
    const auto x1 = jmin(start.getX(), end.getX());
    const auto x2 = jmax(start.getX(), end.getX());
    const auto y1 = jmin(start.getY(), end.getY());
    const auto y2 = jmax(start.getY(), end.getY());
    const Point<float> startOffset(x1 - PADDING, y1 - PADDING);
    this->line = { start, end };

    this->path.clear();
    //path.startNewSubPath(start - startOffset);
    //path.lineTo(end - startOffset);
    path.startNewSubPath(end - startOffset);
    path.lineTo(start - startOffset);
    static Array<float> dashes(8.f, 4.f);
    PathStrokeType(3.f).createDashedStroke(this->path, this->path,
        dashes.getRawDataPointer(), dashes.size());

    this->setBounds(int(x1) - PADDING, int(y1) - PADDING,
        int(x2 - x1) + PADDING * 2, int(y2 - y1) + PADDING * 2);
}

void KnifeToolHelper::updateCutMarks()
{
    for (auto &m : this->cutMarkers)
    {
        m.second.get()->updateBounds(true);
    }
}

void KnifeToolHelper::fadeIn()
{
    rootAnimator().animateComponent(this, this->getBounds(), 1.f, 150, false, 0.f, 0.f);
}

void KnifeToolHelper::setStartPosition(const Point<float> &mousePos)
{
    this->startPosition = mousePos.toDouble() / this->getParentSize();
}

void KnifeToolHelper::setEndPosition(const Point<float> &mousePos)
{
    this->endPosition = mousePos.toDouble() / this->getParentSize();
}

void KnifeToolHelper::addOrUpdateCutPoint(NoteComponent *nc, float beat)
{
    jassert(this->getParentComponent() != nullptr);

    if (!this->cutMarkers.contains(nc->getNote()))
    {
        this->cutMarkers[nc->getNote()] = this->createCutPointMark(nc, beat / nc->getLength());
    }
    else
    {
        this->cutMarkers[nc->getNote()].get()->updatePosition(beat / nc->getLength());
    }

    this->cutPoints[nc->getNote()] = beat;
}

void KnifeToolHelper::removeCutPointIfExists(const Note &note)
{
    if (this->cutMarkers.contains(note))
    {
        this->cutMarkers.erase(note);
    }

    if (this->cutPoints.contains(note))
    {
        this->cutPoints.erase(note);
    }
}

UniquePointer<CutPointMark> KnifeToolHelper::createCutPointMark(NoteComponent *nc, float pos)
{
    jassert(this->getParentComponent() != nullptr);

    UniquePointer<CutPointMark> p;
    p.reset(new CutPointMark(nc, pos));
    this->getParentComponent()->addAndMakeVisible(p.get());
    p.get()->updateBounds();
    p.get()->fadeIn();
    return p;
}

const Point<double> KnifeToolHelper::getParentSize() const
{
    if (const auto *p = this->getParentComponent())
    {
        return { double(p->getWidth()), double(p->getHeight()) };
    }

    return { 1.0, 1.0 };
}

void KnifeToolHelper::getCutPoints(Array<Note> &outNotes, Array<float> &outBeats) const
{
    outNotes.clearQuick();
    outBeats.clearQuick();
    for (const auto &cp : this->cutPoints)
    {
        outNotes.add(cp.first);
        outBeats.add(cp.second);
    }
}
