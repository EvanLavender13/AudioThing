#ifndef SHADER_CONFIG_H
#define SHADER_CONFIG_H

#include <string>
#include <sstream>

struct ShaderConfig {
  float fadeFactor = 0.99f;      // Fade factor for the shader
  int pixelSize = 25;            // Pixel size for pixelation effect
  float blendFactor = 0.0f;      // Blend factor for the shader
  float fadeThreshold = 0.01f;   // Threshold for eliminating dark pixels
  
  // Enhancement effects
  float saturationBoost = 1.15f;  // Saturation multiplier (1.0 = no boost, higher = more vivid trails)
  float ditherStrength = 0.01f;   // Dithering strength for anti-banding (0.0 = off)
  
  // Serialization methods
  std::string toJSON(int indent = 0) const {
    std::string indentStr(indent, ' ');
    std::ostringstream oss;
    oss << indentStr << "{\n";
    oss << indentStr << "  \"fadeFactor\": " << fadeFactor << ",\n";
    oss << indentStr << "  \"pixelSize\": " << pixelSize << ",\n";
    oss << indentStr << "  \"blendFactor\": " << blendFactor << ",\n";
    oss << indentStr << "  \"fadeThreshold\": " << fadeThreshold << ",\n";
    oss << indentStr << "  \"saturationBoost\": " << saturationBoost << ",\n";
    oss << indentStr << "  \"ditherStrength\": " << ditherStrength << "\n";
    oss << indentStr << "}";
    return oss.str();
  }
  
  void fromJSON(const std::string& json);
};

#endif // SHADER_CONFIG_H
