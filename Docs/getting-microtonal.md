# Getting microtonal

The piano roll comes with a number of built-in microtonal temperaments and 7-tone scales for each of them, but you can also add your own if necessary.

Helio's default instrument and built-in SoundFont player work with all temperaments out of the box, with no setup required. Use them if you only want to get familiar with microtonal scales and get a sense of how they sound, and [skip](#switching-temperaments) the instrument setup steps below.

On mobile platforms, the built-in SoundFont player is likely the only way to have an instrument of choice playing in microtonal temperament. It will automatically recalculate the pitches and ranges of your samples to match the frequencies of the current temperament (see [the example](https://youtu.be/eDXmhGmJP6A)).

## Setting up instruments

The following describes how to set up external (VST/whatever) instruments such as Pianoteq to work with extended microtonal piano roll.

First, you need to find a plugin that supports microtonal temperaments: here's a [list](https://xen.wiki/w/List_of_microtonal_software_plugins) with some examples.

Then, set up keyboard mapping for that instrument: this must be done both in the plugin's settings and in the host (this app), to overcome the limitations of MIDI 1.0. The piano roll works as a microtonal keyboard with a wide key range from 0 up to 2048, and it needs to map every key across 16 MIDI channels, of 128 keys each; these MIDI messages are sent to the plugin, which needs to know how to interpret them, or how to map them back from multi-channel data.

Once you've added an [instrument](getting-started.md#instruments) to the stage, select *"Edit keyboard mapping"* from the menu. "Keyboard mapping" in this context refers to how MIDI events are transformed when they are routed from the 'Midi In' node to the plugin node(s).

### Keyboard mapping page

*Note: this describes key mapping on the host side, but you'll need to set it up on the plugin side as well. Unfortunately, can't help you with that, as it depends on the plugin; see a couple of [examples below](#examples).*

The page allows you to manually adjust key mappings and preview mapped keys by clicking on them. The upper limit for each source channel is set to 2048 keys, this is the maximum number of keys that can fit into 16 target MIDI channels. 2048 keys would be enough to handle temperaments of size up to 192-EDO:

![keyboard-mapping-page]

*(source channel selection at the top, channel paging at the bottom)*

Some additional actions are available via context menu:
 * reset to the default modulo-based mapping,
 * load custom channel mapping(s) in [Scala .kbm format](#scala-keyboard-mapping),
 * copy/paste the mapping as a string in Helio's [own format](#helio-keyboard-mapping-format) into/from the system clipboard,
 * select one of the presets or save your own.

![keyboard-mapping-menu]

While you can set up virtually any custom mapping by hand, there are a couple of (sometimes) more convenient ways, described below.

### Scala keyboard mapping

*Note: don't confuse Scala keyboard mapping (.kbm files) with Scala tuning files. The latter are of little use to the piano roll because it doesn't care about cents and ratios - those are only needed by instruments.*

Helio can load [Scala keyboard mapping(s)](http://www.huygens-fokker.org/scala/help.htm#mappings), including multichannel mappings which consist of a set of single mapping files with the same name followed by an underscore and a channel number. When you pick a .kbm file, Helio will search for "same_name_*.kbm" files to try to determine whether or not there are multiple channel mappings, and try to load them all.

Keep in mind that the mapping on the sequencer side will be *reversed* when compared to the same mapping on the instrument side.
The piano roll works as a microtonal keyboard that needs to overcome the limitations of MIDI channels by mapping all keys above 127 across multiple channels.

### Helio keyboard mapping format

Although the Scala kbm format is flexible, I find it unintuitive, so let me reinvent the wheel and introduce another keyboard mapping format, supposed to be shorter, cleaner and kind of human-readable. You can use it on the keyboard mapping page by simply copying and pasting the mapping as a string.

Let's start with an example: the entire multi-channel mapping [for Pianoteq](https://forum.modartt.com/viewtopic.php?id=4307) 31-EDO could be written like this:

`0/1:0/14,31+ 31:0/15,31+ 62:0/16,31+ 93:0/1,31+ 124:0/2,31+ 155:0/3,31+ 186:0/4,31+ 217:0/5,31+ 248:0/6,31+ 279:0/7,31+ 310:0/8,31+`

Which reads:
 * starting from key `0` channel `1` of the source channel, map it to the key `0` of channel `14`, then map the next `31` keys in a sequential manner: for example, key `1` maps to `1/14`, key `2` maps to `2/14`, and so on,
 * starting from key `31`, map it to the key `0` of channel `15`, and, again, map the next `31` keys sequentially, and so on,
 * for each next channel the mapping will pick the key from the previous channel and increase the channel number.

Source channel can be omitted, since you probably only need channel 1. E.g., the 22-EDO Pianoteq mapping would look like this:

`0:0/14,22+ 22:0/15,22+ 44:0/16,22+ 66:0/1,22+ 88:0/2,22+ 110:0/3,22+ 132:0/4,22+ 154:0/5,22+ 176:0/6,22+ 198:0/7,22+ 220:0/8,22+`

For 34-EDO mapping you might want to start one octave lower since the key range is wider:

`0:0/13,34+ 34:0/14,34+ 68:0/15,34+ 102:0/16,34+ 136:0/1,34+ 170:0/2,34+ 204:0/3,34+ 238:0/4,34+ 272:0/5,34+ 306:0/6,34+ 340:0/7,34+`

#### Format description

First, the format assumes the default modulo-based mapping:
 * keys 0..127 are mapped as they are to channel 1,
 * keys 128..255 are mapped as 0..127 in channel 2, and so on.

Everything that differs from the default values is serialized. The serialized mapping is just a string divided by whitespaces into chunks.

Chunks follow this pattern: `base:key/channel,key2/channel2,key3/channel3`. Which means:
 * for the `base` key in the sequencer, the specified `key/channel` will be sent to the instrument,
 * then, for the `base + 1` key in the sequencer, the next key and channel will be sent, and so on.

If each next key just incrementally moves by 1, the chunk could be shortened like this: `base:key/channel,count+`.

// TODO more examples (if you have set up keyboard mappings, which work well for some of your instruments, feel free to [share them](https://github.com/helio-fm/helio-sequencer/discussions) to improve these docs)

## Switching temperaments

The most straightforward way to start playing around with microtonal scales is to convert any of your twelve-tone pieces (or a new project) into other temperament.

![change-temperament]

The project refactoring menus provide two choices:
 * the `"Change temperament"` command will simply switch the piano roll highlighting and update key signatures,
 * the `"Convert to temperament"` command will, in addition, update notes: the conversion is done using chromatic scale approximations (see the [temperament model](configs.md#temperaments) for more information). Helio will use these chromatic maps as the "least common denominators" among temperaments to convert from one to another,
   * *tip: use this menu with any modifier key pressed for the alternative conversion mode: the converter will assume equal temperaments and use key proportions instead of 12-tone maps for better accuracy.*

All tool that work with scales will only show scales with octave size matching current temperament. The piano roll doesn't distinguish between EDOs and non-EDOs; all that matters is the number of keys per octave, highlighting, key names, and so on.

### Limitations

The built-in temperaments [list](configs.md#temperaments) includes 12-edo, 19-edo, 22-edo, 26-edo, and 31-edo.

The built-in scales [list](configs.md#scales) only includes 7-tone scales for each of those temperaments because several parts of the app, such as the chord tool and the arpeggiator tool, still assume working with diatonic scales. Given that limitation, using these tools with any built-in temperament or scale makes sense.

### Adding a custom temperament

To add custom temperaments and their scales, follow the guide in the [сonfigs](configs.md#user-configs) page. Use the built-in [temperaments](configs.md#temperaments) and [scales](configs.md#scales) configuration files as a reference.

## Recording MIDI from 12-tone keyboard

The app makes it possible to play and improvise on a standard 12-tone physical keyboard and record MIDI regardless of what temperament is used. For that, it uses the current temperament's 'chromatic mapping' scale to readjust the incoming MIDI data so that all notes sound like their closest 12-tone approximations.

Here is an example of each chromatic scale key played on a 12-tone MIDI keyboard and recorded in a 19-EDO project:

![readjust-midi-input-recording]

This feature is enabled by default, but it can be disabled in the audio settings section, just below the MIDI input device box. If you're using a microtonal physical keyboard, uncheck this box so that the app doesn't mess up MIDI input:

![readjust-midi-input-checkbox]

## Examples

This section will describe setting up various microtonal plugins in Helio. For now, it's just a couple of examples, 
if you managed to make any other plugin work, please [share](https://github.com/helio-fm/helio-sequencer/blob/develop/Docs/getting-microtonal.md) your findings.

### Pianoteq

Steps to set up a custom equal temperament:

 * In Pianoteq UI, navigate to the advanced tuning page:
   * there, create a custom microtonal temperament via the `"Temperament" -> "Make equal temperament..."` dropdown menu,
   * make sure to choose the `"Full rebuild"` option nearby instead of the `"String tension"` option,
   * in the `"Keyboard mapping" -> "Extended layout for up to 16\*128 notes"` dropdown menu, check `"Multi-channel MIDI layout"`,
   * the `"Main MIDI Channel"` option in the same menu should be set to the default `"MIDI channel 1"`.
 * In Helio UI, navigate to your instrument's keyboard mapping page:
   * there, either select a keyboard mapping preset for your temperament from the menu, if it's available: for now there are presets for 19-edo, 22-edo, 26-edo and 31-edo,
   * or, set up your own multi-channel mapping (see examples above on this page).

For more details on all Pianoteq's tuning parameters refer to [the manual](https://www.modartt.com/user_manual?product=pianoteq&lang=en).

### Surge XT

Surge synth supports multi-channel mapping since recent versions of Surge XT, so a custom equal temperament can be set up similarly:

 * In the main menu, open the tuning settings submenu and check these two options there:
   * `"Use MIDI channel for octave shift"`
   * `"Tuning applied at MIDI input"`
 * From the same tuning submenu, load the Scala tuning (Surge installation provides a set of equal temperament tunings),
 * Navigate to your instrument's keyboard mapping page in Helio UI and choose a keyboard mapping preset; the same Pianoteq multi-channel presets will work for Surge.


[keyboard-mapping-page]: images/keyboard-mapping-page.png "Keyboard mapping page layout"
[keyboard-mapping-menu]: images/keyboard-mapping-menu.png "Keyboard mapping page menu"
[change-temperament]: images/change-temperament.png "Switching and converting temperaments"
[readjust-midi-input-recording]: images/readjust-midi-input-recording.png "Every key of one octave played on a 12-tone keyboard, recorded in 19-edo"
[readjust-midi-input-checkbox]: images/readjust-midi-input-checkbox.png "Enable/disable recording microtonal notes from 12-tone keyboard"
