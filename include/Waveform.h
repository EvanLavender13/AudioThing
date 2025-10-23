#ifndef WAVEFORM_H
#define WAVEFORM_H

#include "Pipeline.h"
#include "WaveformConfig.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Waveform {
public:
  Waveform();
  explicit Waveform(const WaveformConfig &config);
  ~Waveform() = default;

  // Disable copy (because FilterPipeline can't be copied)
  Waveform(const Waveform &) = delete;
  Waveform &operator=(const Waveform &) = delete;

  // Update waveform vertices based on audio data
  void update(const std::vector<double> &audioBuffer, float globalHue,
              float deltaTime, float width, float height);

  // Render waveform to render texture
  void render(sf::RenderTexture &renderTexture);

  // Configuration access
  WaveformConfig &getConfig() { return config; }
  const WaveformConfig &getConfig() const { return config; }
  void setConfig(const WaveformConfig &newConfig) { config = newConfig; }

  // Filter pipeline access
  FilterPipeline &getFilterPipeline() { return filterPipeline; }
  const FilterPipeline &getFilterPipeline() const { return filterPipeline; }

private:
  WaveformConfig config;
  FilterPipeline filterPipeline;

  sf::VertexArray normalWaveform;
  sf::VertexArray thickWaveform;

  float rotationAngle;
};

#endif // WAVEFORM_H
