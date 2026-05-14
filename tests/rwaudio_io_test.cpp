#include <wx/defs.h>  // for WXUNUSED
#include <rtaudio/RtAudio.h>
#include <RWAudio_IO.h>

#include <cmath>
#include <string>
#include <rtaudio/RtAudio.h>
#include <atomic>

extern int inout(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
                 double WXUNUSED(streamTime), RtAudioStreamStatus status, void* data);

float* g_OscBuffer_Left;
float* g_OscBuffer_Right;
long int g_OscBufferPosition;
std::atomic<bool> g_OscBufferChanged{false};
float* g_SpeBuffer_Left;
float* g_SpeBuffer_Right;
long int g_SpeBufferPosition;
std::atomic<bool> g_SpeBufferChanged{false};

int main() {
  const unsigned int nf = 128;
  float input[nf * 2];
  float output[nf * 2];
  std::string rwinfo = "xx";
  RWAudio* m_RWAudio = new RWAudio();

  int ret = m_RWAudio->InitSnd((nf * 4), nf * 4, rwinfo, 44100);
  if (ret) {
    std::cout << "error in initsnd" << std::endl;
    return 1;
  }

  std::cout << rwinfo << std::endl;

  ret = inout(output, input, nf, 0, 0, m_RWAudio);
  ret = inout(output, input, nf, 0, 0, m_RWAudio);
  ret = inout(output, input, nf, 0, 0, m_RWAudio);
  ret = inout(output, input, nf, 0, 0, m_RWAudio);
  ret = inout(output, input, nf, 0, 0, m_RWAudio);

  return 0;
}
