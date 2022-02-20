# Getting microtonal

Before you get started with microtonal music, you will need two things:
 * Find a plugin (VST/whatever), which supports microtonal temperaments: here's a [list](https://github.com/suhr/awesome-microtonal#synths) with some examples.
 * Set up keyboard mapping for that instrument: this needs to be done both in the plugin's settings and in the host (this app), to overcome the limits of MIDI 1.0. The piano roll works as a microtonal keyboard with a wide key range from 0 up to 2048, and it needs to map every key over 16 MIDI channels, of 128 keys each; then those MIDI messages are sent to the plugin, which needs to know how to interpret them, or how to map them back from multi-channel data.

## Setting up instruments

You can [skip](#switching-temperaments) the instrument setup steps, if you only want to get familiar with microtonal scales. The default instrument in Helio is silly, but it works out of the box with all temperaments with no setup required, and should give you some idea of how things work.

Otherwise, once you've [added an instrument](getting-started.md#instruments) to the stage, just pick *"Edit keyboard mapping"* menu item. The "keyboard mapping" here means mapping MIDI events routed from the `Midi In` node to the plugin node(s).

### Keyboard mapping page

*Note: this describes the key mapping on the host side, but you'll need to set it up on the plugin's side as well. Unfortunately, can't help you with that, as it depends on the plugin, but here's an [example](https://soundbytesmag.net/microtonality-in-falcon/).*

The page allows you to adjust key mappings manually and preview the mapped keys by clicking on them. The upper limit is set to 2048 keys - this is the maximum number of keys that can fit into 16 MIDI channels. Which would be enough to handle temperaments of size up to 192-EDO:

![keyboard-mapping]

Some additional actions are available via context menu:
 * reset to the default mapping (modulo based),
 * load custom channel mapping(s) in [Scala .kbm format](#scala-keyboard-mapping),
 * copy/paste the mapping into/from the system clipboard as a string in Helio's [own format](#helio-keyboard-mapping-format).

While you can set up virtually any custom mapping by hand, there are a couple of (sometimes) more convenient ways, described below.

### Scala keyboard mapping

*Note: don't confuse Scala keyboard mapping (.kbm files) with Scala tuning files. The latter are not of much use for the piano roll, since it doesn't care about cents and ratios - those are only needed by instruments.*

Helio can load [Scala keyboard mapping(s)](http://www.huygens-fokker.org/scala/help.htm#mappings), including multichannel mappings which consist of a set of single mapping files with the same name followed by an underscore and a channel number. When you pick a .kbm file, Helio will search for "same_name_*.kbm" files to try to determine whether or not there are multiple channels' mappings, and try to load them all.

Keep in mind that, on the sequencer side, the mapping will be *reversed* compared to the same mapping on the instrument side.
The piano roll acts like a microtonal keyboard, which needs to overcome the limits of MIDI channels, and thus is mapping all the keys, which are above 127, across multiple channels.

// TODO examples

### Helio keyboard mapping format

I think that Scala kbm format is obscure and unintuitive, so let me reinvent the wheel one more time and introduce another keyboard mapping format, supposed to be shorter, cleaner and kind of human-readable. You can use it on the keyboard mapping page by simply copying and pasting the mapping as a string.

Let's start with an example. The entire multi-channel mapping [for Pianoteq](https://forum.modartt.com/viewtopic.php?id=4307) 31-EDO could be written like this:

`0:0/14,31+ 31:0/15,31+ 62:0/16,31+ 93:0/1,31+ 124:0/2,31+ 155:0/3,31+ 186:0/4,31+ 217:0/5,31+ 248:0/6,31+ 279:0/7,31+ 310:0/8,31+`

Which reads:
 * starting from key `0` of the piano roll, map it to the key `0` of channel `14`, then map the next `31` keys in a sequential manner: for example, key `1` maps to `1/14`, key `2` maps to `2/14`, and so on,
 * starting from key `31`, map it to the key `0` of channel `15`, and, again, map the next `31` keys sequentially,
 * and so on.

Similarly, the 22-EDO Pianoteq mapping would look like this:

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

// TODO more examples (if you have set up keyboard mappings, which work well for some of your instruments, feel free to [share them](https://github.com/helio-fm/helio-workstation/discussions) to improve these docs)

## Switching temperaments

The most straightforward way to start playing around with microtonal scales is to convert any of your twelve-tone pieces (or a new project) into other temperament.

![change-temperament]

The project refactoring menus provides two choices:
 * the `"Change temperament"` command will simply switch the piano roll highlighting and update key signatures,
 * the `"Convert to temperament"` command will, in addition, update notes: the conversion is made using chromatic scale approximations - see the temperament model [description](configs.md#temperaments) - Helio will use these chromatic maps as the "least common denominators" among temperaments to convert from one to another.

All scale-related tools will only show those scales with octave size matched with current temperament (the piano roll doesn't distinguish EDO's from non-EDO's, all that matters is the number of keys in octave, highlighting, key names, etc).

### Limitations

The default built-in temperaments [list](configs.md#temperaments) includes 12-edo, 19-edo, 22-edo and 31-edo.

And the built-in scales [list](configs.md#scales) only includes 7-tone scales for each of those temperaments, because there are several pieces of the app which still assume working with diatonic scales, e.g. the chord tool or the arpeggiator tool, - so given that limitation, using these tools with any built-in temperament or scale would make at least some kind of sense.

### Adding a custom temperament

To add custom temperaments and their scales, follow the guide in the [сonfigs](configs.md#user-configs) page. Use the built-in [temperaments](configs.md#temperaments) and [scales](configs.md#scales) configuration files as a reference.

## Recording MIDI from 12-tone keyboard

The app makes it possible to play and improvise on a standard 12-tone physical keyboard and record MIDI regardless of what temperament is used. For that, it uses the 'chromatic mapping' scale of the current temperament to readjust the incoming MIDI data so that all notes sound like their closest 12-tone equivalents.

Here's an example of every key of chromatic scale played on a 12-tone MIDI keyboard and recorded in a 19-edo project:

![readjust-midi-input-recording]

This feature is on by default, but it can be turned off in the audio settings section just below the MIDI device box:

![readjust-midi-input-checkbox]

If you're using a microtonal physical keyboard, make sure to uncheck this box so that the app doesn't mess up MIDI input.

[keyboard-mapping]: images/keyboard-mapping.png "Keyboard mapping page layout and menu"
[change-temperament]: images/change-temperament.png "Switching and converting temperaments"
[readjust-midi-input-recording]: images/readjust-midi-input-recording.png "Every key of one octave played on a 12-tone keyboard, recorded in 19-edo"
[readjust-midi-input-checkbox]: images/readjust-midi-input-checkbox.png "Enable/disable recording microtonal notes from 12-tone keyboard"
