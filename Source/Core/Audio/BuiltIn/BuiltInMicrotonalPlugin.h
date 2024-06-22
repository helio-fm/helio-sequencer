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

struct BuiltInMicrotonalPlugin : public AudioPluginInstance
{
    virtual ~BuiltInMicrotonalPlugin() = default;

    // what we want here is to make all built-in temperaments
    // work out of the box with some built-in instruments, so that
    // all features are easily previewed even before the user
    // sets up any external instruments and keyboard mappings;
    // for that we need to let the instrument know which temperament
    // the project is currently in; also all built-in instruments will
    // assume equal divisions per octave, although built-in temperaments
    // don't say anything about tuning; maybe someday I'll come up with
    // a better approach, or just get rid of this hack;
    virtual void setTemperament(Temperament::Ptr temperament) = 0;
};
