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

//===----------------------------------------------------------------------===//
// Pragmas
//===----------------------------------------------------------------------===//

// Unreferenced formal parameter
#pragma warning(disable: 4100)
// Hides class member
#pragma warning(disable: 4458)
// std::uninitialized_copy::_Unchecked_iterators::_Deprecate
#pragma warning(disable: 4996)

//===----------------------------------------------------------------------===//
// JUCE
//===----------------------------------------------------------------------===//

#if !defined JUCE_ANDROID
#   define JUCE_USE_FREETYPE_AMALGAMATED 1
#   define JUCE_AMALGAMATED_INCLUDE 1
#endif

#include "JuceHeader.h"
#include "Serializable.h"

#include <limits.h>
#include <float.h>
#include <math.h>

//===----------------------------------------------------------------------===//
// SparsePP
//===----------------------------------------------------------------------===//

#include "../../ThirdParty/SparseHashMap/sparsepp/spp.h"

template <class Key, class T, class HashFcn = spp::spp_hash<Key>, class EqualKey = std::equal_to<Key>>
using SparseHashMap = spp::sparse_hash_map<Key, T, HashFcn, EqualKey>;

template <class Value, class HashFcn = spp::spp_hash<Value>, class EqualKey = std::equal_to<Value>>
using SparseHashSet = spp::sparse_hash_set<Value, HashFcn, EqualKey>;

using HashCode = size_t;

#if !defined HASH_CODE_MAX
#   define HASH_CODE_MAX SIZE_MAX
#endif

struct StringHash
{
    inline HashCode operator()(const juce::String &key) const noexcept
    {
        return static_cast<HashCode>(key.hashCode()) % HASH_CODE_MAX;
    }
};

struct IdentifierHash
{
    inline HashCode operator()(const juce::Identifier &key) const noexcept
    {
        return static_cast<HashCode>(key.toString().hashCode()) % HASH_CODE_MAX;
    }

    static int generateHash(const Identifier& key, int upperLimit) noexcept
    {
        return uint32(key.toString().hashCode()) % (uint32)upperLimit;
    }
};

//===----------------------------------------------------------------------===//
// Various helpers
//===----------------------------------------------------------------------===//

template <class T>
using UniquePointer = std::unique_ptr<T>;

template <class T>
using Function = std::function<T>;

#if _MSC_VER
inline float roundf(float x)
{
    return (x >= 0.0f) ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
#endif

#if JUCE_ANDROID || JUCE_IOS
#   define HELIO_MOBILE 1
#else
#   define HELIO_DESKTOP 1
#endif

// Beat is essentially a quarter-note
#define BEATS_PER_BAR 4

// Defines a maximum available resolution
#define TICKS_PER_BEAT 16

#define VELOCITY_SAVE_ACCURACY 1024.f

// Rolls allow up to 16 divisions per beat, there's no need for better accuracy:
inline float roundBeat(float beat)
{
    return roundf(beat * static_cast<float>(TICKS_PER_BEAT)) / static_cast<float>(TICKS_PER_BEAT);
}

#define forEachValueTreeChildWithType(parentElement, child, requiredType) \
    for (const auto &child : parentElement) if (child.hasType(requiredType))

#define callMessageThreadFrom(threadType, function) \
    MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void* \
        { \
            const auto self = static_cast<threadType *>(ptr); \
            function(self); \
            return nullptr; \
        }, this)

//===----------------------------------------------------------------------===//
// Internationalization
//===----------------------------------------------------------------------===//

#include "TranslationsManager.h"

#if defined TRANS
#   undef TRANS
#endif

#define TRANS(stringLiteral) TranslationsManager::getInstance().translate(stringLiteral)
#define TRANS_PLURAL(stringLiteral, intValue) TranslationsManager::getInstance().translate(stringLiteral, intValue)
