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

class MidiEvent;
class MidiLayer;
class ProjectInfo;

class ProjectListener
{
public:

    ProjectListener() {}

    virtual ~ProjectListener() {}

    virtual void onEventChanged(const MidiEvent &oldEvent, const MidiEvent &newEvent) = 0;

    virtual void onEventAdded(const MidiEvent &event) = 0;

    virtual void onEventRemoved(const MidiEvent &event) = 0; // вызывается прямо перед удалением события

    virtual void onEventRemovedPostAction(const MidiLayer *layer) {} // вызывается после удаления события, надо будет переименовать эти методы по-человечески

    virtual void onLayerChanged(const MidiLayer *layer) = 0;

    virtual void onLayerAdded(const MidiLayer *layer) = 0;

    virtual void onLayerRemoved(const MidiLayer *layer) = 0; // вызывается прямо перед удалением слоя
    
    virtual void onLayerMoved(const MidiLayer *layer) {} // этот метод нужен далеко не всем

    virtual void onInfoChanged(const ProjectInfo *info) {} // этот метод нужен далеко не всем

    virtual void onProjectBeatRangeChanged(float firstBeat, float lastBeat) = 0;

};
