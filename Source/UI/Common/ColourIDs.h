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

namespace ColourIDs
{
    Array<Colour> getColoursList();

    // JUCE internal colour id's all look like 0x1xxxxxx;
    // so we start from 0x2000000 to make sure id's do not overlap.

    // The colour are generated in HelioTheme::initColours,
    // based on current colour scheme containing a number of common colours.

    // Tech debt warning: lots of colours out there in the app are hardcoded in components,
    // hopefully in future they all will be set in HelioTheme and accessed via these ids.

    namespace Common
    {
        static const int borderLineLight        = 0x2000000;
        static const int borderLineDark         = 0x2000001;
    }

    namespace Roll
    {
        static const int blackKey               = 0x2000100;
        static const int blackKeyAlt            = 0x2000101;
        static const int whiteKey               = 0x2000102;
        static const int whiteKeyAlt            = 0x2000103;

        static const int rowLine                = 0x2000104;
        static const int barLine                = 0x2000105;
        static const int barLineBevel           = 0x2000106;
        static const int beatLine               = 0x2000107;
        static const int snapLine               = 0x2000108;

        static const int headerFill             = 0x2000109;
        static const int headerBorder           = 0x2000110;
        static const int headerSnaps            = 0x2000111;
        static const int headerRecording        = 0x2000112;

        static const int playheadPlayback       = 0x2000113;
        static const int playheadRecording      = 0x2000114;
        static const int playheadShade          = 0x2000115;

        static const int trackHeaderFill        = 0x2000120;
        static const int trackHeaderBorder      = 0x2000121;

        static const int noteFill               = 0x2000130;
        static const int noteNameFill           = 0x2000131;
        static const int noteNameBorder         = 0x2000132;
        static const int noteNameShadow         = 0x2000133;

        static const int draggingGuide          = 0x2000140;
        static const int draggingGuideShadow    = 0x2000141;
    }

    namespace TransportControl
    {
        static const int recordInactive         = 0x2000150;
        static const int recordHighlight        = 0x2000151;
        static const int recordActive           = 0x2000152;

        static const int playInactive           = 0x2000160;
        static const int playHighlight          = 0x2000161;
        static const int playActive             = 0x2000162;
    }

    namespace Callout
    {
        static const int fill                   = 0x2000200;
        static const int frame                  = 0x2000201;
    }

    namespace Icons
    {
        static const int fill                   = 0x2000500;
        static const int shadow                 = 0x2000501;
    }

    namespace Instrument
    {
        static const int midiIn                 = 0x2000600;
        static const int midiOut                = 0x2000601;
        static const int audioIn                = 0x2000602;
        static const int audioOut               = 0x2000603;
        static const int midiConnector          = 0x2000604;
        static const int audioConnector         = 0x2000605;
        static const int pinShadow              = 0x2000606;
        static const int connectorShadow        = 0x2000607;
    }

    namespace VersionControl
    {
        static const int connector              = 0x2000610;
        static const int outline                = 0x2000611;
        static const int highlight              = 0x2000612;
    }

    namespace Panel
    {
        static const int border                 = 0x2000700;
    }

    namespace Backgrounds
    {
        static const int pageFillA              = 0x2000800;
        static const int pageFillB              = 0x2000801;
        static const int sidebarFill            = 0x2000802;
        static const int headlineFill           = 0x2000803;
        static const int dialogFill             = 0x2000804;
    }

    namespace Tooltip
    {
        static const int messageFill            = 0x2000900;
        static const int okIconFill             = 0x2000901;
        static const int okIconForeground       = 0x2000902;
        static const int failIconFill           = 0x2000903;
        static const int failIconForeground     = 0x2000904;
    }

    namespace TrackScroller
    {
        static const int borderLineDark         = 0x2001200;
        static const int borderLineLight        = 0x2001201;
        static const int screenRangeFill        = 0x2001202;
        static const int scrollerFill           = 0x2001203;
    }

    namespace SelectionComponent
    {
        static const int fill                   = 0x2001300;
        static const int outline                = 0x2001301;
    }

    namespace RollHeader
    {
        static const int selection              = 0x2001310;
        static const int soundProbe             = 0x2001311;
        static const int timeDistance           = 0x2001312;
    }

    namespace Logo
    {
        static const int fill                   = 0x2001500;
    }

    namespace ColourButton
    {
        static const int outline                = 0x2001600;
    }

    namespace AudioMonitor
    {
        static const int foreground             = 0x2001700;
    }

    namespace RenderProgressBar
    {
        static const int fill                   = 0x2001800;
        static const int outline                = 0x2001801;
        static const int progress               = 0x2001802;
        static const int waveform               = 0x2001803;
    }

    namespace TapTempoControl
    {
        static const int fill                   = 0x2001900;
        static const int fillHighlighted        = 0x2001901;
        static const int outline                = 0x2001902;
    }
} // namespace ColourIDs
