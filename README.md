# AUDio MEasurement System

Multi-platfrom system for audio measurement through sound card in the
PC.  Incorporates Generator, Oscilloscope, Fast Fourier Transform,
Sweep frequency characteristic.

Project page: <https://sourceforge.net/projects/audmes/>

Source code: <https://sourceforge.net/p/audmes/git/ci/master/tree/>

Contact: see project page

Features:

- generator
- spectrum analyzer
- oscilloscope
- frequency response - just frequency stepping

Operating systems: Linux, Windows, MacOS

Dependencies:

- wxWidgets 3.0.x (<https://www.wxwidgets.org/>)
- RtAudio 5.1 (<https://github.com/thestk/rtaudio>) - is already in git repo
- libfccp (<https://github.com/ben-strasser/fast-cpp-csv-parser>)
- CMake 3.x (<https://cmake.org>)

## Compiling

### Linux

The following instructions are for Debian, Ubuntu and similar.

Install the basic development tools and dependencies

    sudo apt install build-essential git
    sudo apt install libwxgtk3.0-dev # libwxgtk3.0-gtk3-dev in Debian 11
    sudo apt install cmake libfccp-dev libpulse-dev

Fetch the source

    git clone https://git.code.sf.net/p/audmes/git audmes-git

Compile

    cd audmes-git
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build .

Test the result

    ./AudMeS

Package and install

    cmake --build . --target package
    sudo dpkg -i AudMeS-0.1.1-Linux.deb

### Windows 10 with MinGW

Get the following software:

MinGW <https://sourceforge.net/projects/mingw-w64/files/mingw-w64/>

- mingw-w64-install.exe

Install the 32 bit version:

    Version     8.1.0
    Arch        i686
    Threads     posix
    Exceptions  sjlj
    Build       0

If you have a previous MinGW installation, say in "C:\MinGW",
take care not to the two. See setting the path below or simply
uninstall the previous installation.

Git <https://git-scm.com/download/win>

- Git-2.26.2-64-bit.exe

Cmake <https://cmake.org/download/>

- cmake-3.17.2-win64-x64.msi

The next instructions assume that you use git bash (not command.exe).
Use the desktop shortcut "Git bash" you get after after installing git.

Create a directory to store the project.

    $ENV{HOMEDRIVE}$ENV{HOMEPATH}/projects

WxWidgets <https://github.com/wxWidgets/wxWidgets/releases/tag/v3.0.5>

- wxMSW-3.0.5_gcc810_Dev.7z
- wxWidgets-3.0.5-headers.7z

Extract the archives to $ENV{HOMEDRIVE}$ENV{HOMEPATH}/projects/wx3.0.5.

Clone audmes:

    cd $ENV{HOMEDRIVE}$ENV{HOMEPATH}/projects
    git clone https://git.code.sf.net/p/audmes/git audmes-git

FCCP <https://github.com/ben-strasser/fast-cpp-csv-parser>

- libfccp

Clone into $ENV{HOMEDRIVE}$ENV{HOMEPATH}/projects/audmes-git/libfccp

Make sure you have mingw and cmake in your bash path.
You need to add the following using the windows
"System Properties->Environment Variables" window.
    "C:\Program Files (x86)\mingw-w64\i686-8.1.0-posix-sjlj-rt_v6-rev0\mingw32\bin"
    "C:\Program Files\CMake\bin"
or add it to the path in the bash profile.

Compile 32bit Windows version (git bash in source directory):

    cd $ENV{HOMEDRIVE}$ENV{HOMEPATH}/projects/audmes-git
    mkdir build && cd build
    cmake .. -G "MinGW Makefiles" # run it twice
    cmake .. -G "MinGW Makefiles" # 2nd time should work
    mingw32-make

Install libraries:

    cp /c/Program\ Files\ \(x86\)/mingw-w64/i686-8.1.0-posix-sjlj-rt_v6-rev0/mingw32/bin/libwinpthread-1.dll .
    cp /c/Program\ Files\ \(x86\)/mingw-w64/i686-8.1.0-posix-sjlj-rt_v6-rev0/mingw32/bin/libstdc++-6.dll .
    cp /c/Program\ Files\ \(x86\)/mingw-w64/i686-8.1.0-posix-sjlj-rt_v6-rev0/mingw32/bin/libgcc_s_sjlj-1.dll .
    cp ../../wx3.0.5/lib/gcc810_dll/wxbase30u_gcc810.dll .
    cp ../../wx3.0.5/lib/gcc810_dll/wxmsw30u_core_gcc810.dll .

Run the program:

    AudMeS.exe

## Troubleshooting

If a popup appears about sound card issues,
make sure you have stereo input and output available.
You may need to plug in a cable.
Mono channels on sound cards are not yet supported, sorry.

If you see messages like
`WARNING **: invalid source position for vertical gradient`
then your Gnome Theme has a bug. It is annoying but harmless.
On Debian the issue disappears when using e.g. `materia-gtk-theme`.

If you get an error like `wxbase30ud_gcc810.dll not found` (note the "d_")
when running AudMeS.exe, you compiled in Debug mode. Run cmake again with
"-DCMAKE_BUILD_TYPE=Release".
