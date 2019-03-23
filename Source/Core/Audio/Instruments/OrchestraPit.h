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

#include "OrchestraListener.h"

struct PluginDescriptionDragnDropWrapper final : ReferenceCountedObject
{
    PluginDescription pluginDescription;
    using Ptr = ReferenceCountedObjectPtr<PluginDescriptionDragnDropWrapper>;
};

class OrchestraPit
{
public:

    OrchestraPit() = default;
    virtual ~OrchestraPit();

    virtual Array<Instrument *> getInstruments() const = 0;
    virtual Instrument *findInstrumentById(const String &id) const = 0;

public:

    void addOrchestraListener(OrchestraListener *listener);
    void removeOrchestraListener(OrchestraListener *listener);
    void removeAllOrchestraListeners();

protected:

    void broadcastInstrumentRemoved(Instrument *instrument);
    void broadcastInstrumentRemovedPostAction();
    void broadcastInstrumentAdded(Instrument *instrument);

private:

    ListenerList<OrchestraListener> orchestraListeners;

};
