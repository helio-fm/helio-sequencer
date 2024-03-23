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
#include "PianoSequence.h"

// todo: fine-tuning dynamics in a non-destructive way somehow?

class TuningSequenceModifier final : public SequenceModifier
{
public:

    TuningSequenceModifier() = default;
    TuningSequenceModifier(const TuningSequenceModifier &other) noexcept = default;

    void processSequence(const ProjectNode &project,
        const Clip &clip, const PianoSequence &sequence) override
    {
        if (!this->isEnabled())
        {
            return;
        }

        jassertfalse; // not implemented
    }

    bool hasParameters() const override
    {
        jassertfalse; // not implemented
        return false;
    }

    String getDescription() const override
    {
        jassertfalse; // not implemented
        return {};
    }

    Icons::Id getIconId() const override
    {
        jassertfalse; // not implemented
        return 0;
    }

    SequenceModifier::Ptr withEnabledFlag(bool shouldBeEnabled) const override
    {
        auto m = make<TuningSequenceModifier>(*this);
        m->enabled = shouldBeEnabled;
        return SequenceModifier::Ptr(m.release());
    }
    
    bool isEquivalentTo(SequenceModifier::Ptr other) const override
    {
        jassert(other != nullptr);
        if (other.get() == this) { return true; }
        if (other->isEnabled() != this->isEnabled()) { return false; }

        if (const auto *casted = dynamic_cast<TuningSequenceModifier *>(other.get()))
        {
            jassertfalse; // not implemented
            return true;
        }

        return false;
    }

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override
    {
        using namespace Serialization;

        SerializedData tree(Modifiers::tuningModifier);
        
        if (!this->enabled)
        {
            tree.setProperty(Modifiers::isEnabled, false);
        }

        jassertfalse; // not implemented

        return tree;
    }

    void deserialize(const SerializedData &data) override
    {
        using namespace Serialization;
        jassert(data.hasType(Modifiers::tuningModifier));

        this->enabled = data.getProperty(Modifiers::isEnabled, true);

        jassertfalse; // not implemented
    }

    void reset() override {}

private:

    JUCE_LEAK_DETECTOR(TuningSequenceModifier)
};
