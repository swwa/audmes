//////////////////////////////////////////////////////////////////////
// RWAudio_IO.cpp: impementation of audio interface
//
//////////////////////////////////////////////////////////////////////

#include "RWAudio_IO.h"
#include <stdio.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define PI            3.14159265358979

#ifdef _DEBUG
FILE * ddbg;
#endif

//extern variables from AudMeS.cpp
extern short * g_OscBuffer_Left;
extern short * g_OscBuffer_Right;
extern long int g_OscBufferPosition;

extern short * g_SpeBuffer_Left;
extern short * g_SpeBuffer_Right;
extern long int g_SpeBufferPosition;

extern int g_OscBufferChanged;
extern int g_SpeBufferChanged;

//////////////////////////////////////////////////////////////////////
// Callback functions
//////////////////////////////////////////////////////////////////////
int inout( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
		   double streamTime, RtAudioStreamStatus status, void *data )
{
  RWAudio * aRWAudioClass = (RWAudio *) data;
  unsigned long bytes = aRWAudioClass->m_bufferBytes;

  // copy input buffer into two L/R channels
  if( 0 != aRWAudioClass->m_Buflen_Changed ) {
    aRWAudioClass->m_Buflen_Changed = 0;
    free( g_OscBuffer_Left);
    free( g_OscBuffer_Right);
    free( g_SpeBuffer_Left);
    free( g_SpeBuffer_Right);
    g_OscBufferPosition = 0;
    g_SpeBufferPosition = 0;
    g_OscBuffer_Left = (short *) malloc( aRWAudioClass->m_OscBufferLen* sizeof(short));
    g_OscBuffer_Right = (short *) malloc( aRWAudioClass->m_OscBufferLen* sizeof(short));
    g_SpeBuffer_Left = (short *) malloc( aRWAudioClass->m_SpeBufferLen* sizeof(short));
    g_SpeBuffer_Right = (short *) malloc( aRWAudioClass->m_SpeBufferLen* sizeof(short));

  }
//   for (unsigned long dw = 0; dw < bytes/2; dw++) {
//     //copy audio signal to fft real component.
//     g_InBuffer_Left[g_InBufferPosition] = (double)((short*)inputBuffer)[2*dw];
//     g_InBuffer_Right[g_InBufferPosition] = (double)((short*)inputBuffer)[2*dw+1];
//     g_InBufferPosition++;
//     if (g_InBufferPosition == aRWAudioClass->m_InBufferLen) {
//       g_InBufferPosition = 0;
//       g_InBufferChanged = 1;
//     }
//   }



  // fill output buffer
  unsigned short * outBuf = (unsigned short *) outputBuffer;


  for ( unsigned long i = 0; i < bytes/2; i=i+4) {

    /* tady to obalit podle m_genShape_l/m_genShape_r - obdelnik, pila, trojuhelnik atd. */
    double y = 0;
    double y2 = 0;
    /* left channel */
    switch( aRWAudioClass->m_genShape_l)
      {
      case 1:
	if ( aRWAudioClass->m_genFI_l < PI)
	  {
	    y = 1.0* aRWAudioClass->m_genGain_l;
	  } else {
	    y = 1.0* -aRWAudioClass->m_genGain_l;
	  }
	break;
      case 2:
	y  = aRWAudioClass->m_genGain_l * (aRWAudioClass->m_genFI_l - PI)/PI;
	break;
      case 3:
	if ( aRWAudioClass->m_genFI_l < PI)
	  {
	    y = aRWAudioClass->m_genGain_l * 2 * (aRWAudioClass->m_genFI_l - PI/2)/PI;
	  } else {
	    y = aRWAudioClass->m_genGain_l * 2*(3*PI/2 - aRWAudioClass->m_genFI_l)/PI;
	  }
	break;
      default: /* sine wave */
	y  = aRWAudioClass->m_genGain_l * sin( aRWAudioClass->m_genFI_l);
	break;
      }
    /* right channel */
    switch( aRWAudioClass->m_genShape_r)
      {
      case 1:
	if ( (aRWAudioClass->m_genFI_r - aRWAudioClass->m_genPhaseDif) < PI)
	  {
	    y2 = aRWAudioClass->m_genGain_r;
	  } else {
	    y2 = -aRWAudioClass->m_genGain_r;
	  }
	break;
      case 2:
	y2 = aRWAudioClass->m_genGain_r * ((aRWAudioClass->m_genFI_r - aRWAudioClass->m_genPhaseDif)-PI)/PI;
	break;
      case 3:
	if ( aRWAudioClass->m_genFI_l < PI)
	  {
	    y2 = aRWAudioClass->m_genGain_r * 2 * (aRWAudioClass->m_genFI_r - aRWAudioClass->m_genPhaseDif - PI/2)/PI;
	  } else {
	    y2 = aRWAudioClass->m_genGain_r * 2*(3*PI/2 - aRWAudioClass->m_genFI_r + aRWAudioClass->m_genPhaseDif)/PI;
	  }
	break;
      default: /* sine wave */
	y2 = aRWAudioClass->m_genGain_r * sin( aRWAudioClass->m_genFI_r - aRWAudioClass->m_genPhaseDif);
	break;
      }
    /* konec obalu */

    aRWAudioClass->m_genFI_l += (float) 2.0 * PI * aRWAudioClass->m_genFR_l/aRWAudioClass->m_sampleRate;
    aRWAudioClass->m_genFI_r += (float) 2.0 * PI * aRWAudioClass->m_genFR_r/aRWAudioClass->m_sampleRate;

    if ( (2.0*PI) < aRWAudioClass->m_genFI_l) aRWAudioClass->m_genFI_l -= 2.0*PI;
    if ( (2.0*PI) < aRWAudioClass->m_genFI_r) aRWAudioClass->m_genFI_r -= 2.0*PI;

      *outBuf++ = (signed short) (32768.f * y);
      *outBuf++ = (signed short) (32768.f * y2);
  }

}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RWAudio::RWAudio( long int oscbufferlen, long int spebufferlen)
{
  m_DrvRunning = 0;

  // init of RtAudio
  if (m_AudioDriver.getDeviceCount() < 1 ) {
  } else {
    // everything is ok
    RtAudio::StreamParameters iParams;
    RtAudio::StreamParameters oParams;
    RtAudio::StreamOptions rtAOptions;
    unsigned int bufferFrames = 1024;

    m_sampleRate = 44100;

    iParams.deviceId = m_AudioDriver.getDefaultInputDevice();
    oParams.deviceId = m_AudioDriver.getDefaultOutputDevice();
    iParams.nChannels = 2;
    oParams.nChannels = 2;
    iParams.firstChannel = 0;
    oParams.firstChannel = 0;

    rtAOptions.flags |= RTAUDIO_NONINTERLEAVED;

    try {
      m_AudioDriver.openStream( &oParams, &iParams, RTAUDIO_SINT16, m_sampleRate, &bufferFrames, &inout, (void *)this, &rtAOptions );
    }
    catch ( RtError& e ) {
      strcpy( m_ErrorMessage, e.getMessage().c_str() );
      //std::cout << '\n' << e.getMessage() << '\n' << std::endl;
      exit( 1 );
    }
    m_bufferBytes = bufferFrames * iParams.nChannels * sizeof( int );

    m_genFR_l = m_genFR_r = 0.0;
    m_genShape_l = m_genShape_r = 0;
    m_genGain_l = m_genGain_r = 1.0;
    m_genFI_l = m_genFI_r = 0.0;
    m_genPhaseDif = 0.0;

    m_OscBufferLen = oscbufferlen;
    m_SpeBufferLen = spebufferlen;
    m_Buflen_Changed = 0;

    g_OscBufferPosition = 0;
    g_SpeBufferPosition = 0;
    g_OscBuffer_Left = (short *) malloc( m_OscBufferLen* sizeof(short));
    g_OscBuffer_Right = (short *) malloc( m_OscBufferLen* sizeof(short));
    g_SpeBuffer_Left = (short *) malloc( m_SpeBufferLen* sizeof(short));
    g_SpeBuffer_Right = (short *) malloc( m_SpeBufferLen* sizeof(short));

    m_AudioDriver.startStream();

    m_DrvRunning = 1;

  }

#ifdef _DEBUG
  ddbg = fopen("debug.log","wb");
#endif
}

RWAudio::~RWAudio()
{
  m_AudioDriver.stopStream();
  m_AudioDriver.closeStream();

#ifdef _DEBUG
	fclose(ddbg);
#endif
}

/********************************************************************/
/*************         Recording part             *******************/
/********************************************************************/
int RWAudio::GetRecordDevices(SndDevArray * devlist)
{
//   WAVEINCAPS     wic;
//   unsigned long   iNumDevs;
//   int i;

//   iNumDevs = waveInGetNumDevs();

//   for (i = 0; i < iNumDevs; i++) {
//     if (!waveInGetDevCaps(i, &wic, sizeof(WAVEINCAPS))) {
//       //*devlist[i] = (char*) malloc(sizeof(wic.szPname));
//       strcpy((*devlist)[i], wic.szPname);
//     }
//   }

//   return iNumDevs;
  return 0;
}


/********************************************************************/
/*************          Playing part              *******************/
/********************************************************************/
int RWAudio::GetPlayDevices(SndDevArray * devlist)
{
//   WAVEOUTCAPS     woc;
//   unsigned long   iNumDevs;
//   int i;

//   /* pocet zarizeni */
//   iNumDevs = waveOutGetNumDevs();
//   /* Go through all of those devices, displaying their names */
//   /* just first 5 devices to choose from... */
//   for (i = 0; i < iNumDevs; i++) {
//     /* Get info about the next device */
//     if (!waveOutGetDevCaps(i, &woc, sizeof(WAVEOUTCAPS))) {
//       /* Display its Device ID and name */
//       //*devlist[i] = (char*) malloc(sizeof(woc.szPname+1));
//       strcpy((*devlist)[i], woc.szPname);
//       //printf("Device ID #%u: %s\r\n", i, woc.szPname);
//       //MessageBox(0,woc.szPname,woc.szPname,0);
//     }
//   }

//   return iNumDevs;

  return 0;
}

/********************************************************************/
/*************      Parameter settings            *******************/
/********************************************************************/
int RWAudio::PlaySetGenerator( float f1, float f2, int s1, int s2, float g1, float g2)
{
  if ( 2*f1 < m_sampleRate) m_genFR_l = f1;
  else m_genFR_l = m_sampleRate/2;
  if ( 2*f2 < m_sampleRate) m_genFR_r = f2;
  else m_genFR_r = m_sampleRate/2;

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

  return 1;
}

void RWAudio::SetSndDevices( int irec, int iplay)
{
  if ( -1 == irec) {
    m_RecSndCard = m_AudioDriver.getDefaultInputDevice();
  } else {
    m_RecSndCard = irec;
  }
  if (-1 == iplay) {
    m_PlaySndCard = m_AudioDriver.getDefaultOutputDevice();
  } else {
    m_PlaySndCard = iplay;
  }
}
