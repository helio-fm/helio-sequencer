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

    namespace Common
    {
        static const int borderLineLight        = 0x2000000;
        static const int borderLineDark         = 0x2000001;

        static const int separatorLineLight     = 0x2000002;
        static const int separatorLineDark      = 0x2000003;
    }

    namespace Roll
    {
        static const int blackKey               = 0x2000100;
        static const int whiteKey               = 0x2000101;
        static const int rootKey                = 0x2000102;

        static const int rowLine                = 0x2000104;
        static const int barLine                = 0x2000105;
        static const int barLineBevel           = 0x2000106;
        static const int beatLine               = 0x2000107;
        static const int snapLine               = 0x2000108;

        static const int headerFill             = 0x2000109;
        static const int headerBorder           = 0x2000110;
        static const int headerSnaps            = 0x2000111;
        static const int headerRecording        = 0x2000112;
        static const int headerReprise          = 0x2000113;

        static const int playheadPlayback       = 0x2000114;
        static const int playheadRecording      = 0x2000115;
        static const int playheadShade          = 0x2000116;

        static const int patternRowFill         = 0x2000120;
        static const int trackHeaderFill        = 0x2000121;
        static const int trackHeaderShadow      = 0x2000122;
        static const int trackHeaderBorderLight = 0x2000123;
        static const int trackHeaderBorderDark  = 0x2000124;

        static const int noteFill               = 0x2000130;
        static const int noteNameFill           = 0x2000131;
        static const int noteNameBorder         = 0x2000132;
        static const int noteNameShadow         = 0x2000133;
        static const int clipFill               = 0x2000134;
        static const int clipForeground         = 0x2000135;

        static const int noteCutMark            = 0x2000140;
        static const int noteCutMarkOutline     = 0x2000141;
        static const int cuttingGuide           = 0x2000142;
        static const int cuttingGuideOutline    = 0x2000143;
        static const int draggingGuide          = 0x2000144;
        static const int draggingGuideShadow    = 0x2000145;
        static const int resizingGuideFill      = 0x2000146;
        static const int resizingGuideOutline   = 0x2000147;
        static const int resizingGuideShadow    = 0x2000148;
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
    
    namespace Shadows
    {
        static const int fillLight              = 0x2000300;
        static const int borderLight            = 0x2000301;
        static const int fillNormal             = 0x2000302;
        static const int borderNormal           = 0x2000303;
        static const int fillHard               = 0x2000304;
        static const int borderHard             = 0x2000305;
    }

    namespace Icons
    {
        static const int fill                   = 0x2000500;
        static const int shadow                 = 0x2000501;
    }

    namespace Instrument
    {
        static const int fill                   = 0x2000600;
        static const int outline                = 0x2000601;
        static const int text                   = 0x2000602;
        static const int midiNode               = 0x2000603;
        static const int audioNode              = 0x2000604;
        static const int midiConnector          = 0x2000605;
        static const int audioConnector         = 0x2000606;
        static const int pinShadow              = 0x2000607;
        static const int connectorShadow        = 0x2000608;
    }

    namespace VersionControl
    {
        static const int revisionConnector      = 0x2000610;
        static const int revisionOutline        = 0x2000611;
        static const int revisionHighlight      = 0x2000612;
        static const int revisionFill           = 0x2000613;
        static const int stageSelectionFill     = 0x2000614;
    }

    namespace Panel
    {
        static const int border                 = 0x2000700;
        static const int pageFillA              = 0x2000701;
        static const int pageFillB              = 0x2000702;
        static const int sidebarFill            = 0x2000703;
        static const int bottomPanelFill        = 0x2000704;
    }

    namespace Arrow
    {
        static const int lineStart              = 0x2000750;
        static const int lineEnd                = 0x2000751;
        static const int shadowStart            = 0x2000752;
        static const int shadowEnd              = 0x2000753;
    }

    namespace Breadcrumbs
    {
        static const int fill                   = 0x2000800;
        static const int selectionMarker        = 0x2000801;
    }

    namespace Dialog
    {
        static const int fill                   = 0x2000810;
        static const int header                 = 0x2000811;
    }

    namespace Menu
    {
        static const int fill                   = 0x2000820;
        static const int header                 = 0x2000821;
        static const int selectionMarker        = 0x2000822;
    }

    namespace Tooltip
    {
        static const int messageFill            = 0x2000900;
        static const int messageBorder          = 0x2000901;
        static const int messageText            = 0x2000902;
        static const int okIconFill             = 0x2000903;
        static const int okIconForeground       = 0x2000904;
        static const int failIconFill           = 0x2000905;
        static const int failIconForeground     = 0x2000906;
    }

    namespace TrackScroller
    {
        static const int borderLineDark         = 0x2001200;
        static const int borderLineLight        = 0x2001201;
        static const int viewBeatRangeFill      = 0x2001202;
        static const int viewBeatRangeBorder    = 0x2001203;
        static const int viewRangeFill          = 0x2001204;
        static const int projectOutRangeFill    = 0x2001205;
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
        static const int highlight              = 0x2001601;
        static const int pressed                = 0x2001602;
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
