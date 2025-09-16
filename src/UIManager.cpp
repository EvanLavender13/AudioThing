#include "UIManager.h"

UIManager::UIManager(VisualizerConfig &config) : config(config) {}

UIManager::~UIManager() {}

void UIManager::drawUI(float fps, float frameTime) {
  ImGui::Begin("Waveform Settings");

  ImGui::Text("FPS: %.1f", fps);
  ImGui::Text("Frame Time: %.2f ms", frameTime);

  ImGui::SliderInt("Smoothness", &config.smoothness, 1, 50);
  ImGui::SliderFloat("Fade", &config.fadeFactor, 0.0f, 1.0f);
  ImGui::SliderFloat("Height", &config.waveformHeight, 50.0f, 300.0f);
  ImGui::SliderFloat("Rotation", &config.rotationSpeed, 0.01f, 1.0f);
  ImGui::SliderFloat("Radius", &config.radiusFactor, 0.1f, 1.0f);
  ImGui::SliderInt("Pixel Size", &config.pixelSize, 1, 100);
  ImGui::SliderFloat("Blend Factor", &config.blendFactor, 0.0f, 1.0f, "%.1f");
  ImGui::SliderInt("Thickness", &config.thickness, 1, 20);
  ImGui::SliderFloat("Contrast", &config.hueOffset, 0.0f, 1.0f);

  ImGui::End();
}
