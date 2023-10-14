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

#pragma once

class RollBase;
class NoteComponent;
class NoteCutPointMark;

#include "Note.h"

class KnifeToolHelper final : public Component
{
public:

    KnifeToolHelper(RollBase &roll);
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

    RollBase &roll;

    Point<double> startPosition;
    Point<double> endPosition;

    Line<float> line;
    Path path;

    UniquePointer<NoteCutPointMark> createCutPointMark(NoteComponent *nc, float beat);
    const Point<double> getParentSize() const;

    FlatHashMap<Note, UniquePointer<NoteCutPointMark>, MidiEventHash> noteCutMarks;
    FlatHashMap<Note, float, MidiEventHash> cutPoints;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KnifeToolHelper)
};
