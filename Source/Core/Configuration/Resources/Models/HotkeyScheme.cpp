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

#include "Common.h"
#include "HotkeyScheme.h"
#include "SerializationKeys.h"

// this class copies createFromDescription() etc from the KeyPress class,
// except it allows to specify spacebar as a modifier as well as a keypress:
// the app uses spacebar as a temporary toggle for the panning mode,
// so we want navigation hotkeys to be consistent with it, e.g. spacebar + cursor keys
struct KeyPressReader final
{
    static const char *numberPadPrefix() noexcept
    {
        return "numpad ";
    }

    static int getNumpadKeyCode(const String &desc)
    {
        if (desc.containsIgnoreCase(numberPadPrefix()))
        {
            const auto lastChar = desc.trimEnd().getLastCharacter();
            switch (lastChar)
            {
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                    return int(KeyPress::numberPad0 + int(lastChar) - '0');
                case '+':   return KeyPress::numberPadAdd;
                case '-':   return KeyPress::numberPadSubtract;
                case '*':   return KeyPress::numberPadMultiply;
                case '/':   return KeyPress::numberPadDivide;
                case '.':   return KeyPress::numberPadDecimalPoint;
                case '=':   return KeyPress::numberPadEquals;
                default:    break;
            }

            if (desc.endsWith ("separator"))  return KeyPress::numberPadSeparator;
            if (desc.endsWith ("delete"))     return KeyPress::numberPadDelete;
        }

        return 0;
    }

    static KeyPress createFromDescription(const String &desc)
    {
        struct KeyNameAndCode final
        {
            const char *name;
            int code;
        };

        const KeyNameAndCode translations[] =
        {
            { "spacebar",       KeyPress::spaceKey },
            { "return",         KeyPress::returnKey },
            { "escape",         KeyPress::escapeKey },
            { "backspace",      KeyPress::backspaceKey },
            { "cursor left",    KeyPress::leftKey },
            { "cursor right",   KeyPress::rightKey },
            { "cursor up",      KeyPress::upKey },
            { "cursor down",    KeyPress::downKey },
            { "page up",        KeyPress::pageUpKey },
            { "page down",      KeyPress::pageDownKey },
            { "home",           KeyPress::homeKey },
            { "end",            KeyPress::endKey },
            { "delete",         KeyPress::deleteKey },
            { "insert",         KeyPress::insertKey },
            { "tab",            KeyPress::tabKey },
            { "play",           KeyPress::playKey },
            { "stop",           KeyPress::stopKey },
            { "fast forward",   KeyPress::fastForwardKey },
            { "rewind",         KeyPress::rewindKey }
        };

        struct ModifierDescription final
        {
            const char *name;
            int flag;
        };

        static const ModifierDescription modifierNames[] =
        {
            { "ctrl",      ModifierKeys::ctrlModifier },
            { "control",   ModifierKeys::ctrlModifier },
            { "ctl",       ModifierKeys::ctrlModifier },
            { "shift",     ModifierKeys::shiftModifier },
            { "shft",      ModifierKeys::shiftModifier },
            { "alt",       ModifierKeys::altModifier },
            { "option",    ModifierKeys::altModifier },
            { "command",   ModifierKeys::commandModifier },
            { "cmd",       ModifierKeys::commandModifier },
            // let's reuse the MMB modifier for the spacebar modifier,
            // since their function is nearly identical in the app (panning mode);
            // see also the HotkeyScheme::dispatchKeyPress method
            { "spacebar",  ModifierKeys::middleButtonModifier }
        };

        int modifiers = 0;
        StringArray usedModifierNames;
        for (int i = 0; i < numElementsInArray(modifierNames); ++i)
        {
            const String modifierName(modifierNames[i].name);
            if (desc.containsWholeWordIgnoreCase(modifierName) &&
                !desc.endsWithIgnoreCase(modifierName)) // is a modifier, not a keypress
            {
                modifiers |= modifierNames[i].flag;
                usedModifierNames.add(modifierName);
            }
        }

        int key = 0;
        for (int i = 0; i < numElementsInArray(translations); ++i)
        {
            const String translationName(translations[i].name);
            if (desc.containsWholeWordIgnoreCase(translationName) &&
                !usedModifierNames.contains(translationName)) // is a keypress, not a modifier
            {
                key = translations[i].code;
                break;
            }
        }

        if (key == 0)
        {
            key = KeyPressReader::getNumpadKeyCode(desc);
        }

        if (key == 0)
        {
            // see if it's a function key..
            if (!desc.containsChar('#')) // avoid mistaking hex-codes like "#f1"
            {
                for (int i = 1; i <= 35; ++i)
                {
                    if (desc.containsWholeWordIgnoreCase("f" + String(i)))
                    {
                        if (i <= 16)        key = KeyPress::F1Key + i - 1;
                        else if (i <= 24)   key = KeyPress::F17Key + i - 17;
                        else if (i <= 35)   key = KeyPress::F25Key + i - 25;
                    }
                }
            }

            if (key == 0)
            {
                // give up and use the hex code..
                auto hexCode = desc.fromFirstOccurrenceOf("#", false, false)
                                   .retainCharacters("0123456789abcdefABCDEF")
                                   .getHexValue32();

                if (hexCode > 0)
                {
                    key = hexCode;
                }
                else
                {
                    key = (int)CharacterFunctions::toUpperCase(desc.getLastCharacter());
                }
            }
        }

        return KeyPress(key, ModifierKeys(modifiers), 0);
    }
};

bool operator==(const HotkeyScheme::Hotkey &lhs, const HotkeyScheme::Hotkey &rhs)
{
    return lhs.keyPress == rhs.keyPress &&
        lhs.componentId == rhs.componentId;
}

HotkeyScheme::HotkeyScheme(const HotkeyScheme &other)
{
    operator= (other);
}

String HotkeyScheme::findHotkeyDescription(int commandId) const noexcept
{
    for (const auto &key : this->keyPresses)
    {
        if (key.second == commandId)
        {
            return key.first.keyPress.getTextDescriptionWithIcons();
        }
    }

    return {};
}

Array<KeyPress> HotkeyScheme::findAllKeyPressesFor(int commandId) const noexcept
{
    Array<KeyPress> result;

    for (const auto &key : this->keyPresses)
    {
        if (key.second == commandId)
        {
            result.add(key.first.keyPress);
        }
    }

    return result;
}

bool HotkeyScheme::dispatchKeyPress(KeyPress keyPress,
    WeakReference<Component> keyPressReceivedFrom,
    WeakReference<Component> componentToSendMessageTo)
{
    // a hack to allow using spacebar as a modifier, see the comments in KeyPressReader
    static KeyPress spaceKey(KeyPress::spaceKey);
    const auto modifiedKeyPress = (spaceKey.isCurrentlyDown() && keyPress != spaceKey) ?
        KeyPress(keyPress.getKeyCode(), ModifierKeys(ModifierKeys::middleButtonModifier), 0) :
        keyPress;

    for (const auto &key : this->keyPresses)
    {
        if (key.first.keyPress == modifiedKeyPress)
        {
            if (this->sendHotkeyCommand(key.first.componentId,
                key.second, keyPressReceivedFrom, componentToSendMessageTo))
            {
                this->lastKeyPress = modifiedKeyPress;
                return true;
            }
        }
    }

    return false;
}

bool HotkeyScheme::dispatchKeyStateChange(bool isKeyDown,
    WeakReference<Component> keyPressReceivedFrom,
    WeakReference<Component> componentToSendMessageTo)
{
    // TODO test multiple key-downs:

    if (isKeyDown)
    {
        for (const auto &key : this->keyDowns)
        {
            if (key.first.keyPress.isCurrentlyDown() &&
                this->sendHotkeyCommand(key.first.componentId,
                    key.second, keyPressReceivedFrom, componentToSendMessageTo))
            {
                this->holdKeys.insert(key.first.keyPress);
                return true;
            }
        }
    }
    else
    {
        for (const auto &key : this->keyUps)
        {
            if (!key.first.keyPress.isCurrentlyDown() && 
                this->holdKeys.contains(key.first.keyPress) &&
                this->sendHotkeyCommand(key.first.componentId, key.second,
                    keyPressReceivedFrom, componentToSendMessageTo))
            {
                this->holdKeys.erase(key.first.keyPress);
                return true;
            }
        }
    }

    return false;
}

static Component *findMessageReceiver(Component *root, const String &id)
{
    if (root->getComponentID() == id)
    {
        return root;
    }

    for (auto *child : root->getChildren())
    {
        if (auto *found = findMessageReceiver(child, id))
        {
            return found;
        }
    }

    return nullptr;
}

bool HotkeyScheme::sendHotkeyCommand(const String &keyPressComponentId,
    CommandIDs::Id commandId,
    WeakReference<Component> keyPressReceivedFrom,
    WeakReference<Component> componentToSendMessageTo)
{
    if (componentToSendMessageTo != this->lastReceiver)
    {
        this->lastReceiver = componentToSendMessageTo;
        this->receiversById.clear();
    }

    auto receiver = this->receiversById[keyPressComponentId];
    if (receiver == nullptr)
    {
        // if there's a modal component that dispatched the keypress, ignore everyone else;
        // otherwise, either the keypress receiver is a target or one of the children it specified
        auto *modalComponent = Component::getCurrentlyModalComponent();
        if (modalComponent != nullptr &&
            modalComponent == keyPressReceivedFrom)
        {
            if (modalComponent->getComponentID() == keyPressComponentId)
            {
                receiver = modalComponent;
            }
        }
        else if (keyPressReceivedFrom != nullptr &&
            keyPressReceivedFrom->getComponentID() == keyPressComponentId)
        {
            receiver = keyPressReceivedFrom;
        }
        else if (componentToSendMessageTo != nullptr)
        {
            receiver = findMessageReceiver(componentToSendMessageTo, keyPressComponentId);
        }

        this->receiversById[keyPressComponentId] = receiver;
    }

    if (receiver != nullptr && receiver->isEnabled() && receiver->isShowing())
    {
        receiver->postCommandMessage(commandId);
        return true;
    }

    return false;
}

//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

SerializedData HotkeyScheme::serialize() const
{
    SerializedData tree(Serialization::UI::Hotkeys::scheme);
    tree.setProperty(Serialization::UI::Hotkeys::schemeName, this->name);
    jassert(false); // not implemented
    return tree;
}

static inline HotkeyScheme::Hotkey deserializeHotkey(const SerializedData &e, const String &receiver = "")
{
    HotkeyScheme::Hotkey result;
    
    const auto keyPressDesc =
        e.getProperty(Serialization::UI::Hotkeys::hotkeyDescription);

    result.keyPress = KeyPressReader::createFromDescription(keyPressDesc);
    result.componentId = receiver.isNotEmpty() ?
        receiver :
        e.getProperty(Serialization::UI::Hotkeys::hotkeyReceiver).toString();

    return result;
}

static inline CommandIDs::Id deserializeCommand(const SerializedData &e)
{
    const auto command = e.getProperty(Serialization::UI::Hotkeys::hotkeyCommand);
    return CommandIDs::getIdForName(command);
}

void HotkeyScheme::deserialize(const SerializedData &data)
{
    // don't reset so that user's scheme appends
    // the built-in one instead of replacing it
    // this->reset();

    using namespace Serialization::UI;

    const auto root =
        data.hasType(Hotkeys::scheme) ?
        data : data.getChildWithName(Hotkeys::scheme);

    if (!root.isValid())
    {
        return;
    }

    this->name = root.getProperty(Hotkeys::schemeName);

    // plain format:

    forEachChildWithType(root, e, Hotkeys::keyPress)
    {
        this->keyPresses[deserializeHotkey(e)] = deserializeCommand(e);
    }

    forEachChildWithType(root, e, Hotkeys::keyDown)
    {
        this->keyDowns[deserializeHotkey(e)] = deserializeCommand(e);
    }

    forEachChildWithType(root, e, Hotkeys::keyUp)
    {
        this->keyUps[deserializeHotkey(e)] = deserializeCommand(e);
    }

    // grouped format:

    forEachChildWithType(root, group, Hotkeys::group)
    {
        const auto receiver = group.getProperty(Hotkeys::hotkeyReceiver);

        forEachChildWithType(group, e, Hotkeys::keyPress)
        {
            this->keyPresses[deserializeHotkey(e, receiver)] = deserializeCommand(e);
        }

        forEachChildWithType(group, e, Hotkeys::keyDown)
        {
            this->keyDowns[deserializeHotkey(e, receiver)] = deserializeCommand(e);
        }

        forEachChildWithType(group, e, Hotkeys::keyUp)
        {
            this->keyUps[deserializeHotkey(e, receiver)] = deserializeCommand(e);
        }
    }
}

void HotkeyScheme::reset()
{
    this->name.clear();
    this->keyPresses.clear();
    this->keyDowns.clear();
    this->keyUps.clear();
    this->receiversById.clear();
    this->holdKeys.clear();
    this->lastReceiver = nullptr;
}

HotkeyScheme &HotkeyScheme::operator=(const HotkeyScheme &other)
{
    this->name = other.name;
    this->keyPresses.clear();
    this->keyDowns.clear();
    this->keyUps.clear();
    this->keyPresses.insert(other.keyPresses.begin(), other.keyPresses.end());
    this->keyDowns.insert(other.keyDowns.begin(), other.keyDowns.end());
    this->keyUps.insert(other.keyUps.begin(), other.keyUps.end());
    return *this;
}

//===----------------------------------------------------------------------===//
// BaseResource
//===----------------------------------------------------------------------===//

String HotkeyScheme::getResourceId() const noexcept
{
    return this->name;
}

Identifier HotkeyScheme::getResourceType() const noexcept
{
    return Serialization::Resources::hotkeySchemes;
}

KeyPress HotkeyScheme::getLastKeyPress() const noexcept
{
    return this->lastKeyPress;
}

const HotkeyScheme::HotkeyMap &HotkeyScheme::getKeyPresses() const noexcept
{
    return this->keyPresses;
}

bool operator==(const HotkeyScheme &l, const HotkeyScheme &r)
{
    return &l == &r || l.name == r.name;
}
