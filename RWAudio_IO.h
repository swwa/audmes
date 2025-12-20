//////////////////////////////////////////////////////////////////////
// RWAudio_IO - interface class between AudMeS and RtAudio
//            - multi-platform
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

#ifndef RWAUDIO_IO_H
#define RWAUDIO_IO_H

#include <string>
#include <vector>
#include <memory>

struct DeviceInfo {
  std::string name;              /*!< Character string device identifier. */
  unsigned int outputChannels{}; /*!< Maximum output channels supported by device. */
  unsigned int inputChannels{};  /*!< Maximum input channels supported by device. */
  unsigned int
      duplexChannels{}; /*!< Maximum simultaneous input/output channels supported by device. */
  std::vector<unsigned int>
      sampleRates; /*!< Supported sample rates (queried from list of standard rates). */
  unsigned int
      preferredSampleRate{}; /*!< Preferred sample rate, e.g. for WASAPI the system sample rate. */
};

struct RWAudioDevList {
  std::vector<DeviceInfo> card_info;
  std::vector<unsigned int> card_pos;
};

class RWAudio {
 public:
  RWAudio();
  ~RWAudio();

  int InitSnd(long int oscbuflen, long int spebuflen, std::string& rtinfo, unsigned int srate);
  int StartSnd();
  int StopSnd();

  void SetSndDevices(unsigned int irec, unsigned int iplay, unsigned int srate);

  void ChangeBufLen(long int oscbuflen, long int spebuflen) {
    m_OscBufferLen = oscbuflen;
    m_SpeBufferLen = spebuflen;
    m_Buflen_Changed = true;
  };

  int GetRWAudioDevices(RWAudioDevList* play, RWAudioDevList* record);

  /* generator */
  enum Waveform { SINE, RECT, SAW, TRI, NOISE, WOBBLE };
  int PlaySetGenerator(float, float, Waveform, Waveform, float, float);

  void PlaySetPhaseDiff(float i_fi) {
    m_genPhaseDif = i_fi;
    m_genPhase_r = m_genPhase_l;
  };

  float m_genFR_l, m_genFR_r;
  Waveform m_genShape_l, m_genShape_r;
  float m_genGain_l, m_genGain_r;
  float m_genPhase_l, m_genPhase_r;
  float m_genPhaseDif;

  unsigned int m_sampleRate;
  int m_channels_in;
  int m_channels_out;

  long int m_OscBufferLen;
  long int m_SpeBufferLen;
  bool m_Buflen_Changed;

 private:
  class Impl;  // Forward declaration
  std::unique_ptr<Impl> pImpl;
  int stream_running;
  unsigned int cardrec, cardplay;
  int StartAudio(int recDevId, int playDevId);
};

#endif  // RWAUDIO_IO_H
