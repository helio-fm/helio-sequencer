# Getting started

## UI overview

![helio-ui]

This is what the sequencer page looks like in Helio, as of version 3. There are other pages besides this, but you'll spend most of the time in the sequencer.

The UI is separated into these parts:

* the **breadcrumb** [navigation control](#workspace-navigation) is on the top,
* the [**editor**](#editing-and-arranging) canvas is in the middle,
* the project **mini-map** is on the bottom,
* the [left sidebar](#left-sidebar) is for track **navigation tools** and **UI control**,
* the [right sidebar](#right-sidebar) is for **editing tools** and **playback control**.

They will be described below, but, before you dive in,

### A bit of a history and a couple of silly excuses

This project was started out of the need for an advanced MIDI editor — think something like Sublime Text for music.

I was also sick and tired of visual over-stimulation, which most of the music tools out there tend to have more and more (just google some pictures for "digital audio workstation"). As one of the main goals, I wanted a tool that feels right: something with an uncluttered and non-distractive UI.

So generally, I'm always trying to avoid adding UI controls if there's a way to do without them. As it turned out, though, there are a couple of challenges with that approach (for which I don't see simple solutions, UX design is hard):
 * one challenge is to keep the UI both simple or even minimalistic and not disorienting at the same time,
 * another challenge is to keep the UI look and behave consistent across all platforms, especially desktop and mobile

If something feels misleading to you —  I beforehand apologize, feel free to [report](https://github.com/helio-fm/helio-workstation/issues/new) that to help me identify the main friction points.

### Workspace navigation

The breadcrumb control on the top is responsible for the navigation over the workspace.

The root node called `Studio` contains links to all available pages and open projects:

![breadcrumbs-root-menu]

#### Workspace structure

Breadcrumbs are displaying a hierarchy of the current page you're on.

Under the hood, all pages live in a tree-like structure like this:

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

After starting Helio the first time you should have a simple example project already open in the editor. Here's a couple of steps that can help you to get started:

 * to rename the project, click `Example Project` in the breadcrumb control, and you'll see the project info page with some details available for editing,
 * after renaming, go back by pressing `Alt + Left Arrow` or by pressing the back button on the top left, and you should get back in the piano roll page,
 * note that you can only edit one track at a time: use right-click to switch to another track (or long tap on mobile),
 * switch to the pattern roll be pressing `Tab` or `Page Down`, or by clicking the uppermost button in the left sidebar, to play with arrangement,
 * double-click any clip to return to the piano roll with that clip in focus; at this point you should get an idea how things work in the sequencer layout.

To start a new project from scratch, navigate to the dashboard by pressing `Home` key, or clicking the `Studio` node in the breadcrumbs. There you'll see the list of recent projects, and a couple of buttons:

 * create an empty project,
 * open a project (this also imports MIDI files).

### Switching between projects

There are several ways:
 * use the `/` hotkey to show the projects list in the [command palette](tips-and-tricks.md#command-palette),
 * or hover the `Studio` item in the breadcrumb control, which shows the menu with all open projects (the most inconvinient way so far),
 * back and forward buttons also can be useful sometimes, the related hotkeys are `Alt + Cursor Left` and `Alt + Cursor Right`.


## Instruments

### Instruments management

The most notable difference in the instruments management from the most sequencers out there is that Helio separates instruments from projects.

Each project only holds the instrument "references" (basically, the hashcodes of the instrument info), so that the instrument settings are not saved in the project file, but rather in the application workspace settings.

Instruments are also created and set up in a separate page, called Orchestra Pit.

The reason for implementing it this way was ~~separation of concerns, yo~~ that in my workflow, I tend to use all the same instruments for all the projects. The app was designed primarily as a sketching and prototyping tool, and I usually have lots of sketches, so all the operations that involve switching between projects, opening and closing them, or checking out in the version control, were ought to be as fast as possible, and not eat up all the memory.

If your setup implies always having different instruments or instrument settings for each project, or if you want the project file to contain the instrument details, Helio will make you suffer.

On the other hand, if you have an instrument library you're comfortable with (e.g. VSL or some selected soundfonts collection), and you want to set it up once and forget about it, you'll probably like this approach.


### Orchestra pit page

The orchestra pit page has two sections:

 * all found plugins are displayed on the left side
 * all instruments on stage, created from those plugins, are on the right

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

 * left-click on the node will create a plugin window, it it has one, or just select it, if it doesn't
 * right-click on the node will just select it

While it is possible to set up routing in Helio, the convenience of the instruments page was not of a particular concern. If you are running it under Linux, it might be a better idea to add [Carla](https://kx.studio/Applications:Carla) as an instrument, and use it to manage VST/whatever plugins and routing.


## Editing and arranging

### Timeline control

... TODO

#### Annotations

...

#### Key signatures

...

#### Time signatures

...

#### Reprise

...

### Left sidebar

... todo navigation and UI flags

 * toggle switching betwenn the piano roll and the pattern roll,
 * ...
 * toggle [velocity map](#velocity-map),
 * toggle displaying [note guides](tips-and-tricks.md##ui-flags),
 * toggle [scales highlighting](tips-and-tricks.md##ui-flags),

#### Velocity map

Velocity levels map/editor provides a way to visualize and draw gradual increase/decrease in note volume. It is toggled by `'v'` hotkey or a volume button:

![velocity-map-toggle]

At the moment of writing, only linear ramps are implemented:

![velocity-map-ramps]

### Right sidebar

#### Edit modes

...

## Piano roll

...

## Pattern roll

You don't necessarily need that editor. Helio was designed to be a hybrid linear-based/pattern-based sequencer, so you could just stay in the piano roll mode and treat your project as one big canvas.

However, the pattern roll is helpful for rearranging experiments:

![patterns]

## MIDI recording

The record button will try to auto-detect the available and enabled MIDI input device or provide a choice if there are more than one:

![...]

If the recording mode is on, but the playback has not started yet, it will wait until it receives the first MIDI event and start recording & playback.

In the piano roll mode, it always records to the selected track/clip:

In the pattern roll, it either also records to the selected track/clip, or, if no piano clip is selected, it adds one, once the actual recording starts:

![...]


## Version control

### Basic concepts

...

### Saving and resetting changes

...

### Synchronization across devices

...




[helio-ui]: images/screen-v3.png "UI overview"
[orchestra-pit]: images/orchestra-pit.png "The instruments management page"
[instrument-routing]: images/instrument-routing.png "The instrument details page"

[breadcrumbs-root-menu]: images/breadcrumbs-root-menu.png "Breadcrumbs control, root menu"
[breadcrumbs-menus]: images/breadcrumbs-menus.png "Breadcrumbs control, conetxt menus"

[patterns]: images/patterns-arrange.png "Pattern mode for arrangements"
[patterns-clips]: images/patterns-track-clips.png "Track instances (clips) and their modifications"

[velocity-map-toggle]: images/velocity-map-toggle.png "Velocity map overview"
[velocity-map-ramps]: images/velocity-map-ramps.png "Velocity map linear ramps"
