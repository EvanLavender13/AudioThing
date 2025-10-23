#include "Waveform.h"
#include "WaveformDrawer.h"

Waveform::Waveform() : rotationAngle(0.0f) {
  normalWaveform.setPrimitiveType(sf::LineStrip);
  thickWaveform.setPrimitiveType(sf::LineStrip);
}

Waveform::Waveform(const WaveformConfig &config)
    : config(config), rotationAngle(0.0f) {
  normalWaveform.setPrimitiveType(sf::LineStrip);
  thickWaveform.setPrimitiveType(sf::LineStrip);
}

void Waveform::update(const std::vector<double> &audioBuffer, float globalHue,
                      float deltaTime, float width, float height) {
  if (!config.enabled) {
    return;
  }

  // Update rotation
  rotationAngle += config.rotationSpeed * deltaTime;

  // Process audio through filter pipeline
  std::vector<double> filteredAudio = audioBuffer;
  // filterPipeline.process(audioBuffer);

  // Draw waveform using filtered audio data
  drawWaveform(filteredAudio, normalWaveform, thickWaveform,
               config.displayHeight, config.smoothness, -rotationAngle,
               config.radiusFactor, width, height, globalHue, config.thickness,
               config.hueOffset, config.alpha, config.thickAlpha);
}

void Waveform::render(sf::RenderTexture &renderTexture) {
  if (!config.enabled) {
    return;
  }

  renderTexture.draw(normalWaveform);
  renderTexture.draw(thickWaveform);
}
