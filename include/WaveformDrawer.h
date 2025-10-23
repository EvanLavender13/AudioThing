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
  // Normalize hue to [0, 1] range
  hue = hue - std::floor(hue);
  if (hue < 0.0f) hue += 1.0f;
  
  // Clamp saturation and value to [0, 1]
  saturation = std::max(0.0f, std::min(1.0f, saturation));
  value = std::max(0.0f, std::min(1.0f, value));
  
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
    // This should never happen with proper modulo, but return red as fallback
    return sf::Color(static_cast<sf::Uint8>(value * 255.0f), 0, 0);
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
inline void drawWaveform(const std::vector<double> &buffer,
                         sf::VertexArray &waveform,
                         sf::VertexArray &thickWaveform, float displayHeight,
                         int smoothness, float rotationAngle,
                         float radiusFactor, float width, float height,
                         float hue, float thickness,
                         float thickWaveformHueOffset = 0.5f,
                         sf::Uint8 waveformAlpha = 255,
                         sf::Uint8 thickWaveformAlpha = 255) {
  if (buffer.empty()) {
    return;
  }

  // Use static vectors to avoid repeated allocations
  static std::vector<double> extendedBuffer;
  static std::vector<double> smoothedAudioBuffer;
  static std::vector<float> cosCache;
  static std::vector<float> sinCache;

  // Reserve capacity to avoid reallocations
  const size_t requiredSize = buffer.size() * 2;
  if (extendedBuffer.capacity() < requiredSize) {
    extendedBuffer.reserve(requiredSize * 1.5); // Reserve extra to reduce future reallocations
  }
  
  extendedBuffer.resize(requiredSize);
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
  
  // Pre-calculate exact thick waveform vertex count
  const size_t thicknessSteps = static_cast<size_t>((thickness + 1.0f) / 0.5f);
  const size_t totalThickVertices = numPoints * thicknessSteps;
  thickWaveform.resize(totalThickVertices);

  float centerX = width / 2.0f;
  float centerY = height / 2.0f;
  float radius = std::min(centerX, centerY) * radiusFactor;

  // Smooth the audio data - reuses existing allocation
  smoothAudioData(extendedBuffer, smoothedAudioBuffer, smoothness);

  // Pre-compute values
  float hueOffset = -rotationAngle / (2.0f * static_cast<float>(M_PI));
  // Normalize thickHue to [0, 1] range
  float thickHue = hue + thickWaveformHueOffset;
  thickHue = thickHue - std::floor(thickHue);
  if (thickHue < 0.0f) thickHue += 1.0f;

  // Cache size calculations to avoid repeating them
  const size_t extSize = extendedBuffer.size();
  const size_t smoothSize = smoothedAudioBuffer.size();

  // Pre-compute sin/cos for all angles to avoid redundant calculations
  cosCache.resize(numPoints);
  sinCache.resize(numPoints);
  for (size_t i = 0; i < numPoints; ++i) {
    float angle = static_cast<float>(i) / static_cast<float>(numPoints) * 2.0f *
                      static_cast<float>(M_PI) +
                  rotationAngle;
    cosCache[i] = std::cos(angle);
    sinCache[i] = std::sin(angle);
  }

  // Draw normal waveform using cached trig values
  for (size_t i = 0; i < numPoints; ++i) {
    float cosAngle = cosCache[i];
    float sinAngle = sinCache[i];

    size_t index = (i / pointMultiplier) % extSize;
    float fraction = static_cast<float>(i % pointMultiplier) /
                     static_cast<float>(pointMultiplier);

    // Optimized wraparound using conditionals instead of modulo
    size_t idx0, idx1, idx2, idx3;
    
    // idx0 = index - 2 with wraparound
    if (index >= 2) {
      idx0 = index - 2;
    } else {
      idx0 = index + extSize - 2;
    }
    
    // idx1 = index - 1 with wraparound
    if (index >= 1) {
      idx1 = index - 1;
    } else {
      idx1 = index + extSize - 1;
    }
    
    // idx2 = index + 1 with wraparound
    idx2 = index + 1;
    if (idx2 >= extSize) {
      idx2 -= extSize;
    }
    
    // idx3 = index + 2 with wraparound
    idx3 = index + 2;
    if (idx3 >= extSize) {
      idx3 -= extSize;
    }

    float sampleValue = static_cast<float>(
        cubicInterpolate(extendedBuffer[idx0], extendedBuffer[idx1],
                         extendedBuffer[idx2], extendedBuffer[idx3], fraction));

    float r = radius + sampleValue * displayHeight;
    float x = centerX + r * cosAngle;
    float y = centerY + r * sinAngle;

    // Normalize hue value to [0, 1] range
    float normalizedHue = hue + hueOffset + static_cast<float>(i) / static_cast<float>(numPoints);
    normalizedHue = normalizedHue - std::floor(normalizedHue);
    if (normalizedHue < 0.0f) normalizedHue += 1.0f;
    
    sf::Color color = hsvToRgb(normalizedHue, 1.0f, 0.7f);
    color.a = waveformAlpha;
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

  // Process thick waveform using cached trig values
  size_t thickVertexIndex = 0;
  for (size_t i = 0; i < numPoints; ++i) {
    float cosAngle = cosCache[i];
    float sinAngle = sinCache[i];

    size_t index = (i / pointMultiplier) % smoothSize;
    float fraction = static_cast<float>(i % pointMultiplier) /
                     static_cast<float>(pointMultiplier);

    // Optimized wraparound using conditionals instead of modulo
    size_t idx0, idx1, idx2, idx3;
    
    // idx0 = index - 2 with wraparound
    if (index >= 2) {
      idx0 = index - 2;
    } else {
      idx0 = index + smoothSize - 2;
    }
    
    // idx1 = index - 1 with wraparound
    if (index >= 1) {
      idx1 = index - 1;
    } else {
      idx1 = index + smoothSize - 1;
    }
    
    // idx2 = index + 1 with wraparound
    idx2 = index + 1;
    if (idx2 >= smoothSize) {
      idx2 -= smoothSize;
    }
    
    // idx3 = index + 2 with wraparound
    idx3 = index + 2;
    if (idx3 >= smoothSize) {
      idx3 -= smoothSize;
    }

    float sampleValue = static_cast<float>(cubicInterpolate(
        smoothedAudioBuffer[idx0], smoothedAudioBuffer[idx1],
        smoothedAudioBuffer[idx2], smoothedAudioBuffer[idx3], fraction));

    float r = radius + sampleValue * displayHeight;

    // Normalize hue value to [0, 1] range
    float normalizedThickHue = thickHue + hueOffset + static_cast<float>(i) / static_cast<float>(numPoints);
    normalizedThickHue = normalizedThickHue - std::floor(normalizedThickHue);
    if (normalizedThickHue < 0.0f) normalizedThickHue += 1.0f;

    sf::Color color = hsvToRgb(normalizedThickHue, 1.0f, 1.0f);
    color.a = thickWaveformAlpha;

    // Use index-based access instead of append - CRITICAL OPTIMIZATION
    for (float offset = -thickness / 2.0f; offset <= thickness / 2.0f;
         offset += 0.5f) {
      float offsetRadius = r + offset;
      sf::Vector2f thickPoint(centerX + offsetRadius * cosAngle,
                              centerY + offsetRadius * sinAngle);
      thickWaveform[thickVertexIndex].position = thickPoint;
      thickWaveform[thickVertexIndex].color = color;
      ++thickVertexIndex;
    }
  }
  
  // Resize to actual vertex count to remove any uninitialized vertices
  thickWaveform.resize(thickVertexIndex);
}

#endif // WAVEFORM_DRAWER_H
