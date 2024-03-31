# Tips and tricks

This page lists features, hacks, and nuances that may be handy, non-obvious, or both.

### Faster playback

Hitting `Space` or `Enter` twice will start playback at 1.5x speed, which can be useful for quickly previewing a sequence, e.g. when prototyping a chord progression.

Alternatively, double-click or middle-click in the roll header while holding any modifier key, or long-tap the play button in the corner.

### Warnings

During playback, the sequencer may sometimes draw red or yellow vertical warning lines:

![warnings]

These are the clipping and over-saturation warning markers:

 - red lines indicate problematic regions with [clipping sound](https://en.wikipedia.org/wiki/Clipping_(audio)),

 - yellow warning lines will appear in the areas where the perceived loudness (a.k.a. the root mean square loudness) is way lower than the peak loudness, which is considered ~~harmful~~ unhealthy: in my setup it typically means that I have some redundant duplicate notes in the same place.

### Spacebar panning

One quick way to switch between the current editing mode and the canvas panning mode is holding the `Space` key:

![space-drag]

Dragging with the right mouse button does the same thing, but it also switches to another track when clicked on any semi-transparent note.

### Time measure tool

Hold the `Space` key, then click-and-drag over the timeline to measure time between two points on the timeline:

![time-measure-tool]

### Sound probe tool

Finally, holding `Space` and clicking on the timeline is what I call a "sound probe" and it's supposed to give you an idea of what notes are playing at the given point:

![sound-probe]

### Range selection

Click-and-drag on the timeline while holding any modifier key (`Control`/`Alt`/`Shift`) to select all notes or clips in a time range:

![range-select]

### Resizing a group

Resize a group of notes proportionally by holding `Shift`:

![group-resizing]

### Drag-and-copy

Hold `Shift` to drag-and-copy notes in the piano roll, clips in the pattern roll, key/time signatures, annotations or automation events:

![drag-and-copy]

### Pen tool

Also hold `Shift` or any modifier key to change the behavior of the pen tool when adding notes. By default, the newly added note is edited in transpose-and-resize mode. Alternatively, there is the drag mode, which is more familiar:

![pen-tool-alt]

### Fine-tuning dynamics

Use the pen tool to hand-draw custom ramps in the volume editor panel. Control how the ramp curve blends with the original velocities by using the mouse wheel:

![velocity-panel-hand-drawing]

In the default edit mode, fine-tune volume of the selected notes: dragging them vertically with no modifier keys will shift the velocities linearly, dragging while holding `Alt` will scale them.

Holding `Shift` while dragging will shape-shift group's velocities into a sine (when dragging up) or flatten them, reducing dynamic range (when dragging down):

![velocity-panel-fine-tuning]

*(the indicator displays the group's lowest and highest MIDI volume)*

You can also adjust notes volume linearly just by middle-button dragging the note components in the piano roll directly.

### UI flags

A couple of display options are available to provide a visual cue. They can be toggled using the navigation panel or hotkeys (`G` and `H` by default).

The first is used for displaying the note name guides:

![note-names]

Another highlights the in-scale keys of the key signatures that have been added to the timeline. If you prefer C Major coloring in the piano roll, just turn it off:

![scales-highlighting]

The mini-map mode can be toggled with `B` hotkey, or by clicking at any area except the screen range rectangle.

When in compact mode, the mini-map is stretched to fit all project:

![toggle-minimap]

When in full mode, the mini-map allows you to draw a region to zoom in on:

![zoom-to-region]

### Chord tool

By double-clicking on a row in the piano roll you invoke the chord tool:

![chord-tool]

It picks the current key signature from the timeline to determine which scale and root key to use to generate chords. Hence the main limitation of this tool: it can only generate chords that are easy to define with in-scale keys.

It can be dragged around by the center node, which is helpful if you clicked the wrong row or position.

Since it depends on the harmonic context, it will do nothing when placed on an out-of-scale note (darker rows). To avoid confusion, make sure the [scales highlighting](#ui-flags) option is enabled.

### Knife tool

In my workflow, I'm often adding new tracks with a knife tool: even though there's a normal way to add an empty track via project menu, or duplicate a track, I often end up having added some sketches in different places of a single sequence, and then seeing that they represent different parts, and can be cut into different tracks after switching to the pattern mode:

![patterns-knife-tool]

#### Merging tracks

Knife tool has an alternative mode: use right-click and drag (or long-tap and drag on mobile platforms) to merge one clip with another:

![patterns-merge-tool]

### Clips and track grouping

In the example above, two split tracks remain on the same row because the tracks are grouped by name, and the knife tool keeps the track name the same. Pattern roll can also group tracks by color, instrument, or track id — yet grouping by name works better for me.

So, the segments on one row could be either different tracks or multiple instances (or "clips") of the same track. Instances always share the same notes, and have the same name and color, but they can be slightly modified: have different position, key shift or volume multiplier, which is mainly meant for prototyping:

![patterns-clips]

When you select an item in the pattern roll, all of its instances are highlighted with a dashed header: this helps distinguish between "instances" and other tracks with the same name or color on the same row. The `F6` hotkey is a quick way to convert an instance to a unique track.

Track grouping also affects MIDI export: all segments on a single row are exported as a single track in the resulting MIDI file.

#### See also: [piano roll hotkeys](hotkeys.md#piano-roll), [pattern roll hotkeys](hotkeys.md#pattern-roll), [refactoring options](refactoring.md)


[space-drag]: images/space-drag.png "Dragging the canvas"
[time-measure-tool]: images/time-measure.png "Time measure tool"
[sound-probe]: images/sound-probe.png "Sound probe tool"
[range-select]: images/range-select.png "Range selection"

[group-resizing]: images/group-resizing.png "Resizing notes with shift"
[pen-tool-alt]: images/pen-tool-alt.png "Adding notes with shift"
[drag-and-copy]: images/drag-and-copy.png "Drag-and-copy"

[toggle-minimap]: images/toggle-minimap.png "Mini-map view modes"
[zoom-to-region]: images/zoom-to-region.png "Mini-map zoom-to-region"
[scales-highlighting]: images/scales-highlighting.png "Scales highlighting"
[note-names]: images/note-names.png "Note name guides"

[chord-tool]: images/chord-tool.png "The chord tool"

[warnings]: images/warnings.png "Clipping and over-saturation warning markers"
[velocity-panel-fine-tuning]: images/velocity-panel-fine-tuning.png "Velocity fine-tuning"
[velocity-panel-hand-drawing]:  images/velocity-panel-hand-drawing.png "Hand-drawing velocities shape"

[patterns-knife-tool]: images/patterns-knife-tool.png "Using knife tool in pattern mode"
[patterns-merge-tool]: images/patterns-merge-tool.png "Merging tracks in pattern mode"
[patterns-clips]: images/patterns-track-clips.png "Track instances (clips) and their modifications"
