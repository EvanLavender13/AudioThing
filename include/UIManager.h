#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "VisualizerConfig.h"
#include "ShaderConfig.h"
#include "ConfigSerializer.h"
#include <imgui.h>
#include <string>
#include <vector>

// Forward declaration
class AudioVisualizer;

class UIManager {
public:
  UIManager(VisualizerConfig &config, ShaderConfig &shaderConfig);
  ~UIManager();

  void drawUI(float fps, float frameTime, AudioVisualizer *visualizer = nullptr);

private:
  VisualizerConfig &config; // Reference to the shared configuration
  ShaderConfig &shaderConfig; // Reference to shader configuration

  int selectedWaveformIndex = 0; // Currently selected waveform in UI
  
  // Preset management
  std::vector<std::string> availablePresets;
  int selectedPresetIndex = -1;
  char presetNameBuffer[256] = "MyPreset";
  bool showSaveDialog = false;
  bool showLoadDialog = false;
  
  // Helper methods for drawing sections within the single window
  void drawPerformanceSection(float fps, float frameTime);
  void drawShaderEffectsSection();
  void drawWaveformListSection(AudioVisualizer *visualizer);
  void drawWaveformSettingsSection(AudioVisualizer *visualizer);
  void drawPresetManagerSection(AudioVisualizer *visualizer);
  
  // Preset operations
  void refreshPresetList();
  void saveCurrentPreset(AudioVisualizer *visualizer, const std::string& name);
  void loadPreset(AudioVisualizer *visualizer, const std::string& filename);
};

#endif // UI_MANAGER_H
