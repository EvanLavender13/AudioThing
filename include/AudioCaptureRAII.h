#ifndef AUDIO_CAPTURE_RAII_H
#define AUDIO_CAPTURE_RAII_H

#include "AudioCapture.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

// RAII wrapper for AudioCapture to handle automatic cleanup and thread
// management
class AudioCaptureRAII {
public:
  // Constructor automatically initializes capture and starts the thread
  AudioCaptureRAII(int bufferSize, std::vector<double> &sharedBuffer,
                   std::mutex &bufferMutex, std::condition_variable &bufferCV,
                   std::atomic<bool> &running, bool &bufferReady)
      : audioCapture_(bufferSize), sharedBuffer_(sharedBuffer),
        bufferMutex_(bufferMutex), bufferCV_(bufferCV), running_(running),
        bufferReady_(bufferReady) {

    if (!audioCapture_.initialize()) {
      throw std::runtime_error("Failed to initialize audio capture");
    }

    // Start the capture thread
    captureThread_ = std::thread(&AudioCaptureRAII::captureLoop, this);
  }

  // Destructor automatically stops the thread and cleans up
  ~AudioCaptureRAII() {
    // Signal the thread to stop
    running_ = false;

    // Wait for the thread to finish
    if (captureThread_.joinable()) {
      captureThread_.join();
    }
  }

  // Deleted copy/move constructors and assignment to ensure proper RAII
  // behavior
  AudioCaptureRAII(const AudioCaptureRAII &) = delete;
  AudioCaptureRAII &operator=(const AudioCaptureRAII &) = delete;
  AudioCaptureRAII(AudioCaptureRAII &&) = delete;
  AudioCaptureRAII &operator=(AudioCaptureRAII &&) = delete;

private:
  // The audio capture loop that runs in a separate thread
  void captureLoop() {
    std::vector<double> tempBuffer(sharedBuffer_.size());

    while (running_) {
      if (!audioCapture_.captureAudio(tempBuffer)) {
        running_ = false;
        break;
      }

      {
        std::lock_guard<std::mutex> lock(bufferMutex_);
        std::swap(sharedBuffer_, tempBuffer);
        bufferReady_ = true;
      }

      bufferCV_.notify_one();

      // Control capture rate
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  capture::AudioCapture audioCapture_;
  std::vector<double> &sharedBuffer_;
  std::mutex &bufferMutex_;
  std::condition_variable &bufferCV_;
  std::atomic<bool> &running_;
  bool &bufferReady_;
  std::thread captureThread_;
};

#endif // AUDIO_CAPTURE_RAII_H
