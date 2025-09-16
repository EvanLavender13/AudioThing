#include "AudioUtils.h"
#include <algorithm>
#include <cmath>

const double PI = 3.14159265358979323846;

// Existing functions...

std::vector<double> smoothAudioData(const std::vector<double> &audioData,
                                    int smoothness) {
  std::vector<double> smoothedData(audioData.size());
  for (size_t i = 0; i < audioData.size(); ++i) {
    double sum = 0.0;
    int count = 0;
    for (int j = -smoothness; j <= smoothness; ++j) {
      if (i + j >= 0 && i + j < audioData.size()) {
        sum += audioData[i + j];
        ++count;
      }
    }
    smoothedData[i] = sum / count;
  }
  return smoothedData;
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

void normalizeAudioData(std::vector<double> &audioBuffer) {
  double maxVal = *std::max_element(audioBuffer.begin(), audioBuffer.end());
  double minVal = *std::min_element(audioBuffer.begin(), audioBuffer.end());
  double absMax = std::max(std::abs(maxVal), std::abs(minVal));
  if (absMax > 0.0) {
    for (auto &sample : audioBuffer) {
      sample /= absMax;
    }
  }
}

std::vector<std::complex<double>>
fft(const std::vector<std::complex<double>> &input) {
  size_t N = input.size();
  if (N <= 1)
    return input;

  std::vector<std::complex<double>> even(N / 2);
  std::vector<std::complex<double>> odd(N / 2);
  for (size_t i = 0; i < N / 2; ++i) {
    even[i] = input[i * 2];
    odd[i] = input[i * 2 + 1];
  }

  auto fftEven = fft(even);
  auto fftOdd = fft(odd);

  std::vector<std::complex<double>> result(N);
  for (size_t k = 0; k < N / 2; ++k) {
    std::complex<double> t = std::polar(1.0, -2 * PI * k / N) * fftOdd[k];
    result[k] = fftEven[k] + t;
    result[k + N / 2] = fftEven[k] - t;
  }
  return result;
}

double computeSpectralFlux(const std::vector<double> &currentFrame,
                           const std::vector<double> &previousFrame) {
  std::vector<std::complex<double>> currentComplex(currentFrame.begin(),
                                                   currentFrame.end());
  std::vector<std::complex<double>> previousComplex(previousFrame.begin(),
                                                    previousFrame.end());

  auto currentSpectrum = fft(currentComplex);
  auto previousSpectrum = fft(previousComplex);

  double flux = 0.0;
  for (size_t i = 0; i < currentSpectrum.size(); ++i) {
    double magnitudeCurrent = std::abs(currentSpectrum[i]);
    double magnitudePrevious = std::abs(previousSpectrum[i]);
    flux += std::max(0.0, magnitudeCurrent - magnitudePrevious);
  }
  return flux;
}
