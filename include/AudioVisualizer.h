#ifndef AUDIO_VISUALIZER_H
#define AUDIO_VISUALIZER_H

#include "AudioUtils.h"
#include "VisualizerConfig.h"
#include "ShaderConfig.h"
#include "Waveform.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class AudioVisualizer {
public:
  AudioVisualizer(VisualizerConfig &config, ShaderConfig &shaderConfig);
  ~AudioVisualizer();

  bool initialize(unsigned int width, unsigned int height);
  void handleResize(unsigned int width, unsigned int height);
  void update(const std::vector<double> &audioBuffer, float deltaTime);
  void render(sf::RenderWindow &window);

  // Waveform management
  void addWaveform(const WaveformConfig &config);
  void removeWaveform(size_t index);
  size_t getWaveformCount() const { return waveforms.size(); }
  Waveform* getWaveform(size_t index) { return index < waveforms.size() ? waveforms[index] : nullptr; }
  
  // Sync mode control
  void setSyncMode(bool sync) { syncAllWaveforms = sync; }
  bool getSyncMode() const { return syncAllWaveforms; }

private:
  VisualizerConfig &config; // Non-const reference to configuration
  ShaderConfig &shaderConfig; // Reference to shader configuration

  // Rendering components
  sf::RenderTexture renderTexture;
  sf::Shader shader;
  
  // Waveforms (using raw pointers for simplicity)
  std::vector<Waveform*> waveforms;

  float rotationAngle; // Current rotation angle for global hue
  bool syncAllWaveforms = true; // If true, all waveforms sync with VisualizerConfig
  
  // Helper method to sync waveform configs from visualizer config
  void syncWaveformConfigs();
};

#endif // AUDIO_VISUALIZER_H
