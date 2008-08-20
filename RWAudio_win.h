// Recorder.h: interface for the CRecorder class.
//
//////////////////////////////////////////////////////////////////////

#ifndef RWAUDIO_WIN_H
#define RWAUDIO_WIN_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <mmsystem.h>
//#pragma comment(lib,"winmm.lib")

typedef struct _PCMFORMAT
{
	WORD    wBitsPerSample;//no.of bits per sample for Recording 
	WORD	wChannels;//no.of channels for Recording
	DWORD	dwSampleRate;//Sampling rate for Recording
}PCMFORMAT, *LPPCMFORMAT;

struct WAVECLASS
{
	HWAVE hWave;
	void* lpData;
};

typedef char *SndDevArray[5];


#define MAXNUMOFBUFFER 3

typedef BOOL (*ProcessBuffer)( LPWAVEHDR pwh);

class RWAudio
{
public:
	RWAudio(PCMFORMAT pcm,int nBufferLength=1024);
	RWAudio(int nBufferLength=1024, WORD wBitsPerSample=16,WORD wChannels=2,DWORD dwSampleRate=44100);
	virtual ~RWAudio();

	void SetSndDevices( int irec=WAVE_MAPPER, int iplay=WAVE_MAPPER){ m_RecSndCard = irec; m_PlaySndCard = iplay; };

	/* recording part */
	int GetRecordDevices(SndDevArray * devlist);
	int RecOpen( void);
	int RecClose( void);
	int RecStart( void);
	int RecStop( void);
	void ProcessNextBuffer(LPWAVEHDR pwh);
	void SetFormat(LPPCMFORMAT lpPcmFormat);
	void SetFormat(WORD wBitsPerSample,WORD wChannels,DWORD dwSampleRate);
	BOOL IsRecording() { return m_Recording; };
	BOOL IsRecDeviceOpened() { return m_RecDeviceOpen; };


	void RecSetBufferFunction( ProcessBuffer fnProcess) { fnProcessBuffer = fnProcess; }
	BOOL IsFormatSupported(WAVEFORMATEX wfEx, UINT nDev=WAVE_MAPPER);

	/* playing part */
	int GetPlayDevices(SndDevArray * devlist);
	int PlayOpen( void);
	int PlayClose( void);
	int PlayStart( void);
	int PlayStop( void);
	BOOL IsPlaying() { return m_Playing; };
	BOOL IsPlayDeviceOpened() { return m_PlayDeviceOpen; };

	int FillNextBuffer( void);
	int PlaySetGenerator( float, float, int, int, float, float);
	void PlaySetPhaseDiff( float i_fi) { m_genPhaseDif = i_fi; m_genFI_r = m_genFI_l; };

	HANDLE m_RecEvent;
	LPWAVEHDR m_lpWaveHdr;

protected:
	WAVEFORMATEX m_WaveFormat;
	DWORD m_dwBufferSize;

	/* recording part */
	BOOL m_Recording;
	BOOL m_RecDeviceOpen;
	HANDLE m_RecThread;
	int m_RecSndCard;

	GLOBALHANDLE m_hWaveInHdr[MAXNUMOFBUFFER];
	LPWAVEHDR m_lpWaveInHdr[MAXNUMOFBUFFER];
	GLOBALHANDLE m_hInBuffer[MAXNUMOFBUFFER];
	LPBYTE m_lpInBuffer [MAXNUMOFBUFFER];
	HWAVEIN m_hWaveIn;

	/* playing part */
	BOOL m_Playing;
	BOOL m_PlayDeviceOpen;
	int m_PlaySndCard;
	int m_PlayBufCntr;
	int m_PlaySilence;

	//GLOBALHANDLE m_hWaveOutHdr[MAXNUMOFBUFFER];
	//LPWAVEHDR m_lpWaveOutHdr[MAXNUMOFBUFFER];
	WAVEHDR m_WaveOutHdr[MAXNUMOFBUFFER];
	//GLOBALHANDLE m_hOutBuffer[MAXNUMOFBUFFER];
	//LPBYTE m_lpOutBuffer [MAXNUMOFBUFFER];
	HWAVEOUT m_hWaveOut;


	void * m_pSoundBuffer[MAXNUMOFBUFFER];
	/* generator - before we had global vars */
	float m_genFR_l, m_genFR_r;
	int m_genShape_l, m_genShape_r;
	float m_genGain_l, m_genGain_r;
	float m_genFI_l, m_genFI_r;
	float m_genPhaseDif;

	void* m_lpData;
	ProcessBuffer fnProcessBuffer;
	WAVECLASS m_waveClass;
};


#endif // RWAUDIO_WIN_H
