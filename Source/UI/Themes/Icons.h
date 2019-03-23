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

struct Icons final
{
    using Id = uint32;

    static void initBuiltInImages();
    static void clearBuiltInImages();
    static void clearPrerenderedCache();

    static Image findByName(Icons::Id id, int maxSize);
    static Image renderForTheme(const LookAndFeel &lf, Icons::Id id, int maxSize);
    static void drawImageRetinaAware(const Image &image, Graphics &g, int cx, int cy);

    static Path getPathByName(Icons::Id id);
    static ScopedPointer<Drawable> getDrawableByName(Icons::Id id);

    enum Ids
    {
        empty,

        helio,
        project,
        trackGroup,
        pianoTrack,
        automationTrack,
        versionControl,
        settings,
        patterns,
        orchestraPit,
        instrument,
        instrumentNode,
        audioPlugin,
        annotation,
        colour,
        revision,
        routing,

        piano,
        microphone,
        volume,
        script,

        list,
        ellipsis,
        progressIndicator,

        browse,
        apply,
        toggleOn,
        toggleOff,

        play,
        pause,

        undo,
        redo,

        copy,
        cut,
        paste,

        create,
        remove,
        close,

        fail,
        success,

        zoomIn,
        zoomOut,

        cursorTool,
        drawTool,
        selectionTool,
        zoomTool,
        dragTool,
        cropTool,
        cutterTool,
        eraserTool,
        chordTool,
        chordBuilder,
        stretchLeft,
        stretchRight,
        inverseDown,
        inverseUp,
        expand,

        up,
        down,
        back,
        forward,
        mediaForward,
        mediaRewind,

        pageUp,
        pageDown,
        timelineNext,
        timelinePrevious,

        menu,
        submenu,

        login,
        remote,
        local,
        github,

        commit,
        reset,
        push,
        pull,

        mute,
        unmute,

        arpeggiate,
        refactor,
        render,

        selection,
        selectAll,
        selectNone
    };
};
