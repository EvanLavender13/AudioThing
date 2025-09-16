#ifndef IMGUI_RAII_H
#define IMGUI_RAII_H

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <implot.h>

// RAII wrapper for ImGui to handle automatic initialization and shutdown
class ImGuiRAII {
public:
  // Constructor automatically initializes ImGui
  ImGuiRAII(sf::RenderWindow &window) : window_(window) {
    if (!ImGui::SFML::Init(window)) {
      throw std::runtime_error("Failed to initialize ImGui-SFML");
    }

    ImPlot::CreateContext();
  }

  // Destructor automatically cleans up ImGui resources
  ~ImGuiRAII() {
    ImPlot::DestroyContext();
    ImGui::SFML::Shutdown();
  }

  // Process events with ImGui
  void processEvent(const sf::Event &event) {
    ImGui::SFML::ProcessEvent(event);
  }

  // Update ImGui with delta time
  void update(float deltaTime) {
    ImGui::SFML::Update(window_, sf::seconds(deltaTime));
  }

  // Render ImGui
  void render() { ImGui::SFML::Render(window_); }

  // Deleted copy/move constructors and assignment to ensure proper RAII
  // behavior
  ImGuiRAII(const ImGuiRAII &) = delete;
  ImGuiRAII &operator=(const ImGuiRAII &) = delete;
  ImGuiRAII(ImGuiRAII &&) = delete;
  ImGuiRAII &operator=(ImGuiRAII &&) = delete;

private:
  sf::RenderWindow &window_;
};

#endif // IMGUI_RAII_H
