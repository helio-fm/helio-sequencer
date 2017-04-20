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

#ifndef JUCE_ANDROID
    #define JUCE_USE_FREETYPE_AMALGAMATED 1
    #define JUCE_AMALGAMATED_INCLUDE 1
#endif

// Unreferenced formal parameter
#pragma warning(disable: 4100)
// Hides class member
#pragma warning(disable: 4458)

#include "AppConfig.h"

#include "juce_audio_basics.h"
#include "juce_audio_devices.h"
#include "juce_audio_formats.h"
#include "juce_audio_processors.h"
#include "juce_audio_utils.h"
#include "juce_core.h"
#include "juce_cryptography.h"
#include "juce_data_structures.h"
#include "juce_events.h"
#include "juce_graphics.h"
#include "juce_gui_basics.h"
#include "juce_gui_extra.h"
#include "juce_opengl.h"

#if _MSC_VER
inline float roundf(float x)
{
    return (x >= 0.0f) ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
#endif

using namespace juce;

// Internationalization
#include "TranslationManager.h"
#if defined TRANS
#   undef TRANS
#endif
#define TRANS(stringLiteral) TranslationManager::getInstance().translate(stringLiteral)
#define TRANS_PLURAL(stringLiteral, intValue) TranslationManager::getInstance().translate(stringLiteral, intValue)

#if JUCE_ANDROID || JUCE_IOS
#   define HELIO_MOBILE 1
#else
#   define HELIO_DESKTOP 1
#endif

// #if JUCE_ANDROID || JUCE_WINDOWS
// #   define HELIO_HAS_LOTS_OF_FADEINS 0
// #else
#   define HELIO_HAS_LOTS_OF_FADEINS 1
// #endif
