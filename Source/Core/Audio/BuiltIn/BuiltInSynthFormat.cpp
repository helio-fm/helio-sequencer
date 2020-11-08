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
#include "BuiltInSynthFormat.h"
#include "BuiltInSynthAudioPlugin.h"

const String BuiltInSynthFormat::formatName = "BuiltIn";
const String BuiltInSynthFormat::formatIdentifier = "BuiltIn";

BuiltInSynthFormat::BuiltInSynthFormat()
{
    BuiltInSynthAudioPlugin defaultOne;
    defaultOne.fillInPluginDescription(this->defaultInstrument);
}

String BuiltInSynthFormat::getName() const
{
    return BuiltInSynthFormat::formatName;
}

void BuiltInSynthFormat::findAllTypesForFile(OwnedArray <PluginDescription> &description, const String &id)
{
    if (id == BuiltInSynthAudioPlugin::instrumentId)
    {
        description.add(new PluginDescription(this->defaultInstrument));
    }
}

bool BuiltInSynthFormat::fileMightContainThisPluginType(const String &fileOrIdentifier)
{
    const bool match = (fileOrIdentifier.isEmpty() ||
        fileOrIdentifier == BuiltInSynthFormat::formatIdentifier);
    
    return match;
}

void BuiltInSynthFormat::createPluginInstance(const PluginDescription &desc,
    double initialSampleRate, int initialBufferSize, PluginCreationCallback callback)
{
    if (desc.name == this->defaultInstrument.name)
    {
        callback(make<BuiltInSynthAudioPlugin>(), {});
        return;
    }
    
    callback(nullptr, {});
}
