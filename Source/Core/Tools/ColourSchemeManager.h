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

#include "ColourScheme.h"
#include "Serializable.h"
#include "RequestColourSchemesThread.h"

class ColourSchemeManager :
    public ChangeBroadcaster,
    private Serializable,
    private Timer,
    private RequestColourSchemesThread::Listener
{
public:

    static ColourSchemeManager &getInstance()
    {
        static ColourSchemeManager Instance;
        return Instance;
    }

    void initialise(const String &commandLine);
    void shutdown();

    bool isPullPending() const;
    void pull();

    Array<ColourScheme> getSchemes() const;
    ColourScheme getCurrentScheme() const;
    void setCurrentScheme(const ColourScheme &scheme);

private:

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:

    ScopedPointer<RequestColourSchemesThread> requestThread;

    void schemesRequestOk(RequestColourSchemesThread *thread) override;
    void schemesRequestFailed(RequestColourSchemesThread *thread) override;
    
private:

    Array<ColourScheme> schemes;

    ColourSchemeManager() {}
    void timerCallback() override;

    void reloadSchemes();

    void saveConfigSchemes();
    String getConfigSchemes();

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColourSchemeManager)

};
