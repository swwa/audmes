//////////////////////////////////////////////////////////////////////
// RWAudio_IO.cpp: impementation of audio interface
//
//////////////////////////////////////////////////////////////////////

#include "RWAudio_IO.h"
#include <stdio.h>
#include <math.h>

//#define _DEBUG

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
#ifdef _DEBUG
      fprintf(ddbg,"Jsme tady ");
#endif

  RWAudio * aRWAudioClass = (RWAudio *) data;
  unsigned long i;

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

  short * inBuf = (short *) inputBuffer;

  // make a copy for oscilloscope
  if ( 0 == g_OscBufferChanged) {
    for (i = 0; i < nBufferFrames; i++) {
      //copy audio signal to fft real component.
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

  inBuf = (short *) inputBuffer;
  // make a copy for spectrum analyzer
  if ( 0 == g_SpeBufferChanged) {

    for (i = 0; i < nBufferFrames; i++) {
      //copy audio signal to fft real component.
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
  short * outBuf = (short *) outputBuffer;

#ifdef _DEBUG
      fprintf(ddbg,"\n Frames: %d\n ",nBufferFrames);
#endif

  for ( unsigned long i = 0; i < nBufferFrames; i++) {

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

      *outBuf++ = (short)(32768.f * y);
      *outBuf++ = (short)(32768.f * y2);
#ifdef _DEBUG
      //fprintf(ddbg,"%04X %04X ",(short)(32768.f * y), (short)(32768.f * y2));
#endif
  }
  return 0;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RWAudio::RWAudio( long int oscbufferlen, long int spebufferlen)
{
  m_DrvRunning = 0;

#ifdef _DEBUG
  ddbg = fopen("debug.log","wb");
      fprintf(ddbg,"Novy soubor\n");
#endif
  // init of RtAudio
  if (m_AudioDriver.getDeviceCount() < 1 ) {
#ifdef _DEBUG
      fprintf(ddbg,"No devices??? Nbr = %d  \n",m_AudioDriver.getDeviceCount());
#endif
  } else {
    // everything is ok
    RtAudio::StreamParameters iParams;
    RtAudio::StreamParameters oParams;
    RtAudio::StreamOptions rtAOptions;
    unsigned int bufferFrames = 512;

    m_sampleRate = 44100;

    iParams.deviceId = m_AudioDriver.getDefaultInputDevice();
    oParams.deviceId = m_AudioDriver.getDefaultOutputDevice();
    iParams.nChannels = 2;
    oParams.nChannels = 2;
    iParams.firstChannel = 0;
    oParams.firstChannel = 0;

    rtAOptions.flags |= RTAUDIO_NONINTERLEAVED;

    try {
#ifdef _DEBUG
      fprintf(ddbg,"Initing Driver: \n");
#endif
      m_AudioDriver.openStream( &oParams, &iParams, RTAUDIO_SINT16, m_sampleRate, &bufferFrames, &inout, (void *)this);//, &rtAOptions );
    }
    catch ( RtError& e ) {
      strcpy( m_ErrorMessage, e.getMessage().c_str() );
      //std::cout << '\n' << e.getMessage() << '\n' << std::endl;
#ifdef _DEBUG
      fprintf(ddbg,"Driver error: %s\n", e.getMessage().c_str());
#endif
      exit( 1 );
    }
    m_bufferBytes = bufferFrames * iParams.nChannels * sizeof( short );

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

#ifdef _DEBUG
      fprintf(ddbg,"Driver is running\n");
#endif
  }
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

  // if (f1 < 0.001) m_genFI_l = 0;
  // if (f2 < 0.001) m_genFI_r = 0;

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
