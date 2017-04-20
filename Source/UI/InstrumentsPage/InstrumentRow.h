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

#if HELIO_DESKTOP
#    define PLUGINSLIST_ROW_HEIGHT (65)
#elif HELIO_MOBILE
#    define PLUGINSLIST_ROW_HEIGHT (90)
#endif

class InstrumentRow : public Component
{
public:

    explicit InstrumentRow(PluginDescription description);

    ~InstrumentRow() override;


    void refreshPluginDescription(PluginDescription val);

    void setSelected(bool val);


    //===------------------------------------------------------------------===//
    // Component
    //===------------------------------------------------------------------===//

    void paint(Graphics &g) override;


private:

    PluginDescription pluginDescription;

    bool isSelected;


    WeakReference<InstrumentRow>::Master masterReference;

    friend class WeakReference<InstrumentRow>;

};
