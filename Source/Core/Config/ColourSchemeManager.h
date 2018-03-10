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
#include "RequestResourceThread.h"

class ColourSchemeManager :
    public ChangeBroadcaster,
    private Serializable,
    private Timer,
    private RequestResourceThread::Listener
{
public:

    static ColourSchemeManager &getInstance()
    {
        static ColourSchemeManager Instance;
        return Instance;
    }

    void initialise(const String &commandLine);
    void shutdown();
    void pull();

    Array<ColourScheme> getSchemes() const;
    ColourScheme getCurrentScheme() const;
    void setCurrentScheme(const ColourScheme &scheme);

private:

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    ValueTree serialize() const override;
    void deserialize(const ValueTree &tree) override;
    void reset() override;
    
private:

    ScopedPointer<RequestResourceThread> requestThread;

    void requestResourceOk(const ValueTree &resource) override;
    void requestResourceFailed(const Array<String> &errors) override;

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
