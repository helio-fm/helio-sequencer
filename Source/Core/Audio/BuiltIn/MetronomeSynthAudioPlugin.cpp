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
#include "MetronomeSynthAudioPlugin.h"
#include "BuiltInSynthsPluginFormat.h"
#include "DocumentHelpers.h"
#include "MetronomeEditor.h"
#include "Workspace.h"
#include "SerializationKeys.h"
#include "PlayButton.h"
#include "IconButton.h"
#include "HelioTheme.h"

const String MetronomeSynthAudioPlugin::instrumentId = "<metronome>";
const String MetronomeSynthAudioPlugin::instrumentName = "Metronome";

//===----------------------------------------------------------------------===//
// Metronome synth's UI allowing to pick custom samples
//===----------------------------------------------------------------------===//

class MetronomeSynthEditor final : public AudioProcessorEditor
{
public:

    explicit MetronomeSynthEditor(WeakReference<MetronomeSynthAudioPlugin> metronomePlugin) :
        AudioProcessorEditor(metronomePlugin),
        metronomePlugin(metronomePlugin)
    {
        for (int i = 0; i < this->allSyllables.size(); ++i)
        {
            const auto syllable = this->allSyllables[i];

            auto syllableIcon = make<MetronomeEditor::SyllableButton>(syllable, false);
            syllableIcon->setMouseCursor(MouseCursor::PointingHandCursor);
            syllableIcon->onTap = [syllable]()
            {
                // a hack to preview the sound:
                // there's no access to Transport class from here,
                // so let's just send a midi message to the metronome directly
                auto *metronome = App::Workspace().getAudioCore().getMetronomeInstrument();

                const auto key = MetronomeSynth::getKeyForSyllable(syllable);
                MidiMessage noteOn(MidiMessage::noteOn(1, key, 1.f));
                noteOn.setTimeStamp(Time::getMillisecondCounterHiRes() * 0.001);
                metronome->getProcessorPlayer().getMidiMessageCollector().addMessageToQueue(noteOn);
            };
            this->addAndMakeVisible(syllableIcon.get());
            this->syllableIcons.add(syllableIcon.release());

            auto samplePathEditor = make<TextEditor>();
            samplePathEditor->setReadOnly(true);
            samplePathEditor->setFont(Globals::UI::Fonts::M);
            samplePathEditor->setJustification(Justification::centredLeft);
            this->addAndMakeVisible(samplePathEditor.get());
            this->samplePaths.add(samplePathEditor.release());

            auto browseButton = make<IconButton>(Icons::browse, CommandIDs::OpenMetronomeSample + i);
            browseButton->setMouseCursor(MouseCursor::PointingHandCursor);
            this->addAndMakeVisible(browseButton.get());
            this->sampleBrowseButtons.add(browseButton.release());

            auto clearButton = make<IconButton>(Icons::close, CommandIDs::ResetMetronomeSample + i);
            clearButton->setMouseCursor(MouseCursor::PointingHandCursor);
            this->addAndMakeVisible(clearButton.get());
            this->sampleClearButtons.add(clearButton.release());
        }

        this->syncDataWithAudioPlugin();
        this->setSize(640, 240);
    }

    void syncDataWithAudioPlugin()
    {
        const auto synthParams = this->metronomePlugin->getCustomSamples();
        for (int i = 0; i < this->allSyllables.size(); ++i)
        {
            const auto customSample = synthParams.customSamples.find(this->allSyllables[i]);
            if (customSample != synthParams.customSamples.end())
            {
                this->samplePaths[i]->setAlpha(1.f);
                this->samplePaths[i]->setInterceptsMouseClicks(true, true);
                this->samplePaths[i]->setText(customSample->second);
            }
            else
            {
                this->samplePaths[i]->setAlpha(0.75f);
                this->samplePaths[i]->setInterceptsMouseClicks(false, false);
                this->samplePaths[i]->setText(TRANS(I18n::Instruments::metronomeBuiltInSoundPlaceholder));
            }
        }
    }

    void resized() override
    {
        const auto getRowArea = [this](float proportionOfHeight, int height, int padding = 10)
        {
            const auto area = this->getLocalBounds().reduced(padding);
            const auto y = area.proportionOfHeight(proportionOfHeight);
            return area.withHeight(height).translated(0, y - height / 2);
        };

        const auto numControls = this->allSyllables.size();

        for (int i = 0; i < numControls; ++i)
        {
            constexpr auto rowHeight = 32;
            constexpr auto iconWidth = 30;
            constexpr auto buttonSize = 20;
            constexpr auto iconMarginX = 8;
            constexpr auto paddingX = 6;

            auto rowArea = getRowArea(float(i + 1) / float(numControls + 1), rowHeight);

            this->syllableIcons[i]->setBounds(rowArea.removeFromLeft(iconWidth).reduced(paddingX, 2).translated(iconMarginX, 0));
            this->sampleClearButtons[i]->setBounds(rowArea.removeFromRight(buttonSize * 2).withSizeKeepingCentre(buttonSize, buttonSize));
            this->sampleBrowseButtons[i]->setBounds(rowArea.removeFromRight(buttonSize * 2).withSizeKeepingCentre(buttonSize, buttonSize));
            this->samplePaths[i]->setBounds(rowArea.reduced(paddingX + iconMarginX, 0).translated(iconMarginX, 0));
        }
    }

    void paint(Graphics &g) override
    {
        const auto &theme = HelioTheme::getCurrentTheme();
        g.setFillType({ theme.getPageBackgroundA(), {} });
        g.fillRect(this->getLocalBounds());
    }

    void handleCommandMessage(int commandId) override
    {
        if (commandId >= CommandIDs::OpenMetronomeSample &&
            commandId < CommandIDs::OpenMetronomeSample + this->allSyllables.size())
        {
            const auto rowNumber = commandId - CommandIDs::OpenMetronomeSample;
            const auto syllable = this->allSyllables[rowNumber];

            this->fileChooser = make<FileChooser>(TRANS(I18n::Dialog::documentLoad),
                this->lastUsedDirectory, ("*.wav;*.flac"), true);

            DocumentHelpers::showFileChooser(this->fileChooser,
                Globals::UI::FileChooser::forFileToOpen,
                [this, syllable, rowNumber](URL &url)
                {
                    if (!url.isLocalFile() || !url.getLocalFile().exists())
                    {
                        return; // or clear?
                    }

                    const auto file = url.getLocalFile();
                    this->lastUsedDirectory = file.getParentDirectory().getFullPathName();
                    this->metronomePlugin->applyCustomSample(syllable, file.getFullPathName());
                    this->syncDataWithAudioPlugin();
                    App::Workspace().autosave();
                });
        }
        else if (commandId >= CommandIDs::ResetMetronomeSample &&
                 commandId < CommandIDs::ResetMetronomeSample + this->allSyllables.size())
        {
            const auto rowNumber = commandId - CommandIDs::ResetMetronomeSample;
            const auto syllable = this->allSyllables[rowNumber];
            this->metronomePlugin->applyCustomSample(syllable, {});
            this->syncDataWithAudioPlugin();
            App::Workspace().autosave();
        }
    }

private:

    const Array<MetronomeScheme::Syllable> allSyllables = MetronomeScheme::getAllOrdered();

    WeakReference<MetronomeSynthAudioPlugin> metronomePlugin;

    OwnedArray<MetronomeEditor::SyllableButton> syllableIcons;
    OwnedArray<TextEditor> samplePaths;
    OwnedArray<IconButton> sampleBrowseButtons;
    OwnedArray<IconButton> sampleClearButtons;

    UniquePointer<FileChooser> fileChooser;
    String lastUsedDirectory = File::getCurrentWorkingDirectory().getFullPathName();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetronomeSynthEditor)
};

//===----------------------------------------------------------------------===//
// MetronomeSynthAudioPlugin
//===----------------------------------------------------------------------===//

MetronomeSynthAudioPlugin::MetronomeSynthAudioPlugin()
{
    this->setPlayConfigDetails(0, 2, this->getSampleRate(), this->getBlockSize());
}

void MetronomeSynthAudioPlugin::fillInPluginDescription(PluginDescription &description) const
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

const String MetronomeSynthAudioPlugin::getName() const
{
    return MetronomeSynthAudioPlugin::instrumentName;
}

void MetronomeSynthAudioPlugin::processBlock(AudioSampleBuffer &buffer, MidiBuffer &midiMessages)
{
    if (this->synth.getNumSounds() == 0 && midiMessages.getNumEvents() > 0)
    {
        // Initialization takes time, i.e. slows app loading way down,
        // and consumes RAM, although user might never use the built-in metronome,
        // so let's do lazy initialization on the first use, here in processBlock:
        this->synth.initVoices();
        this->synth.initSampler(this->synthParameters);
    }

    buffer.clear(0, buffer.getNumSamples());
    this->synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void MetronomeSynthAudioPlugin::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
    this->synth.setCurrentPlaybackSampleRate(sampleRate);
}

void MetronomeSynthAudioPlugin::reset()
{
    this->synth.allNotesOff(0, true);
}

void MetronomeSynthAudioPlugin::releaseResources() {}
double MetronomeSynthAudioPlugin::getTailLengthSeconds() const { return 1.0; }

bool MetronomeSynthAudioPlugin::acceptsMidi() const { return true; }
bool MetronomeSynthAudioPlugin::producesMidi() const { return false; }

bool MetronomeSynthAudioPlugin::hasEditor() const { return true; }
AudioProcessorEditor *MetronomeSynthAudioPlugin::createEditor()
{
    return new MetronomeSynthEditor(this);
}

int MetronomeSynthAudioPlugin::getNumPrograms() { return 0; }
int MetronomeSynthAudioPlugin::getCurrentProgram() { return 0; }
void MetronomeSynthAudioPlugin::setCurrentProgram(int index) {}
const String MetronomeSynthAudioPlugin::getProgramName(int index) { return ""; }
void MetronomeSynthAudioPlugin::changeProgramName(int index, const String &newName) {}

//===----------------------------------------------------------------------===//
// Parameters
//===----------------------------------------------------------------------===//

void MetronomeSynthAudioPlugin::getStateInformation(MemoryBlock &destData)
{
    const auto state = this->synthParameters.serialize();

    String stateAsString;
    if (this->serializer.saveToString(stateAsString, state).ok())
    {
        MemoryOutputStream outStream(destData, false);
        outStream.writeString(stateAsString);
    }
}

void MetronomeSynthAudioPlugin::setStateInformation(const void *data, int sizeInBytes)
{
    MemoryInputStream inStream(data, sizeInBytes, false);
    const auto stateAsString = inStream.readString();
    const auto state = this->serializer.loadFromString(stateAsString);
    this->synthParameters.deserialize(state);
}

void MetronomeSynthAudioPlugin::applyCustomSample(MetronomeScheme::Syllable syllable, const String &samplePath)
{
    if (samplePath.isEmpty())
    {
        this->synthParameters.customSamples.erase(syllable);
    }
    else
    {
        this->synthParameters.customSamples[syllable] = samplePath;
    }

    this->synth.initVoices();
    this->synth.initSampler(this->synthParameters);
}

const MetronomeSynth::SamplerParameters &MetronomeSynthAudioPlugin::getCustomSamples() const noexcept
{
    return this->synthParameters;
}
