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
#include "SoundFontSynthAudioPlugin.h"
#include "BuiltInSynthsPluginFormat.h"
#include "DocumentHelpers.h"
#include "ModeIndicatorComponent.h"
#include "PlayButton.h"
#include "IconButton.h"
#include "SerializationKeys.h"
#include "HelioTheme.h"

const String SoundFontSynthAudioPlugin::instrumentId = "<soundfont-player>";
const String SoundFontSynthAudioPlugin::instrumentName = "SoundFont Player";

//===----------------------------------------------------------------------===//
// A simple UI allowing to pick a file and switch between presets, if any
//===----------------------------------------------------------------------===//

class SoundFontSynthEditor final : public AudioProcessorEditor
{
public:

    explicit SoundFontSynthEditor(WeakReference<SoundFontSynthAudioPlugin> soundFontPlugin) :
        AudioProcessorEditor(soundFontPlugin),
        audioPlugin(soundFontPlugin)
    {
        this->filePath = make<TextEditor>();
        this->filePath->setReadOnly(true);
        this->filePath->setFont(Globals::UI::Fonts::M);
        this->filePath->setJustification(Justification::centredLeft);
        this->addAndMakeVisible(this->filePath.get());

        this->browseButton = make<IconButton>(Icons::browse, CommandIDs::Browse);
        this->browseButton->setMouseCursor(MouseCursor::PointingHandCursor);
        this->addAndMakeVisible(this->browseButton.get());

        this->leftArrow = make<IconButton>(Icons::stretchLeft, CommandIDs::SelectPreviousPreset);
        this->addAndMakeVisible(this->leftArrow.get());

        this->rightArrow = make<IconButton>(Icons::stretchRight, CommandIDs::SelectNextPreset);
        this->addAndMakeVisible(this->rightArrow.get());

        this->programIndexIndicator = make<ModeIndicatorComponent>();
        this->addAndMakeVisible(this->programIndexIndicator.get());

        this->programNameLabel = make<Label>();
        this->programNameLabel->setFont(Globals::UI::Fonts::M);
        this->programNameLabel->setJustificationType(Justification::centred);
        this->addAndMakeVisible(this->programNameLabel.get());

        this->syncDataWithAudioPlugin();
        this->setSize(640, 120);
    }

    void syncDataWithAudioPlugin()
    {
        const auto numPrograms = this->audioPlugin->getNumPrograms();
        const auto currentProgram = this->audioPlugin->getCurrentProgram();
        const bool hasPresetSelection = numPrograms > 1;

        this->leftArrow->setVisible(hasPresetSelection);
        this->leftArrow->setEnabled(currentProgram > 0);

        this->rightArrow->setVisible(hasPresetSelection);
        this->rightArrow->setEnabled(currentProgram < numPrograms - 1);

        this->programIndexIndicator->setVisible(hasPresetSelection);
        this->programIndexIndicator->setNumModes(numPrograms);
        this->programIndexIndicator->setCurrentMode(currentProgram);

        this->programNameLabel->setText(this->audioPlugin->getCurrentProgramName(), dontSendNotification);

        const auto synthParams = this->audioPlugin->getSynthParameters();
        if (synthParams.filePath.isNotEmpty())
        {
            this->filePath->setAlpha(1.f);
            this->filePath->setInterceptsMouseClicks(true, true);
            this->filePath->setText(synthParams.filePath);
        }
        else
        {
            this->filePath->setAlpha(0.75f);
            this->filePath->setInterceptsMouseClicks(false, false);
            this->filePath->setText({}); // todo placeholder?
        }
    }

    void resized() override
    {
        const auto getRowArea = [this](float proportionOfHeight, int height, int padding = 30)
        {
            const auto area = this->getLocalBounds().reduced(padding);
            const auto y = area.proportionOfHeight(proportionOfHeight);
            return area.withHeight(height).translated(0, y - height / 2);
        };

        constexpr auto rowHeight = 32;
        constexpr auto iconWidth = 30;
        constexpr auto iconMarginX = 8;
        constexpr auto paddingX = 6;

        auto browseFileArea = getRowArea(0.15f, rowHeight);
        this->browseButton->setBounds(browseFileArea.removeFromRight(iconWidth).reduced(paddingX, 2).translated(iconMarginX, 0));
        this->filePath->setBounds(browseFileArea.reduced(paddingX, 0));

        auto selectProgramArea = getRowArea(0.7f, rowHeight).reduced(iconWidth + paddingX, 0);
        this->leftArrow->setBounds(selectProgramArea.removeFromLeft(iconWidth).reduced(paddingX, 2).translated(-iconMarginX, 0));
        this->rightArrow->setBounds(selectProgramArea.removeFromRight(iconWidth).reduced(paddingX, 2).translated(iconMarginX, 0));
        this->programNameLabel->setBounds(selectProgramArea.reduced(paddingX, 0));
        this->programIndexIndicator->setBounds(selectProgramArea.translated(0, rowHeight));
    }

    void paint(Graphics &g) override
    {
        const auto &theme = HelioTheme::getCurrentTheme();
        g.setFillType({ theme.getPageBackgroundA(), {} });
        g.fillRect(this->getLocalBounds());
    }

    void handleCommandMessage(int commandId) override
    {
        const auto synthParams = this->audioPlugin->getSynthParameters();

        if (commandId == CommandIDs::Browse)
        {
            this->fileChooser = make<FileChooser>(TRANS(I18n::Dialog::documentLoad),
                this->lastUsedDirectory, ("*.sf2;*.sbk"), true);

            DocumentHelpers::showFileChooser(this->fileChooser,
                Globals::UI::FileChooser::forFileToOpen,
                [this](URL &url)
                {
                    if (!url.isLocalFile() || !url.getLocalFile().exists())
                    {
                        return; // or reset?
                    }

                    const auto file = url.getLocalFile();
                    this->lastUsedDirectory = file.getParentDirectory().getFullPathName();

                    const auto newParams = this->audioPlugin->getSynthParameters()
                        .withSoundFontFile(file.getFullPathName());

                    this->audioPlugin->applySynthParameters(newParams);
                    this->syncDataWithAudioPlugin();
                });
        }
        else if (commandId == CommandIDs::SelectNextPreset)
        {
            const auto nextPreset = jmin(synthParams.programIndex + 1, this->audioPlugin->getNumPrograms() - 1);
            const auto newParams = this->audioPlugin->getSynthParameters().withProgramIndex(nextPreset);
            this->audioPlugin->applySynthParameters(newParams);
            this->syncDataWithAudioPlugin();
        }
        else if (commandId == CommandIDs::SelectPreviousPreset)
        {
            const auto prevPreset = jmax(synthParams.programIndex - 1, 0);
            const auto newParams = this->audioPlugin->getSynthParameters().withProgramIndex(prevPreset);
            this->audioPlugin->applySynthParameters(newParams);
            this->syncDataWithAudioPlugin();
        }
    }

private:

    WeakReference<SoundFontSynthAudioPlugin> audioPlugin;

    // soundfont file picker
    UniquePointer<TextEditor> filePath;
    UniquePointer<IconButton> browseButton;

    UniquePointer<FileChooser> fileChooser;
    String lastUsedDirectory = File::getCurrentWorkingDirectory().getFullPathName();

    // preset selection
    UniquePointer<IconButton> leftArrow;
    UniquePointer<IconButton> rightArrow;
    UniquePointer<Label> programNameLabel;
    UniquePointer<ModeIndicatorComponent> programIndexIndicator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFontSynthEditor)
};

//===----------------------------------------------------------------------===//
// SoundFontSynthAudioPlugin
//===----------------------------------------------------------------------===//

SoundFontSynthAudioPlugin::SoundFontSynthAudioPlugin()
{
    this->setPlayConfigDetails(0, 2, this->getSampleRate(), this->getBlockSize());
}

void SoundFontSynthAudioPlugin::fillInPluginDescription(PluginDescription &description) const
{
    description.name = this->getName();
    description.descriptiveName = description.name;
    description.uniqueId = description.name.hashCode();
    description.category = "Synth";
    description.pluginFormatName = BuiltInSynthsPluginFormat::formatName;
    description.fileOrIdentifier = BuiltInSynthsPluginFormat::formatIdentifier;
    description.manufacturerName = "Helio Workstation";
    description.version = "1.0";
    description.isInstrument = true;
    description.numInputChannels = this->getTotalNumInputChannels();
    description.numOutputChannels = this->getTotalNumOutputChannels();
}

const String SoundFontSynthAudioPlugin::getName() const
{
    return SoundFontSynthAudioPlugin::instrumentName;
}

void SoundFontSynthAudioPlugin::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
    buffer.clear(0, buffer.getNumSamples());
    this->synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void SoundFontSynthAudioPlugin::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    this->synth.setCurrentPlaybackSampleRate(sampleRate);
}

void SoundFontSynthAudioPlugin::reset()
{
    this->synth.allNotesOff(0, true);
}

void SoundFontSynthAudioPlugin::releaseResources() {}
double SoundFontSynthAudioPlugin::getTailLengthSeconds() const { return 3.0; } // hopefully enough?

bool SoundFontSynthAudioPlugin::acceptsMidi() const { return true; }
bool SoundFontSynthAudioPlugin::producesMidi() const { return false; }

bool SoundFontSynthAudioPlugin::hasEditor() const { return true; }
AudioProcessorEditor *SoundFontSynthAudioPlugin::createEditor()
{
    return new SoundFontSynthEditor(this);
}

//===----------------------------------------------------------------------===//
// Presets
//===----------------------------------------------------------------------===//

int SoundFontSynthAudioPlugin::getNumPrograms()
{
    return this->synth.getNumPrograms();
}

int SoundFontSynthAudioPlugin::getCurrentProgram()
{
    return this->synth.getCurrentProgram();
}

void SoundFontSynthAudioPlugin::setCurrentProgram(int index) 
{
    this->synth.setCurrentProgram(index);
}

const String SoundFontSynthAudioPlugin::getProgramName(int index)
{
    return this->synth.getProgramName(index);
}

const String SoundFontSynthAudioPlugin::getCurrentProgramName()
{
    return this->getProgramName(this->getCurrentProgram());
}

void SoundFontSynthAudioPlugin::changeProgramName(int index, const String &newName)
{
    return this->synth.changeProgramName(index, newName);
}

//===----------------------------------------------------------------------===//
// Parameters
//===----------------------------------------------------------------------===//

void SoundFontSynthAudioPlugin::getStateInformation(MemoryBlock &destData)
{
    const auto state = this->synthParameters.serialize();

    String stateAsString;
    if (this->serializer.saveToString(stateAsString, state).ok())
    {
        MemoryOutputStream outStream(destData, false);
        outStream.writeString(stateAsString);
    }
}

void SoundFontSynthAudioPlugin::setStateInformation(const void *data, int sizeInBytes)
{
    MemoryInputStream inStream(data, sizeInBytes, false);
    const auto stateAsString = inStream.readString();
    const auto state = this->serializer.loadFromString(stateAsString);

    SoundFontSynth::Parameters newParameters;
    newParameters.deserialize(state);

    this->applySynthParameters(newParameters);
}

void SoundFontSynthAudioPlugin::applySynthParameters(const SoundFontSynth::Parameters &newParameters)
{
    if (this->synthParameters.filePath != newParameters.filePath)
    {
        this->synth.initSynth(newParameters);
    }
    else if (this->synthParameters.programIndex != newParameters.programIndex)
    {
        // just the program # changed
        jassert(newParameters.programIndex < this->getNumPrograms());
        this->setCurrentProgram(newParameters.programIndex);
    }

    this->synthParameters = newParameters;
}

const SoundFontSynth::Parameters &SoundFontSynthAudioPlugin::getSynthParameters() const noexcept
{
    return this->synthParameters;
}
