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

#include "Common.h"
#include "BuiltInSynthsPluginFormat.h"
#include "DefaultSynthAudioPlugin.h"

const String BuiltInSynthsPluginFormat::formatName = "BuiltIn";
const String BuiltInSynthsPluginFormat::formatIdentifier = "BuiltIn";

BuiltInSynthsPluginFormat::BuiltInSynthsPluginFormat()
{
    DefaultSynthAudioPlugin defaultOne;
    defaultOne.fillInPluginDescription(this->defaultInstrument);
}

String BuiltInSynthsPluginFormat::getName() const
{
    return BuiltInSynthsPluginFormat::formatName;
}

void BuiltInSynthsPluginFormat::findAllTypesForFile(OwnedArray <PluginDescription> &description, const String &id)
{
    if (id == DefaultSynthAudioPlugin::instrumentId)
    {
        description.add(new PluginDescription(this->defaultInstrument));
    }
}

bool BuiltInSynthsPluginFormat::fileMightContainThisPluginType(const String &fileOrIdentifier)
{
    return fileOrIdentifier.isEmpty() || fileOrIdentifier == BuiltInSynthsPluginFormat::formatIdentifier;
}

void BuiltInSynthsPluginFormat::createPluginInstance(const PluginDescription &desc,
    double initialSampleRate, int initialBufferSize, PluginCreationCallback callback)
{
    if (desc.name == this->defaultInstrument.name)
    {
        callback(make<DefaultSynthAudioPlugin>(), {});
        return;
    }
    
    callback(nullptr, {});
}
