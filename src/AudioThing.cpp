#include "AudioCaptureRAII.h"
#include "AudioUtils.h"
#include "AudioVisualizer.h"
#include "ImGuiRAII.h"
#include "UIManager.h"
#include "VisualizerConfig.h"
#include <atomic>
#include <chrono>
#include <cmath>
#include <complex>
#include <fftw3.h>
#include <iostream>
#include <mutex>
#include <numeric>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <thread>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Process all window events
void processEvents(sf::RenderWindow &window, AudioVisualizer &visualizer,
                   ImGuiRAII &imguiManager) {
  sf::Event event;
  while (window.pollEvent(event)) {
    imguiManager.processEvent(event);

    if (event.type == sf::Event::Closed) {
      window.close();
    } else if (event.type == sf::Event::Resized) {
      // Update the visualizer's render texture and view
      visualizer.handleResize(event.size.width, event.size.height);

      // Update the window's view to match the new size
      sf::FloatRect visibleArea(0, 0, static_cast<float>(event.size.width),
                                static_cast<float>(event.size.height));
      window.setView(sf::View(visibleArea));
    }
  }
}

// Process audio data from capture thread to render thread
void processAudioData(std::mutex &audioBufferMutex,
                      std::condition_variable &audioBufferCV,
                      std::vector<double> &audioBuffer,
                      std::vector<double> &renderBuffer, bool &bufferReady) {
  // Wait for new audio data
  std::unique_lock<std::mutex> lock(audioBufferMutex);
  if (!bufferReady) {
    audioBufferCV.wait(lock, [&bufferReady] { return bufferReady; });
  }

  // Swap buffers and mark as processed
  std::swap(renderBuffer, audioBuffer);
  bufferReady = false;

  // Process audio data
  if (isAudioPlaying(renderBuffer)) {
    trimTrailingZeros(renderBuffer);
  }
  normalizeAudioData(renderBuffer);
}

// Perform visualization update and rendering
void updateAndRender(sf::RenderWindow &window, AudioVisualizer &visualizer,
                     const std::vector<double> &renderBuffer, float deltaTime) {
  // Clear the window
  window.clear();

  // Update and render the visualizer
  visualizer.update(renderBuffer, deltaTime);
  visualizer.render(window);
}

// Update and render UI
void updateUI(UIManager &uiManager, ImGuiRAII &imguiManager, float deltaTime) {
  // Update ImGui
  imguiManager.update(deltaTime);

  // Draw UI with performance metrics
  float fps = 1.0f / deltaTime;
  float frameTime = deltaTime * 1000.0f;
  uiManager.drawUI(fps, frameTime);

  // Render ImGui
  imguiManager.render();
}

int main() {
  try {
    constexpr int BUFFER_SIZE = 512;

    // Resources managed with RAII patterns
    std::vector<double> audioBuffer(BUFFER_SIZE);
    std::vector<double> renderBuffer(BUFFER_SIZE);
    std::mutex audioBufferMutex;
    std::condition_variable audioBufferCV;
    std::atomic<bool> capturingAudio(true);
    bool bufferReady = false;

    // Set up SFML window with RAII (SFML already handles resources this way)
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(2560, 1440), "Audio Spectrum",
                            sf::Style::Default, settings);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    // Create configuration and visualizer objects
    VisualizerConfig config;
    AudioVisualizer visualizer(config);
    if (!visualizer.initialize(1920, 1080)) {
      throw std::runtime_error("Failed to initialize visualizer");
    }

    // Create UI manager
    UIManager uiManager(config);

    // Initialize ImGui with RAII
    ImGuiRAII imguiManager(window);

    // Create audio capture with RAII (automatically starts capture thread)
    AudioCaptureRAII audioCaptureRAII(BUFFER_SIZE, audioBuffer,
                                      audioBufferMutex, audioBufferCV,
                                      capturingAudio, bufferReady);

    sf::Clock deltaClock;
    float waveformUpdateInterval = 1.0f / 30.0f;
    float waveformUpdateAccumulator = 0.0f;

    // Main application loop
    while (window.isOpen()) {
      // Process window events
      processEvents(window, visualizer, imguiManager);

      // Calculate frame time
      float deltaTime = deltaClock.restart().asSeconds();
      waveformUpdateAccumulator += deltaTime;

      // Clear the window
      window.clear();

      // Update waveform at fixed interval
      if (waveformUpdateAccumulator >= waveformUpdateInterval) {
        // Process audio data
        processAudioData(audioBufferMutex, audioBufferCV, audioBuffer,
                         renderBuffer, bufferReady);

        visualizer.update(renderBuffer, waveformUpdateAccumulator);
        waveformUpdateAccumulator = 0.0f;
      }

      // Always render the visualizer
      visualizer.render(window);

      // Update and render UI
      updateUI(uiManager, imguiManager, deltaTime);

      // Display the frame
      window.display();
    }

    // ... (existing cleanup code)

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
