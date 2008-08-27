2008-08-27
----------
- sound card switching works; but problems when using different sound cards
for input and output

2008-08-26
----------
- changed colors for numbers in scopes
- OSC - L channel measures frequency - FFT
- "Audio Interface" Dlg extended with sampling frequency
- "Audio Interface" Dlg shows available interfaces but settings do not work yet

2008-08-25
----------
- first Linux port!!! - compilable with wxGTK-2.6.4 wth GTK2 and UNICODE; a plenty of problems:
   - GEN gauges do not work as expected
   - glitches in generator
   - wxTextCtrls do not react to ENTERs
- SA - variable FFT length
- SA - right channel is added
- SA - averaging of FFT values introduced

2008-08-22
----------
- renamed OScopeCtrl->CtrlOScope; double buffering and some other features added
- #define for switch XScale to microseconds/div - but a lot of problems with strange modulation

2008-08-21
----------
- first working version of AudMeS with RtAudio driver - MinGW compiler
- Select Sound Card dlg does not work; default device is used

2008-08-20
----------
- rewrite of audio driver started - RtAudio will be used now

2008-08-15
----------
- added rectangular, triangle, saw generators
- added auto calibration of Y scale ; offset is not changing
- prepared for load/save of configuration
- added menu accelerators

2008-06-06
----------
- triggering completely changed; currently missing control of trigger level and trigger hysteresis
- changed description of X scale and X resolution
- in Makefile.gcc changed directory delimiters in "clean" target

2008-06-04
----------
- Added text controls to set generator frequency more preciously
- phase between L and R channels is now in degrees (before it was radians)

2007-07-25
----------
Project renamed to AUDio MEasurement System - AudMeS

2007-07-15
----------
- RWAudio_win - velky prepis modulu
- Sweep funguje - netestovano v realnem prostredi

2007-07-12
----------
- AudioLab.cpp: OSC - Amplituda zmenena na dB
- AudioLab.cpp: OSC - synchronizace L a P kanalu
- RWAudio.h: pridan parametr faze do PlaySetGenerator()
- RWAudio.cpp: pridana glob_phase1 a glob_phase2

2007-03-27
----------
- audiolab.h: zmena zobrazeni verze - ta je ted v AUDIOLAB_VERSION_STRING
- AudioLab.cpp: pridan vyber SND karty - NEDOKONCENO
- dlg_audiointerface.cpp: NOVY - dialog vyberu IN/OUT zarizeni
- RWAudio_win.cpp: pridano GetPlayDev() a GetRecDev()
- RWAudio_win.cpp: slouceni Rekorderu a Playeru - NEDOKONCENO, NEKONZISTENTNI!!!

Audio Laboratory v0.1 alpha
===========================

email: vaclavpe@seznam.cz

Today is: 2006-04-19

What works:
===========
- generator
- spectrum analyzer
- oscilloscope
- frequency response - just frequency stepping

ToDo
====
General:
- clean up RWAudio file - rewrite to RTAudio lib ??
- porting to Linux - ALSA architecture - rewrite to RTAudio lib ??
- extend OscopeCtrl to add more traces
- extend OscopeCtrl to add zooming of waves
- load/save of config

Generator:

Oscilloscope:
- changeable length of buffer - problem with "slower" Xscale
- peak-to-peak measurement

SA:
- better Y-scaling of spectrum analyzer
- averaging in spectrum analyzer
- add switch between CH1 and CH2 ( left/right)
- extend the range of SA to 20kHz - OscopeCtrl problem
- add filtering windows - is it necessary ??
- changeable FFT length

Freq Response:
- finish input peak value measurement
- add trace selector
