#ifndef AUDIOCAPTURE_H
#define AUDIOCAPTURE_H

#define NOMINMAX // Define NOMINMAX before including any Windows headers
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <vector>
#include <windows.h>

namespace capture {

class AudioCapture {
public:
  AudioCapture(UINT32 bufferSize);
  ~AudioCapture();
  bool initialize();
  bool captureAudio(std::vector<double> &audioBuffer);

  UINT32 getSampleRate() const { return pwfx->nSamplesPerSec; }

private:
  void releaseResources();

  IMMDeviceEnumerator *pEnumerator;
  IMMDevice *pDevice;
  IAudioClient *pAudioClient;
  IAudioCaptureClient *pCaptureClient;
  WAVEFORMATEX *pwfx;
  HRESULT hr;
  UINT32 bufferSize; // Member variable to store buffer size
};

} // namespace capture

#endif // AUDIOCAPTURE_H
