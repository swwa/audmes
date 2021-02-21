//////////////////////////////////////////////////////////////////////
// RWAudio_IO.cpp: impementation of audio interface
//
//////////////////////////////////////////////////////////////////////
/*
 * Copyright (C) 2008 Vaclav Peroutka <vaclavpe@seznam.cz>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "RWAudio_IO.h"

#include <math.h>
#include <stdio.h>
#include <wx/defs.h>  // for WXUNUSED

//#define _DEBUG

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
FILE *ddbg;
#endif

// extern variables from AudMeS.cpp
extern short *g_OscBuffer_Left;
extern short *g_OscBuffer_Right;
extern long int g_OscBufferPosition;

extern short *g_SpeBuffer_Left;
extern short *g_SpeBuffer_Right;
extern long int g_SpeBufferPosition;

extern int g_OscBufferChanged;
extern int g_SpeBufferChanged;

/*
 * pseudo noise generator - linear feedback shift register
 * derived from https://en.wikipedia.org/wiki/Linear-feedback_shift_register
 */
bool lfsr16() {
  uint16_t bit;             /* Must be 16-bit to allow bit<<15 later in the code */
  static uint16_t lfsr = 1; /* Must not be 0 */

  /* taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
  bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1u;
  lfsr = (lfsr >> 1) | (bit << 15);
  return bit;
}

/*
 * callback function to fetch audio input and generate tones
 */
int inout(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
          double WXUNUSED(streamTime), RtAudioStreamStatus WXUNUSED(status), void *data) {
#ifdef _DEBUG
  fprintf(ddbg, "Jsme tady ");
#endif

  RWAudio *aRWAudioClass = (RWAudio *)data;
  unsigned long i;

  // copy input buffer into two L/R channels
  if (0 != aRWAudioClass->m_Buflen_Changed) {
    aRWAudioClass->m_Buflen_Changed = 0;
    free(g_OscBuffer_Left);
    free(g_OscBuffer_Right);
    free(g_SpeBuffer_Left);
    free(g_SpeBuffer_Right);
    g_OscBufferPosition = 0;
    g_SpeBufferPosition = 0;
    g_OscBuffer_Left = (short *)malloc(aRWAudioClass->m_OscBufferLen * sizeof(short));
    g_OscBuffer_Right = (short *)malloc(aRWAudioClass->m_OscBufferLen * sizeof(short));
    g_SpeBuffer_Left = (short *)malloc(aRWAudioClass->m_SpeBufferLen * sizeof(short));
    g_SpeBuffer_Right = (short *)malloc(aRWAudioClass->m_SpeBufferLen * sizeof(short));
  }

  short *inBuf = (short *)inputBuffer;

  // make a copy for oscilloscope
  if (0 == g_OscBufferChanged) {
    for (i = 0; i < nBufferFrames; i++) {
      // copy audio signal to fft real component.
      g_OscBuffer_Left[g_OscBufferPosition] = *inBuf++;
      g_OscBuffer_Right[g_OscBufferPosition] = *inBuf++;

      g_OscBufferPosition++;
      // if the buffer is over we have to pick the data and then circullary fill the next one
      if (g_OscBufferPosition == aRWAudioClass->m_OscBufferLen) {
        g_OscBufferPosition = 0;
        g_OscBufferChanged = 1;
        break;
      }
    }
  }

  inBuf = (short *)inputBuffer;
  // make a copy for spectrum analyzer
  if (0 == g_SpeBufferChanged) {
    for (i = 0; i < nBufferFrames; i++) {
      // copy audio signal to fft real component.
      g_SpeBuffer_Left[g_SpeBufferPosition] = *inBuf++;
      g_SpeBuffer_Right[g_SpeBufferPosition] = *inBuf++;

      g_SpeBufferPosition++;
      // if the buffer is over we have to pick the data and then circullary fill the next one
      if (g_SpeBufferPosition == aRWAudioClass->m_SpeBufferLen) {
        g_SpeBufferPosition = 0;
        g_SpeBufferChanged = 1;
        break;
      }
    }
  }

  // fill output buffer
  short *outBuf = (short *)outputBuffer;

#ifdef _DEBUG
  fprintf(ddbg, "\n Frames: %d\n ", nBufferFrames);
#endif

  /* calculate the wave form according to the selected shape */
  for (unsigned long i = 0; i < nBufferFrames; i++) {
    double y = 0;
    double y2 = 0;
    bool noise = lfsr16();

    /* left channel */
    switch (aRWAudioClass->m_genShape_l) {
      case 1:
        if (aRWAudioClass->m_genFI_l < M_PI) {
          y = 1.0 * aRWAudioClass->m_genGain_l;
        } else {
          y = 1.0 * -aRWAudioClass->m_genGain_l;
        }
        break;
      case 2:
        y = aRWAudioClass->m_genGain_l * (aRWAudioClass->m_genFI_l - M_PI) / M_PI;
        break;
      case 3:
        if (aRWAudioClass->m_genFI_l < M_PI) {
          y = aRWAudioClass->m_genGain_l * 2 * (aRWAudioClass->m_genFI_l - M_PI / 2) / M_PI;
        } else {
          y = aRWAudioClass->m_genGain_l * 2 * (3 * M_PI / 2 - aRWAudioClass->m_genFI_l) / M_PI;
        }
        break;
      case 4:
        if (noise) {
          y = 1.0 * aRWAudioClass->m_genGain_l;
        } else {
          y = 1.0 * -aRWAudioClass->m_genGain_l;
        }
        break;
      default: /* sine wave */
        y = aRWAudioClass->m_genGain_l * sin(aRWAudioClass->m_genFI_l);
        break;
    }
    /* right channel */
    switch (aRWAudioClass->m_genShape_r) {
      case 1:
        if ((aRWAudioClass->m_genFI_r - aRWAudioClass->m_genPhaseDif) < M_PI) {
          y2 = aRWAudioClass->m_genGain_r;
        } else {
          y2 = -aRWAudioClass->m_genGain_r;
        }
        break;
      case 2:
        y2 = aRWAudioClass->m_genGain_r *
             ((aRWAudioClass->m_genFI_r - aRWAudioClass->m_genPhaseDif) - M_PI) / M_PI;
        break;
      case 3:
        if (aRWAudioClass->m_genFI_r < M_PI) {
          y2 = aRWAudioClass->m_genGain_r * 2 *
               (aRWAudioClass->m_genFI_r - aRWAudioClass->m_genPhaseDif - M_PI / 2) / M_PI;
        } else {
          y2 = aRWAudioClass->m_genGain_r * 2 *
               (3 * M_PI / 2 - aRWAudioClass->m_genFI_r + aRWAudioClass->m_genPhaseDif) / M_PI;
        }
        break;
      case 4:
        if (noise) {
          y2 = 1.0 * aRWAudioClass->m_genGain_r;
        } else {
          y2 = 1.0 * -aRWAudioClass->m_genGain_r;
        }
        break;
      default: /* sine wave */
        y2 = aRWAudioClass->m_genGain_r *
             sin(aRWAudioClass->m_genFI_r - aRWAudioClass->m_genPhaseDif);
        break;
    }

    aRWAudioClass->m_genFI_l +=
        (float)2.0 * M_PI * aRWAudioClass->m_genFR_l / aRWAudioClass->m_sampleRate;
    aRWAudioClass->m_genFI_r +=
        (float)2.0 * M_PI * aRWAudioClass->m_genFR_r / aRWAudioClass->m_sampleRate;

    if ((2.0 * M_PI) < aRWAudioClass->m_genFI_l) aRWAudioClass->m_genFI_l -= 2.0 * M_PI;
    if ((2.0 * M_PI) < aRWAudioClass->m_genFI_r) aRWAudioClass->m_genFI_r -= 2.0 * M_PI;

    /* freq 0.0 - turn sound off */
    if (aRWAudioClass->m_genFR_l == 0.0) {
      *outBuf++ = 0;
    } else {
      *outBuf++ = (short)(32768.f * y);
    }
    if (aRWAudioClass->m_genFR_r == 0.0) {
      *outBuf++ = 0;
    } else {
      *outBuf++ = (short)(32768.f * y2);
    }

#ifdef _DEBUG
    // fprintf(ddbg,"%04X %04X ",(short)(32768.f * y), (short)(32768.f * y2));
#endif
  }
  return 0;
}

RWAudio::RWAudio() {
  m_DrvRunning = 0;
  m_Buflen_Changed = false;
  m_sampleRate = 44100;
}

RWAudio::~RWAudio() {
  m_AudioDriver.stopStream();
  m_AudioDriver.closeStream();
}

// Initialize RtAudio and start audio stream
int RWAudio::InitSnd(long int oscbuflen, long int spebuflen) {
  m_OscBufferLen = oscbuflen;
  m_SpeBufferLen = spebuflen;

  m_genFR_l = m_genFR_r = 0.0;
  m_genShape_l = m_genShape_r = 0;
  m_genGain_l = m_genGain_r = 1.0;
  m_genFI_l = m_genFI_r = 0.0;
  m_genPhaseDif = 0.0;

  g_OscBufferPosition = 0;
  g_SpeBufferPosition = 0;
  g_OscBuffer_Left = (short *)malloc(m_OscBufferLen * sizeof(short));
  g_OscBuffer_Right = (short *)malloc(m_OscBufferLen * sizeof(short));
  g_SpeBuffer_Left = (short *)malloc(m_SpeBufferLen * sizeof(short));
  g_SpeBuffer_Right = (short *)malloc(m_SpeBufferLen * sizeof(short));
  // init of RtAudio
  if (m_AudioDriver.getDeviceCount() < 1) return 1;
  // start audio streams
  if (RestartAudio(m_AudioDriver.getDefaultInputDevice(), m_AudioDriver.getDefaultOutputDevice())) {
    return 2;
  }
  return 0;
}

int RWAudio::RestartAudio(int recDevId, int playDevId) {
  // if stream is open (and running), stop it
  if (m_AudioDriver.isStreamOpen()) {
    m_AudioDriver.stopStream();
    m_AudioDriver.closeStream();
  }

  // configure new stream
  RtAudio::StreamParameters iParams;
  RtAudio::StreamParameters oParams;
  RtAudio::StreamOptions rtAOptions;
  unsigned int bufferFrames = 512;

  iParams.deviceId = recDevId;
  oParams.deviceId = playDevId;
  iParams.nChannels = 2;
  oParams.nChannels = 2;
  iParams.firstChannel = 0;
  oParams.firstChannel = 0;

  rtAOptions.flags |= RTAUDIO_NONINTERLEAVED;

  try {
    m_AudioDriver.openStream(&oParams, &iParams, RTAUDIO_SINT16, m_sampleRate, &bufferFrames,
                             &inout, (void *)this);  //, &rtAOptions );
  } catch (RtAudioError &e) {
    // std::cerr << '\n' << e.getMessage() << '\n' << std::endl;
    return 1;
  }
  m_bufferBytes = bufferFrames * iParams.nChannels * sizeof(short);

  m_AudioDriver.startStream();

  m_DrvRunning = 1;

  return 0;
}

/********************************************************************/
/*************      Devices enumeration           *******************/
/********************************************************************/
int RWAudio::GetRWAudioDevices(RWAudioDevList *play, RWAudioDevList *record) {
  // Determine the number of devices available
  unsigned int devices = m_AudioDriver.getDeviceCount();

  // Scan through devices for various capabilities
  RtAudio::DeviceInfo info;

  play->card_info.clear();
  record->card_info.clear();
  play->card_pos.clear();
  record->card_pos.clear();

  for (unsigned int i = 0; i < devices; i++) {
    info = m_AudioDriver.getDeviceInfo(i);

    if (info.probed == true) {
      //      std::cout << "device = " << i << "; name: " << info.name << "\n";

      // add play card
      if ((info.outputChannels > 1) || (info.duplexChannels > 1)) {
        play->card_info.push_back(info);
        play->card_pos.push_back(i);
      }

      // add record card
      if ((info.inputChannels > 1) || (info.duplexChannels > 1)) {
        record->card_info.push_back(info);
        record->card_pos.push_back(i);
      }
    }
  }

  return 0;
}

/********************************************************************/
/*************      Parameter settings            *******************/
/********************************************************************/
int RWAudio::PlaySetGenerator(float f1, float f2, int s1, int s2, float g1, float g2) {
  if (2 * f1 < m_sampleRate)
    m_genFR_l = f1;
  else
    m_genFR_l = m_sampleRate / 2;
  if (2 * f2 < m_sampleRate)
    m_genFR_r = f2;
  else
    m_genFR_r = m_sampleRate / 2;

  // if (f1 < 0.001) m_genFI_l = 0;
  // if (f2 < 0.001) m_genFI_r = 0;

  m_genShape_l = s1;
  m_genShape_r = s2;

  if (1.0 > g1) {
    m_genGain_l = g1;
  } else {
    m_genGain_l = 0.99999;
  }
  if (1.0 > g2) {
    m_genGain_r = g2;
  } else {
    m_genGain_r = 0.99999;
  }

  return 1;
}

void RWAudio::SetSndDevices(unsigned int irec, unsigned int iplay, unsigned long int freq) {
  unsigned int cardrec, cardplay;

  if (1000 == irec) {
    cardrec = m_AudioDriver.getDefaultInputDevice();
  } else {
    cardrec = irec;
  }
  if (1000 == iplay) {
    cardplay = m_AudioDriver.getDefaultOutputDevice();
  } else {
    cardplay = iplay;
  }
  m_sampleRate = freq;

  // redefine audio streams
  RestartAudio(cardrec, cardplay);
}
