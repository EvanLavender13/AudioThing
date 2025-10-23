#ifndef WAVEFORM_CONFIG_H
#define WAVEFORM_CONFIG_H

#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>

struct WaveformConfig {
    float displayHeight = 30.0f;
    int smoothness = 5;
    float rotationSpeed = 1.0f;
    float radiusFactor = 0.3f;
    float thickness = 5.0f;
    float hueOffset = 0.5f;
    sf::Uint8 alpha = 255;
    sf::Uint8 thickAlpha = 255;
    bool enabled = true;
    
    // Serialization methods
    std::string toJSON(int indent = 0) const {
    std::string indentStr(indent, ' ');
  std::ostringstream oss;
        oss << indentStr << "{\n";
        oss << indentStr << "  \"displayHeight\": " << displayHeight << ",\n";
        oss << indentStr << "  \"smoothness\": " << smoothness << ",\n";
        oss << indentStr << "  \"rotationSpeed\": " << rotationSpeed << ",\n";
        oss << indentStr << "  \"radiusFactor\": " << radiusFactor << ",\n";
        oss << indentStr << "  \"thickness\": " << thickness << ",\n";
        oss << indentStr << "  \"hueOffset\": " << hueOffset << ",\n";
        oss << indentStr << "  \"alpha\": " << static_cast<int>(alpha) << ",\n";
oss << indentStr << "  \"thickAlpha\": " << static_cast<int>(thickAlpha) << ",\n";
        oss << indentStr << "  \"enabled\": " << (enabled ? "true" : "false") << "\n";
        oss << indentStr << "}";
        return oss.str();
    }
    
    void fromJSON(const std::string& json);
};

#endif // WAVEFORM_CONFIG_H
