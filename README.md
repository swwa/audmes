# AUDio MEasurement System

## About

Multi-platfrom system for audio measurement through sound card in the
PC.  Incorporates Generator, Oscilloscope, Fast Fourier Transform,
Sweep frequency characteristic.

Project page: <https://sourceforge.net/projects/audmes/>

Source code: <https://sourceforge.net/p/audmes/git/ci/master/tree/>

Contact: see project page

License: GPLv2 (see COPYING)

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

## Installation

You can download precompiled binaries from SourceForge. You find them unter `Files`.
You need a stereo input and output on your sound card.

### Windows

Download the binary from SourceForge. It comes a a ZIP archive.
Uncompress the archive into a directory of your choise.
Create a shortcut for AudMeS.exe to your desktop.
To run, double-click on the shortcut icon.

### MacOS

Download the binary from SourceForge. It comes as a disk image (dmg).
Mount the disk image by double-clicking it.
Drag and drop the app to /Applications.
To run, use LaunchPad and click on the AudMeS icon.

### Linux

Download the binary from SourceForge. It comes as a Debian package (deb).
Install the package with `dpkg -i`.
To run, click on the AudMeS icon when you select "Show Applications".

## Compiling on Linux

The following instructions are for Debian, Ubuntu and similar.

Install the basic development tools and dependencies

    sudo apt install build-essential git

    # Debian 10
    sudo apt install libwxgtk3.0-dev

    # Debian 11
    sudo apt install libwxgtk3.0-gtk3-dev

    sudo apt install cmake libfccp-dev libpulse-dev

Fetch the source

    git clone https://git.code.sf.net/p/audmes/git audmes-git

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

## Compiling on Windows 10 with MinGW

Get the following software:

MinGW:

- Get `mingw-w64-install.exe` from <https://sourceforge.net/projects/mingw-w64/files/mingw-w64/>

Setup MinGW to i686 (32 bit) version:

    Version     8.1.0
    Arch        i686
    Threads     posix
    Exceptions  sjlj
    Build       0

If you have a previous MinGW installation, say in "C:\MinGW",
take care not to mix the two. See setting the path below or simply
uninstall the previous installation.

Git:

- Get `Git-2.26.2-64-bit.exe` from <https://git-scm.com/download/win>

Cmake:

- Get `cmake-3.17.2-win64-x64.msi` from  <https://cmake.org/download/>

The next instructions assume that you use git bash (not command.exe).
Use the desktop shortcut "Git bash" you get after after installing git.

Create a directory to store the project.

    $ENV{HOMEDRIVE}$ENV{HOMEPATH}/projects

WxWidgets:

- Get `wxMSW-3.0.5_gcc810_Dev.7z` and `wxWidgets-3.0.5-headers.7z`
from  <https://github.com/wxWidgets/wxWidgets/releases/tag/v3.0.5>

Extract the archives to `$ENV{HOMEDRIVE}$ENV{HOMEPATH}/projects/wx3.0.5`.
CMake expects the libraries in `...\projects\wx3.0.5\lib\gcc_dll`.
Copy or rename `...\projects\wx3.0.5\lib\gcc810_dll`.

Clone audmes:

    cd $ENV{HOMEDRIVE}$ENV{HOMEPATH}/projects
    git clone https://git.code.sf.net/p/audmes/git audmes-git

Clone libfccp:

    git clone https://github.com/ben-strasser/fast-cpp-csv-parser libfccp

Compiling:

Make sure you have mingw and cmake in your bash path.
You need to add the following using the windows
"System Properties->Environment Variables" window.

    "C:\Program Files (x86)\mingw-w64\i686-8.1.0-posix-sjlj-rt_v6-rev0\mingw32\bin"
    "C:\Program Files\CMake\bin"

or add it to the path in the bash profile.

Compile 32bit Windows version (git bash in source directory):

    cd $ENV{HOMEDRIVE}$ENV{HOMEPATH}/projects/audmes-git
    mkdir build && cd build
    cmake .. -G "MinGW Makefiles"
    mingw32-make

Install libraries:

    cp /c/Program\ Files\ \(x86\)/mingw-w64/i686-8.1.0-posix-sjlj-rt_v6-rev0/mingw32/bin/libwinpthread-1.dll .
    cp /c/Program\ Files\ \(x86\)/mingw-w64/i686-8.1.0-posix-sjlj-rt_v6-rev0/mingw32/bin/libstdc++-6.dll .
    cp /c/Program\ Files\ \(x86\)/mingw-w64/i686-8.1.0-posix-sjlj-rt_v6-rev0/mingw32/bin/libgcc_s_sjlj-1.dll .
    cp ../../wx3.0.5/lib/gcc810_dll/wxbase30u_gcc810.dll .
    cp ../../wx3.0.5/lib/gcc810_dll/wxmsw30u_core_gcc810.dll .

Alternative to the above:

    cmake --build . --target install

Run the program:

    AudMeS.exe

Package into a ZIP:

    cmake --build . --target package

The ZIP archive contains the executable, libraries and text files for distribution.

## Compiling on Windows 10 with Visual Studio 2019

Download Visual Studio 2019 from Microsoft.
The (free) Visual Studio 2019 Community edition is sufficient.
It comes with the necessary C++ compilers, SDK, git client and CMake plugin.

Cmake:

- Get `cmake-3.17.2-win64-x64.msi` from  <https://cmake.org/download/>

WxWidgets:

- Get `wxMSW-3.0.5_vc142_Dev.7z` and `wxWidgets-3.0.5-headers.7z`
from  <https://github.com/wxWidgets/wxWidgets/releases/tag/v3.0.5>

Extract the archives to `$ENV{HOMEDRIVE}$ENV{HOMEPATH}/projects/wx3.0.5`.

Use Visual Studio to clone the project.

Create a CMake configuration `x86-release`.

    {
      "configurations": [
        {
          "name": "x86-Release",
          "generator": "Ninja",
          "configurationType": "Release",
          "buildRoot": "${projectDir}\\build\\${name}",
          "installRoot": "${projectDir}\\out\\install\\${name}",
          "cmakeCommandArgs": "",
          "buildCommandArgs": "",
          "ctestCommandArgs": "",
          "inheritEnvironments": [ "msvc_x86" ]
        }
      ]
    }

Now you can build AudMeS.exe with Visual Studio by selecting "Build -> Build All".

## Compiling on MacOSX

Xcode:

- Install Xcode from the App Store. It comes with the necessary compilers and tools for MacOSX.

Tool to fix paths in App: `install_name_tool`.
The MacOSX equivalent for Linux `ldd` is `otool`.

CMake:

- Download "cmake-3.16.0-rc3-Darwin-x86_64.dmg" from <https://cmake.org/download/>

Homebrew:

- Install Homebrew <https://brew.sh> first and then use `brew` to install wxWidgets

Turn off analytics

    brew analytics off

You may want to get optional install git tools

    brew install git
    brew install git-gui

wxWidgets:

Now use `brew` to get  wxWidgets

    brew install wxwidgets

Brew will get version 3.1 which is fine. On MacOSX wxWidgets 3.0 has issues with AudMeS.

AudMeS source:

    mkdir projects && cd projects
    git clone https://git.code.sf.net/p/audmes/git audmes-git
    cd audmes-git

Libfccp:

Clone into projects/audmes-git/libfccp

    git clone https://github.com/ben-strasser/fast-cpp-csv-parser libfccp

Compile on command line:

    mkdir build && cd build
    cmake ..
    cmake --build .

During compilation some deprecation warnings appear which can be ignored.

Now you can run the result `AudMeS.app` using the Finder. It will ask for permission
to access the microphone. Please allow it.

Package AudMeS as disk image:

    cmake --build . --target package

The result is `AudMeS-0.1.1-Darwin.dmg` which can be mounted using the Finder and allows
to install the app in the usual way.

Using Xcode as IDE:

To prepare the project for Xcode and debugging you can use Cmake.

    cmake .. -G Xcode -DCMAKE_BUILD_TYPE=Debug

Then open the project in XCode. You may need to remove all in the `build` directory
first before running cmake.

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
