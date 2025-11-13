#include "AudioVisualizer.h"
#include "UIManager.h"
#include <iostream>

UIManager::UIManager(VisualizerConfig &config, ShaderConfig &shaderConfig)
    : config(config), shaderConfig(shaderConfig) {
  refreshPresetList();
}

UIManager::~UIManager() {}

void UIManager::drawUI(float fps, float frameTime,
                       AudioVisualizer *visualizer) {
  // Create a single main debug window
  ImGui::Begin("Audio Visualizer Debug", nullptr,
               ImGuiWindowFlags_AlwaysAutoResize);

  // Performance metrics section
  if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
    drawPerformanceSection(fps, frameTime);
  }

  ImGui::Separator();

  // Shader effects section
  if (ImGui::CollapsingHeader("Shader Effects",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    drawShaderEffectsSection();
  }

  ImGui::Separator();

  // Waveform management sections (only if visualizer exists)
  if (visualizer) {
    if (ImGui::CollapsingHeader("Waveforms", ImGuiTreeNodeFlags_DefaultOpen)) {
      drawWaveformListSection(visualizer);
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Waveform Settings")) {
      drawWaveformSettingsSection(visualizer);
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Preset Manager")) {
      drawPresetManagerSection(visualizer);
    }
  }

  ImGui::End();
}

void UIManager::drawPerformanceSection(float fps, float frameTime) {
  ImGui::Text("FPS: %.1f", fps);
  ImGui::Text("Frame Time: %.2f ms", frameTime);
}

void UIManager::drawShaderEffectsSection() {
  ImGui::Text("Shader Effects");
  ImGui::SliderFloat("Fade", &shaderConfig.fadeFactor, 0.0f, 1.0f);
  ImGui::SliderInt("Pixel Size", &shaderConfig.pixelSize, 1, 100);
  ImGui::SliderFloat("Blend Factor", &shaderConfig.blendFactor, 0.0f, 1.0f,
                     "%.1f");
  ImGui::SliderFloat("Fade Threshold", &shaderConfig.fadeThreshold, 0.0f, 0.1f,
                     "%.3f");

  ImGui::Spacing();
  ImGui::Text("Enhancement Effects");

  // Saturation boost
  ImGui::SliderFloat("Saturation Boost", &shaderConfig.saturationBoost, 1.0f,
                     2.0f, "%.2f");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Keeps trail colors vivid as they fade (1.0 = normal "
                      "fade, 1.5+ = very saturated)");
  }

  // Dither strength
  ImGui::SliderFloat("Dither Strength", &shaderConfig.ditherStrength, 0.0f,
                     0.05f, "%.3f");
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip(
        "Adds film grain texture (0.01 = subtle, 0.02+ = visible noise)");
  }
}

void UIManager::drawWaveformListSection(AudioVisualizer *visualizer) {
  size_t waveformCount = visualizer->getWaveformCount();
  ImGui::Text("Waveforms: %zu", waveformCount);

  // Add/Remove buttons
  if (ImGui::Button("Add Waveform")) {
    WaveformConfig newConfig;
    newConfig.displayHeight = config.waveformHeight;
    newConfig.smoothness = config.smoothness;
    newConfig.rotationSpeed = config.rotationSpeed;
    newConfig.radiusFactor = config.radiusFactor;
    newConfig.thickness = static_cast<float>(config.thickness);
    newConfig.hueOffset = config.hueOffset;
    newConfig.alpha = 255;
    newConfig.thickAlpha = 255;
    newConfig.enabled = true;
    visualizer->addWaveform(newConfig);
  }

  ImGui::SameLine();

  if (ImGui::Button("Remove Selected") && waveformCount > 0) {
    visualizer->removeWaveform(selectedWaveformIndex);
    if (selectedWaveformIndex >= visualizer->getWaveformCount() &&
        selectedWaveformIndex > 0) {
      selectedWaveformIndex--;
    }
  }

  ImGui::Spacing();

  // List of waveforms
  for (size_t i = 0; i < waveformCount; ++i) {
    Waveform *waveform = visualizer->getWaveform(i);
    if (!waveform)
      continue;

    bool enabled = waveform->getConfig().enabled;
    char label[64];
    snprintf(label, sizeof(label), "Waveform %zu %s", i,
             enabled ? "[ON]" : "[OFF]");

    if (ImGui::Selectable(label,
                          selectedWaveformIndex == static_cast<int>(i))) {
      selectedWaveformIndex = static_cast<int>(i);
    }
  }
}

void UIManager::drawWaveformSettingsSection(AudioVisualizer *visualizer) {
  if (visualizer->getWaveformCount() == 0) {
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                       "No waveforms available");
    return;
  }

  Waveform *waveform = visualizer->getWaveform(selectedWaveformIndex);
  if (!waveform) {
    return;
  }

  ImGui::Text("Editing Waveform %d", selectedWaveformIndex);
  ImGui::Spacing();

  WaveformConfig &waveConfig = waveform->getConfig();

  ImGui::Checkbox("Enabled", &waveConfig.enabled);

  // All settings are now always editable
  float displayHeight = waveConfig.displayHeight;
  if (ImGui::SliderFloat("Height", &displayHeight, 10.0f, 500.0f)) {
    waveConfig.displayHeight = displayHeight;
  }

  if (ImGui::SliderInt("Smoothness", &waveConfig.smoothness, 1, 50)) {
    // Updated
  }

  float rotationSpeed = waveConfig.rotationSpeed;
  if (ImGui::SliderFloat("Rotation Speed", &rotationSpeed, -2.0f, 2.0f)) {
    waveConfig.rotationSpeed = rotationSpeed;
  }

  float radiusFactor = waveConfig.radiusFactor;
  if (ImGui::SliderFloat("Radius", &radiusFactor, 0.0f, 2.0f)) {
    waveConfig.radiusFactor = radiusFactor;
  }

  float thickness = waveConfig.thickness;
  if (ImGui::SliderFloat("Thickness", &thickness, 1.0f, 30.0f)) {
    waveConfig.thickness = thickness;
  }

  float hueOffset = waveConfig.hueOffset;
  if (ImGui::SliderFloat("Hue Offset", &hueOffset, 0.0f, 1.0f)) {
    waveConfig.hueOffset = hueOffset;
  }

  int alpha = waveConfig.alpha;
  if (ImGui::SliderInt("Alpha", &alpha, 0, 255)) {
    waveConfig.alpha = static_cast<sf::Uint8>(alpha);
  }

  int thickAlpha = waveConfig.thickAlpha;
  if (ImGui::SliderInt("Thick Alpha", &thickAlpha, 0, 255)) {
    waveConfig.thickAlpha = static_cast<sf::Uint8>(thickAlpha);
  }
}

void UIManager::drawPresetManagerSection(AudioVisualizer *visualizer) {
  ImGui::Text("Save/Load Configurations");
  ImGui::Spacing();

  // Save section
  ImGui::Text("Preset Name:");
  ImGui::InputText("##presetname", presetNameBuffer, sizeof(presetNameBuffer));

  if (ImGui::Button("Save Current Config")) {
    std::string name = std::string(presetNameBuffer);
    if (!name.empty()) {
      saveCurrentPreset(visualizer, name);
      refreshPresetList();
    }
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  // Load section
  ImGui::Text("Available Presets:");

  if (ImGui::Button("Refresh List")) {
    refreshPresetList();
  }

  if (availablePresets.empty()) {
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No presets found");
  } else {
    for (size_t i = 0; i < availablePresets.size(); ++i) {
      bool isSelected = (selectedPresetIndex == static_cast<int>(i));

      if (ImGui::Selectable(availablePresets[i].c_str(), isSelected)) {
        selectedPresetIndex = static_cast<int>(i);
      }

      // Double click to load
      if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
        loadPreset(visualizer, availablePresets[i]);
      }
    }

    if (selectedPresetIndex >= 0 &&
        selectedPresetIndex < static_cast<int>(availablePresets.size())) {
      if (ImGui::Button("Load Selected Preset")) {
        loadPreset(visualizer, availablePresets[selectedPresetIndex]);
      }
    }
  }

  ImGui::Spacing();
  ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                     "Tip: Double-click to load");
}

void UIManager::refreshPresetList() {
  availablePresets = ConfigSerializer::getAvailablePresets("presets");
}

void UIManager::saveCurrentPreset(AudioVisualizer *visualizer,
                                  const std::string &name) {
  VisualizerPreset preset;
  preset.name = name;
  preset.visualizerConfig = config;
  preset.shaderConfig = shaderConfig;

  // Collect all waveform configurations
  for (size_t i = 0; i < visualizer->getWaveformCount(); ++i) {
    Waveform *waveform = visualizer->getWaveform(i);
    if (waveform) {
      preset.waveforms.push_back(waveform->getConfig());
    }
  }

  std::string filename = "presets/" + name + ".json";
  if (ConfigSerializer::savePreset(filename, preset)) {
    std::cout << "Preset saved successfully!" << std::endl;
  }
}

void UIManager::loadPreset(AudioVisualizer *visualizer,
                           const std::string &filename) {
  VisualizerPreset preset;
  std::string filepath = "presets/" + filename;

  if (ConfigSerializer::loadPreset(filepath, preset)) {
    // Apply configurations
    config = preset.visualizerConfig;
    shaderConfig = preset.shaderConfig;

    // Clear existing waveforms
    while (visualizer->getWaveformCount() > 0) {
      visualizer->removeWaveform(0);
    }

    // Add loaded waveforms
    for (const auto &waveConfig : preset.waveforms) {
      visualizer->addWaveform(waveConfig);
    }

    // Reset selected waveform index
    selectedWaveformIndex = 0;

    std::cout << "Preset loaded successfully: " << preset.name << std::endl;
  }
}
