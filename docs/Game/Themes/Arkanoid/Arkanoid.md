# Arkanoid - Theme

I am a huge fan of the game Arkanoid. I love the gameplay, the graphics, and the music. I have played it many times and
I still enjoy it. I think it is a classic game that will never get old.

https://www.spieleklassiker.com/spiel/Arkanoid

https://www.spriters-resource.com/arcade/arkanoid/

https://www.youtube.com/watch?v=fA6cfcL7sm8

See mame

I want to create a theme. Will add spritesheets.

## Sound, Sound-Emulators and sound-modules

https://vgmrips.net/packs/pack/arkanoid-arcade

Ayumi
Ayumi (C++): As discussed, this is a great library for accurate AY-3-8910 emulation. You'd feed it register write
commands (from a .vgm file or your own code) and get PCM back.

https://github.com/true-grue/ayumi

The sound chip responsible for Arkanoid's audio was the AY-3-8910 (or a clone like the YM2149). This is a very common
programmable sound generator (PSG) chip from that era, known for its ability to produce:

Square waves: Used for melodies and bass lines.
Triangle waves: Often used for bass or smoother tones.
Noise channels: Essential for percussion, explosions, and other sound effects.

These chips generate sounds by manipulating waveforms mathematically, rather than playing back pre-recorded digital
audio (samples). This was the standard for arcade games and home computers of that period due to limitations in memory
and processing power for storing and playing samples.

MAME, ZX Spectrum Emulators, Amstrad CPC Emulators, MSX Emulators and Atari ST Emulators can emulate it.

Standalone AY/YM Players/Trackers:
There are dedicated software players and trackers designed specifically for chiptunes created on AY/YM chips. These
allow you to play .AY, .YM, .PT3 (Pro Tracker 3), and other formats. They often include their own AY/YM emulation
engine. Examples include:

AVR-AY Player: This is a popular player that also has a corresponding hardware emulator project.
Vortex Tracker II: While primarily a music creation tool, it has an integrated AY/YM player.
AYM.JS: A JavaScript-based emulator and player that runs in a web browser

LV2 for Linux
chiptune" or "8-bit

Core Concepts of Emulating a PSG (Programmable Sound Generator)

Peter Sovietov's Ayumi Library

src/devices/sound/ay8910.cpp

Using MAME's Logging Capabilities

VGM Logging: MAME can log the raw register writes to the sound chips into a .VGM (Video Game Music) file. This file
essentially captures all the commands sent to the sound chip

Launch MAME from the command line (or a shortcut).
Run Arkanoid with the -vgm switch: mame arkanoid -vgm
Play through the game. MAME will create a .vgm file in your MAME root directory (often vgm/arkanoid.vgm).
What you get: A .vgm file doesn't contain audio directly, but rather the "instructions" for the sound chip.
Next step: You'll need a VGM player/converter. Programs like VGMPlay or VGMToolbox (Windows) can play these files and
export them to WAV. This gives you the exact synthesized sound as the chip produced it. VGMToolbox is particularly
powerful for trimming, looping, and converting.
mame arkanoid -wavwrite arkanoid_audio.wav

Tools like M1 (Music Player for Arcade Games) are specifically designed to play and extract music from arcade ROMs.

Place your arkanoid.zip ROM file in M1's roms folder.

Launch M1. It should detect Arkanoid and list its music and sound effects as separate tracks.

VGMRips.net:

VGMPlay

https://github.com/ValleyBell/libvgm
https://github.com/vgmrips/vgmplay-legacy

MOD, IT, XM, S3M

mod, .ay, .sid

SDL_mixer : WAV, OGG, MP3, MOD, XM, S3M, IT

libmodplug

Miniaudio (C)

libsidplayfp

https://github.com/lexus2k/vgm_decoder
VGM/NSF decoder
vgm files (AY-3-8910, YM2149 (MSX2) and NES APU (Nes console))
nsf files (Nintendo Sound Format - NES APU)

https://www.zophar.net/music
https://vgmrips.net/

https://www.youtube.com/watch?v=fA6cfcL7sm8&t=352s
