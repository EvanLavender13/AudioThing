#include "AudioUtils.h"
#include <algorithm>
#include <cmath>

// Optimized sliding window smoothing - O(N) instead of O(N * smoothness)
void smoothAudioData(const std::vector<double> &audioData,
                     std::vector<double> &smoothedData,
                     int smoothness) {
  if (smoothness <= 0 || audioData.empty()) {
    smoothedData = audioData;
    return;
  }

  smoothedData.resize(audioData.size());

  // Calculate window size
  int windowSize = 2 * smoothness + 1;

  // Initialize first window sum
  double windowSum = 0.0;
  int count = 0;

  for (int j = -smoothness; j <= smoothness; ++j) {
    if (j >= 0 && static_cast<size_t>(j) < audioData.size()) {
      windowSum += audioData[j];
      ++count;
    }
  }
  smoothedData[0] = windowSum / count;

  // Slide window across the data
  for (size_t i = 1; i < audioData.size(); ++i) {
    // Remove element leaving the window
    int removeIdx = static_cast<int>(i) - smoothness - 1;
    if (removeIdx >= 0) {
      windowSum -= audioData[removeIdx];
      --count;
    }

    // Add element entering the window
    int addIdx = static_cast<int>(i) + smoothness;
    if (addIdx < static_cast<int>(audioData.size())) {
      windowSum += audioData[addIdx];
      ++count;
    }

    smoothedData[i] = windowSum / count;
  }
}

void trimTrailingZeros(std::vector<double> &audioBuffer) {
  while (!audioBuffer.empty() && audioBuffer.back() == 0.0) {
    audioBuffer.pop_back();
  }
}

bool isAudioPlaying(const std::vector<double> &audioBuffer) {
  for (double value : audioBuffer) {
    if (value != 0.0) {
      return true;
    }
  }
  return false;
}

// Optimized to find min and max in single pass
void normalizeAudioData(std::vector<double> &audioBuffer) {
  if (audioBuffer.empty()) {
    return;
  }

  // Find min and max in a single pass
  double maxVal = audioBuffer[0];
  double minVal = audioBuffer[0];

  for (size_t i = 1; i < audioBuffer.size(); ++i) {
    if (audioBuffer[i] > maxVal) {
      maxVal = audioBuffer[i];
    }
    if (audioBuffer[i] < minVal) {
      minVal = audioBuffer[i];
    }
  }

  double absMax = std::max(std::abs(maxVal), std::abs(minVal));
  if (absMax > 0.0) {
    for (auto &sample : audioBuffer) {
      sample /= absMax;
    }
  }
}
