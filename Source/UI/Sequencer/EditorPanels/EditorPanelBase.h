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

class Clip;
class RollBase;
class MidiSequence;
class Lasso;

class EditorPanelBase : public Component
{
public:

    EditorPanelBase() = default;
    virtual ~EditorPanelBase() = default;

    virtual void switchToRoll(SafePointer<RollBase> roll) = 0;

    virtual bool canEditSequence(WeakReference<MidiSequence> sequence) const = 0;

    struct EventFilter final
    {
        // the meaning of this id is defined by subclasses;
        // for now it can only mean automation controller #'s
        int id = 0;

        // e.g. "velocity", "tempo", "sustain pedal",
        String name;

        bool operator== (const EventFilter &other) const noexcept = default;

        static int compareElements(const EventFilter &a, const EventFilter &b) noexcept
        {
            return a.id - b.id;
        }
    };

    virtual Array<EventFilter> getAllEventFilters() const = 0;

    virtual void setEditableClip(Optional<Clip> clip) = 0;
    virtual void setEditableClip(const Clip &selectedClip, const EventFilter &filter) = 0;
    virtual void setEditableSelection(WeakReference<Lasso> selection) = 0;

    class Listener
    {
    public:

        Listener() = default;
        virtual ~Listener() = default;

        virtual void onUpdateEventFilters() = 0;
    };

    void addListener(Listener *listener)
    {
        jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
        this->listeners.add(listener);
    }

    void removeListener(Listener *listener)
    {
        jassert(MessageManager::getInstance()->currentThreadHasLockedMessageManager());
        this->listeners.remove(listener);
    }

protected:

    ListenerList<Listener> listeners;

};

// Ramer-Douglas-Peucker algorithm for point reduction,
// to be used in editor panels for hand-drawing random shapes:
template <typename T>
struct PointReduction final
{
    static float getPerpendicularDistance(const Point<T> &p,
        const Point<T> &lineP1, const Point<T> &lineP2)
    {
        const Point<T> vec1(p.x - lineP1.x, p.y - lineP1.y);
        const Point<T> vec2(lineP2.x - lineP1.x, lineP2.y - lineP1.y);
        const auto distance = sqrt(vec2.x * vec2.x + vec2.y * vec2.y);
        const auto crossProduct = vec1.x * vec2.y - vec2.x * vec1.y;
        return fabs(float(crossProduct / distance));
    }

    static Array<Point<T>> simplify(const Array<Point<T>> &points, float epsilon = 0.04f)
    {
        int index = 0;
        float maxDistance = 0;

        for (int i = 1; i < points.size() - 1; ++i)
        {
            const auto d = getPerpendicularDistance(points.getUnchecked(i),
                points.getFirst(), points.getLast());

            if (d > maxDistance)
            {
                index = i;
                maxDistance = d;
            }
        }

        if (maxDistance > epsilon)
        {
            Array<Point<T>> previousPart, nextPart;

            for (int i = 0; i <= index; ++i)
            {
                previousPart.add(points.getUnchecked(i));
            }

            for (int i = index; i < points.size(); ++i)
            {
                nextPart.add(points.getUnchecked(i));
            }

            auto subList1 = simplify(previousPart, epsilon);
            auto subList2 = simplify(nextPart, epsilon);

            subList1.removeLast();
            subList1.addArray(subList2);
            return subList1;
        }

        return {
            points.getFirst(),
            points.getLast()
        };
    }
};
