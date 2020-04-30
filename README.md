AUDio MEasurement System
========================

Multi-platfrom system for audio measurement through sound card in the
PC.  Incorporates Generator, Oscilloscope, Fast Fourier Transform,
Sweep frequency characteristic.

email: vaclavpe@seznam.cz

Project page: https://sourceforge.net/projects/audmes/

What works:
===========
 - generator
 - spectrum analyzer
 - oscilloscope
 - frequency response - just frequency stepping

ToDo
====

General
-------

 - clean up RWAudio file - rewrite to RTAudio lib ??
 - porting to Linux - ALSA architecture - rewrite to RTAudio lib ??
 - extend OscopeCtrl to add more traces
 - extend OscopeCtrl to add zooming of waves
 - load/save of config

Generator
---------

Oscilloscope
------------
 - changeable length of buffer - problem with "slower" Xscale
 - peak-to-peak measurement

Spectrum Analyzer
-----------------
 - better Y-scaling of spectrum analyzer
 - averaging in spectrum analyzer
 - add switch between CH1 and CH2 ( left/right)
 - extend the range of SA to 20kHz - OscopeCtrl problem
 - add filtering windows - is it necessary ??
 - changeable FFT length

Frequency Response
------------------
 - finish input peak value measurement
 - add trace selector
 - support load/save measurements, for example as XML/CVS/JSON/IEC
   60076-18 1.0/MLSSA/FRD File Format.
 - save pictures of measurement graph
 - measure frequencies 10k-20k
