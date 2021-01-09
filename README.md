AUDio MEasurement System
========================

Multi-platfrom system for audio measurement through sound card in the
PC.  Incorporates Generator, Oscilloscope, Fast Fourier Transform,
Sweep frequency characteristic.

email: vaclavpe@seznam.cz

Project page: https://sourceforge.net/projects/audmes/

Source code: https://sourceforge.net/p/audmes/git/ci/master/tree/


What works:
===========
 - generator
 - spectrum analyzer
 - oscilloscope
 - frequency response - just frequency stepping

 Operating systems: Linux, Windows, MacOS


Dependencies
============
 - wxWidgets 3.0.x (https://www.wxwidgets.org/)
 - RtAudio 5.1 (https://github.com/thestk/rtaudio) - is already in git repo
 - libfccp (https://github.com/ben-strasser/fast-cpp-csv-parser)
 - CMake 3.x (https://cmake.org)


Compiling
==========


Linux
-----
The following istructions are for Debian, Ubuntu and similar.

Install the basic development tools and dependencies

    apt install build-essential git
    apt install cmake libfccp-dev libwxgtk3.0-dev

Fetch the source

    git clone git clone https://git.code.sf.net/p/audmes/git audmes-git

Compile

    cd audmes-git
    mkdir build && cd build
    cmake ..
    cmake --build .

Test the result

    ./AudMeS

Package and install

    cmake --build . --target package
    sudo dpkg -i AudMeS-0.1.1-Linux.deb


Windows 10
----------
Get the following software:

https://sourceforge.net/projects/mingw-w64/files/mingw-w64/
- mingw-w64-install.exe

Install the 32 bit version:

    Version	8.1.0
    Arch	i686
    Threads	posix
    Exceptions	jlj
    Build	0

https://git-scm.com/download/win
- Git-2.26.2-64-bit.exe

https://cmake.org/download/
- cmake-3.17.2-win64-x64.msi

https://github.com/wxWidgets/wxWidgets/releases/tag/v3.0.5
- wxMSW-3.0.5_gcc810_x64_Dev.7z
- wxWidgets-3.0.5-headers.7z

https://github.com/thestk/rtaudio
- RtAudio 5.1

https://github.com/ben-strasser/fast-cpp-csv-parser
- libfccp

Compile with (git bash in source directory):

    mkdir build && cd build
    cmake .. -G "MinGW Makefiles" # run it twice
    cmake .. -G "MinGW Makefiles" # 2nd time should work
    mingw32-make


ToDo
====

General
-------
 - extend OscopeCtrl to add more traces
 - extend OscopeCtrl to add zooming of waves
 - load/save of config

Generator
---------
 - white noise

Oscilloscope
------------
 - changeable length of buffer - problem with "slower" Xscale
 - peak-to-peak measurement

Spectrum Analyzer
-----------------
 - averaging in spectrum analyzer
 - add switch between CH1 and CH2 ( left/right)

Frequency Response
------------------
 - add trace selector
 - support load/save measurements, for example as XML/CVS/JSON/IEC
   60076-18 1.0/MLSSA/FRD File Format.
 - save pictures of measurement graph
