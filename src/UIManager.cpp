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
  drawGlobalSettings(fps, frameTime);

  if (visualizer) {
    drawWaveformList(visualizer);
    drawWaveformSettings(visualizer);
    drawPresetManager(visualizer);
  }
}

void UIManager::drawGlobalSettings(float fps, float frameTime) {
  ImGui::Begin("Global Settings");

  ImGui::Text("FPS: %.1f", fps);
  ImGui::Text("Frame Time: %.2f ms", frameTime);

  ImGui::Separator();
  ImGui::Text("Global Waveform Settings");
  ImGui::SliderInt("Smoothness", &config.smoothness, 1, 50);
  ImGui::SliderFloat("Height", &config.waveformHeight, 50.0f, 300.0f);
  ImGui::SliderFloat("Rotation", &config.rotationSpeed, 0.01f, 1.0f);
  ImGui::SliderFloat("Radius", &config.radiusFactor, 0.1f, 1.0f);
  ImGui::SliderInt("Thickness", &config.thickness, 1, 20);
  ImGui::SliderFloat("Contrast", &config.hueOffset, 0.0f, 1.0f);

  ImGui::Separator();
  ImGui::Text("Shader Effects");
  ImGui::SliderFloat("Fade", &shaderConfig.fadeFactor, 0.0f, 1.0f);
  ImGui::SliderInt("Pixel Size", &shaderConfig.pixelSize, 1, 100);
  ImGui::SliderFloat("Blend Factor", &shaderConfig.blendFactor, 0.0f, 1.0f,
                     "%.1f");
  ImGui::SliderFloat("Fade Threshold", &shaderConfig.fadeThreshold, 0.0f, 0.1f,
                     "%.3f");

  ImGui::End();
}

void UIManager::drawWaveformList(AudioVisualizer *visualizer) {
  ImGui::Begin("Waveforms");

  bool syncMode = visualizer->getSyncMode();
  if (ImGui::Checkbox("Sync All Waveforms", &syncMode)) {
    visualizer->setSyncMode(syncMode);
  }

  ImGui::Separator();

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

  ImGui::Separator();

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

  ImGui::End();
}

void UIManager::drawWaveformSettings(AudioVisualizer *visualizer) {
  if (visualizer->getWaveformCount() == 0) {
    return;
  }

  Waveform *waveform = visualizer->getWaveform(selectedWaveformIndex);
  if (!waveform) {
    return;
  }

  ImGui::Begin("Waveform Settings");

  ImGui::Text("Editing Waveform %d", selectedWaveformIndex);

  if (visualizer->getSyncMode()) {
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
                       "Sync Mode: Settings synced with Global");
    ImGui::Text("Disable 'Sync All Waveforms' to edit individually");
  } else {
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
                       "Independent Mode: Edit this waveform");
  }

  ImGui::Separator();

  WaveformConfig &waveConfig = waveform->getConfig();

  ImGui::Checkbox("Enabled", &waveConfig.enabled);

  // Only allow editing if sync mode is off
  if (!visualizer->getSyncMode()) {
    float displayHeight = waveConfig.displayHeight;
    if (ImGui::SliderFloat("Height", &displayHeight, 10.0f, 300.0f)) {
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
  } else {
    ImGui::Text("Height: %.1f (synced)", waveConfig.displayHeight);
    ImGui::Text("Smoothness: %d (synced)", waveConfig.smoothness);
    ImGui::Text("Rotation Speed: %.2f (synced)", waveConfig.rotationSpeed);
    ImGui::Text("Radius: %.2f (synced)", waveConfig.radiusFactor);
    ImGui::Text("Thickness: %.1f (synced)", waveConfig.thickness);
    ImGui::Text("Hue Offset: %.2f (synced)", waveConfig.hueOffset);
  }

  ImGui::End();
}

void UIManager::drawPresetManager(AudioVisualizer *visualizer) {
  ImGui::Begin("Preset Manager");

  ImGui::Text("Save/Load Configurations");
  ImGui::Separator();

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

  ImGui::Separator();

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

  ImGui::Separator();
  ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                     "Tip: Double-click to load");

  ImGui::End();
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
  preset.syncMode = visualizer->getSyncMode();

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
    visualizer->setSyncMode(preset.syncMode);

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
