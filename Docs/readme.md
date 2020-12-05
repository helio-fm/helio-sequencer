# Introduction

Welcome to the documentation for the Helio project, a free lighweight music sequencer, which runs on the all major desktop and mobile operating systems.

Helio was designed to save me the time of struggling with the MIDI editor, so I could focus more on musical ideas. In this documentation, I'll also try to save some time and write it down as short and simple as I can.

If you notice that something important is missing, which I'm pretty sure it is, send me an angry [email](mailto:peter.rudenko@gmail.com), or create a [PR](https://github.com/helio-fm/helio-workstation/pulls) on the Github.

#### How to read this

If you're starting out with Helio, just continue with the [Getting Started](getting-started.md) page to learn the basic concepts of UI navigation, setting up [instruments](getting-started.md#instruments), [creating](getting-started.md#creating-a-project) a project, [editing and arranging](getting-started.md#editing-and-arranging) it and saving your work in the [version control](getting-started.md#version-control).

Otherwise, you might find useful the [editing tips](tips-and-tricks.md): the majority of all the nifty tools and hacks will be described there.

If you've already tinkered with Helio for a while, you'll probably only need the [hotkeys](hotkeys.md) section.


## Installation

Helio is released as a single portable executable file, where possible, but installers are also available for some systems.
In both flavours it has small disk footprint (less than 10 Mb) and should run on most available hardware and platform versions.

### Portable vs installer

Installers are provided for Windows and Debian-based Linux distributions. The installer version is only needed, if you want to have the desktop shortcut and the uninstall tool.

With portable version, just download the compressed archive and unzip it to a folder of your choice.

### 32 or 64 bits

The only practical difference between the two is that the 64-bit version will only be able to host 64-bit plugins, and the 32-bit version will only host 32-bit plugins.

At the moment of writing this, Helio does not support plugin sandboxing or hosting both 32-bit and 64-bit plugins at the same time.

### Master build or development build

Master builds are the latest stable versions. You can keep up to date with the latest changes through the [release notes](changelog.md). If you'd like to preview the latest features or verify bug fixes, you can install the development build.


## Configuration files

Helio keeps all files in two directories: one for the project files, and one for the configuration files.

Note that the installer-based versions don't remove any of these when uninstalling the app. If you want to remove Helio from the system completely, you need to delete them manually.

### The configuration directory

All the configuration files are created on the first start under the user application data directory. The directory is a platform-dependent location:

* Windows: %APPDATA%\Helio
* macOS: ~/Library/Application Support/Helio
* Linux: ~/.config/Helio

#### `settings.helio`

This file basically contains all the settings, so if you delete or rename it, the app would run as if it was the first time. The settings are supposed to be human-readable and are stored in XML format.

#### `translations.helio` and maybe others

Some additional configuration resources are dynamically updated in the runtime, if the newer version is available. At the moment of writing, only translations are updated this way.

In future, the app may sync more configs, including default scales, chords, arpeggiators, hotkey schemes, colour schemes, etc.

### The projects directory

The projects files are created in the `Helio` subfolder of the user's default documents folder. The location of this directory is also platform-dependent:

* Windows: %HOMEPATH%\Helio (for example, "c:\Users\Peter\Documents\Helio\")
* macOS: ~/Documents/Helio
* Linux: ~/Documents/Helio

## Building from source

Building the app from source will require a C++11 compiler, e.g. GCC 4.8 on Linux, Visual Studio 2015 on Windows, or Xcode 7.3.1 on macOS.

Minimum deployment targets are:

* Windows: Windows 7
* macOS: macOS 10.7
* Linux: mainstream distributions

#### Basic build instructions

* Clone with submodules: `git clone --recurse-submodules https://github.com/helio-fm/helio-workstation.git`.
* Install dependencies:
  * On Windows, get ASIO SDK (which can't be redistributed in this project due to licensing restrictions, but you may use `ThirdParty/ASIO/get_asio_sdk.ps1` powershell script to download and extract the SDK source).
  * On Linux, you'll need to have the following packages installed: `libfreetype6-dev libx11-dev libxinerama-dev libxrandr-dev libxcursor-dev libxcomposite-dev mesa-common-dev freeglut3-dev libcurl4-openssl-dev libasound2-dev libjack-dev libc++-dev`; the makefile assumes you've set up either `export CONFIG=Debug`, `export CONFIG=Release32` or `export CONFIG=Release64` before you `make`.
* Pick the right project for your OS from the `Projects` directory and build.
