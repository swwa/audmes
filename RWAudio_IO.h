//////////////////////////////////////////////////////////////////////
// RWAudio_IO - interface class between AudMeS and RtAudio
//            - multi-platform
//////////////////////////////////////////////////////////////////////

#ifndef RWAUDIO_IO_H
#define RWAUDIO_IO_H

#include "audiolib/RtAudio.h"

struct RWAudioDevList
{
  std::vector<int> card_position;
  std::vector<std::string> card_name;
};

class RWAudio
{
public:
	RWAudio( long int oscbuflen, long int spebuflen);
	~RWAudio();

	void SetSndDevices( int irec=-1, int iplay=-1);

	void ChangeBufLen( long int oscbuflen, long int spebuflen) { m_OscBufferLen = oscbuflen; m_SpeBufferLen = spebuflen; m_Buflen_Changed = 1; };

	int GetRWAudioDevices( RWAudioDevList * play, RWAudioDevList * record, std::vector<long int> * freqs);

	/* parameter settings */
	int PlaySetGenerator( float, float, int, int, float, float);
	void PlaySetPhaseDiff( float i_fi) { m_genPhaseDif = i_fi; m_genFI_r = m_genFI_l; };

	int GetRunningStatus( void) { return m_DrvRunning; };
	char * GetErrorMessage( void) { return m_ErrorMessage; };

	// stuff for callback function
	unsigned int m_bufferBytes;
	/* generator - before we had global vars */
	float m_genFR_l, m_genFR_r;
	int m_genShape_l, m_genShape_r;
	float m_genGain_l, m_genGain_r;
	float m_genFI_l, m_genFI_r;
	float m_genPhaseDif;

	unsigned int m_sampleRate;

	long int m_OscBufferLen;
	long int m_SpeBufferLen;
	unsigned int m_Buflen_Changed;

protected:
	RtAudio m_AudioDriver;

	int m_RecSndCard;
	int m_PlaySndCard;


	int m_DrvRunning; // is driver properly initialized ?
	char * m_ErrorMessage;
};


#endif // RWAUDIO_IO_H
