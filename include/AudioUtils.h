#ifndef AUDIO_UTILS_H
#define AUDIO_UTILS_H

#include <complex>
#include <vector>

// Function declarations
std::vector<double> smoothAudioData(const std::vector<double> &audioData,
                                    int smoothness);
void trimTrailingZeros(std::vector<double> &audioBuffer);
bool isAudioPlaying(const std::vector<double> &audioBuffer);
void normalizeAudioData(std::vector<double> &audioData);

// Add FFT and spectral flux function declarations
std::vector<std::complex<double>> fft(const std::vector<double> &input);
double computeSpectralFlux(const std::vector<double> &currentFrame,
                           const std::vector<double> &previousFrame);

#endif // AUDIO_UTILS_H
