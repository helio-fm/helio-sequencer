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
#include "KnifeToolHelper.h"
#include "NoteComponent.h"
#include "CutPointMark.h"
#include "RollBase.h"

KnifeToolHelper::KnifeToolHelper(RollBase &roll) : roll(roll)
{
    this->setAlpha(0.f);
    this->setWantsKeyboardFocus(false);
    this->setInterceptsMouseClicks(false, false);
}

KnifeToolHelper::~KnifeToolHelper()
{
    Desktop::getInstance().getAnimator().animateComponent(this,
        this->getBounds(), 0.f, Globals::UI::fadeOutShort, true, 0.0, 0.0);
}

Line<float> KnifeToolHelper::getLine() const noexcept
{
    return this->line;
}

void KnifeToolHelper::paint(Graphics &g)
{
    g.setColour(Colours::black);
    g.strokePath(this->path, PathStrokeType(.5f));

    g.setColour(Colours::white.withAlpha(0.65f));
    g.fillPath(this->path);
}

void KnifeToolHelper::updateBounds()
{
    static constexpr auto padding = 2;

    const auto parentSize = this->getParentSize();
    const auto start = (this->startPosition * parentSize).toFloat();
    const auto end = (this->endPosition * parentSize).toFloat();
    this->line = { start, end };

    const auto x1 = jmin(start.getX(), end.getX());
    const auto x2 = jmax(start.getX(), end.getX());
    const auto y1 = jmin(start.getY(), end.getY());
    const auto y2 = jmax(start.getY(), end.getY());
    const Point<float> startOffset(x1 - padding, y1 - padding);

    this->path.clear();
    const auto pathStart = start - startOffset;
    const auto pathEnd = end - startOffset;
    this->path.startNewSubPath(pathEnd);
    this->path.lineTo(pathStart);
    static Array<float> dashes(6.f, 8.f);
    PathStrokeType(2.f).createDashedStroke(this->path, this->path,
        dashes.getRawDataPointer(), dashes.size());

    this->path.addEllipse(pathStart.x - 1.f, pathStart.y - 1.f, 2.f, 2.f);

    this->setBounds(int(x1) - padding, int(y1) - padding,
        int(x2 - x1) + padding * 2, int(y2 - y1) + padding * 2);
}

void KnifeToolHelper::updateCutMarks()
{
    for (auto &m : this->noteCutMarks)
    {
        m.second.get()->updateBounds(true);
    }
}

void KnifeToolHelper::fadeIn()
{
    Desktop::getInstance().getAnimator().animateComponent(this,
        this->getBounds(), 1.f, Globals::UI::fadeInShort, false, 0.0, 0.0);
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

    if (!this->noteCutMarks.contains(nc->getNote()))
    {
        this->noteCutMarks[nc->getNote()] = this->createCutPointMark(nc, beat / nc->getLength());
    }
    else
    {
        this->noteCutMarks[nc->getNote()].get()->updatePosition(beat / nc->getLength());
    }

    this->cutPoints[nc->getNote()] = beat;
}

void KnifeToolHelper::removeCutPointIfExists(const Note &note)
{
    if (this->noteCutMarks.contains(note))
    {
        this->noteCutMarks.erase(note);
    }

    if (this->cutPoints.contains(note))
    {
        this->cutPoints.erase(note);
    }
}

UniquePointer<NoteCutPointMark> KnifeToolHelper::createCutPointMark(NoteComponent *nc, float pos)
{
    jassert(this->getParentComponent() != nullptr);
    auto mark = make<NoteCutPointMark>(nc, pos);
    this->getParentComponent()->addAndMakeVisible(mark.get());
    mark->updateBounds();
    mark->fadeIn();
    return mark;
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
