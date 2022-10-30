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

class BuiltInSynthsPluginFormat final : public AudioPluginFormat
{
public:

    BuiltInSynthsPluginFormat();

    static const String formatName;
    static const String formatIdentifier;

    String getName() const override;

    bool fileMightContainThisPluginType(const String &fileOrIdentifier) override;

    FileSearchPath getDefaultLocationsToSearch() override
    {
        return {};
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
        return {};
    }

    void createPluginInstance(const PluginDescription &, double initialSampleRate,
        int initialBufferSize, PluginCreationCallback) override;

    bool requiresUnblockedMessageThreadDuringCreation(const PluginDescription&) const noexcept override
    {
        return false;
    }

    bool isTrivialToScan() const noexcept override
    {
        return true;
    };

private:

    PluginDescription defaultInstrument;
    PluginDescription metronomeInstrument;
    PluginDescription soundFontPlayerInstrument;

};
