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
#include <iostream>
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
 * callback function to fetch audio input and generate tones
 */
int inout(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
          const PaStreamCallbackTimeInfo *WXUNUSED(timeInfo), PaStreamCallbackFlags statusFlags,
          void *userData) {
  RWAudio *aRWAudioClass = (RWAudio *)userData;
  unsigned i;

  if (statusFlags & paInputOverflow) std::cerr << "Audio input overflow detected." << std::endl;
  if (statusFlags & paInputUnderflow) std::cerr << "Audio input underflow detected." << std::endl;
  if (statusFlags & paOutputOverflow) std::cerr << "Audio output overflow detected." << std::endl;
  if (statusFlags & paOutputUnderflow) std::cerr << "Audio output underflow detected." << std::endl;

  // copy input buffer into two L/R channels
  if (aRWAudioClass->m_Buflen_Changed) {
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
    for (i = 0; i < framesPerBuffer; i++) {
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
        g_OscBufferChanged.store(true);
        break;
      }
    }
  }

  inBuf = (float *)inputBuffer;
  // make a copy for spectrum analyzer
  if (!g_SpeBufferChanged.load()) {
    for (i = 0; i < framesPerBuffer; i++) {
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
        g_SpeBufferChanged.store(true);
        break;
      }
    }
  }

  // fill output buffer
  float *outBuf = (float *)outputBuffer;

#ifdef _DEBUG
  // fprintf(ddbg, "\n Frames: %d\n ", framesPerBuffer);
#endif

  /* calculate the wave form according to the selected shape */
  for (i = 0; i < framesPerBuffer; i++) {
    double y = 0;
    double y2 = 0;
    bool noise = lfsr16();

    /* left channel */
    switch (aRWAudioClass->m_genShape_l) {
      case RWAudio::RECT:
        if (aRWAudioClass->m_genPhase_l < M_PI) {
          y = 1.0;
        } else {
          y = -1.0;
        }
        break;
      case RWAudio::SAW:
        y = (aRWAudioClass->m_genPhase_l - M_PI) / M_PI;
        break;
      case RWAudio::TRI:
        if (aRWAudioClass->m_genPhase_l < M_PI) {
          y = 2 * (aRWAudioClass->m_genPhase_l - M_PI / 2) / M_PI;
        } else {
          y = 2 * (3 * M_PI / 2 - aRWAudioClass->m_genPhase_l) / M_PI;
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
        y = sin(aRWAudioClass->m_genPhase_l + sin(ph_wobble));
        break;
      default: /* sine wave */
        y = sin(aRWAudioClass->m_genPhase_l);
        break;
    }
    /* right channel */
    switch (aRWAudioClass->m_genShape_r) {
      case RWAudio::RECT:
        if ((aRWAudioClass->m_genPhase_r - aRWAudioClass->m_genPhaseDif) < M_PI) {
          y2 = 1;
        } else {
          y2 = -1;
        }
        break;
      case RWAudio::SAW:
        y2 = ((aRWAudioClass->m_genPhase_r - aRWAudioClass->m_genPhaseDif) - M_PI) / M_PI;
        break;
      case RWAudio::TRI:
        if (aRWAudioClass->m_genPhase_r < M_PI) {
          y2 = 2 * (aRWAudioClass->m_genPhase_r - aRWAudioClass->m_genPhaseDif - M_PI / 2) / M_PI;
        } else {
          y2 = 2 * (3 * M_PI / 2 - aRWAudioClass->m_genPhase_r + aRWAudioClass->m_genPhaseDif) /
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
        y2 = sin(aRWAudioClass->m_genPhase_r - aRWAudioClass->m_genPhaseDif + sin(ph_wobble));
        break;
      default: /* sine wave */
        y2 = sin(aRWAudioClass->m_genPhase_r - aRWAudioClass->m_genPhaseDif);
        break;
    }

    if (aRWAudioClass->m_genGain_l == 0.0)
      aRWAudioClass->m_genPhase_l = 0.0;
    else
      aRWAudioClass->m_genPhase_l +=
          (float)2.0 * M_PI * aRWAudioClass->m_genFR_l / aRWAudioClass->m_sampleRate;

    if (aRWAudioClass->m_genGain_r == 0.0)
      aRWAudioClass->m_genPhase_r = 0.0;
    else
      aRWAudioClass->m_genPhase_r +=
          (float)2.0 * M_PI * aRWAudioClass->m_genFR_r / aRWAudioClass->m_sampleRate;

    if ((2.0 * M_PI) < aRWAudioClass->m_genPhase_l) aRWAudioClass->m_genPhase_l -= 2.0 * M_PI;
    if ((2.0 * M_PI) < aRWAudioClass->m_genPhase_r) aRWAudioClass->m_genPhase_r -= 2.0 * M_PI;
    ph_wobble += 30.0 / aRWAudioClass->m_sampleRate;
    *outBuf++ = (float)(aRWAudioClass->m_genGain_l * y);
    if (aRWAudioClass->m_channels_out > 1) *outBuf++ = (float)(aRWAudioClass->m_genGain_r * y2);

#ifdef _DEBUG
    // fprintf(ddbg,"%04X %04X ",(float)(32768.f * y), (float)(32768.f * y2));
#endif
  }
  return 0;
}

RWAudio::RWAudio() {
  m_Buflen_Changed = false;
  m_sampleRate = 0;
  m_InputStream = NULL;
  m_OutputStream = NULL;
  m_StreamActive = false;

  // Initialize PortAudio
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
  }
  stream_running = 0;
}

RWAudio::~RWAudio() {
  // Stop and close any open streams
  if (m_StreamActive) {
    if (m_InputStream) {
      Pa_StopStream(m_InputStream);
      Pa_CloseStream(m_InputStream);
    }
    if (m_OutputStream) {
      Pa_StopStream(m_OutputStream);
      Pa_CloseStream(m_OutputStream);
    }
  }

  // Terminate PortAudio
  Pa_Terminate();
}

// Initialize PortAudio and start audio stream
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

  // Get PortAudio version information
  rtinfo = "\nPortAudio Version: ";
  rtinfo += Pa_GetVersionText();
  rtinfo += "\n";

  // Get host API information
  int numHostAPIs = Pa_GetHostApiCount();
  rtinfo += "Available Host APIs:\n";

  for (int i = 0; i < numHostAPIs; i++) {
    const PaHostApiInfo *apiInfo = Pa_GetHostApiInfo(i);
    if (apiInfo) {
      rtinfo += "  ";
      rtinfo += apiInfo->name;
      rtinfo += "\n";
    }
  }

  // Get default host API info
  int defaultHostApi = Pa_GetDefaultHostApi();
  const PaHostApiInfo *defaultApiInfo = Pa_GetHostApiInfo(defaultHostApi);
  if (defaultApiInfo) {
    rtinfo += "Current API: ";
    rtinfo += defaultApiInfo->name;
    rtinfo += "\n";
  }

  // Check if there are any devices available
  int numDevices = Pa_GetDeviceCount();
  if (numDevices < 1) return 1;

  // Get device info for logging purposes
  for (int i = 0; i < numDevices; i++) {
    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
    if (deviceInfo) {
      // We're just enumerating devices here
    }
  }
  cardrec = Pa_GetDefaultInputDevice();
  cardplay = Pa_GetDefaultOutputDevice();
  return 0;
}

int RWAudio::StartSnd() {
  if (StartAudio(cardrec, cardplay)) {
    return 2;
  }
  stream_running++;
  return 0;
}

int RWAudio::StopSnd() {
  if (stream_running > 0) {
    stream_running--;
    if (stream_running == 0) {
      // Stop and close any open streams
      if (m_StreamActive) {
        if (m_InputStream) {
          Pa_StopStream(m_InputStream);
          Pa_CloseStream(m_InputStream);
        }
        if (m_OutputStream) {
          Pa_StopStream(m_OutputStream);
          Pa_CloseStream(m_OutputStream);
        }
        m_StreamActive = false;
      }
    }
  }
  return 0;
}

int RWAudio::StartAudio(int recDevId, int playDevId) {
  if (m_StreamActive) {
    return 0;
  }

  // Determine number of channels for input and output devices
  const PaDeviceInfo *inputInfo = Pa_GetDeviceInfo(recDevId);
  if (inputInfo && inputInfo->maxInputChannels > 1)
    m_channels_in = 2;
  else
    m_channels_in = 1;

  const PaDeviceInfo *outputInfo = Pa_GetDeviceInfo(playDevId);
  if (outputInfo && outputInfo->maxOutputChannels > 1)
    m_channels_out = 2;
  else
    m_channels_out = 1;

  // Configure input stream parameters
  PaStreamParameters inputParams;
  inputParams.device = recDevId;
  inputParams.channelCount = m_channels_in;
  inputParams.sampleFormat = paFloat32;
  inputParams.suggestedLatency = inputInfo ? inputInfo->defaultLowInputLatency : 0.1;
  inputParams.hostApiSpecificStreamInfo = NULL;

  // Configure output stream parameters
  PaStreamParameters outputParams;
  outputParams.device = playDevId;
  outputParams.channelCount = m_channels_out;
  outputParams.sampleFormat = paFloat32;
  outputParams.suggestedLatency = outputInfo ? outputInfo->defaultLowOutputLatency : 0.1;
  outputParams.hostApiSpecificStreamInfo = NULL;

  // Open a duplex stream
  unsigned int bufferFrames = 2048;
  PaError err = Pa_OpenStream(&m_InputStream,  // Stream pointer
                              &inputParams,    // Input parameters
                              &outputParams,   // Output parameters
                              m_sampleRate,    // Sample rate
                              bufferFrames,    // Frames per buffer
                              paNoFlag,        // Stream flags
                              inout,           // Callback function
                              this);           // User data passed to callback

  if (err != paNoError) {
    std::cerr << "Error opening PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
    return 1;
  }

  // Start the stream
  err = Pa_StartStream(m_InputStream);
  if (err != paNoError) {
    std::cerr << "Error starting PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
    Pa_CloseStream(m_InputStream);
    m_InputStream = NULL;
    return 1;
  }

  m_StreamActive = true;
  return 0;
}

/********************************************************************/
/*************      Devices enumeration           *******************/
/********************************************************************/
int RWAudio::GetRWAudioDevices(RWAudioDevList *play, RWAudioDevList *record) {
  // Determine the number of devices available
  int numDevices = Pa_GetDeviceCount();

  // If streams are active, stop them to avoid conflicts
  if (m_StreamActive) {
    if (m_InputStream) {
      Pa_StopStream(m_InputStream);
      Pa_CloseStream(m_InputStream);
      m_InputStream = NULL;
    }
    if (m_OutputStream) {
      Pa_StopStream(m_OutputStream);
      Pa_CloseStream(m_OutputStream);
      m_OutputStream = NULL;
    }
    m_StreamActive = false;
  }

  play->card_info.clear();
  record->card_info.clear();
  play->card_pos.clear();
  record->card_pos.clear();

  int defaultInputDevice = Pa_GetDefaultInputDevice();
  int defaultOutputDevice = Pa_GetDefaultOutputDevice();

  // Scan through devices for various capabilities
  for (int i = 0; i < numDevices; i++) {
    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);

    if (deviceInfo) {
      RWAudioDeviceInfo rwDeviceInfo;
      rwDeviceInfo.name = deviceInfo->name;
      rwDeviceInfo.maxInputChannels = deviceInfo->maxInputChannels;
      rwDeviceInfo.maxOutputChannels = deviceInfo->maxOutputChannels;
      rwDeviceInfo.defaultSampleRate = deviceInfo->defaultSampleRate;
      rwDeviceInfo.isDefaultInput = (i == defaultInputDevice);
      rwDeviceInfo.isDefaultOutput = (i == defaultOutputDevice);

      // Add to playback list if it has output channels
      if (deviceInfo->maxOutputChannels > 0) {
        play->card_info.push_back(rwDeviceInfo);
        play->card_pos.push_back(i);
      }

      // Add to recording list if it has input channels
      if (deviceInfo->maxInputChannels > 0) {
        record->card_info.push_back(rwDeviceInfo);
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

void RWAudio::SetSndDevices(int irec, int iplay, unsigned int srate) {
  int cardrec, cardplay;

  cardrec = irec;
  cardplay = iplay;
  m_sampleRate = srate;

  if (stream_running > 0) {
    StartAudio(cardrec, cardplay);
  }
}
