#ifndef VISUALIZER_CONFIG_H
#define VISUALIZER_CONFIG_H

#include <string>
#include <sstream>

struct VisualizerConfig {
  // Waveform display settings
  int smoothness = 1;     // Smoothness of the waveform
  float rotationSpeed = 0.25f;   // Base rotation speed (radians per second)
  float waveformHeight = 225.0f; // Height of the waveform
  float radiusFactor = 0.6f;     // Radius factor for the waveform
  int thickness = 10;       // Thickness of the waveform
  float hueOffset = 0.95f;       // Hue offset for thick waveforms

  // Global animation settings
  float hue = 0.0f;     // Current hue value (updated by visualizer)
  float hueRotationSpeed = 0.5f; // Speed of hue rotation
  
  // Serialization methods
  std::string toJSON(int indent = 0) const {
    std::string indentStr(indent, ' ');
    std::ostringstream oss;
    oss << indentStr << "{\n";
    oss << indentStr << "  \"smoothness\": " << smoothness << ",\n";
    oss << indentStr << "  \"rotationSpeed\": " << rotationSpeed << ",\n";
    oss << indentStr << "  \"waveformHeight\": " << waveformHeight << ",\n";
    oss << indentStr << "  \"radiusFactor\": " << radiusFactor << ",\n";
    oss << indentStr << "  \"thickness\": " << thickness << ",\n";
    oss << indentStr << "  \"hueOffset\": " << hueOffset << ",\n";
    oss << indentStr << "  \"hue\": " << hue << ",\n";
    oss << indentStr << "  \"hueRotationSpeed\": " << hueRotationSpeed << "\n";
    oss << indentStr << "}";
    return oss.str();
  }
  
  void fromJSON(const std::string& json);
};

#endif // VISUALIZER_CONFIG_H
