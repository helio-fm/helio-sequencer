/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

//===----------------------------------------------------------------------===//
// Pragmas
//===----------------------------------------------------------------------===//

// unreferenced formal parameter
#pragma warning(disable: 4100)
// hides class member
#pragma warning(disable: 4458)
// decorated name length exceeded, name was truncated
#pragma warning(disable: 4503)
// conditional expression is constant
#pragma warning(disable: 4127)

//===----------------------------------------------------------------------===//
// JUCE
//===----------------------------------------------------------------------===//

#include "JuceHeader.h"

#include <climits>
#include <cstring>
#include <cfloat>
#include <cmath>

//===----------------------------------------------------------------------===//
// A better hash map
//===----------------------------------------------------------------------===//

#define TSL_NO_EXCEPTIONS 1

#include "../../ThirdParty/HopscotchMap/include/tsl/hopscotch_map.h"

template <class Key, class T, class HashFn = std::hash<Key>, class EqualKey = std::equal_to<Key>>
using FlatHashMap = tsl::hopscotch_map<Key, T, HashFn, EqualKey>;

#include "../../ThirdParty/HopscotchMap/include/tsl/hopscotch_set.h"

template <class Value, class HashFn = std::hash<Value>, class EqualKey = std::equal_to<Value>>
using FlatHashSet = tsl::hopscotch_set<Value, HashFn, EqualKey>;

using HashCode = size_t;

struct StringHash
{
    inline HashCode operator()(const juce::String &key) const noexcept
    {
        return static_cast<HashCode>(key.hashCode());
    }
};

struct IdentifierHash
{
    inline HashCode operator()(const Identifier &key) const noexcept
    {
        return static_cast<HashCode>(key.toString().hashCode());
    }

    static int generateHash(const Identifier &key, int upperLimit) noexcept
    {
        return uint32(key.toString().hashCode()) % (uint32)upperLimit;
    }
};

//===----------------------------------------------------------------------===//
// Various helpers
//===----------------------------------------------------------------------===//

// More aliases to avoid stl::influenced<eye_bleeding> _code_style
// in favour of a cleaner, C#-like, pidgin C++

template <typename T>
using UniquePointer = std::unique_ptr<T>;

template <typename T, typename... Args> inline
UniquePointer<T> make(Args&&... args)
{
    return UniquePointer<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
using Function = std::function<T>;

using std::move;

#if _MSC_VER
inline float roundf(float x)
{
    return (x >= 0.0f) ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
#endif

// todo replace this with std::bit_cast when migrating to C++20
template <class To, class From>
inline To bitCast(const From &src) noexcept
{
    static_assert(sizeof(To) == sizeof(From));
    static_assert(std::is_trivially_copyable_v<From>);
    static_assert(std::is_trivially_copyable_v<To>);
    static_assert(std::is_trivially_constructible_v<To>);
    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

#if JUCE_ANDROID || JUCE_IOS
#   define PLATFORM_MOBILE 1
#else
#   define PLATFORM_DESKTOP 1
#endif

#define forEachChildWithType(parentElement, child, requiredType) \
    for (const auto &(child) : (parentElement)) if ((child).hasType(requiredType))

#define callbackOnMessageThread(cls, function, ...) \
    MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void* \
        { \
            const auto *self = static_cast<cls *>(ptr); \
            if (self->function != nullptr) \
            { \
                self->function(__VA_ARGS__); \
            } \
            return nullptr; \
        }, this)

#define findDefaultColour(x) LookAndFeel::getDefaultLookAndFeel().findColour(x)

constexpr uint32 fnv1a32val = 0x811c9dc5;
constexpr uint64 fnv1a32prime = 0x1000193;
inline constexpr uint32 constexprHash(const char *const str, const uint32 value = fnv1a32val) noexcept
{
    return (str[0] == '\0') ? value : constexprHash(&str[1], uint32(value ^ uint32(str[0])) * fnv1a32prime);
}

#if JUCE_UNIT_TESTS
namespace juce
{
    namespace UnitTestCategories
    {
        // declare an additional category for all our tests 
        static const String helio { "Helio" };
    }
}
#endif

// Catch floating point exceptions like division by zero
#if DEBUG && JUCE_WINDOWS
#include <float.h>
unsigned int fpControlState = _controlfp(_EM_INEXACT, _MCW_EM);
#endif

//===----------------------------------------------------------------------===//
// Global constants
// (note: changing any of these constants may, and very probably will,
// lead to unpredictable results, e.g. messing up all your projects)
//===----------------------------------------------------------------------===//

namespace Globals
{
    // Beat is essentially a quarter note
    static constexpr auto beatsPerBar = 4;

    // Defines the maximum available resolution
    // (tech debt warning: while this is used in rolls to limit the grid resolution,
    // this is also used in serialization, so saved projects assume this constant;
    // in future we need to update the serialization format in a way that
    // doesn't break existing projects, and allows to increase this limit)
    static constexpr auto ticksPerBeat = 16;

    static constexpr auto minClipLength = 1.f / static_cast<float>(ticksPerBeat);
    static constexpr auto minNoteLength = 1.f / static_cast<float>(ticksPerBeat);

    static constexpr auto velocitySaveResolution = 1024.f;

    static constexpr auto maxKeyboardSize = 2048;
    static constexpr auto twelveToneKeyboardSize = 128;
    static constexpr auto twelveTonePeriodSize = 12;
    static constexpr auto numPeriodsInKeyboard =
        static_cast<float>(twelveToneKeyboardSize) /
        static_cast<float>(twelveTonePeriodSize);

    static constexpr auto numChannels = 16;

    // 240 bpm (== 250 ms per quarter note)
    static constexpr auto maxMsPerBeat = 250.0;

    namespace Defaults
    {
        // Milliseconds per quarter note, default 120 BPM
        static constexpr auto msPerBeat = 500;
        static constexpr auto tempoBpm = 60000 / msPerBeat;

        // Any length here is in beats
        static constexpr auto projectLength = static_cast<float>(beatsPerBar * 4);

        static constexpr auto newNoteLength = 0.5f;
        static constexpr auto newNoteVelocity = 0.25f;
        static constexpr auto emptyClipLength = static_cast<float>(beatsPerBar * 2);

        // Note previews in various tools
        static constexpr auto previewNoteVelocity = 0.25f;
        static constexpr auto previewNoteLength = static_cast<float>(beatsPerBar);

        // The default time is common time, 4/4
        static constexpr auto timeSignatureNumerator = 4;
        static constexpr auto timeSignatureDenominator = 4;

        static constexpr auto onOffControllerState = 0.f;
        static constexpr auto automationControllerCurve = 0.5f;
    }

    namespace UI
    {
        static constexpr auto headlineHeight = 32;
        static constexpr auto menuPanelRowHeight = 32;

        #if PLATFORM_MOBILE
        static constexpr auto sidebarWidth = 44;
        static constexpr auto editorPanelHeight = 108;
        #elif PLATFORM_DESKTOP
        static constexpr auto sidebarWidth = 38;
        static constexpr auto editorPanelHeight = 128;
        #endif

        static constexpr auto sidebarFooterHeight = 72;
        static constexpr auto projectMapHeight = 72;
        static constexpr auto defaultBeatWidth = 48;
        static constexpr auto rollShadowSize = 12;
        static constexpr auto rollHeaderHeight = 35;
        static constexpr auto rollScrollerHeight = 30;
        static constexpr auto headlineIconSize = 16;
        static constexpr auto sidebarRowHeight = 36;
        static constexpr auto sidebarShadowSize = 22;
        static constexpr auto textEditorHeight = 32;

        static constexpr auto cursorBlinkDelayMs = 530;

        static constexpr auto fadeInShort = 60;
        static constexpr auto fadeOutShort = 90;
        static constexpr auto fadeInLong = 120;
        static constexpr auto fadeOutLong = 150;

        static constexpr auto tooltipDelayMs = 650;

        namespace Fonts
        {
            #if PLATFORM_MOBILE
            static constexpr auto XS = 12.f;
            static constexpr auto S = 14.f;
            static constexpr auto M = 16.f;
            static constexpr auto L = 18.f;
            static constexpr auto XL = 24.f;
            static constexpr auto XXL = 32.f;
            #elif PLATFORM_DESKTOP
            static constexpr auto XS = 14.f;
            static constexpr auto S = 16.f;
            static constexpr auto M = 18.f;
            static constexpr auto L = 21.f;
            static constexpr auto XL = 28.f;
            static constexpr auto XXL = 37.f;
            #endif
        }

        namespace FileChooser
        {
            static constexpr auto forFileToSave =
                FileBrowserComponent::saveMode |
                FileBrowserComponent::canSelectFiles |
                FileBrowserComponent::warnAboutOverwriting;

            static constexpr auto forFileToOpen =
                FileBrowserComponent::openMode |
                FileBrowserComponent::canSelectFiles;

            static constexpr auto forDirectory =
                FileBrowserComponent::openMode |
                FileBrowserComponent::canSelectDirectories;
        }
    }
}

// Rolls support up to 16 divisions per quarter note (see the comment above)
inline float roundBeat(float beat)
{
    return roundf(beat * static_cast<float>(Globals::ticksPerBeat)) /
        static_cast<float>(Globals::ticksPerBeat);
}

//===----------------------------------------------------------------------===//
// Internationalization
//===----------------------------------------------------------------------===//

#include "Serializable.h"

#include "App.h"
#include "TranslationKeys.h"

#if defined TRANS
#   undef TRANS
#endif

#define TRANS(singular) App::translate(singular)
#define TRANS_PLURAL(plural, number) App::translate(plural, number)
