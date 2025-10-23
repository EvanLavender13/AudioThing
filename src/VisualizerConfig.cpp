#include "VisualizerConfig.h"
#include <string>

// Helper function to extract value from JSON
static std::string extractValue(const std::string& content, const std::string& key) {
    std::string searchKey = "\"" + key + "\":";
    size_t pos = content.find(searchKey);
    if (pos == std::string::npos) return "";

    pos += searchKey.length();
    size_t endPos = content.find_first_of(",\n}", pos);
    if (endPos == std::string::npos) return "";

    std::string value = content.substr(pos, endPos - pos);
    
    // Trim whitespace and quotes
    size_t first = value.find_first_not_of(" \t\n\r\"");
    if (first == std::string::npos) return "";
    size_t last = value.find_last_not_of(" \t\n\r\",\"");
    return value.substr(first, last - first + 1);
}

void VisualizerConfig::fromJSON(const std::string& json) {
    std::string val;
    
    val = extractValue(json, "smoothness");
    if (!val.empty()) smoothness = std::stoi(val);
    
  val = extractValue(json, "rotationSpeed");
    if (!val.empty()) rotationSpeed = std::stof(val);

    val = extractValue(json, "waveformHeight");
    if (!val.empty()) waveformHeight = std::stof(val);
    
    val = extractValue(json, "radiusFactor");
    if (!val.empty()) radiusFactor = std::stof(val);
    
    val = extractValue(json, "thickness");
 if (!val.empty()) thickness = std::stoi(val);
    
    val = extractValue(json, "hueOffset");
    if (!val.empty()) hueOffset = std::stof(val);
    
    val = extractValue(json, "hue");
    if (!val.empty()) hue = std::stof(val);
    
  val = extractValue(json, "hueRotationSpeed");
if (!val.empty()) hueRotationSpeed = std::stof(val);
}
