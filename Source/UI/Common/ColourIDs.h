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

namespace ColourIDs
{
    // JUCE internal colour id's all look like 0x1xxxxxx;
    // so we start from 0x2000000 to make sure id's do not overlap.

    // The colour are generated in HelioTheme::initColours,
    // based on current colour scheme containing a number of common colours.

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
        static const int headerSnaps            = 0x2000110;
        static const int playhead               = 0x2000111;
        static const int playheadShade          = 0x2000112;
        static const int trackHeaderFill        = 0x2000113;
        static const int trackHeaderBorder      = 0x2000114;
        static const int noteFill               = 0x2000115;
    }

    namespace Callout
    {
        static const int fill                   = 0x2000200;
        static const int frame                  = 0x2000201;
    }
    
    namespace HelperRectangle
    {
        static const int fill                   = 0x2000300;
        static const int outline                = 0x2000301;
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
        static const int shadowPin              = 0x2000606;
        static const int shadowConnector        = 0x2000607;
    }

    namespace VersionControl
    {
        static const int connector              = 0x2000610;
        static const int outline                = 0x2000611;
        static const int highlight              = 0x2000612;
    }

    namespace Panel
    {
        static const int fill                   = 0x2000700;
        static const int border                 = 0x2000701;
    }

    namespace BackgroundA
    {
        static const int fill                   = 0x2000800;
    }

    namespace BackgroundB
    {
        static const int fill                   = 0x2000900;
    }

    namespace BackgroundC
    {
        static const int fill                   = 0x2001000;
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

    namespace ScriptEditor
    {
        static const int error                  = 0x2001400;
        static const int comment                = 0x2001401;
        static const int keyword                = 0x2001402;
        static const int identifier             = 0x2001403;
        static const int operatorToken          = 0x2001404;
        static const int integerType            = 0x2001405;
        static const int floatType              = 0x2001406;
        static const int stringType             = 0x2001407;
        static const int bracket                = 0x2001408;
        static const int punctuation            = 0x2001409;
        static const int builtInClass           = 0x200140a;
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
} // namespace ColourIDs
