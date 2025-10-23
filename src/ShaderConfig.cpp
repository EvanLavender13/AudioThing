#include "ShaderConfig.h"
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

void ShaderConfig::fromJSON(const std::string& json) {
 std::string val;
    
    val = extractValue(json, "fadeFactor");
    if (!val.empty()) fadeFactor = std::stof(val);
    
    val = extractValue(json, "pixelSize");
    if (!val.empty()) pixelSize = std::stoi(val);
    
    val = extractValue(json, "blendFactor");
    if (!val.empty()) blendFactor = std::stof(val);
 
    val = extractValue(json, "fadeThreshold");
    if (!val.empty()) fadeThreshold = std::stof(val);
}
