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

class Icons final
{
public:

    using Id = uint32;

    static void initBuiltInImages();
    static void clearBuiltInImages();
    static void clearPrerenderedCache();

    static Image findByName(Icons::Id id, int maxSize);
    static Image renderForTheme(const LookAndFeel &lf, Icons::Id id, int maxSize);
    static void drawImageRetinaAware(const Image &image, Graphics &g, int cx, int cy);

    static Path getPathByName(Icons::Id id);
    static UniquePointer<Drawable> getDrawableByName(Icons::Id id);

    static MouseCursor getCopyingCursor();
    static MouseCursor getErasingCursor();

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
        meter,
        metronome,

        volume,
        volumePanel,
        bottomBar,

        list,
        ellipsis,
        progressIndicator,
        console,

        browse,
        apply,
        toggleOn,
        toggleOff,

        play,
        pause,
        stop,
        record,

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
        zoomToFit,

        cursorTool,
        drawTool,
        selectionTool,
        dragTool,
        cropTool,
        cutterTool,
        chordBuilder,
        submenu,

        expand,
        stretchLeft,
        stretchRight,
        inverseDown,
        inverseUp,
        inversion,
        retrograde,
        legato,
        staccato,
        snap,
        cleanup,

        up,
        down,
        back,
        forward,
        reprise,

        timelineNext,
        timelinePrevious,
        paint,
        tag,

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
