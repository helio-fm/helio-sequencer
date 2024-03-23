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
#include "Arpeggiator.h"
#include "SequencerOperations.h"
#include "PianoSequence.h"
#include "ProjectNode.h"
#include "ProjectMetadata.h"
#include "ProjectTimeline.h"

class ArpeggiationSequenceModifier final : public SequenceModifier
{
public:

    ArpeggiationSequenceModifier() = default;
    ArpeggiationSequenceModifier(const ArpeggiationSequenceModifier &other) noexcept = default;

    explicit ArpeggiationSequenceModifier(const Arpeggiator::Ptr arpeggiator, float speed = 1.f) :
        arpeggiator(arpeggiator), speedMultiplier(speed) {}

    void processSequence(const ProjectNode &project,
        const Clip &clip, const PianoSequence &sequence) override
    {
        if (!this->isEnabled())
        {
            return;
        }

        jassert(this->arpeggiator != nullptr);
        if (this->arpeggiator != nullptr)
        {
            SequencerOperations::arpeggiate(sequence, clip, this->arpeggiator,
                project.getProjectInfo()->getTemperament(),
                project.getTimeline()->getKeySignaturesSequence(),
                project.getTimeline()->getTimeSignaturesAggregator(),
                this->speedMultiplier,
                0.f, false, false, false, false);
        }
    }

    bool hasParameters() const override
    {
        return true;
    }

    String getDescription() const override
    {
        return TRANS(I18n::Menu::Selection::notesArpeggiate) +
            " (" + String(this->arpeggiator->getName()) + 
                (this->speedMultiplier != 1.f ? (" x" + String(this->speedMultiplier)) : "") + ")";
    }
    
    Icons::Id getIconId() const override
    {
        return Icons::Ids::arpeggiate;
    }

    SequenceModifier::Ptr withEnabledFlag(bool shouldBeEnabled) const override
    {
        auto m = make<ArpeggiationSequenceModifier>(*this);
        m->enabled = shouldBeEnabled;
        return SequenceModifier::Ptr(m.release());
    }

    bool isEquivalentTo(SequenceModifier::Ptr other) const override
    {
        jassert(other != nullptr);
        if (other.get() == this) { return true; }
        if (other->isEnabled() != this->isEnabled()) { return false; }

        if (const auto *casted = dynamic_cast<ArpeggiationSequenceModifier *>(other.get()))
        {
            if (casted->arpeggiator == nullptr || this->arpeggiator == nullptr)
            {
                jassertfalse; // not really expecting invaid objects here
                return casted->arpeggiator.get() != this->arpeggiator.get();
            }

            return casted->speedMultiplier == this->speedMultiplier &&
                *casted->arpeggiator == *this->arpeggiator;
        }

        return false;
    }

    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//

    SerializedData serialize() const override
    {
        using namespace Serialization;

        SerializedData tree(Modifiers::arpeggiationModifier);

        if (!this->enabled)
        {
            tree.setProperty(Modifiers::isEnabled, false);
        }

        jassert(this->arpeggiator != nullptr);
        if (this->arpeggiator != nullptr)
        {
            tree.appendChild(this->arpeggiator->serialize());
        }

        if (this->speedMultiplier != 1.f)
        {
            tree.setProperty(Modifiers::arpeggiationSpeed, this->speedMultiplier);
        }

        return tree;
    }

    void deserialize(const SerializedData &data) override
    {
        using namespace Serialization;
        jassert(data.hasType(Modifiers::arpeggiationModifier));

        this->enabled = data.getProperty(Modifiers::isEnabled, true);

        jassert(data.getNumChildren() == 1);
        if (data.getNumChildren() == 1)
        {
            this->arpeggiator = Arpeggiator::Ptr(new Arpeggiator());
            this->arpeggiator->deserialize(data.getChild(0));
        }

        this->speedMultiplier = jlimit(0.25f, 8.f,
            float(data.getProperty(Modifiers::arpeggiationSpeed, 1.f)));
    }

    void reset() override
    {
        this->arpeggiator = nullptr;
        this->speedMultiplier = 1.f;
    }

private:

    Arpeggiator::Ptr arpeggiator;

    float speedMultiplier = 1.f;

    JUCE_LEAK_DETECTOR(ArpeggiationSequenceModifier)
};
