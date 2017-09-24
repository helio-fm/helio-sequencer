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

class Icons
{
public:

    static const String empty;

    static const String menu;
    static const String project;
    static const String play;
    static const String pause;
    static const String instrumentSettings;
    static const String instrumentGraph;
    static const String vcs;
    static const String automation;
    static const String workspace;
    static const String layer;
    static const String group;
    static const String backward;
    static const String forward;
    static const String left;
    static const String right;
    static const String settings;
    static const String saxophone;
    static const String trash;
    static const String ellipsis;
    
    static const String undo;
    static const String redo;
    
    static const String close;
    static const String apply;
    static const String create;
    static const String open;
    static const String minus;
    static const String plus;
    static const String colour;

    static const String login;
    static const String fail;
    static const String success;
    static const String progressIndicator;

    static const String remote;
    static const String local;
    static const String commit;
    static const String reset;
    static const String push;
    static const String pull;

    static const String copy;
    static const String paste;
    static const String cut;

    static const String zoomIn;
    static const String zoomOut;

    static const String update;

    static const String up;
    static const String down;

    static const String previous;
    static const String next;

    static const String volumeUp;
    static const String volumeOff;

    static const String render;

    static const String cursorTool;
    static const String drawTool;
    static const String selectionTool;
    static const String zoomTool;
    static const String dragTool;
    
    static const String insertSpaceTool;
    static const String wipeScapeTool;
    static const String cropTool;
    
    static const String annotation;
    static const String arpeggiator;
    static const String switcher;

    static const String stretchLeft;
    static const String stretchRight;

    static const String toggleOn;
    static const String toggleOff;

    static const String roman1;
    static const String roman2;
    static const String roman3;
    static const String roman4;
    static const String roman5;
    static const String roman6;
    static const String roman7;

    enum ColourIds
    {
        iconColourId        = 0x99010000,
        iconShadowColourId  = 0x99011000
    };

    static void clearBuiltInImages();
    static void setupBuiltInImages();
    
    static void clearPrerenderedCache();
    
    static Image findByName(const String &name, int maxSize);
    static Image findByName(const String &name, int maxSize, LookAndFeel &lf);

    static Path getPathByName(const String &name);
    static ScopedPointer<Drawable> getDrawableByName(const String &name);

    static void drawImageRetinaAware(const Image &image, Graphics &g, int cx, int cy);
    
    
    
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Icons);

};
