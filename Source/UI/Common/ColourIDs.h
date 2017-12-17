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
    // JUCE internal colour id's all look like 0x1xxxxxx,
    // so we start from 0x2000000 to make sure id's do not overlap:

    namespace Common
    {
        enum
        {
            resizerLineColourId             = 0x2000000,
            resizerShadowColourId           = 0x2000001
        };
    }

    namespace Roll
    {
        enum
        {
            blackKeyColourId                = 0x2000100,
            blackKeyBrightColourId          = 0x2000101,
            whiteKeyColourId                = 0x2000102,
            whiteKeyBrightColourId          = 0x2000103,
            rowLineColourId                 = 0x2000104,
            barLineColourId                 = 0x2000105,
            barLineBevelColourId            = 0x2000106,
            beatLineColourId                = 0x2000107,
            snapLineColourId                = 0x2000108,
            headerColourId                  = 0x2000109,
            headerSnapsColourId             = 0x2000110,
            playheadColourId                = 0x2000111,
            playheadShadeColourId           = 0x2000112
        };
    }

    namespace Callout
    {
        enum
        {
            blurColourId                    = 0x2000200,
            fillColourId                    = 0x2000201,
            frameColourId                   = 0x2000202
        };
    }
    
    namespace HelperRectangle
    {
        enum
        {
            fillColourId                    = 0x2000300,
            outlineColourId                 = 0x2000301,
        };
    }

    namespace Lasso
    {
        enum
        {
            lassoFillColourId               = 0x2000400,
            lassoOutlineColourId            = 0x2000401,
        };
    }

    namespace Icons
    {
        enum
        {
            iconColourId                    = 0x2000500,
            iconShadowColourId              = 0x2000501
        };
    }

    namespace Instrument
    {
        enum
        {
            midiInColourId                  = 0x2000600,
            midiOutColourId                 = 0x2000601,
            audioInColourId                 = 0x2000602,
            audioOutColourId                = 0x2000603
        };
    }

    namespace Panel
    {
        enum
        {
            panelFillColourId               = 0x2000700,
            panelBorderColourId             = 0x2000701
        };
    }

    namespace BackgroundA
    {
        enum
        {
            panelFillStartId                = 0x2000800,
            panelFillEndId                  = 0x2000801
        };
    }

    namespace BackgroundB
    {
        enum
        {
            panelFillId                     = 0x2000900
        };
    }

    namespace BackgroundC
    {
        enum
        {
            panelFillId                     = 0x2001000
        };
    }

    namespace SizeSwitcher
    {
        enum
        {
            borderColourId                  = 0x2001100
        };
    }

    namespace TrackScroller
    {
        enum
        {
            borderDarkLineColourId          = 0x2001200,
            borderLightLineColourId         = 0x2001201
        };
    }
} // namespace ColourIDs
