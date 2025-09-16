#ifndef WAVEFORM_DRAWER_H
#define WAVEFORM_DRAWER_H

#include "AudioUtils.h"
#include <algorithm>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Function to convert HSV to RGB
inline sf::Color hsvToRgb(float hue, float saturation, float value) {
  int h = static_cast<int>(hue * 6.0f);
  float f = hue * 6.0f - static_cast<float>(h);
  float p = value * (1.0f - saturation);
  float q = value * (1.0f - f * saturation);
  float t = value * (1.0f - (1.0f - f) * saturation);

  switch (h % 6) {
  case 0:
    return sf::Color(static_cast<sf::Uint8>(value * 255.0f),
                     static_cast<sf::Uint8>(t * 255.0f),
                     static_cast<sf::Uint8>(p * 255.0f));
  case 1:
    return sf::Color(static_cast<sf::Uint8>(q * 255.0f),
                     static_cast<sf::Uint8>(value * 255.0f),
                     static_cast<sf::Uint8>(p * 255.0f));
  case 2:
    return sf::Color(static_cast<sf::Uint8>(p * 255.0f),
                     static_cast<sf::Uint8>(value * 255.0f),
                     static_cast<sf::Uint8>(t * 255.0f));
  case 3:
    return sf::Color(static_cast<sf::Uint8>(p * 255.0f),
                     static_cast<sf::Uint8>(q * 255.0f),
                     static_cast<sf::Uint8>(value * 255.0f));
  case 4:
    return sf::Color(static_cast<sf::Uint8>(t * 255.0f),
                     static_cast<sf::Uint8>(p * 255.0f),
                     static_cast<sf::Uint8>(value * 255.0f));
  case 5:
    return sf::Color(static_cast<sf::Uint8>(value * 255.0f),
                     static_cast<sf::Uint8>(p * 255.0f),
                     static_cast<sf::Uint8>(q * 255.0f));
  default:
    return sf::Color(255, 255, 255);
  }
}

// Cubic interpolation function
inline double cubicInterpolate(double y0, double y1, double y2, double y3,
                               double mu) {
  double mu2 = mu * mu;
  double a0 = y3 - y2 - y0 + y1;
  double a1 = y0 - y1 - a0;
  double a2 = y2 - y0;
  double a3 = y1;

  return (a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3);
}

// Function to draw the waveform
// Function to draw the waveform
inline void drawWaveform(const std::vector<double> &buffer,
                         sf::VertexArray &waveform,
                         sf::VertexArray &thickWaveform, float displayHeight,
                         int smoothness, float rotationAngle,
                         float radiusFactor, float width, float height,
                         float hue, float thickness,
                         float thickWaveformHueOffset = 0.5f) {
  if (buffer.empty()) {
    return;
  }

  // Use static vectors to avoid repeated allocations
  static std::vector<double> extendedBuffer;
  static std::vector<double> smoothedAudioBuffer;

  // Resize once instead of reallocating
  extendedBuffer.resize(buffer.size() * 2);
  std::copy(buffer.begin(), buffer.end(), extendedBuffer.begin());
  std::copy(buffer.rbegin(), buffer.rend(),
            extendedBuffer.begin() + buffer.size());

  // Ensure continuity at the join point to avoid artifacts
  if (!buffer.empty()) {
    // Set the join point to the average of the two ends
    double avg = 0.5 * (buffer.front() + buffer.back());
    extendedBuffer[buffer.size() - 1] = avg;
    extendedBuffer[buffer.size()] = avg;
  }

  const int pointMultiplier = 10;
  const size_t numPoints = extendedBuffer.size() * pointMultiplier;
  waveform.resize(numPoints);
  thickWaveform.clear();

  float centerX = width / 2.0f;
  float centerY = height / 2.0f;
  float radius = std::min(centerX, centerY) * radiusFactor;

  // Smooth the audio data
  smoothedAudioBuffer = smoothAudioData(extendedBuffer, smoothness);

  // Pre-compute values
  float hueOffset = -rotationAngle / (2.0f * static_cast<float>(M_PI));
  float thickHue = fmodf(hue + thickWaveformHueOffset, 1.0f);

  // Cache size calculations to avoid repeating them
  const size_t extSize = extendedBuffer.size();
  const size_t smoothSize = smoothedAudioBuffer.size();

  // Draw normal waveform with fewer redundant calculations
  for (size_t i = 0; i < numPoints; ++i) {
    float angle = static_cast<float>(i) / static_cast<float>(numPoints) * 2.0f *
                      static_cast<float>(M_PI) +
                  rotationAngle;
    float cosAngle = std::cos(angle); // Calculate once
    float sinAngle = std::sin(angle); // Calculate once

    size_t index = (i / pointMultiplier) % extSize;
    float fraction = static_cast<float>(i % pointMultiplier) /
                     static_cast<float>(pointMultiplier);

    // Use fewer modulo operations
    size_t idx0 = (index + extSize - 2) % extSize;
    size_t idx1 = (index + extSize - 1) % extSize;
    size_t idx2 = (index + 1) % extSize;
    size_t idx3 = (index + 2) % extSize;

    float sampleValue = static_cast<float>(
        cubicInterpolate(extendedBuffer[idx0], extendedBuffer[idx1],
                         extendedBuffer[idx2], extendedBuffer[idx3], fraction));

    float r = radius + sampleValue * displayHeight;
    float x = centerX + r * cosAngle; // Reuse cosAngle
    float y = centerY + r * sinAngle; // Reuse sinAngle

    sf::Color color = hsvToRgb(
        hue + hueOffset + static_cast<float>(i) / static_cast<float>(numPoints),
        1.0f, 0.7f);
    waveform[i].position = sf::Vector2f(x, y);
    waveform[i].color = color;
  }

  // Prevent vertical line artifact by not connecting last to first if
  // discontinuous
  if (waveform.getVertexCount() > 1) {
    sf::Vector2f first = waveform[0].position;
    sf::Vector2f last = waveform[waveform.getVertexCount() - 1].position;
    float dist = std::hypot(first.x - last.x, first.y - last.y);
    float threshold = radius * 0.5f; // Heuristic: half the radius
    if (dist > threshold) {
      // Overwrite last vertex to match first, breaking the line
      waveform[waveform.getVertexCount() - 1].position = first;
    }
  }

  // Process thick waveform more efficiently
  for (size_t i = 0; i < numPoints; ++i) {
    float angle = static_cast<float>(i) / static_cast<float>(numPoints) * 2.0f *
                      static_cast<float>(M_PI) +
                  rotationAngle;
    float cosAngle = std::cos(angle);
    float sinAngle = std::sin(angle);

    size_t index = (i / pointMultiplier) % smoothSize;
    float fraction = static_cast<float>(i % pointMultiplier) /
                     static_cast<float>(pointMultiplier);

    size_t idx0 = (index + smoothSize - 2) % smoothSize;
    size_t idx1 = (index + smoothSize - 1) % smoothSize;
    size_t idx2 = (index + 1) % smoothSize;
    size_t idx3 = (index + 2) % smoothSize;

    float sampleValue = static_cast<float>(cubicInterpolate(
        smoothedAudioBuffer[idx0], smoothedAudioBuffer[idx1],
        smoothedAudioBuffer[idx2], smoothedAudioBuffer[idx3], fraction));

    float r = radius + sampleValue * displayHeight;

    sf::Color color =
        hsvToRgb(thickHue + hueOffset +
                     static_cast<float>(i) / static_cast<float>(numPoints),
                 1.0f, 1.0f);
    color.a = 255;

    // Batch vertices for thickWaveform to reduce calls to append
    for (float offset = -thickness / 2.0f; offset <= thickness / 2.0f;
         offset += 0.5f) {
      float offsetRadius = r + offset;
      sf::Vector2f thickPoint(centerX + offsetRadius * cosAngle,
                              centerY + offsetRadius * sinAngle);
      thickWaveform.append(sf::Vertex(thickPoint, color));
    }
  }
}

#endif // WAVEFORM_DRAWER_H
