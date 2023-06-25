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

#include <wx/defs.h>  // for WXUNUSED
#include "RWAudio_IO.h"

#include <cmath>
#include <stdio.h>

#include <atomic>
#include <map>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// extern variables from AudMeS.cpp
extern float *g_OscBuffer_Left;
extern float *g_OscBuffer_Right;
extern long int g_OscBufferPosition;

extern float *g_SpeBuffer_Left;
extern float *g_SpeBuffer_Right;
extern long int g_SpeBufferPosition;

extern std::atomic<bool> g_OscBufferChanged;
extern std::atomic<bool> g_SpeBufferChanged;

static double ph_wobble = 0.0;

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
 * callback function to catch runtime errors
 */
void catcherr(RtAudioError::Type WXUNUSED(type), const std::string &errorText) {
  std::cerr << '\n' << errorText << '\n' << std::endl;
}

/*
 * callback function to fetch audio input and play tones
 */
int inout(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
          double WXUNUSED(streamTime), RtAudioStreamStatus status, void *data) {
  RWAudio *aRWAudioClass = (RWAudio *)data;
  unsigned i;

  if (status) std::cerr << "Audio stream over/underflow detected." << std::endl;

  // copy input buffer into two L/R channels
  if (aRWAudioClass->m_Buflen_Changed.load()) {
    aRWAudioClass->m_Buflen_Changed = false;
    free(g_OscBuffer_Left);
    free(g_OscBuffer_Right);
    free(g_SpeBuffer_Left);
    free(g_SpeBuffer_Right);
    g_OscBufferPosition = 0;
    g_SpeBufferPosition = 0;
    g_OscBuffer_Left = (float *)malloc(aRWAudioClass->m_OscBufferLen * sizeof(float));
    g_OscBuffer_Right = (float *)malloc(aRWAudioClass->m_OscBufferLen * sizeof(float));
    g_SpeBuffer_Left = (float *)malloc(aRWAudioClass->m_SpeBufferLen * sizeof(float));
    g_SpeBuffer_Right = (float *)malloc(aRWAudioClass->m_SpeBufferLen * sizeof(float));
  }

  float *inBuf = (float *)inputBuffer;

  // make a copy for oscilloscope
  if (!g_OscBufferChanged.load()) {
    for (i = 0; i < nBufferFrames; i++) {
      // copy audio signal to fft real component.
      g_OscBuffer_Left[g_OscBufferPosition] = *inBuf++;
      if (aRWAudioClass->m_channels_in > 1)
        g_OscBuffer_Right[g_OscBufferPosition] = *inBuf++;
      else
        g_OscBuffer_Right[g_OscBufferPosition] = 0;

      g_OscBufferPosition++;
      // if the buffer is over we have to pick the data and then circullary fill the next one
      if (g_OscBufferPosition == aRWAudioClass->m_OscBufferLen) {
        g_OscBufferPosition = 0;
        g_OscBufferChanged = true;
        break;
      }
    }
  }

  inBuf = (float *)inputBuffer;
  // make a copy for spectrum analyzer
  if (!g_SpeBufferChanged.load()) {
    for (i = 0; i < nBufferFrames; i++) {
      // copy audio signal to fft real component.
      g_SpeBuffer_Left[g_SpeBufferPosition] = *inBuf++;
      if (aRWAudioClass->m_channels_in > 1)
        g_SpeBuffer_Right[g_SpeBufferPosition] = *inBuf++;
      else
        g_SpeBuffer_Right[g_SpeBufferPosition] = 0;

      g_SpeBufferPosition++;
      // if the buffer is over we have to pick the data and then circullary fill the next one
      if (g_SpeBufferPosition == aRWAudioClass->m_SpeBufferLen) {
        g_SpeBufferPosition = 0;
        g_SpeBufferChanged = true;
        break;
      }
    }
  }

  // fill output buffer
  float *outBuf = (float *)outputBuffer;
  for (unsigned long i = 0; i < nBufferFrames; i++) {
    *outBuf++ = (float)(aRWAudioClass->m_genGain_l * aRWAudioClass->ringb[aRWAudioClass->rring++]);
    if (aRWAudioClass->m_channels_out > 1)
      *outBuf++ =
          (float)(aRWAudioClass->m_genGain_r * aRWAudioClass->ringb[aRWAudioClass->rring++]);
    aRWAudioClass->rring %= aRWAudioClass->ringsize;
  }
  //if (aRWAudioClass->rring > aRWAudioClass->wring)
  //  std::cerr << "underrun " << aRWAudioClass->wring << "," <<aRWAudioClass->rring << std::endl;
  return 0;
}

void RWAudio::calcwave() {
  const int Frames = 2048*10;
  while (true) {
    if (rring > wring) {
      if (ringsize+wring-Frames >= rring)
        break;
    } else {
      if (wring-Frames >= rring)
        break;
    }

    /* calculate the wave form according to the selected shape */
    float y = 0;
    float y2 = 0;
    bool noise = lfsr16();

    /* left channel */
    switch (m_genShape_l) {
      case RWAudio::RECT:
        if (m_genPhase_l < M_PI) {
          y = 1.0;
        } else {
          y = -1.0;
        }
        break;
      case RWAudio::SAW:
        y = (m_genPhase_l - M_PI) / M_PI;
        break;
      case RWAudio::TRI:
        if (m_genPhase_l < M_PI) {
          y = 2 * (m_genPhase_l - M_PI / 2) / M_PI;
        } else {
          y = 2 * (3 * M_PI / 2 - m_genPhase_l) / M_PI;
        }
        break;
      case RWAudio::NOISE:
        if (noise) {
          y = 1.0;
        } else {
          y = -1.0;
        }
        break;
      case RWAudio::WOBBLE:
        y = sin(m_genPhase_l + sin(ph_wobble));
        break;
      default: /* sine wave */
        y = sin(m_genPhase_l);
        break;
    }
    /* right channel */
    switch (m_genShape_r) {
      case RWAudio::RECT:
        if ((m_genPhase_r - m_genPhaseDif) < M_PI) {
          y2 = 1;
        } else {
          y2 = -1;
        }
        break;
      case RWAudio::SAW:
        y2 = ((m_genPhase_r - m_genPhaseDif) - M_PI) / M_PI;
        break;
      case RWAudio::TRI:
        if (m_genPhase_r < M_PI) {
          y2 = 2 * (m_genPhase_r - m_genPhaseDif - M_PI / 2) / M_PI;
        } else {
          y2 = 2 * (3 * M_PI / 2 - m_genPhase_r + m_genPhaseDif) /
               M_PI;
        }
        break;
      case RWAudio::NOISE:
        if (noise) {
          y2 = 1.0;
        } else {
          y2 = -1.0;
        }
        break;
      case RWAudio::WOBBLE:
        y2 = sin(m_genPhase_r - m_genPhaseDif + sin(ph_wobble));
        break;
      default: /* sine wave */
        y2 = sin(m_genPhase_r - m_genPhaseDif);
        break;
    }

      if (m_genGain_l == 0.0)
        m_genPhase_l = 0.0;
      else
        m_genPhase_l += (float)2.0 * M_PI * m_genFR_l / m_sampleRate;

      if (m_genGain_r == 0.0)
        m_genPhase_r = 0.0;
      else
        m_genPhase_r += (float)2.0 * M_PI * m_genFR_r / m_sampleRate;

    if ((2.0 * M_PI) < m_genPhase_l) m_genPhase_l -= 2.0 * M_PI;
    if ((2.0 * M_PI) < m_genPhase_r) m_genPhase_r -= 2.0 * M_PI;
    ph_wobble += 30.0 / m_sampleRate;
    ringb[wring++] = m_genGain_l * y;
    if (m_channels_out > 1) ringb[wring++] = m_genGain_r * y2;
    wring %= ringsize;
#ifdef __DEBUG
    std::cerr << y << "," << y2 << std::endl;
#endif
  }
#ifdef _DEBUG
  std::cerr << wring << "," << rring << std::endl;
#endif
}

RWAudio::RWAudio() {
  m_Buflen_Changed = false;
  m_sampleRate = 0;
  rring = 0;
  wring = 0;
}

RWAudio::~RWAudio() {
  m_AudioDriver->stopStream();
  m_AudioDriver->closeStream();
}

// Initialize RtAudio and start audio stream
int RWAudio::InitSnd(long int oscbuflen, long int spebuflen, std::string &rtinfo,
                     unsigned int srate) {
  m_OscBufferLen = oscbuflen;
  m_SpeBufferLen = spebuflen;
  m_sampleRate = srate;

  m_genFR_l = m_genFR_r = 0.0;
  m_genShape_l = m_genShape_r = SINE;
  m_genGain_l = m_genGain_r = 0.0;
  m_genPhase_l = m_genPhase_r = 0.0;
  m_genPhaseDif = 0.0;

  g_OscBufferPosition = 0;
  g_SpeBufferPosition = 0;

  g_OscBuffer_Left = (float *)malloc(m_OscBufferLen * sizeof(float));
  g_OscBuffer_Right = (float *)malloc(m_OscBufferLen * sizeof(float));
  g_SpeBuffer_Left = (float *)malloc(m_SpeBufferLen * sizeof(float));
  g_SpeBuffer_Right = (float *)malloc(m_SpeBufferLen * sizeof(float));

  RtAudio::DeviceInfo info;
  unsigned int devices;

  // Create an api map.
  std::map<int, std::string> apiMap;
  apiMap[RtAudio::MACOSX_CORE] = "OS-X Core Audio";
  apiMap[RtAudio::WINDOWS_ASIO] = "Windows ASIO";
  apiMap[RtAudio::WINDOWS_DS] = "Windows DirectSound";
  apiMap[RtAudio::WINDOWS_WASAPI] = "Windows WASAPI";
  apiMap[RtAudio::UNIX_JACK] = "Jack Client";
  apiMap[RtAudio::LINUX_ALSA] = "Linux ALSA";
  apiMap[RtAudio::LINUX_PULSE] = "Linux PulseAudio";
  apiMap[RtAudio::LINUX_OSS] = "Linux OSS";
  apiMap[RtAudio::RTAUDIO_DUMMY] = "RtAudio Dummy";

  std::vector<RtAudio::Api> apis;
  // init of RtAudio
  RtAudio::getCompiledApi(apis);

  rtinfo = "\nRtAudio Version ";
  rtinfo = rtinfo + RtAudio::getVersion() + "\nCompiled APIs:\n";
  for (unsigned int i = 0; i < apis.size(); i++) rtinfo = rtinfo + "  " + apiMap[apis[i]] + "\n";

  m_AudioDriver = new RtAudio();
  rtinfo = rtinfo + "Current API: " + apiMap[m_AudioDriver->getCurrentApi()] + "\n";

  devices = m_AudioDriver->getDeviceCount();
  if (devices < 1) return 1;

  for (unsigned int i = 0; i < devices; i++) {
    info = m_AudioDriver->getDeviceInfo(i);
  }

  // start audio streams
  if (RestartAudio(m_AudioDriver->getDefaultInputDevice(),
                   m_AudioDriver->getDefaultOutputDevice())) {
    return 2;
  }
  return 0;
}

int RWAudio::RestartAudio(int recDevId, int playDevId) {
  // if stream is open (and running), stop it
  if (m_AudioDriver->isStreamOpen()) {
    m_AudioDriver->stopStream();
    m_AudioDriver->closeStream();
  }

  // adapt to number of channels
  RtAudio::DeviceInfo info = m_AudioDriver->getDeviceInfo(recDevId);
  if (info.inputChannels > 1 || info.duplexChannels > 1)
    m_channels_in = 2;
  else
    m_channels_in = 1;
  info = m_AudioDriver->getDeviceInfo(playDevId);
  if (info.outputChannels > 1 || info.duplexChannels > 1)
    m_channels_out = 2;
  else
    m_channels_out = 1;

  // configure new stream
  RtAudio::StreamParameters iParams;
  RtAudio::StreamParameters oParams;
  RtAudio::StreamOptions rtAOptions;
  unsigned int bufferFrames = 512;

  iParams.deviceId = recDevId;
  oParams.deviceId = playDevId;
  iParams.nChannels = m_channels_in;
  oParams.nChannels = m_channels_out;
  iParams.firstChannel = 0;
  oParams.firstChannel = 0;

  rtAOptions.flags = 0;

  try {
    m_AudioDriver->openStream(&oParams, &iParams, RTAUDIO_FLOAT32, m_sampleRate, &bufferFrames,
                              &inout, (void *)this, &rtAOptions, &catcherr);
  } catch (RtAudioError &e) {
    // std::cerr << '\n' << e.getMessage() << '\n' << std::endl;
    return 1;
  }

  calcwave();

  try {
    m_AudioDriver->startStream();
  } catch (RtAudioError &e) {
    // std::cerr << '\n' << e.getMessage() << '\n' << std::endl;
    return 1;
  }

  return 0;
}

/********************************************************************/
/*************      Devices enumeration           *******************/
/********************************************************************/
int RWAudio::GetRWAudioDevices(RWAudioDevList *play, RWAudioDevList *record) {
  // Determine the number of devices available
  unsigned int devices = m_AudioDriver->getDeviceCount();

  // Scan through devices for various capabilities
  RtAudio::DeviceInfo info;
  // if stream is open (and running), stop it
  if (m_AudioDriver->isStreamOpen()) {
    m_AudioDriver->stopStream();
    m_AudioDriver->closeStream();
  }

  play->card_info.clear();
  record->card_info.clear();
  play->card_pos.clear();
  record->card_pos.clear();

  for (unsigned int i = 0; i < devices; i++) {
    info = m_AudioDriver->getDeviceInfo(i);

    if (info.probed == true) {
      //      std::cout << "device = " << i << "; name: " << info.name << "\n";

      // add play card
      if ((info.outputChannels > 0) || (info.duplexChannels > 0)) {
        play->card_info.push_back(info);
        play->card_pos.push_back(i);
      }

      // add record card
      if ((info.inputChannels > 0) || (info.duplexChannels > 0)) {
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
int RWAudio::PlaySetGenerator(float f1, float f2, Waveform s1, Waveform s2, float g1, float g2) {
  if (2 * f1 < m_sampleRate)
    m_genFR_l = f1;
  else
    m_genFR_l = m_sampleRate / 2;
  if (2 * f2 < m_sampleRate)
    m_genFR_r = f2;
  else
    m_genFR_r = m_sampleRate / 2;

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

void RWAudio::SetSndDevices(unsigned int irec, unsigned int iplay, unsigned int srate) {
  unsigned int cardrec, cardplay;

  cardrec = irec;
  cardplay = iplay;
  m_sampleRate = srate;

  // redefine audio streams
  RestartAudio(cardrec, cardplay);
}
