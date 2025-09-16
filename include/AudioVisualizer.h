#ifndef AUDIO_VISUALIZER_H
#define AUDIO_VISUALIZER_H

#include "AudioUtils.h"
#include "VisualizerConfig.h"
#include "WaveformDrawer.h"
#include <SFML/Graphics.hpp>
#include <vector>

class AudioVisualizer {
public:
  AudioVisualizer(VisualizerConfig &config); // Use non-const reference
  ~AudioVisualizer();

  bool initialize(unsigned int width, unsigned int height);
  void handleResize(unsigned int width, unsigned int height);
  void update(const std::vector<double> &audioBuffer, float deltaTime);
  void render(sf::RenderWindow &window);

private:
  VisualizerConfig &config; // Non-const reference to configuration

  // Rendering components
  sf::RenderTexture renderTexture;
  sf::Shader shader;
  sf::VertexArray waveform;
  sf::VertexArray thickWaveform;
  sf::VertexArray largeWaveform;
  sf::VertexArray largeThickWaveform;

  float rotationAngle; // Current rotation angle
};

#endif // AUDIO_VISUALIZER_H
