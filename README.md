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
 - RtAudio 5.1 (https://github.com/thestk/rtaudio)
 - libfccp (https://github.com/ben-strasser/fast-cpp-csv-parser)
 - CMake 3.x (https://cmake.org)


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
 - better Y-scaling of spectrum analyzer
 - averaging in spectrum analyzer
 - add switch between CH1 and CH2 ( left/right)

Frequency Response
------------------
 - finish input peak value measurement
 - add trace selector
 - support load/save measurements, for example as XML/CVS/JSON/IEC
   60076-18 1.0/MLSSA/FRD File Format.
 - save pictures of measurement graph
