### oamlStudio [![Build Status](https://travis-ci.org/oamldev/oamlStudio.svg)](https://travis-ci.org/oamldev/oamlStudio.svg?branch=master)

oamlStudio is the graphical user interface (GUI) to configure and export the music package to be used by a game that implements adaptive music using the [Open Adative Music Library](https://github.com/oamldev/oaml).


### Requirements

- wxWidgets 3.0
- libarchive
- libogg
- libvorbis
- oaml


### Compiling

1. First install the required packages:
- Ubuntu: `sudo apt install g++ cmake libwxgtk3.0-dev libogg-dev libvorbis-dev libsoxr-dev libarchive-dev`
- OS X: `brew install cmake wxwidgets libogg libvorbis libsoxr libarchive`

2. Now install OAML:
- [Open Adative Music Library](https://github.com/oamldev/oaml#how-to-compile)

3. Finally compile oamlStudio:
- On Linux and OS X: `mkdir build; cd build; cmake ..; make`
- On Windows with Visual Studio check the folder 'vs'.


### Troubleshoot

WAV files that use 8 bits data will not be resampled properly. For now you can convert the file to 16 bits and it will work.


### TODO
- Export a wav as a preview?
- Add a small tutorial/guide at start
- Add controls for missing values for tracks (groups, fades, etc)
- Implement a knob control for some values


### Credits

oamlStudio uses the following graphics:

    Icon: images/play.png images/pause.png
    Designer: Janik Baumgartner [http://kinaj.com/](http://kinaj.com/)
    License: Creative Commons Attribution


### About

Copyright (c) 2015-2018 Marcelo Fernandez

