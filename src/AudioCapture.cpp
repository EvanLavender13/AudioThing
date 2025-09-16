#include "AudioCapture.h"
#include <iostream>
#include <numeric> // Include this header for std::accumulate

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "winmm.lib")

namespace capture {

AudioCapture::AudioCapture(UINT32 bufferSize)
    : pEnumerator(nullptr), pDevice(nullptr), pAudioClient(nullptr),
      pCaptureClient(nullptr), pwfx(nullptr), hr(S_OK), bufferSize(bufferSize) {
}

AudioCapture::~AudioCapture() { releaseResources(); }

void AudioCapture::releaseResources() {
  if (pAudioClient) {
    pAudioClient->Stop();
  }
  if (pCaptureClient) {
    pCaptureClient->Release();
  }
  if (pAudioClient) {
    pAudioClient->Release();
  }
  if (pDevice) {
    pDevice->Release();
  }
  if (pEnumerator) {
    pEnumerator->Release();
  }
  if (pwfx) {
    CoTaskMemFree(pwfx);
  }
  CoUninitialize();
}

bool AudioCapture::initialize() {
  hr = CoInitialize(nullptr);
  if (FAILED(hr)) {
    std::cerr << "Failed to initialize COM. Error: " << std::hex << hr
              << std::endl;
    return false;
  }

  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                        __uuidof(IMMDeviceEnumerator), (void **)&pEnumerator);
  if (FAILED(hr)) {
    std::cerr << "Failed to create device enumerator. Error: " << std::hex << hr
              << std::endl;
    return false;
  }

  hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
  if (FAILED(hr)) {
    std::cerr << "Failed to get default audio endpoint. Error: " << std::hex
              << hr << std::endl;
    return false;
  }

  hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                         (void **)&pAudioClient);
  if (FAILED(hr)) {
    std::cerr << "Failed to activate audio client. Error: " << std::hex << hr
              << std::endl;
    return false;
  }

  hr = pAudioClient->GetMixFormat(&pwfx);
  if (FAILED(hr)) {
    std::cerr << "Failed to get mix format. Error: " << std::hex << hr
              << std::endl;
    return false;
  }

  hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, pwfx,
                                nullptr);
  if (FAILED(hr)) {
    std::cerr << "Failed to initialize audio client. Error: " << std::hex << hr
              << std::endl;
    return false;
  }

  hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient),
                                (void **)&pCaptureClient);
  if (FAILED(hr)) {
    std::cerr << "Failed to get capture client. Error: " << std::hex << hr
              << std::endl;
    return false;
  }

  hr = pAudioClient->Start();
  if (FAILED(hr)) {
    std::cerr << "Failed to start audio client. Error: " << std::hex << hr
              << std::endl;
    return false;
  }

  return true;
}

bool AudioCapture::captureAudio(std::vector<double> &audioBuffer) {
  UINT32 packetLength = 0;
  hr = pCaptureClient->GetNextPacketSize(&packetLength);
  if (FAILED(hr)) {
    std::cerr << "Failed to get next packet size. Error: " << std::hex << hr
              << std::endl;
    return false;
  }

  while (packetLength != 0) {
    BYTE *pData = nullptr;
    UINT32 numFramesAvailable = 0;
    DWORD flags = 0;

    hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, nullptr,
                                   nullptr);
    if (FAILED(hr)) {
      std::cerr << "Failed to get buffer. Error: " << std::hex << hr
                << std::endl;
      return false;
    }

    // Ensure buffer is large enough once
    if (audioBuffer.size() < numFramesAvailable) {
      audioBuffer.resize(numFramesAvailable, 0.0);
    }

    if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
      std::fill(audioBuffer.begin(), audioBuffer.begin() + numFramesAvailable,
                0.0);
      // Optionally log silence only in debug mode
    } else {
      const float *floatData = reinterpret_cast<const float *>(pData);
      for (UINT32 i = 0; i < numFramesAvailable; ++i) {
        audioBuffer[i] = static_cast<double>(floatData[i]);
      }
    }

    hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
    if (FAILED(hr)) {
      std::cerr << "Failed to release buffer. Error: " << std::hex << hr
                << std::endl;
      return false;
    }

    hr = pCaptureClient->GetNextPacketSize(&packetLength);
    if (FAILED(hr)) {
      std::cerr << "Failed to get next packet size. Error: " << std::hex << hr
                << std::endl;
      return false;
    }
  }

  return true;
}

} // namespace capture
