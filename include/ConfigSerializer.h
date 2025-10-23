#ifndef CONFIG_SERIALIZER_H
#define CONFIG_SERIALIZER_H

#include "VisualizerConfig.h"
#include "ShaderConfig.h"
#include "WaveformConfig.h"
#include <string>
#include <vector>

// Forward declaration
class ConfigSerializer;

// Simple JSON-like structure for configuration
struct VisualizerPreset {
    std::string name;
    VisualizerConfig visualizerConfig;
    ShaderConfig shaderConfig;
    std::vector<WaveformConfig> waveforms;
    bool syncMode;
    
    // Serialization methods
    std::string toJSON() const;
    void fromJSON(const std::string& json);
};

class ConfigSerializer {
public:
    // Save preset to file
    static bool savePreset(const std::string& filepath, const VisualizerPreset& preset);
    
    // Load preset from file
    static bool loadPreset(const std::string& filepath, VisualizerPreset& preset);
    
    // Get list of available presets in a directory
    static std::vector<std::string> getAvailablePresets(const std::string& directory = "presets");

    // Utility functions (public so VisualizerPreset can use them)
    static std::string extractValue(const std::string& content, const std::string& key);
    static std::string extractSection(const std::string& content, const std::string& key);
};

#endif // CONFIG_SERIALIZER_H
