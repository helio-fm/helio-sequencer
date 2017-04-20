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

#define HELIO_BUILT_IN_PLUGIN_FORMAT_NAME "BuiltIn"
#define HELIO_BUILT_IN_PLUGIN_IDENTIFIER "BuiltIn"

namespace BuiltInSynth
{
    static const String pianoId = "<piano>";
} // namespace BuiltInSynth

class BuiltInSynthFormat : public AudioPluginFormat
{
public:

    BuiltInSynthFormat();

    String getName() const override;

    bool fileMightContainThisPluginType(const String &fileOrIdentifier) override;

    FileSearchPath getDefaultLocationsToSearch() override
    {
        // app path / scripts
        return FileSearchPath();
    }

    bool canScanForPlugins() const override
    {
        return false;
    }

    void findAllTypesForFile(OwnedArray <PluginDescription> &, const String &) override;

    bool doesPluginStillExist(const PluginDescription &) override
    {
        return true;
    }

    String getNameOfPluginFromIdentifier(const String &fileOrIdentifier) override
    {
        return fileOrIdentifier;
    }

    bool pluginNeedsRescanning(const PluginDescription&) override
    {
        return false;
    }

    StringArray searchPathsForPlugins(const FileSearchPath &, bool, bool) override
    {
        return StringArray();
    }

    void createPluginInstance(const PluginDescription&, double initialSampleRate,
                                      int initialBufferSize, void *userData,
                                      void (*callback) (void*, AudioPluginInstance*, const String&)) override;

    bool requiresUnblockedMessageThreadDuringCreation (const PluginDescription&) const noexcept override
    {
        return false;
    }

private:

    PluginDescription pianoDescription;
    
    // TODO more

};
