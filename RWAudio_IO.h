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

#include <rtaudio/RtAudio.h>

struct RWAudioDevList {
  std::vector<RtAudio::DeviceInfo> card_info;
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

  /* parameter settings */
  enum Waveform { SINE, RECT, SAW, TRI, NOISE, WOBBLE };
  int PlaySetGenerator(float, float, Waveform, Waveform, float, float);

  void PlaySetPhaseDiff(float i_fi) {
    m_genPhaseDif = i_fi;
    m_genPhase_r = m_genPhase_l;
  };

  /* generator */
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

 protected:
  RtAudio* m_AudioDriver;

 private:
  int stream_running;
  unsigned int cardrec, cardplay;
  int StartAudio(int recDevId, int playDevId);
};

#endif  // RWAUDIO_IO_H
