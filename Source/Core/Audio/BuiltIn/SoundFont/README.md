The SoundFont synth implementation in this folder is largely based on SFZero by Steve Folta, extended by Leo Olivers and Cognitone, all distributed under MIT license:

Original code copyright (C) 2012 Steve Folta
https://github.com/stevefolta/SFZero

Converted to Juce module (C) 2016 Leo Olivers
https://github.com/altalogix/SFZero

Extended for multi-timbral operation (C) 2017 Cognitone
https://github.com/cognitone/SFZeroMT

The additions on top of that are:
 * support arbitrary (microtonal) equal temperaments by readjusting sample pitches
 * support SF3/SF4 format with OGG/FLAC-compressed samples
 * fixed voices not playing all regions while playing some of them twice sometimes
 * fixed min/max envelope values according to specs
