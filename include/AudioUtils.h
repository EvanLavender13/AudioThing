#ifndef AUDIO_UTILS_H
#define AUDIO_UTILS_H

#include <vector>

// Function declarations
void smoothAudioData(const std::vector<double> &audioData,
                     std::vector<double> &smoothedData,
                     int smoothness);
void trimTrailingZeros(std::vector<double> &audioBuffer);
bool isAudioPlaying(const std::vector<double> &audioBuffer);
void normalizeAudioData(std::vector<double> &audioBuffer);

#endif // AUDIO_UTILS_H
