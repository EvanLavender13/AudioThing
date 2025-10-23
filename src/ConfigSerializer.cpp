#include "ConfigSerializer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

// VisualizerPreset serialization
std::string VisualizerPreset::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"name\": \"" << name << "\",\n";
    oss << "  \"syncMode\": " << (syncMode ? "true" : "false") << ",\n";
    oss << "  \"visualizerConfig\": " << visualizerConfig.toJSON(2) << ",\n";
    oss << "  \"shaderConfig\": " << shaderConfig.toJSON(2) << ",\n";
    oss << "  \"waveforms\": [\n";
    
    for (size_t i = 0; i < waveforms.size(); ++i) {
        oss << waveforms[i].toJSON(4);
        if (i < waveforms.size() - 1) {
      oss << ",";
        }
        oss << "\n";
  }
    
    oss << "  ]\n";
  oss << "}\n";
    return oss.str();
}

void VisualizerPreset::fromJSON(const std::string& json) {
    // Parse name
    name = ConfigSerializer::extractValue(json, "name");
    
    // Parse sync mode
    std::string syncModeStr = ConfigSerializer::extractValue(json, "syncMode");
    syncMode = (syncModeStr == "true");
    
    // Parse visualizer config
    std::string vizConfigJson = ConfigSerializer::extractSection(json, "visualizerConfig");
    visualizerConfig.fromJSON(vizConfigJson);
    
    // Parse shader config
    std::string shaderConfigJson = ConfigSerializer::extractSection(json, "shaderConfig");
 shaderConfig.fromJSON(shaderConfigJson);
    
    // Parse waveforms array
    waveforms.clear();
    size_t waveformsStart = json.find("\"waveforms\"");
    if (waveformsStart != std::string::npos) {
        size_t arrayStart = json.find("[", waveformsStart);
        size_t arrayEnd = json.rfind("]");
        
        if (arrayStart != std::string::npos && arrayEnd != std::string::npos) {
      std::string arrayContent = json.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
            
  // Parse each waveform object
            int braceCount = 0;
      size_t objStart = 0;
      
       for (size_t i = 0; i < arrayContent.length(); ++i) {
           if (arrayContent[i] == '{') {
     if (braceCount == 0) objStart = i;
        braceCount++;
                } else if (arrayContent[i] == '}') {
         braceCount--;
if (braceCount == 0) {
      std::string waveJson = arrayContent.substr(objStart, i - objStart + 1);
            WaveformConfig config;
        config.fromJSON(waveJson);
         waveforms.push_back(config);
        }
   }
   }
        }
    }
}

// ConfigSerializer implementation
bool ConfigSerializer::savePreset(const std::string& filepath, const VisualizerPreset& preset) {
  std::ofstream file(filepath);
if (!file.is_open()) {
   std::cerr << "Failed to open file for writing: " << filepath << std::endl;
        return false;
    }

    file << preset.toJSON();
  file.close();
    
    std::cout << "Preset saved to: " << filepath << std::endl;
 return true;
}

bool ConfigSerializer::loadPreset(const std::string& filepath, VisualizerPreset& preset) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << filepath << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    preset.fromJSON(content);
    
    std::cout << "Preset loaded from: " << filepath << std::endl;
    return true;
}

std::vector<std::string> ConfigSerializer::getAvailablePresets(const std::string& directory) {
    std::vector<std::string> presets;
    
    if (!fs::exists(directory)) {
        fs::create_directory(directory);
        return presets;
    }

  for (const auto& entry : fs::directory_iterator(directory)) {
     if (entry.is_regular_file() && entry.path().extension() == ".json") {
            presets.push_back(entry.path().filename().string());
     }
 }

    std::sort(presets.begin(), presets.end());
    return presets;
}

std::string ConfigSerializer::extractValue(const std::string& content, const std::string& key) {
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

std::string ConfigSerializer::extractSection(const std::string& content, const std::string& key) {
    std::string searchKey = "\"" + key + "\":";
    size_t pos = content.find(searchKey);
    if (pos == std::string::npos) return "";
    
    size_t start = content.find("{", pos);
    if (start == std::string::npos) return "";
    
    // Find matching closing brace
    int braceCount = 1;
  size_t end = start + 1;
 while (end < content.length() && braceCount > 0) {
        if (content[end] == '{') braceCount++;
        else if (content[end] == '}') braceCount--;
        end++;
  }
    
    if (braceCount == 0) {
        return content.substr(start, end - start);
    }
    
    return "";
}
