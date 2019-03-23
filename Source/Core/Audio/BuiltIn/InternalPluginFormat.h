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

#include "Instrument.h"

class InternalPluginFormat final : public AudioPluginFormat
{
public:

    InternalPluginFormat();

    enum InternalFilterType
    {
        audioInputFilter = 0,
        audioOutputFilter,
        midiInputFilter,
        midiOutputFilter,

        endOfFilterTypes
    };

    void getAllTypes(OwnedArray <PluginDescription> &results);
    const PluginDescription *getDescriptionFor(const InternalFilterType type);

    bool pluginNeedsRescanning(const PluginDescription &) override { return false; }
    String getName() const override;
    bool fileMightContainThisPluginType(const String &fileOrIdentifier) override;
    FileSearchPath getDefaultLocationsToSearch() override { return {}; }
    bool canScanForPlugins() const override { return false; }
    void findAllTypesForFile(OwnedArray<PluginDescription> &, const String &) override {}
    bool doesPluginStillExist(const PluginDescription &) override { return true; }
    String getNameOfPluginFromIdentifier(const String &fileOrIdentifier) override { return fileOrIdentifier; }
    StringArray searchPathsForPlugins(const FileSearchPath &, bool, bool) override { return {}; }
    bool requiresUnblockedMessageThreadDuringCreation(const PluginDescription&) const noexcept override { return false; }

    void createPluginInstance(const PluginDescription&, double initialSampleRate,
        int initialBufferSize, void *userData,
        void (*callback) (void*, AudioPluginInstance*, const String&)) override;


private:

    PluginDescription audioInDesc;
    PluginDescription audioOutDesc;
    PluginDescription midiInDesc;
    PluginDescription midiOutDesc;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InternalPluginFormat)
};
