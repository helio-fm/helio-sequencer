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
#include "BuiltInSynthPiano.h"

BuiltInSynthFormat::BuiltInSynthFormat()
{
    BuiltInSynthPiano piano;
    piano.fillInPluginDescription(this->pianoDescription);
}

String BuiltInSynthFormat::getName() const
{
    return HELIO_BUILT_IN_PLUGIN_FORMAT_NAME;
}

void BuiltInSynthFormat::findAllTypesForFile(OwnedArray <PluginDescription> &description, const String &id)
{
    if (id == BuiltInSynth::pianoId)
    {
        description.add(new PluginDescription(this->pianoDescription));
    }
}

bool BuiltInSynthFormat::fileMightContainThisPluginType(const String &fileOrIdentifier)
{
    const bool match = (fileOrIdentifier.isEmpty() ||
                        fileOrIdentifier == HELIO_BUILT_IN_PLUGIN_IDENTIFIER);
    
    return match;
}

void BuiltInSynthFormat::createPluginInstance(const PluginDescription &desc, double initialSampleRate,
                                              int initialBufferSize, void *userData,
                                              void (*callback) (void*, AudioPluginInstance*, const String&))
{
    if (desc.name == this->pianoDescription.name)
    {
        callback(userData, new BuiltInSynthPiano(), {});
        return;
    }
    
    callback(userData, nullptr, {});
}
