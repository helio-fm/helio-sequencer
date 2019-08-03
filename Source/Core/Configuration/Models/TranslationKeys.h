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

namespace I18n
{
    namespace Colours
    {
        static const Identifier black = "colours::black";
        static const Identifier blue = "colours::blue";
        static const Identifier blueViolet = "colours::blueviolet";
        static const Identifier crimson = "colours::crimson";
        static const Identifier darkOrange = "colours::darkorange";
        static const Identifier darkViolet = "colours::darkviolet";
        static const Identifier deepPink = "colours::deeppink";
        static const Identifier gold = "colours::gold";
        static const Identifier greenYellow = "colours::greenyellow";
        static const Identifier lime = "colours::lime";
        static const Identifier none = "colours::none";
        static const Identifier orangeRed = "colours::orangered";
        static const Identifier red = "colours::red";
        static const Identifier royalBlue = "colours::royalblue";
        static const Identifier springGreen = "colours::springgreen";
        static const Identifier tomato = "colours::tomato";
        static const Identifier white = "colours::white";
    }

    namespace Common
    {
        static const Identifier conjunction = "common::and";
        static const Identifier yesterday = "common::yesterday";
        static const Identifier instrumentsInitialScan = "instruments::initialscan";
        static const Identifier networkError = "network error";
        static const Identifier updateProceed = "update::proceed";
    }

    namespace Defaults
    {
        static const Identifier newProjectFirstCommit = "defaults::newproject::firstcommit";
        static const Identifier newProjectName = "defaults::newproject::name";
        static const Identifier midiTrackName = "defaults::newtrack::name";
        static const Identifier tempoTrackName = "defaults::tempotrack::name";
    }

    namespace Dialog
    {
        static const Identifier addArpCaption = "dialog::addarp::caption";
        static const Identifier addArpProceed = "dialog::addarp::proceed";
        static const Identifier addTrackCaption = "dialog::addtrack::caption";
        static const Identifier addTrackProceed = "dialog::addtrack::proceed";
        static const Identifier annotationAddCaption = "dialog::annotation::add::caption";
        static const Identifier annotationAddProceed = "dialog::annotation::add::proceed";
        static const Identifier annotationEditApply = "dialog::annotation::edit::apply";
        static const Identifier annotationEditCaption = "dialog::annotation::edit::caption";
        static const Identifier annotationEditDelete = "dialog::annotation::edit::delete";
        static const Identifier annotationRenameCancel = "dialog::annotation::rename::cancel";
        static const Identifier annotationRenameCaption = "dialog::annotation::rename::caption";
        static const Identifier annotationRenameProceed = "dialog::annotation::rename::proceed";
        static const Identifier authGithub = "dialog::auth::github";
        static const Identifier cancel = "dialog::common::cancel";
        static const Identifier deleteProjectCaption = "dialog::deleteproject::caption";
        static const Identifier deleteProjectConfirmCaption = "dialog::deleteproject::confirm::caption";
        static const Identifier deleteProjectConfirmProceed = "dialog::deleteproject::confirm::proceed";
        static const Identifier deleteProjectProceed = "dialog::deleteproject::proceed";
        static const Identifier documentExport = "dialog::document::export";
        static const Identifier documentExportDone = "dialog::document::export::done";
        static const Identifier documentImport = "dialog::document::import";
        static const Identifier documentLoad = "dialog::document::load";
        static const Identifier documentSave = "dialog::document::save";
        static const Identifier instrumentRenameCaption = "dialog::instrument::rename::caption";
        static const Identifier instrumentRenameProceed = "dialog::instrument::rename::proceed";
        static const Identifier keySignatureAddCaption = "dialog::keysignature::add::caption";
        static const Identifier keySignatureAddProceed = "dialog::keysignature::add::proceed";
        static const Identifier keySignatureEditApply = "dialog::keysignature::edit::apply";
        static const Identifier keySignatureEditCaption = "dialog::keysignature::edit::caption";
        static const Identifier keySignatureEditDelete = "dialog::keysignature::edit::delete";
        static const Identifier openglCaption = "dialog::opengl::caption";
        static const Identifier openglProceed = "dialog::opengl::proceed";
        static const Identifier renameTrackCaption = "dialog::renametrack::caption";
        static const Identifier renameTrackProceed = "dialog::renametrack::proceed";
        static const Identifier renderAbort = "dialog::render::abort";
        static const Identifier renderCaption = "dialog::render::caption";
        static const Identifier renderClose = "dialog::render::close";
        static const Identifier renderProceed = "dialog::render::proceed";
        static const Identifier renderSelectFile = "dialog::render::selectfile";
        static const Identifier scanFolderCaption = "dialog::scanfolder::caption";
        static const Identifier timeSignatureAddCaption = "dialog::timesignature::add::caption";
        static const Identifier timeSignatureAddProceed = "dialog::timesignature::add::proceed";
        static const Identifier timeSignatureEditApply = "dialog::timesignature::edit::apply";
        static const Identifier timeSignatureEditCaption = "dialog::timesignature::edit::caption";
        static const Identifier timeSignatureEditDelete = "dialog::timesignature::edit::delete";
        static const Identifier vcsCheckoutProceed = "dialog::vcs::checkout::proceed";
        static const Identifier vcsCheckoutWarning = "dialog::vcs::checkout::warning";
        static const Identifier vcsCommitCaption = "dialog::vcs::commit::caption";
        static const Identifier vcsCommitProceed = "dialog::vcs::commit::proceed";
        static const Identifier vcsResetCaption = "dialog::vcs::reset::caption";
        static const Identifier vcsResetProceed = "dialog::vcs::reset::proceed";
        static const Identifier workspaceCreateProjectCaption = "dialog::workspace::createproject::caption";
    }

    namespace Menu
    {
        static const Identifier annotationAdd = "menu::annotation::add";
        static const Identifier annotationDelete = "menu::annotation::delete";
        static const Identifier annotationRename = "menu::annotation::rename";
        static const Identifier arpeggiatorsCreate = "menu::arpeggiators::create";
        static const Identifier back = "menu::back";
        static const Identifier cancel = "menu::cancel";
        static const Identifier groupByColour = "menu::groupby::colour";
        static const Identifier groupByInstrument = "menu::groupby::instrument";
        static const Identifier groupByName = "menu::groupby::name";
        static const Identifier groupByNone = "menu::groupby::none";
        static const Identifier instrumentAddEffect = "menu::instrument::addeffect";
        static const Identifier instrumentAdd = "menu::instrument::addinstrument";
        static const Identifier instrumentDelete = "menu::instrument::delete";
        static const Identifier instrumentRename = "menu::instrument::rename";
        static const Identifier instrumentSetColour = "menu::instrument::setcolour";
        static const Identifier instrumentSetIcon = "menu::instrument::seticon";
        static const Identifier instrumentShowEditor = "menu::instrument::showeditor";
        static const Identifier instrumentsReload = "menu::instruments::reload";
        static const Identifier instrumentsScanFolder = "menu::instruments::scanfolder";
        static const Identifier keySignatureAdd = "menu::keysignature::add";

        static const Identifier refactoringInverseDown = "menu::refactoring::inversedown";
        static const Identifier refactoringInverseUp = "menu::refactoring::inverseup";
        static const Identifier refactoringMelodicInversion = "menu::refactoring::inversion";
        static const Identifier refactoringRetrograde = "menu::refactoring::retrograde";
        static const Identifier refactoringCleanup = "menu::refactoring::cleanup";

        namespace Project
        {
            static const Identifier addAutomation = "menu::project::addautomation";
            static const Identifier addItems = "menu::project::additems";
            static const Identifier addTrack = "menu::project::addlayer";
            static const Identifier addTempo = "menu::project::addtempo";
            static const Identifier changeInstrument = "menu::project::change::instrument";
            static const Identifier deleteConfirm = "menu::project::delete";
            static const Identifier deleteCancelled = "menu::project::delete::cancelled";
            static const Identifier editorLinear = "menu::project::editor::linear";
            static const Identifier editorPattern = "menu::project::editor::pattern";
            static const Identifier editorVcs = "menu::project::editor::vcs";
            static const Identifier importMidi = "menu::project::import::midi";
            static const Identifier refactor = "menu::project::refactor";
            static const Identifier render = "menu::project::render";
            static const Identifier renderFlac = "menu::project::render::flac";
            static const Identifier renderMidi = "menu::project::render::midi";
            static const Identifier renderSavedTo = "menu::project::render::savedto";
            static const Identifier renderWav = "menu::project::render::wav";
            static const Identifier transposeDown = "menu::project::transpose::down";
            static const Identifier transposeUp = "menu::project::transpose::up";
            static const Identifier unload = "menu::project::unload";
        }

        namespace Selection
        {
            static const Identifier clipsCopy = "menu::selection::clips::copy";
            static const Identifier clipsCut = "menu::selection::clips::cut";
            static const Identifier clipsDelete = "menu::selection::clips::delete";
            static const Identifier clipsEdit = "menu::selection::clips::edit";
            static const Identifier clipsTransposeDown = "menu::selection::clips::transpose::down";
            static const Identifier clipsTransposeUp = "menu::selection::clips::transpose::up";
            static const Identifier notes = "menu::selection::notes";
            static const Identifier notesArpeggiate = "menu::selection::notes::arpeggiate";
            static const Identifier notesCopy = "menu::selection::notes::copy";
            static const Identifier notesCut = "menu::selection::notes::cut";
            static const Identifier notesDelete = "menu::selection::notes::delete";
            static const Identifier notesDivisions = "menu::selection::notes::divisions";
            static const Identifier notesRefactor = "menu::selection::notes::refactor";
            static const Identifier notesRescale = "menu::selection::notes::rescale";
            static const Identifier notesToTrack = "menu::selection::notes::totrack";
            static const Identifier pluginInit = "menu::selection::plugin::init";
            static const Identifier pluginPlug = "menu::selection::plugin::plug";
            static const Identifier pluginRemove = "menu::selection::plugin::remove";
            static const Identifier routeDisconnect = "menu::selection::route::disconnect";
            static const Identifier routeGetAudio = "menu::selection::route::getaudio";
            static const Identifier routeGetMidi = "menu::selection::route::getmidi";
            static const Identifier routeRemove = "menu::selection::route::remove";
            static const Identifier routeSendaudio = "menu::selection::route::sendaudio";
            static const Identifier routeSendmidi = "menu::selection::route::sendmidi";
            static const Identifier vcsCheckout = "menu::selection::vcs::checkout";
            static const Identifier vcsCommit = "menu::selection::vcs::commit";
            static const Identifier vcsHistory = "menu::selection::vcs::history";
            static const Identifier vcsPull = "menu::selection::vcs::pull";
            static const Identifier vcsPush = "menu::selection::vcs::push";
            static const Identifier vcsReset = "menu::selection::vcs::reset";
            static const Identifier vcsSelectall = "menu::selection::vcs::selectall";
            static const Identifier vcsSelectnone = "menu::selection::vcs::selectnone";
            static const Identifier vcsStage = "menu::selection::vcs::stage";
            static const Identifier vcsStash = "menu::selection::vcs::stash";
        }

        static const Identifier timeSignatureAdd = "menu::timesignature::add";
        static const Identifier timeSignatureChange = "menu::timesignature::change";
        static const Identifier timeSignatureDelete = "menu::timesignature::delete";
        static const Identifier trackChangeColour = "menu::track::change::colour";
        static const Identifier trackChangeInstrument = "menu::track::change::instrument";
        static const Identifier trackDelete = "menu::track::delete";
        static const Identifier trackRename = "menu::track::rename";
        static const Identifier trackSelectall = "menu::track::selectall";
        static const Identifier tuplet1 = "menu::tuplet::1";
        static const Identifier tuplet2 = "menu::tuplet::2";
        static const Identifier tuplet3 = "menu::tuplet::3";
        static const Identifier tuplet4 = "menu::tuplet::4";
        static const Identifier tuplet5 = "menu::tuplet::5";
        static const Identifier tuplet6 = "menu::tuplet::6";
        static const Identifier tuplet7 = "menu::tuplet::7";
        static const Identifier tuplet8 = "menu::tuplet::8";
        static const Identifier tuplet9 = "menu::tuplet::9";
        static const Identifier mute = "menu::mute";
        static const Identifier solo = "menu::solo";
        static const Identifier unmute = "menu::unmute";
        static const Identifier unsolo = "menu::unsolo";

        static const Identifier vcsChangesHide = "menu::vcs::changes::hide";
        static const Identifier vcsChangesShow = "menu::vcs::changes::show";
        static const Identifier vcsCommitAll = "menu::vcs::commitall";
        static const Identifier vcsResetAll = "menu::vcs::resetall";
        static const Identifier vcsStash = "menu::vcs::stash";
        static const Identifier vcsSyncAll = "menu::vcs::syncall";
        static const Identifier workspaceProjectCreate = "menu::workspace::project::create";
        static const Identifier workspaceProjectOpen = "menu::workspace::project::open";
    }

    namespace Page
    {
        static const Identifier orchestraCategory = "page::orchestra::category";
        static const Identifier orchestraFormat = "page::orchestra::format";
        static const Identifier orchestraInstruments = "page::orchestra::instruments";
        static const Identifier orchestraPlugins = "page::orchestra::plugins";
        static const Identifier orchestraVendorandname = "page::orchestra::vendorandname";
        static const Identifier projectAuthor = "page::project::author";
        static const Identifier projectDefaultAuthor = "page::project::default::author";
        static const Identifier projectDefaultLicense = "page::project::default::license";
        static const Identifier projectDefaultValueDesktop = "page::project::default::value::desktop";
        static const Identifier projectDefaultValueMobile = "page::project::default::value::mobile";
        static const Identifier projectDescription = "page::project::description";
        static const Identifier projectDuration = "page::project::duration";
        static const Identifier projectFilelocation = "page::project::filelocation";
        static const Identifier projectLicense = "page::project::license";
        static const Identifier projectStartdate = "page::project::startdate";
        static const Identifier projectStatsContent = "page::project::stats::content";
        static const Identifier projectStatsVcs = "page::project::stats::vcs";
        static const Identifier projectTitle = "page::project::title";
    }

    namespace Popup
    {
        static const Identifier cancelled = "popup::cancelled";
        static const Identifier chordFunction1 = "popup::chord::function::1";
        static const Identifier chordFunction2 = "popup::chord::function::2";
        static const Identifier chordFunction3 = "popup::chord::function::3";
        static const Identifier chordFunction4 = "popup::chord::function::4";
        static const Identifier chordFunction5 = "popup::chord::function::5";
        static const Identifier chordFunction6 = "popup::chord::function::6";
        static const Identifier chordFunction7 = "popup::chord::function::7";
        static const Identifier chordRootKey = "popup::chord::rootkey";
    }

    namespace Settings
    {
        static const Identifier audio = "settings::audio";
        static const Identifier audioBufferSize = "settings::audio::buffersize";
        static const Identifier audioDevice = "settings::audio::device";
        static const Identifier audioDriver = "settings::audio::driver";
        static const Identifier audioSampleRate = "settings::audio::samplerate";
        static const Identifier languageHelp = "settings::language::help";
        static const Identifier rendererCoreGraphics = "settings::renderer::coregraphics";
        static const Identifier rendererDefault = "settings::renderer::default";
        static const Identifier rendererDirect2d = "settings::renderer::direct2d";
        static const Identifier rendererNative = "settings::renderer::native";
        static const Identifier rendererOpengl = "settings::renderer::opengl";
        static const Identifier sync = "settings::sync";
        static const Identifier ui = "settings::ui";
        static const Identifier uiFont = "settings::ui::font";
    }

    namespace Tree
    {
        static const Identifier instruments = "tree::instruments";
        static const Identifier patterns = "tree::patterns";
        static const Identifier root = "tree::root";
        static const Identifier settings = "tree::settings";
        static const Identifier vcs = "tree::vcs";
    }

    namespace VCS
    {
        static const Identifier deltaTypeAdded = "vcs::delta::type::added";
        static const Identifier deltaTypeChanged = "vcs::delta::type::changed";
        static const Identifier deltaTypeRemoved = "vcs::delta::type::removed";
        static const Identifier historyCaption = "vcs::history::caption";
        static const Identifier stageCaption = "vcs::stage::caption";
        static const Identifier syncDone = "vcs::sync::done";
        static const Identifier syncUptodate = "vcs::sync::uptodate";
        static const Identifier warningCannotCommit = "vcs::warning::cannotcommit";
        static const Identifier warningCannotReset = "vcs::warning::cannotreset";
        static const Identifier warningCannotRevert = "vcs::warning::cannotrevert";
        static const Identifier projectMetadata = "vcs::items::projectinfo";
        static const Identifier projectTimeline = "vcs::items::timeline";
    }
} // namespace Translations
