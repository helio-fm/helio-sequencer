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

#include "SequenceModifier.h"
#include "SequencerOperations.h"
#include "PianoSequence.h"
#include "ProjectNode.h"
#include "ProjectMetadata.h"
#include "ProjectTimeline.h"
#include "MidiTrack.h"

class RefactoringSequenceModifier final : public SequenceModifier
{
public:

    enum class Type
    {
        MelodicInversion,
        Retrograde,
        CleanupOverlaps,
        ChordInversionUp,
        ChordInversionDown,
        InScaleTranspositionUp,
        InScaleTranspositionDown,
        SnapToScale,
        Legato,
        Staccato,
        Quantization
    };

    static constexpr Type allTypes[11] = {
        Type::MelodicInversion,
        Type::Retrograde,
        Type::CleanupOverlaps,
        Type::ChordInversionUp,
        Type::ChordInversionDown,
        Type::InScaleTranspositionUp,
        Type::InScaleTranspositionDown,
        Type::SnapToScale,
        Type::Legato,
        Type::Staccato,
        Type::Quantization
    };

    RefactoringSequenceModifier() = default;
    RefactoringSequenceModifier(const RefactoringSequenceModifier &other) noexcept = default;

    explicit RefactoringSequenceModifier(Type type, int parameterValue = 0) :
        type(type), parameterValue(parameterValue) {}

    void processSequence(const ProjectNode &project,
        const Clip &clip, const PianoSequence &sequence) override
    {
        if (!this->isEnabled())
        {
            return;
        }

        switch (this->type)
        {
        case Type::MelodicInversion:
            SequencerOperations::melodicInversion(sequence, false, false);
            break;
        case Type::Retrograde:
            SequencerOperations::retrograde(sequence, false, false);
            break;
        case Type::CleanupOverlaps:
            SequencerOperations::cleanupOverlaps(sequence, false, false);
            break;
        case Type::ChordInversionUp:
        case Type::ChordInversionDown:
            jassert(this->parameterValue != 0);
            for (int i = 0; i < abs(this->parameterValue); ++i)
            {
                SequencerOperations::invertChord(sequence,
                    (this->parameterValue > 0 ? 1 : -1) * project.getProjectInfo()->getPeriodSize(),
                    false, false);
            }
            break;
        case Type::InScaleTranspositionUp:
        case Type::InScaleTranspositionDown:
        case Type::SnapToScale:
            SequencerOperations::shiftInScaleKeyRelative(sequence, clip,
                project.getTimeline()->getKeySignaturesSequence(),
                project.getProjectInfo()->getTemperament()->getHighlighting(),
                this->parameterValue, false, false);
            break;
        case Type::Legato:
            SequencerOperations::makeLegato(sequence, 0.f, false, false);
            break;
        case Type::Staccato:
            jassert(this->parameterValue >= 1 && this->parameterValue <= 64);
            SequencerOperations::makeStaccato(sequence,
                (1.f / float(this->parameterValue)) * float(Globals::beatsPerBar), false, false);
            break;
        case Type::Quantization:
            jassert(this->parameterValue >= 1 && this->parameterValue <= 32);
            SequencerOperations::quantize(sequence, float(this->parameterValue), false, false);
            break;
        default:
            jassertfalse;
            break;
        }
    }

    bool hasParameters() const override
    {
        return hasParameters(this->type);
    }

    static bool hasParameters(Type type)
    {
        switch (type)
        {
        case Type::CleanupOverlaps:
        case Type::MelodicInversion:
        case Type::Retrograde:
        case Type::Legato:
        case Type::SnapToScale:
            return false;
        case Type::ChordInversionUp:
        case Type::ChordInversionDown:
        case Type::InScaleTranspositionUp:
        case Type::InScaleTranspositionDown:
        case Type::Staccato:
        case Type::Quantization:
            return true;
        default:
            jassertfalse;
            break;
        }

        return false;
    }

    String getDescription() const override
    {
        return getDescription(this->type, this->parameterValue);
    }

    static String getDescription(Type type, int parameterValue = 0)
    {
        String stepsString;
        if (parameterValue != 0)
        {
            stepsString = " (" +
                (parameterValue > 0 ? "+" + String(parameterValue) : String(parameterValue)) +
                ")";
        }

        String durationString;
        if (parameterValue != 0)
        {
            durationString = " (1/" + String(parameterValue) + ")";
        }

        switch (type)
        {
        case Type::MelodicInversion:
            return TRANS(I18n::Menu::Refactor::melodicInversion);
        case Type::Retrograde:
            return TRANS(I18n::Menu::Refactor::retrograde);
        case Type::CleanupOverlaps:
            return TRANS(I18n::Menu::Refactor::cleanup);
        case Type::ChordInversionUp:
            return TRANS(I18n::Menu::Refactor::inverseUp) + stepsString;
        case Type::ChordInversionDown:
            return TRANS(I18n::Menu::Refactor::inverseDown) + stepsString;
        case Type::InScaleTranspositionUp:
            return TRANS(I18n::Menu::Refactor::inScaleTransposeUp) + stepsString;
        case Type::InScaleTranspositionDown:
            return TRANS(I18n::Menu::Refactor::inScaleTransposeDown) + stepsString;
        case Type::SnapToScale:
            return TRANS(I18n::Menu::Refactor::alignToScale);
        case Type::Legato:
            return TRANS(I18n::Menu::Refactor::legato);
        case Type::Staccato:
            return TRANS(I18n::Menu::Refactor::staccato) + durationString;
        case Type::Quantization:
            if (parameterValue == 0) { return TRANS(I18n::Menu::Selection::notesQuantizeTo); }
            else if (parameterValue == 1) { return TRANS(I18n::Menu::quantizeTo1_1); }
            else if (parameterValue == 2) { return TRANS(I18n::Menu::quantizeTo1_2); }
            else if (parameterValue == 4) { return TRANS(I18n::Menu::quantizeTo1_4); }
            else if (parameterValue == 8) { return TRANS(I18n::Menu::quantizeTo1_8); }
            else if (parameterValue == 16) { return TRANS(I18n::Menu::quantizeTo1_16); }
            else if (parameterValue == 32) { return TRANS(I18n::Menu::quantizeTo1_32); }
            jassertfalse;
            return {};
        default:
            jassertfalse;
            break;
        }

        return {};
    }

    Icons::Id getIconId() const override
    {
        return getIconId(this->type);
    }

    static Icons::Id getIconId(Type type)
    {
        switch (type)
        {
        case Type::MelodicInversion:
            return Icons::inversion;
        case Type::Retrograde:
            return Icons::retrograde;
        case Type::CleanupOverlaps:
            return Icons::cleanup;
        case Type::ChordInversionUp:
            return Icons::inverseUp;
        case Type::ChordInversionDown:
            return Icons::inverseDown;
        case Type::InScaleTranspositionUp:
            return Icons::up;
        case Type::InScaleTranspositionDown:
            return Icons::down;
        case Type::SnapToScale:
            return Icons::snap;
        case Type::Legato:
            return Icons::legato;
        case Type::Staccato:
            return Icons::staccato;
        case Type::Quantization:
            return Icons::ellipsis;
        default:
            jassertfalse;
            break;
        }

        return 0;
    }

    Type getType() const noexcept
    {
        return this->type;
    }

    // all refactorings in this modifier have at most 1 parameter:
    struct Parameter final
    {
        int value;
        String name;

        Parameter() = default;
        Parameter(int value, String name) :
            value(value), name(move(name)) {};

        static Array<Parameter> fromIntegers(Array<int> values)
        {
            Array<Parameter> result;
            for (const auto i : values)
            {
                result.add({ i, i > 0 ? "+" + String(i) : String(i) });
            }
            return result;
        }

        static Array<Parameter> fromNoteFractions(Array<int> values)
        {
            Array<Parameter> result;
            for (const auto i : values)
            {
                result.add({ i, "1/" + String(i) });
            }
            return result;
        }
    };

    static Array<Parameter> getParametersChoice(Type type)
    {
        switch (type)
        {
        case Type::MelodicInversion:
        case Type::Retrograde:
        case Type::CleanupOverlaps:
        case Type::Legato:
        case Type::SnapToScale:
            return {};
        case Type::Staccato:
            return Parameter::fromNoteFractions({ 1, 2, 4, 8, 16, 32, 64 });
        case Type::ChordInversionUp:
            return Parameter::fromIntegers({ 1, 2, 3 });
        case Type::ChordInversionDown:
            return Parameter::fromIntegers({ -1, -2, -3 });
        case Type::InScaleTranspositionUp:
            return Parameter::fromIntegers({ 1, 2, 3, 4, 5, 6, 7 });
        case Type::InScaleTranspositionDown:
            return Parameter::fromIntegers({ -1, -2, -3, -4, -5, -6, -7 });
        case Type::Quantization:
            return {
                Parameter(1, TRANS(I18n::Menu::quantizeTo1_1)),
                Parameter(2, TRANS(I18n::Menu::quantizeTo1_2)),
                Parameter(4, TRANS(I18n::Menu::quantizeTo1_4)),
                Parameter(8, TRANS(I18n::Menu::quantizeTo1_8)),
                Parameter(16, TRANS(I18n::Menu::quantizeTo1_16)),
                Parameter(32, TRANS(I18n::Menu::quantizeTo1_32))
            };
        default:
            jassertfalse;
            break;
        }

        return {};
    }

    SequenceModifier::Ptr withEnabledFlag(bool shouldBeEnabled) const override
    {
        auto m = make<RefactoringSequenceModifier>(*this);
        m->enabled = shouldBeEnabled;
        return SequenceModifier::Ptr(m.release());
    }

    bool isEquivalentTo(SequenceModifier::Ptr other) const override
    {
        jassert(other != nullptr);
        if (other.get() == this) { return true; }
        if (other->isEnabled() != this->isEnabled()) { return false; }

        if (const auto *casted = dynamic_cast<RefactoringSequenceModifier *>(other.get()))
        {
            return casted->type == this->type &&
                casted->parameterValue == this->parameterValue;
        }

        return false;
    }

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override
    {
        using namespace Serialization;

        SerializedData tree(Modifiers::refactoringModifier);

        if (!this->enabled)
        {
            tree.setProperty(Modifiers::isEnabled, false);
        }

        switch (this->type)
        {
        case Type::MelodicInversion:
            tree.setProperty(Modifiers::refactoringType, Modifiers::refactoringMelodicInversion.toString());
            break;
        case Type::Retrograde:
            tree.setProperty(Modifiers::refactoringType, Modifiers::refactoringRetrograde.toString());
            break;
        case Type::CleanupOverlaps:
            tree.setProperty(Modifiers::refactoringType, Modifiers::refactoringCleanupOverlaps.toString());
            break;
        case Type::ChordInversionUp:
        case Type::ChordInversionDown:
            tree.setProperty(Modifiers::refactoringType, Modifiers::refactoringChordInversion.toString());
            break;
        case Type::InScaleTranspositionUp:
        case Type::InScaleTranspositionDown:
        case Type::SnapToScale:
            tree.setProperty(Modifiers::refactoringType, Modifiers::refactoringInScaleTransposition.toString());
            break;
        case Type::Legato:
            tree.setProperty(Modifiers::refactoringType, Modifiers::refactoringLegato.toString());
            break;
        case Type::Staccato:
            tree.setProperty(Modifiers::refactoringType, Modifiers::refactoringStaccato.toString());
            break;
        case Type::Quantization:
            tree.setProperty(Modifiers::refactoringType, Modifiers::refactoringQuantization.toString());
            break;
        default:
            jassertfalse;
            break;
        }

        if (this->parameterValue != 0)
        {
            tree.setProperty(Modifiers::refactoringParameter, this->parameterValue);
        }

        return tree;
    }

    void deserialize(const SerializedData &data) override
    {
        using namespace Serialization;
        jassert(data.hasType(Modifiers::refactoringModifier));

        this->enabled = data.getProperty(Modifiers::isEnabled, true);

        this->parameterValue = int(data.getProperty(Modifiers::refactoringParameter, 0));

        const auto typeId = Identifier(data.getProperty(Modifiers::refactoringType));
        if (typeId == Modifiers::refactoringMelodicInversion)
        {
            this->type = Type::MelodicInversion;
        }
        else if (typeId == Modifiers::refactoringRetrograde)
        {
            this->type = Type::Retrograde;
        }
        else if (typeId == Modifiers::refactoringCleanupOverlaps)
        {
            this->type = Type::CleanupOverlaps;
        }
        else if (typeId == Modifiers::refactoringChordInversion)
        {
            this->parameterValue = jlimit(-3, 3, this->parameterValue);
            this->type = this->parameterValue > 0 ?
                Type::ChordInversionUp : Type::ChordInversionDown;
        }
        else if (typeId == Modifiers::refactoringInScaleTransposition)
        {
            this->parameterValue = jlimit(-7, 7, this->parameterValue);
            this->type = this->parameterValue == 0 ? this->type = Type::SnapToScale :
                (this->parameterValue > 0 ? Type::InScaleTranspositionUp : Type::InScaleTranspositionDown);
        }
        else if (typeId == Modifiers::refactoringLegato)
        {
            this->type = Type::Legato;
        }
        else if (typeId == Modifiers::refactoringStaccato)
        {
            this->parameterValue = jlimit(1, 64, this->parameterValue);
            this->type = Type::Staccato;
        }
        else if (typeId == Modifiers::refactoringQuantization)
        {
            this->parameterValue = jlimit(1, 32, this->parameterValue);
            this->type = Type::Quantization;
        }
        else
        {
            jassertfalse;
        }
    }

    void reset() override
    {
        this->parameterValue = 0;
    }

private:

    Type type = Type::MelodicInversion;

    int parameterValue = 0;

    JUCE_LEAK_DETECTOR(RefactoringSequenceModifier)
};
