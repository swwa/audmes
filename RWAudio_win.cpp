// RWAudio.cpp: implementation of the RWAudio class.
//
//////////////////////////////////////////////////////////////////////

#include "RWAudio_win.h"
#include <stdio.h>
#include <math.h>
#include <windows.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define PI            3.14159265358979

#ifdef _DEBUG
FILE * ddbg;
#endif

//////////////////////////////////////////////////////////////////////
// Callback functions
//////////////////////////////////////////////////////////////////////

void CALLBACK waveInProc( HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	RWAudio* pRWAudio = NULL;
	switch (uMsg)
	{
	case WIM_OPEN:
		break;
	case WIM_DATA:
		//if ((((LPWAVEHDR)dwParam1)->dwFlags) == WHDR_DONE)
		{
			pRWAudio = (RWAudio*)(((LPWAVEHDR)dwParam1)->dwUser);
			pRWAudio->m_lpWaveHdr = (LPWAVEHDR)dwParam1;
			SetEvent(pRWAudio->m_RecEvent);
		}
		break;
	case WIM_CLOSE:
		break;
	default:
		break;
	}
}
DWORD CALLBACK RWAudioRecThreadFunc(LPVOID lpThreadData)
{
	DWORD dwResult;
	RWAudio* pRWAudio = NULL;
	pRWAudio = (RWAudio*)lpThreadData;
	while (pRWAudio->IsRecording())
	{
		dwResult = WaitForSingleObject(pRWAudio->m_RecEvent,1000);
		if (WAIT_OBJECT_0 == dwResult) {
			pRWAudio->ProcessNextBuffer(pRWAudio->m_lpWaveHdr);
		}
	}
	return 0;
}

void CALLBACK waveOutProc( HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
  RWAudio *pRWAudio = NULL;

  switch (uMsg)
  {
  case WOM_OPEN:
    break;
  case WOM_CLOSE:
    break;
  case WOM_DONE:
		{
			pRWAudio = (RWAudio*) dwInstance;
			if ( pRWAudio)	pRWAudio->FillNextBuffer( );
		}
    break;
  }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RWAudio::RWAudio( PCMFORMAT pcm, int nBufferLength)
{
	m_WaveFormat.wFormatTag      = WAVE_FORMAT_PCM;
	m_WaveFormat.nChannels       = pcm.wChannels;
	m_WaveFormat.wBitsPerSample  = pcm.wBitsPerSample;
	m_WaveFormat.nSamplesPerSec  = pcm.dwSampleRate;
	m_WaveFormat.nBlockAlign     = m_WaveFormat.nChannels * m_WaveFormat.wBitsPerSample/8;
	m_WaveFormat.nAvgBytesPerSec = m_WaveFormat.nBlockAlign * m_WaveFormat.nSamplesPerSec;

	m_dwBufferSize = (nBufferLength * pcm.wChannels * pcm.wBitsPerSample / 8);
	
	/* Recorder */
	fnProcessBuffer = NULL;
	m_lpWaveHdr = NULL;
	m_Recording = FALSE;
	m_RecDeviceOpen = FALSE;
	m_RecEvent = NULL;
	m_RecThread = NULL;
	for(int i=0; i<MAXNUMOFBUFFER; i++)
	{
		m_hWaveInHdr[i] = NULL;
		m_hInBuffer[i] = NULL;
	}

	/* Player */
	m_Playing = FALSE;
	m_PlayDeviceOpen = FALSE;

	SetSndDevices();
	m_genFR_l = m_genFR_r = 0.0;
	m_genShape_l = m_genShape_r = 0;
	m_genGain_l = m_genGain_r = 1.0;
	m_genFI_l = m_genFI_r = 0.0;
	m_genPhaseDif = 0.0;

}

RWAudio::RWAudio( int nBufferLength, WORD wBitsPerSample,WORD wChannels,DWORD dwSampleRate)
{
	m_WaveFormat.wFormatTag      = WAVE_FORMAT_PCM;
	m_WaveFormat.nChannels       = wChannels;
	m_WaveFormat.wBitsPerSample  = wBitsPerSample;
	m_WaveFormat.nSamplesPerSec  = dwSampleRate;
	m_WaveFormat.nBlockAlign     = m_WaveFormat.nChannels * m_WaveFormat.wBitsPerSample/8;
	m_WaveFormat.nAvgBytesPerSec = m_WaveFormat.nBlockAlign * m_WaveFormat.nSamplesPerSec;

	m_dwBufferSize = (nBufferLength * wChannels * wBitsPerSample / 8);

	/* Recorder */
	fnProcessBuffer = NULL;
	m_lpWaveHdr = NULL;
	m_Recording = FALSE;
	m_RecDeviceOpen = FALSE;
	m_RecEvent = NULL;
	m_RecThread = NULL;
	for(int i=0; i<MAXNUMOFBUFFER; i++)
	{
		m_hWaveInHdr[i] = NULL;
		m_hInBuffer[i] = NULL;
	}

	/* Player */
	m_Playing = FALSE;
	m_PlayDeviceOpen = FALSE;

	SetSndDevices();
	m_genFR_l = m_genFR_r = 0.0;
	m_genShape_l = m_genShape_r = 0;
	m_genGain_l = m_genGain_r = 1.0;
	m_genFI_l = m_genFI_r = 0.0;
	m_genPhaseDif = 0.0;

#ifdef _DEBUG
  ddbg = fopen("debug.log","wb");
#endif
}

RWAudio::~RWAudio()
{
	if(m_Recording)
	  RecStop();
	if(m_RecDeviceOpen)
	  RecClose();
	fnProcessBuffer = NULL;

	if(m_Playing)
	  PlayStop();
	if(m_PlayDeviceOpen)
	  PlayClose();

#ifdef _DEBUG
	fclose(ddbg);
#endif
}

/********************************************************************/
/*************         Recording part             *******************/
/********************************************************************/
int RWAudio::RecOpen( void)
{
    if(m_RecDeviceOpen)
	{
		printf("REC Device Already Opened. Please close it before attempting to open\n");
		return 1;
	}

	for(int i=0; i<MAXNUMOFBUFFER; i++)
	{
		m_hWaveInHdr[i] = GlobalAlloc(GHND | GMEM_SHARE , sizeof(WAVEHDR));
		m_lpWaveInHdr[i] = (LPWAVEHDR)GlobalLock(m_hWaveInHdr[i]);
		m_hInBuffer[i] = GlobalAlloc(GHND | GMEM_SHARE , m_dwBufferSize);
		m_lpInBuffer[i] = (LPBYTE)GlobalLock(m_hInBuffer[i]);
		m_lpWaveInHdr[i]->lpData = (LPSTR)m_lpInBuffer[i];
		m_lpWaveInHdr[i]->dwBufferLength = m_dwBufferSize;
		m_lpWaveInHdr[i]->dwBytesRecorded = 0L;
		m_lpWaveInHdr[i]->dwUser = (DWORD)(void*)this;
		m_lpWaveInHdr[i]->dwFlags = 0L;
		m_lpWaveInHdr[i]->dwLoops = 1L;
		m_lpWaveInHdr[i]->lpNext = NULL;
		m_lpWaveInHdr[i]->reserved = 0L;
	}

	m_waveClass.lpData = this;
	if(waveInOpen((LPHWAVEIN)&m_waveClass, m_RecSndCard, &m_WaveFormat, (DWORD)waveInProc , (DWORD)this, (DWORD)CALLBACK_FUNCTION) || m_waveClass.hWave==0)
		return 2;
	m_waveClass.lpData = this;
	m_hWaveIn = (HWAVEIN)m_waveClass.hWave;
	m_RecEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	m_RecDeviceOpen=TRUE;

    return 0;
}

int RWAudio::RecStart()
{
	if(!m_RecDeviceOpen)
	{
		printf("Device not Opened. Please open device before attempting to Start\n");
		return 1;
	}

	for(int i=0; i<MAXNUMOFBUFFER; i++)
	{
		// Prepare wave in header
		if(waveInPrepareHeader(m_hWaveIn, m_lpWaveInHdr[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			return 2;

		// Add buffer into recording queue
		if(waveInAddBuffer(m_hWaveIn, m_lpWaveInHdr[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			return 3;
	}
	// Begin sampling
	m_Recording = TRUE;
	m_RecThread = CreateThread( NULL, 0, RWAudioRecThreadFunc, this, 0, NULL);
	waveInStart(m_hWaveIn);
	//ASSERT(m_RecThread!=NULL);
	SetPriorityClass(m_RecThread,REALTIME_PRIORITY_CLASS);
	SetThreadPriority(m_RecThread,THREAD_PRIORITY_HIGHEST);

	return 0;
}

int RWAudio::RecStop()
{
	if(!m_RecDeviceOpen || !m_Recording)
		return 1;
	if(waveInStop(m_hWaveIn) != MMSYSERR_NOERROR)
		return 2;
	else
		m_Recording=FALSE;

	return 0;
}

int RWAudio::RecClose()
{
	if(m_Recording)
        RecStop();
	if (m_RecThread != NULL)
		CloseHandle(m_RecThread);
	if(m_RecDeviceOpen)
		waveInClose(m_hWaveIn);
	for(int i=0; i<MAXNUMOFBUFFER; i++)
	{
		if (m_hWaveInHdr[i] != NULL)
		{
			if (GlobalUnlock(m_hWaveInHdr[i]))
				GlobalFree(m_hWaveInHdr[i]);
			if (GlobalUnlock(m_hInBuffer[i]))
				GlobalFree(m_hInBuffer[i]);
			m_hWaveInHdr[i] = NULL;
			m_hInBuffer[i] = NULL;
		}
	}
	m_RecDeviceOpen=FALSE;
	m_Recording = FALSE;
	m_RecThread = NULL;

	return 0;
}

void RWAudio::ProcessNextBuffer(LPWAVEHDR pwh)
{
	if (fnProcessBuffer != NULL)
		fnProcessBuffer( pwh);
	waveInUnprepareHeader(m_hWaveIn, pwh, sizeof(WAVEHDR));
	waveInPrepareHeader (m_hWaveIn, pwh, sizeof(WAVEHDR));
	waveInAddBuffer(m_hWaveIn, pwh, sizeof(WAVEHDR));
}

BOOL RWAudio::IsFormatSupported(WAVEFORMATEX wfEx, UINT nDev)
{
	MMRESULT mm = waveInOpen(0,nDev,&wfEx,0,0,WAVE_FORMAT_QUERY);
	return (BOOL)mm == MMSYSERR_NOERROR;		
}


/********************************************************************/
/*************          Playing part              *******************/
/********************************************************************/
int RWAudio::PlayOpen( void)
{
    if(m_PlayDeviceOpen)
	{
		printf("PLAY Device Already Opened. Please close it before attempting to Open\n");
		return 1;
	}

	if( MMSYSERR_NOERROR != waveOutOpen( &m_hWaveOut, m_PlaySndCard, &m_WaveFormat, (DWORD)waveOutProc , (DWORD)this, (DWORD)CALLBACK_FUNCTION))
		return 2;

	m_PlayDeviceOpen=TRUE;
	m_Playing = TRUE;
	// Alloc the sample buffers.
	for (int i=0;i< MAXNUMOFBUFFER;i++) {
#ifdef _DEBUG
fprintf(ddbg,"Memory: allocate %d buffer\n",i); fflush(ddbg);
#endif
	  m_pSoundBuffer[i] = malloc(m_dwBufferSize);
#ifdef _DEBUG
fprintf(ddbg,"Memory: memsetting %d buffer\n",i); fflush(ddbg);
#endif
	  memset( &m_WaveOutHdr[i],0,sizeof(WAVEHDR));
	}
#ifdef _DEBUG
fprintf(ddbg,"Memory allocated\n"); fflush(ddbg);
#endif
	m_PlaySilence = 1; /* Play silent sound */
	m_PlayBufCntr = 0;
	for(int i=0; i<MAXNUMOFBUFFER; i++) {
	  	  FillNextBuffer();
	}

    return 0;
}


int RWAudio::PlayStart()
{
  m_PlaySilence = 0;
}

int RWAudio::PlayStop()
{
  m_PlaySilence = 1;
}

int RWAudio::PlayClose()
{
  if(m_Playing)  PlayStop();
  m_Playing = FALSE;
  //	if (m_PlayThread != NULL)		CloseHandle(m_PlayThread);
  //fprintf(ddbg,"RWAudio::PlayClose(): after PlayStop\n"); fflush(ddbg);
  if(m_PlayDeviceOpen) {
    //fprintf(ddbg,"RWAudio::PlayClose():Before RESET\n"); fflush(ddbg);
    waveOutReset(m_hWaveOut);					// Reset tout.
    //fprintf(ddbg,"RWAudio::PlayClose(): Afetr RESET\n"); fflush(ddbg);
    for (int i=0;i<MAXNUMOFBUFFER;i++)
      {
	//fprintf(ddbg,"RWAudio::PlayClose(): Releasing buffer Nr. %d\n", i); fflush(ddbg);
	if (m_WaveOutHdr[ m_PlayBufCntr].dwFlags & WHDR_PREPARED)
	  waveOutUnprepareHeader(m_hWaveOut,&m_WaveOutHdr[i],sizeof(WAVEHDR));

	free(m_pSoundBuffer[i]);
      }
    waveOutClose(m_hWaveOut);
  }

	m_PlayDeviceOpen=FALSE;

	return 0;
}

int RWAudio::FillNextBuffer()
{
  signed short * b;
  signed short sample;
  long int i;
 
  if (  m_Playing == TRUE) {

  // check if the buffer is already prepared (should not !)
  if (m_WaveOutHdr[m_PlayBufCntr].dwFlags&WHDR_PREPARED)
    waveOutUnprepareHeader(m_hWaveOut,&m_WaveOutHdr[m_PlayBufCntr],sizeof(WAVEHDR));

  //naplnit hodnoty z generatoru
  b = (signed short *) m_pSoundBuffer[m_PlayBufCntr];
#ifdef _DEBUG
fprintf(ddbg,"Filling buffer %d\n", m_PlayBufCntr); fflush(ddbg);
#endif
  for ( i = 0; i < m_dwBufferSize; i=i+4) {

    /* tady to obalit podle m_genShape_l/m_genShape_r - obdelnik, pila, trojuhelnik atd. */
    double y = 0;
    double y2 = 0;
    /* left channel */
    switch( m_genShape_l)
      {
      case 1:
	if ( m_genFI_l < PI)
	  {
	    y = 1.0* m_genGain_l;
	  } else {
	    y = 1.0* -m_genGain_l;
	  }
	break;
      case 2:
	y  = m_genGain_l * (m_genFI_l - PI)/PI;
	break;
      case 3:
	if ( m_genFI_l < PI)
	  {
	    y = m_genGain_l * 2 * (m_genFI_l - PI/2)/PI;
	  } else {
	    y = m_genGain_l * 2*(3*PI/2 - m_genFI_l)/PI;
	  }
	break;
      default: /* sine wave */
	y  = m_genGain_l * sin( m_genFI_l);
	break;
      }
    /* right channel */
    switch( m_genShape_r)
      {
      case 1:
	if ( (m_genFI_r - m_genPhaseDif) < PI)
	  {
	    y2 = m_genGain_r;
	  } else {
	    y2 = -m_genGain_r;
	  }
	break;
      case 2:
	y2 = m_genGain_r * ((m_genFI_r - m_genPhaseDif)-PI)/PI;
	break;
      case 3:
	if ( m_genFI_l < PI)
	  {
	    y2 = m_genGain_r * 2 * (m_genFI_r - m_genPhaseDif - PI/2)/PI;
	  } else {
	    y2 = m_genGain_r * 2*(3*PI/2 - m_genFI_r + m_genPhaseDif)/PI;
	  }
	break;
      default: /* sine wave */
	y2 = m_genGain_r * sin( m_genFI_r - m_genPhaseDif);
	break;
      }
    /* konec obalu */

    m_genFI_l += (float) 2.0 * PI * m_genFR_l/m_WaveFormat.nSamplesPerSec;
    m_genFI_r += (float) 2.0 * PI * m_genFR_r/m_WaveFormat.nSamplesPerSec;

    if ( (2.0*PI) < m_genFI_l) m_genFI_l -= 2.0*PI;
    if ( (2.0*PI) < m_genFI_r) m_genFI_r -= 2.0*PI;

      *b++ = (signed short) (32768.f * y);
      *b++ = (signed short) (32768.f * y2);
  }

  // Prepare the buffer to be sent to the WaveOut API
  m_WaveOutHdr[m_PlayBufCntr].lpData = (char*)m_pSoundBuffer[m_PlayBufCntr];
  m_WaveOutHdr[m_PlayBufCntr].dwBufferLength = m_dwBufferSize;
  waveOutPrepareHeader(m_hWaveOut,&m_WaveOutHdr[m_PlayBufCntr],sizeof(WAVEHDR));

  // Send the buffer the the WaveOut queue
  waveOutWrite(m_hWaveOut,&m_WaveOutHdr[m_PlayBufCntr],sizeof(WAVEHDR));

  m_PlayBufCntr++;
  if (MAXNUMOFBUFFER <= m_PlayBufCntr) m_PlayBufCntr = 0;
  }
}

int RWAudio::PlaySetGenerator( float f1, float f2, int s1, int s2, float g1, float g2)
{
  if ( 2*f1 < m_WaveFormat.nSamplesPerSec) m_genFR_l = f1;
  else m_genFR_l = m_WaveFormat.nSamplesPerSec/2;
  if ( 2*f2 < m_WaveFormat.nSamplesPerSec) m_genFR_r = f2;
  else m_genFR_r = m_WaveFormat.nSamplesPerSec/2;

  m_genShape_l = s1;
  m_genShape_r = s2;

  if ( 1.0 > g1) {
    m_genGain_l = g1;
  } else {
    m_genGain_l = 0.99999;
  }
  if ( 1.0 > g2) {
    m_genGain_r = g2;
  } else {
    m_genGain_r = 0.99999;
  }
}

int RWAudio::GetPlayDevices(SndDevArray * devlist)
{
  WAVEOUTCAPS     woc;
  unsigned long   iNumDevs;
  int i;

  /* pocet zarizeni */
  iNumDevs = waveOutGetNumDevs();
  /* Go through all of those devices, displaying their names */
  /* just first 5 devices to choose from... */
  for (i = 0; i < iNumDevs; i++) {
    /* Get info about the next device */
    if (!waveOutGetDevCaps(i, &woc, sizeof(WAVEOUTCAPS))) {
      /* Display its Device ID and name */
      //*devlist[i] = (char*) malloc(sizeof(woc.szPname+1));
      strcpy((*devlist)[i], woc.szPname);
      //printf("Device ID #%u: %s\r\n", i, woc.szPname);
      //MessageBox(0,woc.szPname,woc.szPname,0);
    }
  }

  return iNumDevs;
}

int RWAudio::GetRecordDevices(SndDevArray * devlist)
{
  WAVEINCAPS     wic;
  unsigned long   iNumDevs;
  int i;

  iNumDevs = waveInGetNumDevs();

  for (i = 0; i < iNumDevs; i++) {
    if (!waveInGetDevCaps(i, &wic, sizeof(WAVEINCAPS))) {
      //*devlist[i] = (char*) malloc(sizeof(wic.szPname));
      strcpy((*devlist)[i], wic.szPname);
    }
  }

  return iNumDevs;
}
