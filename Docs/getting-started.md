# Getting started

## UI overview

![helio-ui]

This is how the sequencer page looks like in Helio, as of version 3. There are other pages besides this, but you'll spend majority of your time in the sequencer.

The UI is divided into the following sections:

* the **breadcrumb** [navigation control](#workspace-navigation) is on the top,
* the [**editor**](#editing-and-arranging) canvas is in the middle,
* the project **mini-map** is on the bottom,
* the track **navigation tools** are in the [left sidebar](#left-sidebar),
* the **editing tools** are in the [right sidebar](#right-sidebar).

They will be described below, but, before you dive in,

### A little backstory and a couple of silly excuses

This project arose from the need for an advanced MIDI editor, something like Sublime Text for music.

I was also sick and tired of visual overstimulation, which most music tools tend to have more and more (just google some pictures for "digital audio workstation"). One of my main goals was to create a tool that feels right: one with an uncluttered and non-distractive user interface.

So, in general, I try to avoid adding UI controls if there is a way to do without them. As it turned out, though, there are a couple of challenges with that approach (for which I don't see simple solutions, UX design is hard):

* one challenge is to keep the UI simple, even minimalistic, while also not being disorienting,
* another challenge is to keep the UI look and behave consistently across all platforms, both desktop and mobile.

If something feels misleading to you, please [report](https://github.com/helio-fm/helio-sequencer/issues/new) it to help identify the main friction points.

### Workspace navigation

The breadcrumb control on the top is responsible for navigating the workspace.

The `Studio` root node contains links to all available pages and open projects:

![breadcrumbs-root-menu]

#### Workspace structure

Breadcrumbs show the hierarchy of the current page you're on.

Under the hood, all pages have a tree-like structure that looks like this:

- **Dashboard** for the projects list, a fancy logo, and a couple of buttons
- **Settings** for choosing a language, audio drivers, playing with UI flags, etc
- **Orchestra pit** for managing plugins and [instruments](#instruments) on the stage
  - **Instrument 1** page for setting up audio/MIDI routing for all plugins in your instrument
  - **Instrument 2** page and so on
- **Project 1** page for the basic project info: title, author, etc
  - [**Piano roll**](#piano-roll) for editing
  - [**Pattern roll**](#pattern-roll) for arranging
  - [**Version control**](#version-control) page
- **Project 2** page
  - [**Piano roll**](#piano-roll)
  - ..and so on

#### Context menus

Note that all items in the breadcrumb control have their own context menus:

![breadcrumbs-menus]


## Creating a Project

When you first launch Helio, you should see a simple example project open in the editor. Here are a few steps that can help you get started:

 * to rename the project, click `Example Project` in the breadcrumb control, and you'll be taken to the project info page, where you can edit some details,
 * after renaming, return to the piano roll page by pressing `Alt + Left Arrow` or by pressing the back button on the top left.
 * you can only edit one track at a time: use right-click (or long tap on mobile) to switch to another track; holding any modifier key while switching will also adjust the zoom level so that the entire track fits the viewport,
 * to play with arrangement, switch to the pattern roll by pressing `Tab` or `Page Down`, or by clicking the uppermost button in the left sidebar,
 * double-click any clip to return to the piano roll with that clip in focus; at this point you should get an idea of how the sequencer layout works.

To start a new project from scratch, go to the dashboard by pressing the `Home` key, or clicking the `Studio` node in the breadcrumbs. There you'll see a list of recent projects, and a couple of buttons:

 * create an empty project,
 * open an existing project (this also imports MIDI files).

### Switching between projects

There are several ways:
 * use the `/` hotkey to display the projects list in the [command palette](command-palette.md),
 * or hover the `Studio` item in the breadcrumb control to display a menu with all open projects,
 * the back and forward buttons are also useful at times; the associated hotkeys are `Alt + Cursor Left` and `Alt + Cursor Right`.

## Instruments

### Instruments management

The most notable difference between Helio's instrument management and that of the majority of DAWs and sequencers is that Helio separates instruments from projects.

Each project only stores the instrument references, so the instrument settings are saved in the application workspace settings rather than the project files.

Instruments are created and set up in a separate page, called Orchestra Pit.

The reason for implementing it this way was that in my workflow, I tend to use the same instruments for all of my projects. The app was designed primarily as a sketching and prototyping tool, and I usually have lots of sketches, so all the operations involving switching between projects, opening and closing them, or checking out in the version control, were ought to be as fast as possible, and not eat up all the memory.

Helio is probably not the best choice if your setup requires you to always have different instruments or instrument settings for each project, or if you want the project file to contain the instrument details.

On the other hand, if you happen to have an instrument library you're comfortable with (e.g. VSL or a selected soundfonts collection), and you want to set it up once and forget it, you'll probably like this approach.

### Orchestra pit page

The orchestra pit page has two sections:

 * all found plugins are displayed on the left side,
 * all instruments on stage, created from those plugins, are on the right.

![orchestra-pit]

First, you want to scan for available plugins, there are two options in the orchestra pit menu:

 * either pick a specific folder to scan (note that all subfolder will be scanned as well),
 * or perform a global plugins scan: it tries to guess the most possible locations, e.g. Steinberg folders in Program Files, and scans them and all their subfolders.

Once you have a list of plugins, create an instrument using the plugin menu, so that it appears on the stage and can be assigned to your tracks.

### Instrument details and routing

Double-click on any of the instruments to get to the instrument details page.

![instrument-routing]

Most of the actions here, including audio and MIDI routing, are available through the menus.

Interacting with nodes:

 * left-click on the node will create a plugin window, it it has one, or just select it, if it doesn't,
 * right-click on the node will just select it,
 * use mouse drag to connect sockets representing audio and MIDI inputs and outputs.

While it is possible to set up a multi-plugin instrument with audio/MIDI routing in Helio, the convenience of the instrument page was not of a particular concern: the development is rather focused on the sequencer functionality. If you are running it under Linux, it might be a good idea to add [Carla](https://kx.studio/Applications:Carla) as an instrument, and use it to manage VST/whatever plugins and routing.

### Default instruments

A few built-in instruments are created automatically and cannot be removed:

* the "Helio Default" instrument is a simple sine wave voice that supports all built-in temperaments with no setup; aside from being the default instrument, it is used to preview scales in the [key signature](#key-signatures) dialog,
* the "MIDI Output" instrument generates no sound but sends all events, except meta-events, to the MIDI output specified on the settings page,
* the "Metronome" instrument gives you the option to select custom [metronome](getting-polymetric.md#metronome) samples.


## Editing and arranging

### Timeline

On the top of the editor canvas, there's a timeline control. To interact with it:

* left click at the timeline to position the playhead,
* middle-click to start the playback from that position,
* right click to invoke the timeline events menu:

![timeline-events]

Timeline events include annotations, key signatures and time signatures, and they don't affect the playback of your piece in any way, they are rather meant to provide a visual cue.

Manipulating the timeline events:

* click on any event to focus the playhead at its position,
* once focused, click again to edit or delete the event (displays a dialog),
* drag to move, or shift-drag to duplicate the event.

#### Annotations

Annotations are just text markers with optionally adjustable length:

![timeline-annotations]

Right-click on the annotation selects all notes of the active track up to the next annotation.

#### Time signatures

Time signatures simply define the way the vertical grid lines are aligned in the roll:

![timeline-time-signatures]

Right-click on the time signature selects all notes of the active track up to the next time signature.

If you need to manage complex rhythms, see [this page](getting-polymetric.md).

#### Key signatures

Key signatures affect the way the rows are highlighted in the piano roll (but this effect can be [disabled](tips-and-tricks.md#scales-highlighting)).

![timeline-key-signatures]

Key signatures also affect note naming, and various tools like [arpeggiators](refactoring.md#arpeggiators) and [the chord tool](tips-and-tricks.md#chord-tool) rely on key signatures to figure out the harmonic context.

A key signature is a combination of the root key and the scale:

![key-signature-dialog]

*Tip: right-click on any key button to preview it.*

The root key selection allows you to pick one of the key's enharmonic equivalents, which only affects [note naming](tips-and-tricks.md#note-name-guides) to provide better compatibility with traditional notation language.

A scale is defined as a subset of the current temperament's octave keys. While this is not the same as how traditional notation defines, say, [diatonic scales](https://wikipedia.org/wiki/Diatonic_scale), this approach is flexible and makes more sense for the piano roll.

#### Reprise

If you have enabled the playback loop over the selected scope, timeline will display the repeat signs:

![timeline-reprise]

### Left sidebar

This sidebar is responsible for track navigation and UI control.

Most buttons on the sidebars have keyboard shortcuts, which makes them kinda redundant, but they are displayed anyway for the sake of having a consistent UI on mobiles or touch-screen laptops, where you don't have hotkeys.

 * ![sidebar-left-1] — switch the editor view between the piano roll and the pattern roll (`Tab`),
 * ![sidebar-left-2] — zoom out (`Shift + Z`), zoom in (`Z`), and zoom selection (`Control + Tab`),
 * ![sidebar-left-3] — jump over the timeline events (`,` and `.`),
 * ![sidebar-left-4] — toggle the [volume/automation editors](#volume-and-automation-editors) (`V`),
 * ![sidebar-left-5] — UI [flags](tips-and-tricks.md#ui-flags) that toggle scales highlighting and the note guides (`H` and `G`),
 * ![sidebar-left-6] — a simple waveform or spectrogram view.

### Right sidebar

This sidebar is responsible for editing tools and playback control:

 * ![sidebar-right-1] — toggle the playback loop over the selection (`F11`),
 * ![sidebar-right-2] — edit [modes](#edit-modes) (`1`, `2`, `3`),
 * ![sidebar-right-3] — some other tools - the chord tool and arpeggiators, if available,
 * ![sidebar-right-4] — copy and paste, undo and redo,
 * ![sidebar-right-5] — playback (`Space` or `Enter`) and recording (`F12`) control.

## Piano roll

The piano roll always limits the editable scope to a single track. You will see all other tracks in a semi-transparent ghost mode. Most common interactions with the piano roll canvas are:

 * right-click on the inactive note to focus the editor to another track,
 * right-drag the empty space to pan the canvas:

![piano-roll]

Interacting with piano roll also depends on the current edit mode:

### Edit modes

 * **normal mode** to manage selection and edit notes,
 * **pen mode** to add and edit notes,
   * alternatively, use it to delete notes or clips with right mouse button,
 * **knife mode**: cuts notes in the piano roll, [cuts tracks](tips-and-tricks.md#knife-tool) in the pattern roll,
   * alternatively, use it to [merge tracks](tips-and-tricks.md#merging-tracks) or notes with right mouse button,
 * **drag-only mode**: an auxiliary mode, hold `Space` to [toggle](tips-and-tricks.md#spacebar-panning) it temporarily,
 * **selection mode** is only displayed on mobile platforms.

All notes, when edited, are aligned to the grid. The grid resolution (the density of bar lines) depends on the zoom level and supports up to 1/64 notes. Tip: holding `Alt` while editing notes disables snapping to the grid.

All edits are undoable, and there is no limit to the number of undo actions available until the app is restarted. Additionally, each project saves the last 16 undo actions (32 on mobile platforms), which remain undoable after restarting the app. This limit can be changed by editing the main [configuration file](index.md#settingshelio).

All changes are saved automatically: on exit, and on a timeout after the last change.

The editor relies heavily on [hotkeys](hotkeys.md#piano-roll); feel free to explore available actions by browsing breadcrumb control menus or [command palette](command-palette.md).

### Adding new tracks

Add new tracks by duplicating the existing ones (`F5`), or via project menu, or by [cutting](tips-and-tricks.md#knife-tool) tracks with the knife tool in the pattern roll.

### Volume and automation editors

The bottom panel (toggled by `V` hotkey) combines the volume editor and the automation editor:

![bottom-editors-panel]

*(the piano roll allows you to manually switch between volume and automation tracks, while the pattern roll switches the editor type based on the selected clip)*

The volume editor, like the piano roll, restricts its editable scope to the active clip. Furthermore, if any notes in the roll are selected, the editable scope is limited to the selection, making it easier to fine-tune and [draw more complex ramps](tips-and-tricks.md#fine-tuning-dynamics) for different sections of the track. On mobile platforms, this panel is the main way to edit volume; on desktop platforms, you can also adjust note volumes in the piano roll using the middle mouse button and the `+` and `-` hotkeys on the numpad.

The automation editor's editable scope is limited to the active clip and the selected controller. The active clip is determined by the pattern roll's selection or by the closest range intersection with the piano roll's active clip.

Unlike automation clips in the pattern roll, the automation editor allows you to use the pen tool to hand-draw custom curves for curve automations.

Both the volume editor and the automation editor currently only support the default edit mode and the pen tool (no selection or knife tool).

### MIDI recording

The record button (hotkey `F12`) will try to auto-detect the available and enabled MIDI input device, or will provide a selection if there are multiple (the choice is "remembered" and can be changed later in the settings page):

![recording-start]

If the recording mode is on, but playback has not yet started, it will wait until it receives the first MIDI event and start recording & playback.

In the piano roll mode, it always records to the selected track/clip:

![recording-piano-roll]

In the pattern roll, it either records to the selected track/clip, or if no piano clip is selected, it adds one once the recording begins.

## Pattern roll

You don't necessarily need that editor. Helio was designed to be a hybrid linear-based/pattern-based sequencer, so you could just stay in the piano roll mode and treat your project as one big canvas.

However, the pattern roll is helpful for rearranging experiments:

![patterns]

Pattern roll also allows you to tweak some track parameters, like key offset of velocity multiplier. In future, it may shift towards more parametric sequencer features.

#### See also: [track grouping](tips-and-tricks.md#clips-and-track-grouping)

### Global tempo

Pattern roll is also where you can edit various MIDI controller automation tracks, most notably the tempo track(s):

![tempo-automation]

The tempo track, as any other automation track, interpolates the controller value between nodes, and those tiny points are displayed at the exact times each new MIDI event is sent.

To interact with it:
* delete nodes with right click,
* use the pen tool (hotkey `2`) to add nodes,
* use the cursor tool (hotkey `1`) to drag nodes or to adjust the interpolation curve,
* click on the node to invoke the tempo dialog to set the exact BPM value, or to set tempo by tapping:

![tempo-dialog]

*Tip: use the mouse wheel to quickly adjust the tempo in this dialog.*

*Tip: adding a tempo node while holding any mod key will immediately open the tempo dialog.*

Often, you only need one tempo for the whole song - for that, pick *"Set one tempo"* menu item from the tempo track menu or the project refactoring menu.

*Tip: use `Shift +` and `Shift -` hotkeys to shift tempo tracks 1 BPM up or down. It affects all selected tempo tracks in the pattern roll, and all tempo tracks in the piano roll.*

### Mute and solo

Individual clips can be muted via the `M` hotkey or the context menu in the pattern roll.
Except for automation, all clips can be soloed (with the `S` hotkey or via the context menu) for the convenience of isolating chunks of notes without disabling the tempo/pedal/etc. automation at the same time.

All clips that are explicitly or implicitly muted (muted by others) are displayed as dimmed:

![solo-clips]

## Version control

The concept of [version control](https://en.wikipedia.org/wiki/Version_control) comes from the software development world. If you're not familiar with it, you can think of it as of creating "save points" in your project, and tracking changes made since the last time you saved.

The point of having a version control is lowering the friction of committing changes: any changes can be reset if you don't like them, any saved revision can be restored later. Helio was started as a prototyping tool, a playground for ideas, where you'd want to have several variations of your sketch — hence the idea of having a built-in version control.

Notable use cases are:

 * saving a project state at some point, and resetting to any earlier saved state,
 * resetting some of the recent changes (think of it as of another mechanism of undoing things),
 * hiding and restoring back the most recent changes to get the idea of what have been done since the last commit,
 * [synchronizing](#synchronizing-projects) projects across devices.

The UI of the versions page is split in two parts:

![version-control]

The left side lists all changes in the project, compared to the current revision. Items in the list can be checked and unchecked: both committing and resetting changes are applied selectively.

The right side shows the tree of all revisions that you have saved. Note a couple of icons under each revision: they indicate whether the revision is available locally, or remotely, or both.


[helio-ui]: images/screen-v3.png "UI overview"
[orchestra-pit]: images/orchestra-pit.png "The instruments management page"
[instrument-routing]: images/instrument-routing.png "The instrument details page"
[version-control]: images/version-control.png "The version control page"

[breadcrumbs-root-menu]: images/breadcrumbs-root-menu.png "Breadcrumbs control, root menu"
[breadcrumbs-menus]: images/breadcrumbs-menus.png "Breadcrumbs control, context menus"

[piano-roll]: images/piano-roll.png "Interaction with piano roll canvas"
[patterns]: images/patterns-arrange.png "Pattern mode for arrangements"
[patterns-clips]: images/patterns-track-clips.png "Track instances (clips) and their modifications"
[bottom-editors-panel]: images/bottom-editors-panel.png "Volume/automations editors panel"

[tempo-automation]: images/tempo-automation.png "Changing global tempo in pattern mode"
[tempo-dialog]: images/tempo-dialog.png "Not quite my tempo. It's all good, no worries."
[solo-clips]: images/solo-clips.png "While some clips are soloed, all others are implicitly muted."

[timeline-events]: images/timeline-events.png "Timeline: interacting and events menu"
[timeline-annotations]: images/timeline-annotations.png "Timeline: annotation events"
[timeline-key-signatures]: images/timeline-key-signatures.png "Timeline: key signature events"
[timeline-time-signatures]: images/timeline-time-signatures.png "Timeline: time signature events"
[timeline-reprise]: images/timeline-reprise.png "Timeline: repeat signs"
[key-signature-dialog]: images/key-signature-dialog.png "Key signature dialog"

[sidebar-left-1]: images/sidebar-left-1.png "Navigation sidebar 1"
[sidebar-left-2]: images/sidebar-left-2.png "Navigation sidebar 2"
[sidebar-left-3]: images/sidebar-left-3.png "Navigation sidebar 3"
[sidebar-left-4]: images/sidebar-left-4.png "Navigation sidebar 4"
[sidebar-left-5]: images/sidebar-left-5.png "Navigation sidebar 5"
[sidebar-left-6]: images/sidebar-left-6.png "Navigation sidebar 6"

[sidebar-right-1]: images/sidebar-right-1.png "Tooling sidebar 1"
[sidebar-right-2]: images/sidebar-right-2.png "Tooling sidebar 2"
[sidebar-right-3]: images/sidebar-right-3.png "Tooling sidebar 3"
[sidebar-right-4]: images/sidebar-right-4.png "Tooling sidebar 4"
[sidebar-right-5]: images/sidebar-right-5.png "Tooling sidebar 5"

[recording-start]: images/recording-start.png "Start MIDI recording"
[recording-piano-roll]: images/recording-piano-roll.png "MIDI recording in the piano roll"
