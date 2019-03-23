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

class HybridRoll;
class CutPointMark;
class NoteComponent;

#include "Note.h"

class KnifeToolHelper final : public Component
{
public:

    KnifeToolHelper(HybridRoll &roll);
    ~KnifeToolHelper();

    Line<float> getLine() const noexcept;
    void getCutPoints(Array<Note> &outNotes, Array<float> &outBeats) const;

    void setStartPosition(const Point<float> &mousePos);
    void setEndPosition(const Point<float> &mousePos);

    void addOrUpdateCutPoint(NoteComponent *nc, float beat);
    void removeCutPointIfExists(const Note &note);

    void paint(Graphics &g) override;

    void updateBounds();
    void updateCutMarks();
    void fadeIn();

private:

    HybridRoll &roll;

    Point<double> startPosition;
    Point<double> endPosition;

    Line<float> line;
    Path path;

    UniquePointer<CutPointMark> createCutPointMark(NoteComponent *nc, float beat);
    const Point<double> getParentSize() const;

    FlatHashMap<Note, UniquePointer<CutPointMark>, MidiEventHash> cutMarkers;
    FlatHashMap<Note, float, MidiEventHash> cutPoints;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KnifeToolHelper)
};
